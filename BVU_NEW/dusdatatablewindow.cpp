// DusDataTableWindow.cpp
#include "dusdatatablewindow.h"
#include <QHeaderView>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include "loggingutils.h"
#include <QDebug>

DusDataTableWindow::DusDataTableWindow(QWidget *parent) : QWidget(parent) {
    setupTable();
    this->setWindowTitle("ОПЕРАТОР->Сервисные данные ДУС");
    this->setMinimumSize(930, 280);
}

void DusDataTableWindow::setupTable() {
    table = new QTableWidget(this);
    table->setColumnCount(7);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    table->setHorizontalHeaderLabels({
        "№ рабочей тройки ДУС", "Опрашиваемые\nДУС\nрабочей\nтройки\n", "Ось проекции",
        "Значения\nугловой\nскорости\nДУС, град/с", "Значения угла\nповорота\nДУС, град",
        "Время\nот\nвключения\nДУС, c", "Температура\nКорпуса\nДУС, °C"
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

    auto layout = new QVBoxLayout(this);
    layout->addWidget(table);
    setLayout(layout);
}


void DusDataTableWindow::addDusDataRow(int tripleNumber, int addr, const QString &axis,
                         double angularVelocity, double angle, double timeFromStart, double temperatureCase) {

    const int maxRows = 3;

    if (table->rowCount() >= maxRows) {
        table->removeRow(0);
    }

    int newRow = table->rowCount();
    table->insertRow(newRow);

    table->setItem(newRow, 0, new QTableWidgetItem(QString::number(tripleNumber)));
    table->setItem(newRow, 1, new QTableWidgetItem(QString("ДУС %1").arg(addr)));
    table->setItem(newRow, 2, new QTableWidgetItem(axis));
    table->setItem(newRow, 3, new QTableWidgetItem(QString::number(angularVelocity)));
    table->setItem(newRow, 4, new QTableWidgetItem(QString::number(angle)));
    table->setItem(newRow, 5, new QTableWidgetItem(QString::number(timeFromStart)));
    table->setItem(newRow, 6, new QTableWidgetItem(QString::number(temperatureCase)));
    QString logFilePath = LoggingUtils::getLogDirectory() + "/Tablica_dus_dannih_log.txt";
    QFile file(logFilePath);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        QTextStream out(&file);
        out.setCodec("UTF-8");

        static const QList<int> columnWidths = {25, 10, 15, 35, 30, 25, 25};
        static const QStringList headers = {
            "№ рабочей тройки ДУС", "Адрес RS", "Ось проекции",
            "Проекция угловой скорости (рад/с)", "Значения угла поворота ДЦС (рад/с)",
            "Время от включения ДУС", "Температура Корпуса ДУС"
        };

        // Каждые 3 строки — новый заголовок с таймштампом
        if (dusRowGroupCounter % 3 == 0) {
            out << "\n===== Time " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
            << " =====\n";
            for (int i = 0; i < headers.size(); ++i)
                out << headers[i].leftJustified(columnWidths[i]);
            out << "\n";
            for (int w : columnWidths)
                out << QString("-").repeated(w);
            out << "\n";
        }
        QStringList rowItems = {
            QString::number(tripleNumber),
            QString::number(addr),
            axis,
            QString::number(angularVelocity),
            QString::number(angle),
            QString::number(timeFromStart),
            QString::number(temperatureCase)
        };
        for (int i = 0; i < rowItems.size(); ++i)
            out << rowItems[i].leftJustified(columnWidths[i]);
        out << "\n";

        dusRowGroupCounter++;
        file.close();
    } else {
        qWarning() << "Не удалось открыть файл лога:" << logFilePath;
    }
}
