#include "miuspoller.h"
#include <QDebug>
#include <QThread>

MiusPoller::MiusPoller(pultUdpSend *pult, uint8_t interfaceId, QObject *parent)
    : QObject(parent), m_pult(pult), m_interfaceId(interfaceId) {

    connect(&timer, &QTimer::timeout, this, &MiusPoller::onTimeout);
    logFile.setFileName("mius_log.txt");
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        logStream.setDevice(&logFile);
    }
    qDebug() << "[MiusPoller] thread info:"
             << "object thread =" << this->thread()
             << ", current thread =" << QThread::currentThread();

    connect(m_pult, &pultUdpSend::responseReceived, this, &MiusPoller::onResponseReceived);
    qDebug() << "port" << m_interfaceId;
}

void MiusPoller::setTripeInfo(int tripleNumberRec, int addr1Rec, int addr2Rec, int addr3Rec) {

    tripleNumber = tripleNumberRec;
    addr1 = addr1Rec;
    addr2 = addr2Rec;
    addr3 = addr3Rec;
    qDebug() << "tripleNumber =" << tripleNumber << "addr1 =" << addr1 << "addr2 =" << addr2 << "addr3 =" << addr3;
}

void MiusPoller::setFreq(int freqKhzRec) {

    freqKhz = freqKhzRec;
}
void MiusPoller::setPort(int takePortFronMain) { // 20.06
    this->takePortFronMain = takePortFronMain;  // 20.06
}
void MiusPoller::start(int intervalMs, int durationMs) {

    elapsedMs = 0;
    totalDurationMs = durationMs;
    commandCount = 0; // команды
    elapsedTimer.start();  // старт измерения времени
    timer.start(intervalMs);
    emit log("⏱ Опрос МиУСа начат");
}

void MiusPoller::stop() {

    timer.stop();
    qint64 actualElapsedMs = elapsedTimer.elapsed();
    double freqHz = commandCount / (actualElapsedMs / 1000.0);  // команды в секунду
    QString summary = QString("✅ The poll is end. Amount of commands: %1, Time: %2 мс, Frequency: %3 Hz")
                          .arg(commandCount)
                          .arg(actualElapsedMs)
                          .arg(freqHz, 0, 'f', 2);
    writeLog(summary);
    emit log("🛑 Опрос МиУСа остановлен");
    emit log(summary);

}
void MiusPoller::onTimeout() {

   if (elapsedTimer.elapsed() >= totalDurationMs) {
       stop();
       return;
   }
   commandCount++;
   PultErrors errors;
   QByteArray cmd = buildCommand();
   m_interfaceId = takePortFronMain;
   qDebug() << "m_interfaceId" << m_interfaceId;
   if (!m_pult->sendData(m_interfaceId, cmd, this, errors)) {
       emit log("Ошибка");
   }
}
//void MiusPoller::onTimeout() {

//   elapsedMs += timer.interval();
//   if (elapsedMs >= totalDurationMs) {
//       stop();
//       return;
//   }
//   commandCount++;
//   PultErrors errors;
//   QByteArray cmd = buildCommand();
//   if (!m_pult->sendData(m_interfaceId, cmd, this, errors)) {
//       emit log("ОШИБКА");
//   }
//}

QByteArray MiusPoller::buildCommand() {

    QByteArray cmd = QByteArray::fromHex("AAC41600011C82");
    return cmd;
}

void MiusPoller::onResponseReceived(const QByteArray &data, QObject *origin) {

     if (origin == this) {
          processResponse(data);
     }

}

void MiusPoller::processResponse(const QByteArray &packet) {

    const QByteArray payload = packet.mid(6, 32); // вся полезная нагрузка
    auto bytesToSignedLong = [](const QByteArray &arr, int offset) -> qint32 {
        return static_cast<qint32>((arr[offset] << 24) |
                                   ((uint8_t)arr[offset + 1] << 16) |
                                   ((uint8_t)arr[offset + 2] << 8) |
                                   ((uint8_t)arr[offset + 3]));
    };

    if (payload.size() >= 28) {
        qint32 q0 = bytesToSignedLong(payload, 0);
        qint32 q1 = bytesToSignedLong(payload, 4);
        qint32 q2 = bytesToSignedLong(payload, 8);
        qint32 q3 = bytesToSignedLong(payload, 12);
        qint32 wx = bytesToSignedLong(payload, 16);
        qint32 wy = bytesToSignedLong(payload, 20);
        qint32 wz = bytesToSignedLong(payload, 24);

        emit newQuaternionData(tripleNumber, addr1, addr2, addr3, q0, q1, q2, q3);
        emit newVSKData(tripleNumber, addr1, addr2, addr3, wx, wy, wz, freqKhz);
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");

        QString logEntry = QString("[%1] q0=%2 q1=%3 q2=%4 q3=%5 wx=%6 wy=%7 wz=%8")
                               .arg(timestamp).arg(q0).arg(q1).arg(q2).arg(q3)
                               .arg(wx).arg(wy).arg(wz);

//        emit log(logEntry);
        writeLog(logEntry);
    }
}

void MiusPoller::writeLog(const QString &line) {

    if (logFile.isOpen()) {
        logStream << line << "\n";
        logStream.flush();
    }

}
