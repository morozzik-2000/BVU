#ifndef WORKTRIPLEDIALOG_H
#define WORKTRIPLEDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QColor>

class WorkTripleDialog : public QDialog {
    Q_OBJECT

public:
    explicit WorkTripleDialog(QWidget *parent = nullptr);
    int selectedTriple() const;

private:
    QTableWidget *table;
    int selectedColumn = -1;
};

#endif // WORKTRIPLEDIALOG_H
