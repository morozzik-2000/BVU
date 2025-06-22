// QuaternionTableWindow.h
#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>

class QuaternionTableWindow : public QWidget {
    Q_OBJECT

public:
    explicit QuaternionTableWindow(QWidget *parent = nullptr);
    void addQuaternionRow(int tripleNumber, int addr1, int addr2, int addr3, qint32 q0, qint32 q1, qint32 q2, qint32 q3);
    int quaternionRowGroupCounter = 0;

private:
    QTableWidget *table;
    void setupTable();
};
