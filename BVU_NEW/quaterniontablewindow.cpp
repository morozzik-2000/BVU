// QuaternionTableWindow.cpp
#include "QuaternionTableWindow.h"
#include <QHeaderView>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

QuaternionTableWindow::QuaternionTableWindow(QWidget *parent) : QWidget(parent) {
    setupTable();
    this->setWindowTitle("Кватернионы МИУС");
    this->resize(800, 300);
}

void QuaternionTableWindow::setupTable() {
    table = new QTableWidget(this);
    table->setColumnCount(8);
    table->setHorizontalHeaderLabels({
        "№ тройки ДУС", "Адрес1", "Адрес2", "Адрес3", "λ0", "λ1", "λ2", "λ3"
    });
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
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

    table->horizontalHeader()->setFixedHeight(30);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(table);
    setLayout(layout);
}

void QuaternionTableWindow::addQuaternionRow(int tripleNumber, int addr1, int addr2, int addr3,
                                             qint32 q0, qint32 q1, qint32 q2, qint32 q3) {
    int row = table->rowCount();
    table->insertRow(row);
    table->setItem(row, 0, new QTableWidgetItem(QString::number(tripleNumber)));
    table->setItem(row, 1, new QTableWidgetItem(QString::number(addr1)));
    table->setItem(row, 2, new QTableWidgetItem(QString::number(addr2)));
    table->setItem(row, 3, new QTableWidgetItem(QString::number(addr3)));
    table->setItem(row, 4, new QTableWidgetItem(QString::number(q0)));
    table->setItem(row, 5, new QTableWidgetItem(QString::number(q1)));
    table->setItem(row, 6, new QTableWidgetItem(QString::number(q2)));
    table->setItem(row, 7, new QTableWidgetItem(QString::number(q3)));

    // 🔽 Логирование в файл
    QFile file("QuaternionTable_log.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        QTextStream out(&file);
        out.setCodec("UTF-8");

        static const QList<int> columnWidths = {18, 10, 10, 10, 12, 12, 12, 12};
        static const QStringList headers = {
            "№ тройки ДУС", "Адрес1", "Адрес2", "Адрес3", "λ0", "λ1", "λ2", "λ3"
        };

        // Каждые 3 строки — новый заголовок с таймштампом
        if (quaternionRowGroupCounter % 3 == 0) {
            out << "\n===== Time " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << " =====\n";
            for (int i = 0; i < headers.size(); ++i)
                out << headers[i].leftJustified(columnWidths[i]);
            out << "\n";
            for (int w : columnWidths)
                out << QString("-").repeated(w);
            out << "\n";
        }

        QStringList rowItems = {
            QString::number(tripleNumber),
            QString::number(addr1),
            QString::number(addr2),
            QString::number(addr3),
            QString::number(q0),
            QString::number(q1),
            QString::number(q2),
            QString::number(q3)
        };

        for (int i = 0; i < rowItems.size(); ++i)
            out << rowItems[i].leftJustified(columnWidths[i]);
        out << "\n";

        quaternionRowGroupCounter++;

        file.close();
    } else {
        qWarning("Не удалось открыть файл quaternion_log.txt для логирования.");
    }
}
