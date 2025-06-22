#include "hardwaremiusdatatabewindow.h"

#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QHeaderView>

HardwareMiusDataTabeWindow::HardwareMiusDataTabeWindow(QWidget *parent) : QWidget(parent) {
    setupTable();
    this->setWindowTitle("Аппаратные данные модуля МИУС");
    this->resize(250, 220);
}

void HardwareMiusDataTabeWindow::setupTable() {
    table = new QTableWidget(this);
    table->setColumnCount(2);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents); // полные надписи
    table->setHorizontalHeaderLabels({
        "Микросхема", "Состояние"
    });
    // Оформление заголовка
    table->setStyleSheet(
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

    table->setShowGrid(true);
    table->setGridStyle(Qt::SolidLine);

    table->horizontalHeader()->setFixedHeight(30);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(table);
    setLayout(layout);
}

void HardwareMiusDataTabeWindow::addHardwareMiusDataRow(QString microcircuit, QString microcircuitState) {
    int newRow = table->rowCount();
    table->insertRow(newRow);
    table->setItem(newRow, 0, new QTableWidgetItem(microcircuit));
    table->setItem(newRow, 1, new QTableWidgetItem(microcircuitState));
    // 🔽 Логирование в файл с группировкой по 4 строки
    QFile file("HardwareMiusData_log.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        QTextStream out(&file);
        out.setCodec("UTF-8");

        static const QList<int> columnWidths = {20, 30};
        static const QStringList headers = {"Микросхема", "Состояние"};

        // Каждые 4 строки — новый заголовок с таймштампом
        if (hardwareRowGroupCounter % 4 == 0) {
            out << "\n===== Time " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << " =====\n";
            for (int i = 0; i < headers.size(); ++i)
                out << headers[i].leftJustified(columnWidths[i]);
            out << "\n";
            for (int w : columnWidths)
                out << QString("-").repeated(w);
            out << "\n";
        }

        QStringList rowItems = {microcircuit, microcircuitState};
        for (int i = 0; i < rowItems.size(); ++i)
            out << rowItems[i].leftJustified(columnWidths[i]);
        out << "\n";

        hardwareRowGroupCounter++;

        file.close();
    } else {
        qWarning("Не удалось открыть файл HardwareMiusData_log.txt для логирования.");
    }
}

