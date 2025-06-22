#ifndef HARDWAREMIUSDATATABLEWINDOW_H
#define HARDWAREMIUSDATATABLEWINDOW_H

#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>

class HardwareMiusDataTabeWindow : public QWidget {
    Q_OBJECT

public:
    explicit HardwareMiusDataTabeWindow(QWidget *parent = nullptr);

    void addHardwareMiusDataRow(QString microcircuit, QString microcircuitState);
     int hardwareRowGroupCounter = 0;  // счетчик строк для логирования
private:
    QTableWidget *table;
    void setupTable();
};

#endif // HARDWAREMIUSDATATABLEWINDOW_H
