#include "matrixinputdialog.h"
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QHBoxLayout>

MatrixInputDialog::MatrixInputDialog(QWidget *parent)
    : QDialog(parent) {

    setWindowTitle("Ввод матрицы поворота");

    auto *mainLayout = new QVBoxLayout(this);

    // Выбор номера тройки
    auto *tripleLayout = new QHBoxLayout();
    tripleLayout->addWidget(new QLabel("Номер тройки (1–20):"));
    tripleNumberSpinBox = new QSpinBox(this);
    tripleNumberSpinBox->setRange(1, 20);
    tripleLayout->addWidget(tripleNumberSpinBox);
    mainLayout->addLayout(tripleLayout);

    // Цвета
    QString headerStyle = "background-color: #d0e9ff; font-weight: bold; padding: 3px;";
    QString axisStyle   = "background-color: #dfffd0; font-weight: bold; padding: 3px;";
    QString cellStyle   = "background-color: #fffbd0; padding: 3px;";

    // Ввод матрицы 3x3 с осями и подписями
    QGridLayout *gridLayout = new QGridLayout();

    // Верхняя строка с заголовками
    gridLayout->addWidget(createStyledLabel("Коэф. 𝜔I x", headerStyle), 0, 1);
    gridLayout->addWidget(createStyledLabel("Коэф. 𝜔I y", headerStyle), 0, 2);
    gridLayout->addWidget(createStyledLabel("Коэф. 𝜔I z", headerStyle), 0, 3);

    // Строки OX, OY, OZ
    QStringList axes = {"OX", "OY", "OZ"};
    for (int row = 0; row < 3; ++row) {
        gridLayout->addWidget(createStyledLabel(axes[row], axisStyle), row + 1, 0);

        for (int col = 0; col < 3; ++col) {
            QSpinBox *spin = new QSpinBox(this);
            spin->setRange(INT32_MIN, INT32_MAX);
            spin->setAlignment(Qt::AlignRight);
            spin->setValue(1);
            spin->setStyleSheet(cellStyle);
            matrixInputs.append(spin);
            gridLayout->addWidget(spin, row + 1, col + 1);
        }
    }

    mainLayout->addLayout(gridLayout);

    // Кнопки
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this
        );
    connect(buttonBox, &QDialogButtonBox::accepted, this, &MatrixInputDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &MatrixInputDialog::reject);
    mainLayout->addWidget(buttonBox);
}

QLabel* MatrixInputDialog::createStyledLabel(const QString &text, const QString &style) {

    QLabel *label = new QLabel(text);
    label->setStyleSheet(style);
    label->setAlignment(Qt::AlignCenter);
    return label;
}

int MatrixInputDialog::tripleNumber() const {

    return tripleNumberSpinBox->value();
}

QVector<int32_t> MatrixInputDialog::matrixValues() const {

    QVector<int32_t> values;
    for (QSpinBox *spin : matrixInputs)
        values.append(spin->value());
    return values;
}
