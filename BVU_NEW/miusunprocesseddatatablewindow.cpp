
#include "miusunprocesseddatatablewindow.h"
#include <QHeaderView>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

MiusUnProcessedDataTableWindow::MiusUnProcessedDataTableWindow(QWidget *parent) : QWidget(parent) {
    setupTable();
    this->setWindowTitle("Необработанные данные МИУС");
    this->resize(830, 300);
}

void MiusUnProcessedDataTableWindow::setupTable() {
    table = new QTableWidget(this);
    table->setColumnCount(6);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents); // полные надписи
    table->setHorizontalHeaderLabels({
        "№ рабочей\nтройки ДУС", "Опрашиваемые\nДУС\nрабочей\nтройки\n", "Ось проекции",
        "Угловая скорость\nна последнем\nинтервале\nизмерения,град/с", "Угол поворота,\nрад",
        "Состояние ДУС"
    });
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

void MiusUnProcessedDataTableWindow::addUnProcessedMiusDataRow(int tripleNumber, int addr,
                                                            const QString &axis, double angularVelocity,
                                                            double angle, QString &stateDUS) {

    int newRow = table->rowCount();
    table->insertRow(newRow);
    table->setItem(newRow, 0, new QTableWidgetItem(QString::number(tripleNumber)));
    table->setItem(newRow, 1, new QTableWidgetItem(QString::number(addr)));
    table->setItem(newRow, 2, new QTableWidgetItem(axis));
    table->setItem(newRow, 3, new QTableWidgetItem(QString::number(angularVelocity)));
    table->setItem(newRow, 4, new QTableWidgetItem(QString::number(angle)));
    table->setItem(newRow, 5, new QTableWidgetItem(stateDUS));

    // 🔽 Логирование
    QFile file("MiusUnProcessedDataTable_log.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        QTextStream out(&file);
        out.setCodec("UTF-8");

        static const QList<int> columnWidths = {25, 10, 15, 40, 25, 20};
        static const QStringList headers = {
            "№ рабочей тройки ДУС", "Адрес RS", "Ось проекции",
            "Угловая скорость (рад/с)", "Угол поворота (рад)", "Состояние ДУС"
        };

        // Каждые 3 строки — новый заголовок
        if (unprocessedRowGroupCounter % 3 == 0) {
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
            QString::number(addr),
            axis,
            QString::number(angularVelocity),
            QString::number(angle),
            stateDUS
        };

        for (int i = 0; i < rowItems.size(); ++i)
            out << rowItems[i].leftJustified(columnWidths[i]);
        out << "\n";

        unprocessedRowGroupCounter++;

        file.close();
    } else {
        qWarning("Не удалось открыть файл для логирования строки.");
    }
}
