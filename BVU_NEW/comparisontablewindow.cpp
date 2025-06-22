#include "comparisontablewindow.h"
#include <QHeaderView>
#include <QTableWidgetItem>

ComparisonTableWindow::ComparisonTableWindow(QWidget *parent) : QWidget(parent) {
    setupTable();
    this->setWindowTitle("Таблица сравнения");
    this->resize(1400, 400);  // увеличил ширину для новых столбцов
}

void ComparisonTableWindow::setupTable() {
    table = new QTableWidget(this);

    // Теперь 14 столбцов: устройство, источник данных, и по 4 столбца на каждый ДУС (угол, время, темп, частота)
    table->setColumnCount(14);

    table->setHorizontalHeaderLabels({
        "Устройство",
        "Источник данных",

        "ДУС 1\nУгловая\nскорость (град/с)",
        "ДУС 1\nУгол (град)",
        "ДУС 1\nВремя. (°C)",
        "ДУС 1\nЧастота (Гц)",

        "ДУС 2\nУгловая\nскорость (град/с)",
        "ДУС 2\nУгол (град)",
        "ДУС 2\nВремя. (°C)",
        "ДУС 2\nЧастота (Гц)",

        "ДУС 3\nУгловая\nскорость (град/с)",
        "ДУС 3\nУгол (град)",
        "ДУС 3\nВремя. (°C)",
        "ДУС 3\nЧастота (Гц)"
    });

    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(table);
    setLayout(layout);
}

void ComparisonTableWindow::updateDataTable(int tripleNumber, const QString &deviceLabel,
                                            qint32 oxAngle, qint32 oxTime, qint32 oxTemp, qint32 oxFreq,
                                            qint32 oyAngle, qint32 oyTime, qint32 oyTemp, qint32 oyFreq,
                                            qint32 ozAngle, qint32 ozTime, qint32 ozTemp, qint32 ozFreq)
{
    if (!table) return;

    int rowIndex;
    if (tripleRowMap.contains(tripleNumber)) {
        rowIndex = tripleRowMap[tripleNumber];
    } else {
        rowIndex = table->rowCount();
        table->insertRow(rowIndex);
        tripleRowMap[tripleNumber] = rowIndex;
    }

    rowIndex = table->rowCount();
            table->insertRow(rowIndex);
            tripleRowMap[tripleNumber] = rowIndex;

    table->setItem(rowIndex, 0, new QTableWidgetItem(deviceLabel));
    table->setItem(rowIndex, 1, new QTableWidgetItem(QString("Тройка %1").arg(tripleNumber)));

    table->setItem(rowIndex, 2, new QTableWidgetItem(QString::number(oxAngle)));
    table->setItem(rowIndex, 3, new QTableWidgetItem(QString::number(oxTime)));
    table->setItem(rowIndex, 4, new QTableWidgetItem(QString::number(oxTemp)));
    table->setItem(rowIndex, 5, new QTableWidgetItem(QString::number(oxFreq)));

    table->setItem(rowIndex, 6, new QTableWidgetItem(QString::number(oyAngle)));
    table->setItem(rowIndex, 7, new QTableWidgetItem(QString::number(oyTime)));
    table->setItem(rowIndex, 8, new QTableWidgetItem(QString::number(oyTemp)));
    table->setItem(rowIndex, 9, new QTableWidgetItem(QString::number(oyFreq)));

    table->setItem(rowIndex, 10, new QTableWidgetItem(QString::number(ozAngle)));
    table->setItem(rowIndex, 11, new QTableWidgetItem(QString::number(ozTime)));
    table->setItem(rowIndex, 12, new QTableWidgetItem(QString::number(ozTemp)));
    table->setItem(rowIndex, 13, new QTableWidgetItem(QString::number(ozFreq)));




}
