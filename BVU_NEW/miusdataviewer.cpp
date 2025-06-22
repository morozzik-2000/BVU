#include "miusdataviewer.h"
#include <QDebug>

MiusDataViewer::MiusDataViewer(QWidget *parent)
    : QTabWidget(parent) {

    this->setWindowTitle("ОПЕРАТОР->Сервисные данные МИУС");
    this->setMinimumSize(830, 400);

    unprocessedTable = new QTableWidget(this);
    hardwareTable = new QTableWidget(this);

    setupUnprocessedTable();
    setupHardwareTable();

    addTab(unprocessedTable, "Необработанные данные");
    addTab(hardwareTable, "Аппаратные данные");

    this->setStyleSheet(
        "QTabBar::tab {"
        "  font-weight: bold;"
        "  font-size: 14px;"
        "  min-width: 200px;"
        "  padding: 6px 12px;"
        "}"
        "QTabWidget::pane {"
        "  border-top: 2px solid #C2C2C2;"
        "}"
        );

    QString basePath = LoggingUtils::getLogDirectory();

    unprocessedLogFile.setFileName(basePath + "/Tablica_neobrabotannih_dannih_log.txt");
    if (unprocessedLogFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        unprocessedLogStream.setDevice(&unprocessedLogFile);
        unprocessedLogStream.setCodec("UTF-8");
        qDebug() << "Открыт файл для необработанных данных.";
        unprocessedLogStream.flush();
    } else {
        qWarning("Не удалось открыть MiusUnProcessedDataTable_log.txt");
    }

    hardwareLogFile.setFileName(basePath + "/Tablica_sostoyanya_microshem_log.txt");
    if (hardwareLogFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        hardwareLogStream.setDevice(&hardwareLogFile);
        hardwareLogStream.setCodec("UTF-8");
        qDebug() << "Открыт файл для аппаратных данных.";
        hardwareLogStream.flush();
    } else {
        qWarning("Не удалось открыть HardwareMiusData_log.txt");
    }
}

MiusDataViewer::~MiusDataViewer() {

    if (unprocessedLogFile.isOpen()) {
        unprocessedLogFile.close();
    }
    if (hardwareLogFile.isOpen()) {
        hardwareLogFile.close();
    }
}

void MiusDataViewer::setupUnprocessedTable() {

    unprocessedTable->setColumnCount(6);
    unprocessedTable->setHorizontalHeaderLabels({
        "№ рабочей\nтройки ДУС", "Опрашиваемые\nДУС\nрабочей\nтройки\n", "Ось проекции",
        "Угловая скорость\nна последнем\nинтервале\nизмерения,град/с", "Угол поворота,\nград",
        "Состояние ДУС"
    });
    unprocessedTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    unprocessedTable->setShowGrid(true);
    unprocessedTable->setGridStyle(Qt::SolidLine);

    unprocessedTable->setStyleSheet(
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
}

void MiusDataViewer::setupHardwareTable() {

    hardwareTable->setColumnCount(2);
    hardwareTable->setHorizontalHeaderLabels({"Микросхема", "Состояние"});
    hardwareTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    hardwareTable->horizontalHeader()->setFixedHeight(30);
    hardwareTable->setShowGrid(true);
    hardwareTable->setGridStyle(Qt::SolidLine);

    hardwareTable->setStyleSheet(
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
}

void MiusDataViewer::addUnProcessedData(int tripleNumber, int addr, const QString &axis,
                                        double angularVelocity, double angle, const QString &stateDUS) {

    qDebug() << "Добавление необработанных данных в таблицу и лог.";

    const int maxRows = 3;
    if (unprocessedTable->rowCount() >= maxRows) {
        unprocessedTable->removeRow(0);
    }

    int row = unprocessedTable->rowCount();
    unprocessedTable->insertRow(row);

    unprocessedTable->setItem(row, 0, new QTableWidgetItem(QString::number(tripleNumber)));
    unprocessedTable->setItem(row, 1, new QTableWidgetItem(QString("ДУС %1").arg(addr)));
    unprocessedTable->setItem(row, 2, new QTableWidgetItem(axis));
    unprocessedTable->setItem(row, 3, new QTableWidgetItem(QString::number(angularVelocity)));
    unprocessedTable->setItem(row, 4, new QTableWidgetItem(QString::number(angle)));
    unprocessedTable->setItem(row, 5, new QTableWidgetItem(stateDUS));

    static const QList<int> colWidths = {25, 10, 15, 40, 25, 20};
    static const QStringList headers = {
        "№ рабочей тройки ДУС", "Адрес RS", "Ось проекции",
        "Угловая скорость (рад/с)", "Угол поворота (рад)", "Состояние ДУС"
    };

    if (unprocessedRowGroupCounter % 3 == 0) {
        unprocessedLogStream << "\n===== Time " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << " =====\n";
        for (int i = 0; i < headers.size(); ++i)
            unprocessedLogStream << headers[i].leftJustified(colWidths[i]);
        unprocessedLogStream << "\n";
        for (int w : colWidths)
            unprocessedLogStream << QString("-").repeated(w);
        unprocessedLogStream << "\n";
    }

    QStringList rowItems = {
        QString::number(tripleNumber),
        QString::number(addr),
        axis,
        QString::number(angularVelocity),
        QString::number(angle),
        stateDUS
    };

    for (int i = 0; i < rowItems.size(); ++i)
        unprocessedLogStream << rowItems[i].leftJustified(colWidths[i]);
    unprocessedLogStream << "\n";
    unprocessedLogStream.flush();
    unprocessedRowGroupCounter++;
}

void MiusDataViewer::addHardwareData(const QString &microcircuit, const QString &microcircuitState) {

    qDebug() << "Добавление аппаратных данных в таблицу и лог.";

    int row = -1;
    for (int i = 0; i < hardwareTable->rowCount(); ++i) {
        if (hardwareTable->item(i, 0)->text() == microcircuit) {
            row = i;
            break;
        }
    }

    if (row == -1) {
        row = hardwareTable->rowCount();
        hardwareTable->insertRow(row);
    }

    hardwareTable->setItem(row, 0, new QTableWidgetItem(microcircuit));
    hardwareTable->setItem(row, 1, new QTableWidgetItem(microcircuitState));

    static const QList<int> colWidths = {20, 30};
    static const QStringList headers = {"Микросхема", "Состояние"};

    if (hardwareRowGroupCounter % 7 == 0) {
        hardwareLogStream << "\n===== Time " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << " =====\n";
        for (int i = 0; i < headers.size(); ++i)
            hardwareLogStream << headers[i].leftJustified(colWidths[i]);
        hardwareLogStream << "\n";
        for (int w : colWidths)
            hardwareLogStream << QString("-").repeated(w);
        hardwareLogStream << "\n";
    }

    QStringList rowItems = {microcircuit, microcircuitState};
    for (int i = 0; i < rowItems.size(); ++i)
        hardwareLogStream << rowItems[i].leftJustified(colWidths[i]);
    hardwareLogStream << "\n";
    hardwareLogStream.flush();

    hardwareRowGroupCounter++;
}

void MiusDataViewer::clearAllData() {

    unprocessedTable->setRowCount(0);
    hardwareTable->setRowCount(0);
}
