// resultsviewer.h
#ifndef RESULTSVIEWER_H
#define RESULTSVIEWER_H

#include <QTabWidget>
#include <QTableWidget>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDateTime>
#include <QStandardPaths>
#include <QHeaderView>

class ResultsViewer : public QTabWidget
{
    Q_OBJECT
public:
    explicit ResultsViewer(QWidget *parent = nullptr);
    ~ResultsViewer();

    void addQuaternionData(int tripleNumber, int addr1, int addr2, int addr3,
                           double q0, double q1, double q2, double q3);
    void addVSKData(int tripleNumber, int addr1, int addr2, int addr3,
                    double vskX, double vskY, double vskZ, int freqKhz);
    void clearAllData();


private:

    QTableWidget *quaternionTable;
    QTableWidget *vskTable;
    QFile logFile;
    QTextStream logStream;
    QDateTime lastLogTime;

    void setupQuaternionTable();
    void setupVSKTable();
    void setupLogging();
    void logData(const QString &message);
};

#endif // RESULTSVIEWER_H
