#include "freqtabledialog.h"

FreqTableDialog::FreqTableDialog(QWidget *parent)
    : QDialog(parent) {

    setWindowTitle("Введите частоту вручную");

    table = new QTableWidget(1, 1, this);
    table->setHorizontalHeaderLabels(QStringList() << "Частота (Гц)");
    table->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    table->setItem(0, 0, new QTableWidgetItem("1.0"));

    auto okButton = new QPushButton("ОК", this);
    connect(okButton, &QPushButton::clicked, this, &FreqTableDialog::accept);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(table);
    layout->addWidget(okButton);
    setLayout(layout);
}

quint16 FreqTableDialog::getFreqWord() const {

    QString text = table->item(0, 0)->text();
    double freq = text.toDouble();
    if (freq <= 0) return 0;
    quint16 freqVal = static_cast<quint16>(qRound(freq));
    return 0x4000 | (freqVal & 0x1FFF);
}
