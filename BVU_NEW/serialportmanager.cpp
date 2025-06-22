#include "serialportmanager.h"
#include <QDebug>
SerialPortManager* SerialPortManager::s_instance = nullptr;

SerialPortManager* SerialPortManager::instance() {
    if (!s_instance) {
        s_instance = new SerialPortManager();
    }
    return s_instance;
}

SerialPortManager::SerialPortManager(QObject *parent) : QObject(parent) {
    qDebug() << "[SerialPortManager] current thread:" << QThread::currentThread();
    worker = new SerialWorker();
    qDebug() << "[SerialPortManager BEFORE moveToTread]  thread:" << QThread::currentThread();
    worker->moveToThread(&workerThread);
    qDebug() << "[SerialPortManager After moveToThread]  thread:" << QThread::currentThread();
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &SerialPortManager::destroyed, &workerThread, &QThread::quit);

    connect(worker, &SerialWorker::responseReceived, this, &SerialPortManager::responseReceived);
    connect(worker, &SerialWorker::logMessage, this, &SerialPortManager::logMessage);
    connect(worker, &SerialWorker::deliveryFailed, this, &SerialPortManager::deliveryFailed);

    workerThread.start();
}

SerialPortManager::~SerialPortManager() {
    shutdown();
    workerThread.quit();
    workerThread.wait();
    s_instance = nullptr;
}

void SerialPortManager::init(const QString &port, int baudrate) {
    QMetaObject::invokeMethod(worker, "configureAndStart", Qt::QueuedConnection,
                              Q_ARG(QString, port),
                              Q_ARG(int, baudrate));
}




void SerialPortManager::shutdown() {
    QMetaObject::invokeMethod(worker, [=]() {
        worker->stop();
    }, Qt::QueuedConnection);
}

void SerialPortManager::sendCommand(const QByteArray &command, QObject *origin, int minExpectedLength, int maxExpectedLength) {
    QMetaObject::invokeMethod(worker, "sendCommand", Qt::QueuedConnection,
                            Q_ARG(QByteArray, command),
                            Q_ARG(QObject*, origin),
                            Q_ARG(int, minExpectedLength),
                            Q_ARG(int, maxExpectedLength));
}
