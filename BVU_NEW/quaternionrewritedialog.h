#ifndef QUATERNIONREWRITEDIALOG_H
#define QUATERNIONREWRITEDIALOG_H

#include <QDialog>
#include <QSpinBox>
#include <QVector>

class QuaternionRewriteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QuaternionRewriteDialog(QWidget *parent = nullptr);
    ~QuaternionRewriteDialog();

    QByteArray generatePayload() const;  // Метод для генерации полезной нагрузки
    QSpinBox* tripleNumberSpinBox;  // QSpinBox для выбора номера тройки

private:
    QVector<QSpinBox*> quaternionInputs;  // Массив для хранения QSpinBox для значений кватерниона

};

#endif // QUATERNIONREWRITEDIALOG_H
