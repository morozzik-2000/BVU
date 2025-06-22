#ifndef SERIALWORKER_H
#define SERIALWORKER_H

#include <QObject>
#include <QSerialPort>
#include <QTimer>

class SerialWorker : public QObject {
    Q_OBJECT
public:
    explicit SerialWorker(QObject *parent = nullptr);
    void configure(const QString &portName, int baudRate);
    void start();
    void stop();

public slots:
    void sendCommand(const QByteArray &command, QObject *origin, int minExpectedLength, int maxExpectedLength);
    void configureAndStart(const QString &portName, int baudRate);

signals:
    void responseReceived(const QByteArray &data, QObject *origin);
    void logMessage(const QString &message);
    void deliveryFailed(QObject *origin);

private slots:
    void onReadyRead();
    void onTimeout();


private:
    QSerialPort *serial = nullptr;
    QTimer *timeoutTimer;
    QByteArray currentCommand;
    int retryCount = 0;
    QString portName;
    int baudRate;
    QByteArray buffer;

    struct PendingCommand {
        QObject* origin;
        int minExpectedLength;
        int maxExpectedLength;
        QByteArray command;
    };
    PendingCommand pendingCommand;
};

#endif // SERIALWORKER_H
