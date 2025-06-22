#include "pultudpsend.h"
#include <QNetworkDatagram>
#include <QDebug>

constexpr uint8_t MAX_EXPECTED_LENGTH = 35;

pultUdpSend::pultUdpSend(const QString &ip, quint16 port, uint32_t b_address, QObject *parent)
    : QObject(parent),
      m_ip(ip),
      m_port(port),
      base_address(b_address) {

    m_socket = new QUdpSocket(this);
    timeoutTimer = new QTimer(this);
    timeoutTimer->setInterval(300);
    timeoutTimer->setSingleShot(true);
    connect(timeoutTimer, &QTimer::timeout, this, &pultUdpSend::onTimeout);

    if(m_socket->bind(QHostAddress::Any,m_port)){
       qDebug() << "Socket is listening port"<<port;
       connect(m_socket, &QUdpSocket::readyRead, this,&pultUdpSend::listenForResponses);
    }
    else {
       qDebug() << "Failed to bind to port"<<port;
       qDebug() << "port" << m_port;
    }
}
QByteArray pultUdpSend::createHeader(uint32_t address, uint16_t size,bool isReceipt,
                                     bool isWrite, bool isRequest) {

    uint16_t flags = 0;
    if (isReceipt)   flags |= (1 << 13); // Установка бита "квитанция"
    if (isWrite)   flags |= (1 << 14); // Установка бита "запись"
    if (isRequest) flags |= (1 << 15); // Установка бита "запрос"

    QByteArray header(8, 0);
    QDataStream stream(&header, QIODevice::WriteOnly); // Создаем поток данных для записи в буфер
    stream.setByteOrder(QDataStream::LittleEndian); // Устанавливаем порядок байтов little endian
    stream << address << size << static_cast<uint16_t>(flags);
    return header;
}

// Отправка данных
bool pultUdpSend::sendData(uint8_t interfaceId, const QByteArray &data,  QObject *origin, PultErrors &errors) {
     m_lastSender = origin; // запоминаем, кто отправляет
     uint32_t address = base_address + interfaceId; // Адрес памяти устройства выдачи
     QByteArray packet = createHeader(address,data.size(),false,true,true);//формируем заголовок
     packet.append(data);
     pendingCommand.command = packet;
         pendingCommand.origin = origin;
      qDebug() << "packet" << packet;
      qDebug() << "origin" << origin;
         retryCount = 0;
     bool success = sendPacket(packet, errors);
     if (!success) {
         qDebug() << "Error while sending data:" << errors.toString();
     }
     return success;
 }

bool pultUdpSend::receiveData(uint8_t interfaceId, QByteArray &data, PultErrors &errors) {
    uint32_t address = base_address + interfaceId; // Адрес памяти устройства приема
    QByteArray request = createHeader(address,64,false,false,true);// 64 байта макс. размер приема

    if (!sendPacket(request, errors)) {
        qDebug() << "Error while requesting data:" << errors.toString();
        return false;
    }

    QByteArray response;
    if (!receivePacket(response, errors)) {
        qDebug() << "Error while receiving data:" << errors.toString();
        return false;
    }

    data = response.mid(8); // Убираем 8 байт заголовка;
    return true;
}

// Чтение настроек пульта
bool pultUdpSend::readSettings(QByteArray &settings, PultErrors &errors) {
    uint32_t address =  base_address/*0x00040000*/;//базовый адрес настройки пульта
    QByteArray request = createHeader(address,64,false,false,true);
    if (!sendPacket(request, errors)) {
        return false;
    }
    QByteArray response;
    if (!receivePacket(response, errors)) {
        return false;
    }
    settings = response.mid(8);//уберем заголовок
    return true;
}
// Запись настроек пульта
//bool pultUdpSend::writeSettings(const QByteArray &settings, PultErrors &errors) {
//    if(settings.size() != 64) {
//        errors.wrongParam = true;
//        return false;
//    }
//    uint32_t address =  base_address;//базовый адрес настройки пульта
//    QByteArray packet = createHeader(address,settings.size(),false,true,true);

//    packet.append(settings);

//    return sendPacket(packet, errors);
//}
// Отправка UDP-пакета

bool pultUdpSend::sendPacket(const QByteArray &packet, PultErrors &errors) {

    qint64 sent = m_socket->writeDatagram(packet, QHostAddress(m_ip), m_port);
    if (sent == -1) {
        errors.noReply = true;
        qDebug() << "Error: can't send the package!";
        return false;
    }
    qDebug() << "Package sent:" << packet.toHex();
    timeoutTimer->start();
    return true;
}

// Получение UDP-пакета
bool pultUdpSend::receivePacket(QByteArray &packet, PultErrors &errors) {
    if (!m_socket->waitForReadyRead(1000)) { // Ожидание 1 сек
        errors.timeout = true;
        qDebug() << "Error: timeout!";
        return false;
    }
    while (m_socket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = m_socket->receiveDatagram();
        packet = datagram.data();
    }
    if (packet.isEmpty()) {
        errors.emptyBuffer = true;
        qDebug() << "Error: received package is empty!";
        return false;
    }
    return true;
}
void pultUdpSend::listenForResponses() {
    QByteArray receiveBuffer;
    while (m_socket->hasPendingDatagrams()) {
           QNetworkDatagram datagram = m_socket->receiveDatagram();
           QByteArray data = datagram.data();
           receiveBuffer.append(data);
           while (receiveBuffer.size() >= 8) {
                  QDataStream stream(receiveBuffer);
                  stream.setByteOrder(QDataStream::LittleEndian); // Устанавливаем порядок байтов

                  uint32_t address; // Адрес
                  uint16_t size; // Размер
                  uint16_t flags; // Флаги
                  stream >> address >> size >> flags; // Читаем содержимое потока

                  int totalPacketSize = 8 + size;
                  if (receiveBuffer.size() < totalPacketSize)
                      break;
                  QByteArray fulPacket = receiveBuffer.left(totalPacketSize);
                  receiveBuffer.remove(0, totalPacketSize);
                  QByteArray payload = fulPacket.mid(8);
                  processMessagePayload(payload);
         }
     }
}

// Рассчет контрольной суммы
uint8_t pultUdpSend::calculateCRC8(const QByteArray &data) {

    static const uint8_t CRC_TABLE[256] = {
        0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15,
        0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
        0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65,
        0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
        0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5,
        0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
        0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85,
        0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
        0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,
        0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
        0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2,
        0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
        0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32,
        0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
        0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42,
        0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
        0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C,
        0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
        0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC,
        0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
        0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C,
        0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
        0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C,
        0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
        0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B,
        0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
        0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,
        0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
        0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB,
        0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
        0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB,
        0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
    };
    uint8_t crc = 0;
    for (char byte : data) {
        crc = CRC_TABLE[crc ^ static_cast<uint8_t>(byte)];
    }
    return crc;
}

// Обработка полезной нагрузки
void pultUdpSend::processMessagePayload(const QByteArray& payload) {

    static QByteArray messageBuffer;
    static int messageCount = 0;
    messageBuffer.append(payload);
    qDebug() << "Data" << messageBuffer.toHex(' ').toUpper();
    while (true) {
        int start = messageBuffer.indexOf('\xAA');
        if (start == -1) {
            messageBuffer.clear(); // нет начала пакета
            break;
        }
        if (start > 0) {
            messageBuffer.remove(0, start); // удаляем мусор перед AA
        }
        if (messageBuffer.size() < 5) break; // ждём минимум до size байтов

        quint8 size1 = static_cast<quint8>(messageBuffer[3]);
        quint8 size2 = static_cast<quint8>(messageBuffer[4]);
        if (size1 == 0x00 && size2 == 0x00) {
            qDebug() << "pendingCommand.origin" << pendingCommand.origin;
            emit lockCommand(pendingCommand.origin);
            qDebug() << "ZAPRET";
            timeoutTimer->stop(); // Останавливаем таймер при успешном получении
            messageBuffer.clear();
            pendingCommand = {}; // Сбрасываем ожидаемую команду
            return;
        }
        int payloadSize = (size1 << 8) | size2;
        qDebug() << "payloadSize" << payloadSize;
        int expectedLength = 5 + payloadSize + 1; // AA + Addr/size1/size2/KB + payload(+CRC)
        qDebug() << "expectedLength" << expectedLength;

        if (expectedLength <= 0 || expectedLength > MAX_EXPECTED_LENGTH) {
             countFaultMessages++;
             qDebug() << "INVALID EXEPCTED LENGTH:" << expectedLength << "Resetting buffer";
             messageBuffer.clear();
             break;
        }

        if (messageBuffer.size() < expectedLength) {
            // ждём ещё данных
            break;
        }

        if (expectedLength <= 0  || expectedLength > messageBuffer.size()) {
            emit lockCommand(pendingCommand.origin);
            break;
        }
        // есть полный пакет
        QByteArray fullMessage = messageBuffer.left(expectedLength);
        if(fullMessage.size() < expectedLength) {
            emit lockCommand(pendingCommand.origin);
            break;
        }
        if (expectedLength -1 >= fullMessage.size()) {
            emit lockCommand(pendingCommand.origin);
            break;
        }
        messageBuffer.remove(0, expectedLength);
        qDebug() << "received complete message #" << messageCount
                 << ":" << fullMessage.toHex(' ');

        QByteArray core = fullMessage.mid(1, expectedLength - 2); // без AA и CRC
        qDebug() << "core" <<
                 core.toHex(' ');

        quint8 receivedCRC = static_cast<quint8>(fullMessage[expectedLength- 1]);
        quint8 calculatedCRC = calculateCRC8(core);
        if (receivedCRC == calculatedCRC) {
             timeoutTimer->stop(); // Останавливаем таймер при успешном получении
            qDebug() << "OK";
            pendingCommand = {}; // Сбрасываем ожидаемую команду  // 20.06 до этого было после эмита
            emit responseReceived(fullMessage, m_lastSender);


        } else {
            qDebug() << "CRC error. Expected:" << QString::number(calculatedCRC, 16)
            << "Received:" << QString::number(receivedCRC, 16);
        }
    }
}
void pultUdpSend::onTimeout() {
    PultErrors errors;
    if (retryCount < 2) {
        retryCount++;
        qDebug() << QString("Retry to send (%1): %2")
                    .arg(retryCount)
                    .arg(QString(pendingCommand.command.toHex(' ')));

        sendPacket(pendingCommand.command, errors);
        timeoutTimer->start();
    } else {
        qDebug() << "⚠️ Error: device is not answered";
        if (pendingCommand.origin) {
            emit deliveryFailed(pendingCommand.origin);
        } else {
            qDebug() << "⚠️ Доставка не удалась, но origin уже удалён.";
        }
    }
}


