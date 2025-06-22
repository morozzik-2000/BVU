#include "serialworker.h"
#include <QThread>
#include <QDebug>

SerialWorker::SerialWorker(QObject *parent) : QObject(parent) {

}

// void SerialWorker::configure(const QString &portName, int baudRate) {
//    this->portName = portName;
//    this->baudRate = baudRate;
// }

void SerialWorker::configureAndStart(const QString &portName, int baudRate) {
    qDebug() << "[configureAndStart] current thread:" << QThread::currentThread();
    qDebug() << "[worker thread] should be:" << this->thread();
    this->portName = portName;
    this->baudRate = baudRate;
    start();  // вызывается из нужного потока
}


void SerialWorker::start() {

    serial = new QSerialPort(this);
    timeoutTimer = new QTimer(this);

    connect(serial, &QSerialPort::readyRead, this, &SerialWorker::onReadyRead);
    connect(timeoutTimer, &QTimer::timeout, this, &SerialWorker::onTimeout);
    timeoutTimer->setInterval(300);
    timeoutTimer->setSingleShot(true);

    serial->setPortName(portName);
    serial->setBaudRate(baudRate);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::EvenParity);
    serial->setStopBits(QSerialPort::TwoStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
     qDebug() << "[Serial] belongs to thread:" << serial->thread();
    if (!serial->open(QIODevice::ReadWrite)) {
        emit logMessage("Не удалось открыть порт: ");
    } else {
        emit logMessage("Порт открыт успешно");
    }
}

void SerialWorker::stop() {
    if (serial->isOpen()) serial->close();
    timeoutTimer->stop();
}

void SerialWorker::sendCommand(const QByteArray &command, QObject *origin, int minExpectedLength, int maxExpectedLength) {
    if (!serial->isOpen()) {
        emit logMessage("Порт не открыт");
        return;
    }
    pendingCommand.command = command;
    pendingCommand.origin = origin;
    pendingCommand.minExpectedLength = minExpectedLength;
    pendingCommand.maxExpectedLength = maxExpectedLength;
//    currentCommand = command;
    retryCount = 0;
    serial->write(pendingCommand.command);
    emit logMessage("Команда отправлена: " + pendingCommand.command.toHex(' '));
    timeoutTimer->start();
}

void SerialWorker::onReadyRead() {
    timeoutTimer->stop();

    // Читаем все доступные данные
    QByteArray newData = serial->readAll();
    buffer.append(newData);

    qDebug() << "Received data:" << newData.toHex(' ');
    qDebug() << "Current buffer:" << buffer.toHex(' ');

    while (buffer.size() >= std::min(pendingCommand.minExpectedLength, pendingCommand.maxExpectedLength > 0 ? pendingCommand.maxExpectedLength : INT_MAX)) {
        int startPos = buffer.indexOf(0xAA);
        if (startPos == -1) {
            buffer.clear();
            break;
        }

        if (startPos > 0) {
            buffer.remove(0, startPos);
            continue;
        }

        int len1 = pendingCommand.minExpectedLength;
        int len2 = pendingCommand.maxExpectedLength;

        // Проверка на любую из двух длин
        if (buffer.size() >= len1 && (len2 == 0 || buffer.size() < len2)) {
            QByteArray packet = buffer.left(len1);
            buffer.remove(0, len1);
            emit logMessage("Получен ответ (len1): " + packet.toHex(' '));
            emit responseReceived(packet, pendingCommand.origin);
            pendingCommand = {};
            break;
        } else if (len2 > 0 && buffer.size() >= len2) {
            QByteArray packet = buffer.left(len2);
            buffer.remove(0, len2);
            emit logMessage("Получен ответ (len2): " + packet.toHex(' '));
            if (pendingCommand.origin) {
                emit responseReceived(packet, pendingCommand.origin);
            } else {
                emit logMessage("⚠️ Ответ получен, но origin уже удалён.");
            }
            pendingCommand = {};
            break;
        } else {
            break;
        }
    }
}


void SerialWorker::onTimeout() {
    if (!serial->isOpen()) {
        emit logMessage("Ошибка: порт закрыт при повторной отправке");
        if (pendingCommand.origin) {
            emit deliveryFailed(pendingCommand.origin);
        } else {
            emit logMessage("⚠️ Доставка не удалась, но origin уже удалён.");
        }
        return;
    }
    if (retryCount < 2) {
        retryCount++;
        serial->write(pendingCommand.command);
        emit logMessage(QString("Повторная отправка (%1): %2")
                        .arg(static_cast<int>(retryCount))
                        .arg(QString(pendingCommand.command.toHex(' '))));
        timeoutTimer->start();
    } else {
        emit logMessage("Ошибка: устройство не отвечает");

        if (pendingCommand.origin) {
            emit deliveryFailed(pendingCommand.origin);
        } else {
            emit logMessage("⚠️ Доставка не удалась, но origin уже удалён.");
        }

    }
}
