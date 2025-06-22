
#pragma once

#include <QTabWidget>
#include <QTableWidget>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include "loggingutils.h"
#include <QHeaderView>
#include <QDebug>
#include <QTextCodec>
class MiusDataViewer : public QTabWidget
{
    Q_OBJECT

public:
    explicit MiusDataViewer(QWidget *parent = nullptr);
    ~MiusDataViewer();

    void addUnProcessedData(int tripleNumber, int addr, const QString &axis,
                            double angularVelocity, double angle, const QString &stateDUS);
    void addHardwareData(const QString &microcircuit, const QString &microcircuitState);
    void clearAllData();


private:
    void setupUnprocessedTable();
    void setupHardwareTable();
    void logCombinedData();

    QTableWidget *unprocessedTable;
    QTableWidget *hardwareTable;

    QDateTime lastLogTime;
    QList<QStringList> pendingUnprocessedRows;
    QList<QStringList> pendingHardwareRows;

    QFile unprocessedLogFile;
    QTextStream unprocessedLogStream;
    int unprocessedRowGroupCounter = 0;

    QFile hardwareLogFile;
    QTextStream hardwareLogStream;
    int hardwareRowGroupCounter = 0;


};
