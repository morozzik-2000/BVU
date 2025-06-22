#include "quaternionrewritedialog.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>

QuaternionRewriteDialog::QuaternionRewriteDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Перезапись кватерниона");

    // Основной layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Выбор номера тройки
    QHBoxLayout *tripleLayout = new QHBoxLayout();
    tripleLayout->addWidget(new QLabel("Номер тройки (1–20):"));
    tripleNumberSpinBox = new QSpinBox(this);
    tripleNumberSpinBox->setRange(1, 20);
    tripleLayout->addWidget(tripleNumberSpinBox);
    mainLayout->addLayout(tripleLayout);

    // Ввод значений кватерниона (4 компонента по горизонтали)
    QGridLayout *gridLayout = new QGridLayout();

    QStringList labels = {"ʎ0", "ʎ1", "ʎ2", "ʎ3"};
    for (int i = 0; i < 4; ++i) {
        QLabel *label = new QLabel(labels[i] + ":", this);
        label->setAlignment(Qt::AlignCenter);
        gridLayout->addWidget(label, 0, i); // Метки в первой строке

        QSpinBox *spin = new QSpinBox(this);
        spin->setRange(INT32_MIN, INT32_MAX);
        spin->setAlignment(Qt::AlignRight);
        spin->setValue(1);
        quaternionInputs.append(spin);

        gridLayout->addWidget(spin, 1, i); // SpinBox'ы во второй строке
    }

    mainLayout->addLayout(gridLayout);
    // Кнопки "ОК" и "Отмена"
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
    QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);
}

QuaternionRewriteDialog::~QuaternionRewriteDialog()
{

}

QByteArray QuaternionRewriteDialog::generatePayload() const
{
    QByteArray payload;

    // Добавляем номер тройки
    int tripleNumber = tripleNumberSpinBox->value();
    uint8_t code = 0xE0 + static_cast<uint8_t>(tripleNumber);
    payload.append(static_cast<char>(code)); // Добавляем код тройки


    for (QSpinBox *spin : quaternionInputs) {
        int32_t value = spin->value();
        for (int i = 3; i >= 0; --i) {
            payload.append(static_cast<char>((value >> (8 * i)) & 0xFF));  // Little-endian
        }
    }

    return payload;
}
