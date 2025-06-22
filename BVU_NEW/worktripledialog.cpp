#include "worktripledialog.h"

WorkTripleDialog::WorkTripleDialog(QWidget *parent)
    : QDialog(parent), selectedColumn(-1) {

    setWindowTitle("Выбор рабочей тройки ДУС");
    resize(800, 200);

    table = new QTableWidget(4, 20, this);

    // Устанавливаем заголовки строк
    QStringList rowHeaders = { "Номер тройки", "ДУС 1", "ДУС 2", "ДУС 3" };
    table->setVerticalHeaderLabels(rowHeaders);
    table->horizontalHeader()->setVisible(false);
    table->verticalHeader()->setStyleSheet("font-weight: bold; color: #004080"); // Цвет 1

    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectColumns);
    table->setSelectionMode(QAbstractItemView::SingleSelection);

    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    table->horizontalHeader()->setDefaultSectionSize(40);
    table->verticalHeader()->setDefaultSectionSize(30);

//    QVector<QVector<int>> triples = {
//        {1,2,3}, {1,2,4}, {1,2,5}, {1,2,6}, {1,3,4}, {1,3,5}, {1,3,6},
//        {1,4,5}, {1,4,6}, {1,5,6}, {2,3,4}, {2,3,5}, {2,3,6}, {2,4,5},
//        {2,4,6}, {2,5,6}, {3,4,5}, {3,4,6}, {3,5,6}, {4,5,6}
//    };
     QVector<QVector<int>> triples = {
            {1, 3, 5}, {1, 3, 6}, {1, 4, 5}, {1, 4, 6}, {1, 5, 3},
            {1, 5, 4}, {1, 6, 3}, {1, 6, 4}, {2, 3, 5}, {2, 3, 6},
            {2, 4, 5}, {2, 4, 6}, {2, 5, 3}, {2, 5, 4}, {2, 6, 3},
            {3, 1, 5}, {3, 1, 6}, {3, 2, 5}, {4, 1, 6}, {6, 2, 3}
        };


    for (int col = 0; col < 20; ++col) {

        QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(col + 1));
        idItem->setTextAlignment(Qt::AlignCenter);
        idItem->setBackground(QColor("#DDEEFF"));
        table->setItem(0, col, idItem);

        // DUS
        for (int row = 1; row <= 3; ++row) {
            QTableWidgetItem *dusItem = new QTableWidgetItem(QString::number(triples[col][row - 1]));
            dusItem->setTextAlignment(Qt::AlignCenter);
            dusItem->setBackground(QColor("#EEFFDD"));
            table->setItem(row, col, dusItem);
        }
    }

    connect(table, &QTableWidget::cellDoubleClicked, this, [this](int, int column) {
        selectedColumn = column;
        accept();
    });

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(table);
}

int WorkTripleDialog::selectedTriple() const {
    return selectedColumn + 1;
}
