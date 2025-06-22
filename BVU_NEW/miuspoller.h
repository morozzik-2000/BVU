#ifndef MIUSPOLLER_H
#define MIUSPOLLER_H

#pragma once

#include <QObject>
#include <QTimer>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QElapsedTimer>
#include "pultudpsend.h"
#include "resultsviewer.h"


class MiusPoller : public QObject {
    Q_OBJECT

public:
    explicit MiusPoller(pultUdpSend *m_pult, uint8_t m_interfaceId, QObject *parent);

    void start(int intervalMs, int durationMs);
    void setTripeInfo(int tripleNumberRec, int addr1Rec, int addr2Rec, int addr3Rec);
    void setFreq(int freqKhzRec);
    void setPort(int takePortFronMain);

signals:
    void sendCommand(const QByteArray &command, QObject *origin, int minExpectedLength, int maxExpectedLength);
    void log(const QString &text);
    void newQuaternionData(int tripleNumber, int addr1, int addr2, int addr3,
                           double q0, double q1, double q2, double q3);
    void newVSKData(int tripleNumber, int addr1, int addr2, int addr3,
                    double vskX, double vskY, double vskZ, int freqKhz);

private slots:
    void onTimeout();
    void stop();

    void onResponseReceived(const QByteArray &data, QObject *origin);


private:
    QTimer timer;
    int elapsedMs = 0;
    int totalDurationMs = 0;

    QFile logFile;
    QTextStream logStream;

    void writeLog(const QString &line);
    void processResponse(const QByteArray &packet);
    QByteArray buildCommand();

    int commandCount = 0;
    QElapsedTimer elapsedTimer;

    pultUdpSend *m_pult = nullptr;
    uint8_t m_interfaceId;

    int tripleNumber = 0;
    int addr1 = 0;
    int addr2 = 0;
    int addr3 = 0;
    int freqKhz = 0;

    int takePortFronMain;

    void setupLogging();
    void closeLogging();

};


#endif // MIUSPOLLER_H
