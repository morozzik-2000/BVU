#ifndef SERIALPORTMANAGER_H
#define SERIALPORTMANAGER_H

#include <QObject>
#include <QThread>
#include "SerialWorker.h"

class SerialPortManager : public QObject {
    Q_OBJECT
public:
    static SerialPortManager* instance();
    void init(const QString &port, int baudrate);
    void shutdown();
    void sendCommand(const QByteArray &command, QObject *origin, int minExpectedLength, int maxExpectedLength);

signals:
    void responseReceived(const QByteArray &response, QObject *origin);
    void logMessage(const QString &message);
    void deliveryFailed(QObject *origin);

private:
    explicit SerialPortManager(QObject *parent = nullptr);
    ~SerialPortManager();

    static SerialPortManager *s_instance;
    QThread workerThread;
    SerialWorker *worker = nullptr;
};

#endif // SERIALPORTMANAGER_H
