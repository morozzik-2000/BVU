#pragma once

#include <QDialog>
#include <QSpinBox>
#include <QVector>
#include <QLabel>

class MatrixInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MatrixInputDialog(QWidget *parent = nullptr);

    int tripleNumber() const;
    QVector<int32_t> matrixValues() const;

private:
    QSpinBox *tripleNumberSpinBox;
    QVector<QSpinBox*> matrixInputs;

    QLabel* createStyledLabel(const QString &text, const QString &style); // ← добавлено
};
