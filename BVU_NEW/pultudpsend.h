#ifndef PULTUDPSEND_H
#define PULTUDPSEND_H
#include <QObject>
#include <QUdpSocket>
#include <QByteArray>
#include <QDataStream>
#include <QtEndian>
#include <QThread>
#include <QTimer>
#include <QEventLoop>
#include <QQueue>

//Базовые адреса устройств
constexpr uint32_t BASE_UART_ADDRESS = 0x00040000;// Базовый адрес UART
constexpr uint32_t BASE_RK_ADDRESS = 0x30020000;// Базовый адрес РК ПКУ
constexpr uint32_t BASE_BAUDRATE_ADDRESS = 0x00040100;// Базовый адрес скорости обмена
constexpr uint32_t BASE_PARITY_ADDRESS = 0x00040120;// Базовый адрес четности
constexpr uint32_t BASE_STOPBITS_ADDRESS = 0x00040130;// Базовый адрес стоп-битов
// Перечень ошибок работы с пультом
struct PultErrors {
    bool wrongBridgeId = false;
    bool noReply = false;
    bool wrongInterface = false;
    bool timeout = false;
    bool emptyBuffer = false;
    bool wrongParam = false;
    // Метод для получения строкового представления ошибок
    QString toString() const {
        QStringList errorList;
        if (wrongBridgeId) errorList.append("Wrong bridge ID.");
        if (noReply) errorList.append("No reply.");
        if (wrongInterface) errorList.append("Wrong interface.");
        if (timeout) errorList.append("Timeout.");
        if (emptyBuffer) errorList.append("Buffer is empty.");
        if (wrongParam) errorList.append("Wrong parameters.");

        return errorList.isEmpty() ? "No errors." : errorList.join(" ");
    }
};

class pultUdpSend: public QObject {
    Q_OBJECT
signals:
    void responseReceived(const QByteArray &data, QObject *origin);
    void deliveryFailed(QObject *origin);
    void lockCommand(QObject *origin);
public:
    explicit pultUdpSend(const QString &ip, quint16 port, uint32_t b_address, QObject *parent = nullptr);

    // Отправка данных
    bool sendData(uint8_t interfaceId, const QByteArray &data,  QObject *origin, PultErrors &errors);

    // Прием данных
    bool receiveData(uint8_t interfaceId, QByteArray &data, PultErrors &errors);

    // Чтение и запись настроек
    bool readSettings(QByteArray &settings, PultErrors &errors);
//    bool writeSettings(const QByteArray &settings, PultErrors &errors);
    uint8_t calculateCRC8(const QByteArray &data);

    //функция слушает сеть
    void listenForResponses();
    void processMessagePayload(const QByteArray& payload);


private:
    QString m_ip;
    quint16 m_port;
    QUdpSocket *m_socket;
    uint32_t base_address;//Базовый адрес устройства выдачи
    QObject *m_lastSender = nullptr; // Адресат сообщений(из какого окна отправляли)

    bool isValidPacket(const QByteArray& packet, int expectedLength);
    bool sendPacket(const QByteArray &packet, PultErrors &errors);
    bool receivePacket(QByteArray &packet, PultErrors &errors);
    QByteArray createHeader(uint32_t address, uint16_t size,bool isReceipt, bool isWrite, bool isRequest);

    // Удалить это в случае чего
    bool m_responseReceived = false;
    QTimer* timeoutTimer;
    int retryCount = 0;
    struct PendingCommand {
        QByteArray command;
        QObject* origin = nullptr;
    };
    PendingCommand pendingCommand;
    void onTimeout();

    QQueue<QByteArray> m_messageQueue;
    QByteArray m_messageBuffer;
    const int MAX_QUEUE_SIZE = 1000000; // Максимальный размер очереди
    int countFaultMessages = 0; // Количество сообщений с неправильным размером
};


#endif // PULTUDPSEND_H




