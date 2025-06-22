#ifndef FREQTABLEDIALOG_H
#define FREQTABLEDIALOG_H


#pragma once

#include <QDialog>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>

class FreqTableDialog : public QDialog {
    Q_OBJECT

public:
    explicit FreqTableDialog(QWidget *parent = nullptr);
    quint16 getFreqWord() const;

private:
    QTableWidget *table;
};


#endif // FREQTABLEDIALOG_H
