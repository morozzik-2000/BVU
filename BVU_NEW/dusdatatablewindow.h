#ifndef DUSDATATABLEWINDOW_H
#define DUSDATATABLEWINDOW_H

// DusDataTableWindow.h
#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>

class DusDataTableWindow : public QWidget {
    Q_OBJECT

public:
    explicit DusDataTableWindow(QWidget *parent = nullptr);

    void addDusDataRow(int tripleNumber, int addr, const QString &axis, double angularVelocity, double angle, double timeFromStart, double temperatureCase);
    int dusRowGroupCounter = 0;
private:
    QTableWidget *table;
    void setupTable();
    QMap<int, int> tripleRowMap; // tripleNumber → row index
};

#endif // DUSDATATABLEWINDOW_H
