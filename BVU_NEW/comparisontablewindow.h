#ifndef COMPARISONTABLEWINDOW_H
#define COMPARISONTABLEWINDOW_H

#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QMap>

class ComparisonTableWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ComparisonTableWindow(QWidget *parent = nullptr);
    void setupTable();

    // Добавляем параметры для частоты частоты получения данных (Гц)
    void updateDataTable(int tripleNumber, const QString &deviceLabel,
                         qint32 oxAngle, qint32 oxTime, qint32 oxTemp, qint32 oxFreq,
                         qint32 oyAngle, qint32 oyTime, qint32 oyTemp, qint32 oyFreq,
                         qint32 ozAngle, qint32 ozTime, qint32 ozTemp, qint32 ozFreq);

private:
    QTableWidget *table = nullptr;
    QMap<int, int> tripleRowMap; // tripleNumber → row index
};

#endif // COMPARISONTABLEWINDOW_H
