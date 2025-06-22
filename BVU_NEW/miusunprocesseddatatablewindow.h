#ifndef MIUSUNPROCESSEDDATATABLEWINDOW_H
#define MIUSUNPROCESSEDDATATABLEWINDOW_H

// MiusUnProcessedDataTableWindow.h
#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>

class MiusUnProcessedDataTableWindow : public QWidget {
    Q_OBJECT

public:
    explicit MiusUnProcessedDataTableWindow(QWidget *parent = nullptr);

    void addUnProcessedMiusDataRow(int tripleNumber, int addr, const QString &axis,
                                 double angularVelocity, double angle,QString &stateDUS);
    int unprocessedRowGroupCounter = 0;

private:
    QTableWidget *table;
    void setupTable();
};


#endif // MIUSUNPROCESSEDDATATABLEWINDOW_H
