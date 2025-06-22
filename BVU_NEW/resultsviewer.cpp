// resultsviewer.cpp
#include "resultsviewer.h"
#include <QTabWidget>
#include <QTableWidget>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDateTime>
#include <QStandardPaths>
#include <QHeaderView>
#include <QDebug>
#include <QFont>
#include "loggingutils.h"

ResultsViewer::ResultsViewer(QWidget *parent) : QTabWidget(parent) {

    this->setWindowTitle("ОПЕРАТОР->Обработанные данные МИУС");
    this->setMinimumSize(1030, 180);



    quaternionTable = new QTableWidget(this);
    vskTable = new QTableWidget(this);
    quaternionTable->setMinimumSize(1030, 180);
    vskTable->setMinimumSize(1030, 180);
    setupQuaternionTable();
    setupVSKTable();
    addTab(quaternionTable, "Кватернионы");
    addTab(vskTable, "ВСК");
    this->setStyleSheet(
        "QTabBar::tab {"
        "  font-weight: bold;"
        "  font-size: 14px;"
        "  min-width: 150px;"
        "  padding: 6px 12px;"
        "}"
        "QTabWidget::pane {"
        "  border-top: 2px solid #C2C2C2;"
        "}"
        );
}

void ResultsViewer::setupQuaternionTable() {

    quaternionTable->setColumnCount(8);
    quaternionTable->setHorizontalHeaderLabels({
        "№ тройки ДУС", "ДУС №1", "ДУС №2", "ДУС №3", "Кватернион q0", "Кватернион q1", "Кватернион q2", "Кватернион q3"
    });
    quaternionTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    quaternionTable->setStyleSheet(
        "QHeaderView::section {"
        "  background-color: #D3D3D3;"
        "  color: black;"
        "  padding: 4px;"
        "  border-bottom: 2px solid black;"
        "  font-weight: bold;"
        "}"
        "QTableWidget {"
        "  gridline-color: #BBBBBB;"
        "}"
        );

    quaternionTable->setShowGrid(true);
    quaternionTable->setGridStyle(Qt::SolidLine);

}

void ResultsViewer::setupVSKTable() {

    vskTable->setColumnCount(6);
    vskTable->setHorizontalHeaderLabels({
        "№ тройки ДУС", "ДУС №", "Проекция угловой скорости\nна ось OX, рад/с",
                                 "Проекция угловой скорости\nна ось OY, рад/с",
                                 "Проекция угловой скорости\nна ось OZ, рад/с",
                                            "Частота опроса\nМИУС, Гц"
    });
    vskTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    vskTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    vskTable->setStyleSheet(
        "QHeaderView::section {"
        "  background-color: #D3D3D3;"
        "  color: black;"
        "  padding: 4px;"
        "  border-bottom: 2px solid black;"
        "  font-weight: bold;"
        "}"
        "QTableWidget {"
        "  gridline-color: #BBBBBB;"
        "}"
        );

    vskTable->setShowGrid(true);
    vskTable->setGridStyle(Qt::SolidLine);

}

void ResultsViewer::addQuaternionData(int tripleNumber, int addr1, int addr2, int addr3,
                                      double q0, double q1, double q2, double q3) {


    quaternionTable->setRowCount(0);

    int row = -1;
    for (int i = 0; i < quaternionTable->rowCount(); ++i) {
        if (quaternionTable->item(i,0)->text().toInt() == tripleNumber) {
            row = i;
            break;
        }
    }
    if (row == -1) {
        row = quaternionTable->rowCount();
        quaternionTable->insertRow(row);
    }

    quaternionTable->setItem(row, 0, new QTableWidgetItem(QString::number(tripleNumber)));
    quaternionTable->setItem(row, 1, new QTableWidgetItem(QString::number(addr1)));
    quaternionTable->setItem(row, 2, new QTableWidgetItem(QString::number(addr2)));
    quaternionTable->setItem(row, 3, new QTableWidgetItem(QString::number(addr3)));
    quaternionTable->setItem(row, 4, new QTableWidgetItem(QString::number(q0)));
    quaternionTable->setItem(row, 5, new QTableWidgetItem(QString::number(q1)));
    quaternionTable->setItem(row, 6, new QTableWidgetItem(QString::number(q2)));
    quaternionTable->setItem(row, 7, new QTableWidgetItem(QString::number(q3)));

    // logData(QString("Тройка %1: ДУС %2, %3, %4: q0=%5, q1=%6, q2=%7, q3=%8")
    //             .arg(tripleNumber)
    //             .arg(addr1).arg(addr2).arg(addr3)
    //             .arg(q0)
    //             .arg(q1)
    //             .arg(q2)
    //             .arg(q3));
}

void ResultsViewer::addVSKData(int tripleNumber, int addr1, int addr2, int addr3,
                               double vskX, double vskY, double vskZ, int freqKhz) {

    vskTable->setRowCount(0);

    int row = -1;
    for (int i = 0; i < vskTable->rowCount(); ++i) {
        if (vskTable->item(i,0)->text().toInt() == tripleNumber) {
            row = i;
            break;
        }
    }
    if (row == -1) {
        row = vskTable->rowCount();
        vskTable->insertRow(row);
    }

    vskTable->setItem(row, 0, new QTableWidgetItem(QString::number(tripleNumber)));
    vskTable->setItem(row, 1, new QTableWidgetItem(QString("%1, %2, %3").arg(addr1).arg(addr2).arg(addr3)));
    vskTable->setItem(row, 2, new QTableWidgetItem(QString::number(vskX)));
    vskTable->setItem(row, 3, new QTableWidgetItem(QString::number(vskY)));
    vskTable->setItem(row, 4, new QTableWidgetItem(QString::number(vskZ)));
    vskTable->setItem(row, 5, new QTableWidgetItem(QString::number(freqKhz)));

    // logData(QString("Тройка %1: ДУС %2, %3, %4: wx=%5, wy=%6, wz=%7, f=%7")
    //             .arg(tripleNumber)
    //             .arg(addr1).arg(addr2).arg(addr3)
    //             .arg(vskX)
    //             .arg(vskY)
    //             .arg(vskZ));
}

void ResultsViewer::clearAllData() {

    quaternionTable->setRowCount(0);
    vskTable->setRowCount(0);
}
// void ResultsViewer::setupLogging() {

//     QString logFilePath = LoggingUtils::createLogFilePath("Tablica_obrabotannih_dannih_Log");

//     logFile.setFileName(logFilePath);
//     if (logFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
//         logStream.setDevice(&logFile);
//         logStream.setCodec("UTF-8");
//         logStream << "=== Start log  ResultViewer===" << Qt::endl;
//         logStream << "Format of log:" << Qt::endl;
//         logStream << "[date-time] Type of write: data" << Qt::endl;
//         logStream << "================================" << Qt::endl << Qt::endl;
//     } else {
//         qWarning() << "ERROR to open log file!:" << logFilePath;
//     }
// }

// void ResultsViewer::logData(const QString &message) {

//     if (!logFile.isOpen()) return;

//     QDateTime currentTime = QDateTime::currentDateTime();

//     if (lastLogTime.isValid() && lastLogTime.msecsTo(currentTime) > 1000) {
//         logStream << Qt::endl;
//     }

//     logStream << "[" << currentTime.toString("yyyy-MM-dd HH:mm:ss.zzz") << "] "
//               << message << Qt::endl;

//     lastLogTime = currentTime;
// }

ResultsViewer::~ResultsViewer() {

    // if (logFile.isOpen()) {
    //     logStream << "\n=== Логирование завершено ===" << Qt::endl;
    //     logFile.close();
    // }
}
