#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QSerialPortInfo>
#include <QDebug>
#include <QGroupBox>
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();
    connect(btnConnect, &QPushButton::clicked, this, &MainWindow::connectToPortImitation);
    connect(btnSend, &QPushButton::clicked, this, &MainWindow::onSendClicked);
}

MainWindow::~MainWindow() {

}

void MainWindow::onSendClicked() {

    freqHz = spinFrequency->value();
    if (freqHz <= 0) {
        onLogMessage("❌ Недопустимая частота");
        return;
    }
    commandWindow->setFreqHz(freqHz);
    PultErrors errors;
    uint8_t interfaceId = comboPort->currentText().toInt(); // UART0
    QByteArray dataToSend =  buildTestCommand();

    if (m_pult->sendData(interfaceId, dataToSend, this, errors)) {
//         onLogMessage("Data sent successfully.");
    } else {
         onLogMessage("Send error: " + errors.toString());
        return;
    }
}

void MainWindow::setupUI() {

    setWindowTitle("Окно 1. Сетевые настройки поключения к МИУС");
    resize(600, 400);
    setFixedSize(800,980);
    m_pult = new pultUdpSend(ip, port, baseAddress, this);
    connect(m_pult, &pultUdpSend::responseReceived, this, &MainWindow::onResponseReceived);

    if (!commandWindow) {
        commandWindow = new CommandWindow(m_pult, 15);//д
        commandWindow->show();
    }
    connect(commandWindow, &CommandWindow::resultsViewerCreated, this, &MainWindow::setResultsViewer);
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    /*-----------------------------Параметры соединения-------------------------------------------*/

    QLabel *connectionParamters = new QLabel("Параметры соединения:", this);
    mainLayout->addWidget(connectionParamters);
    connectionParamters->setStyleSheet("font-size: 18px; font-weight: 400;");

    QGroupBox *connectionGroup = new QGroupBox();
    QGridLayout *connectionLayout = new QGridLayout(connectionGroup);
    QLabel *choosePortForconnection = new QLabel("Выбор порта подключения:", this);
    choosePortForconnection->setStyleSheet("font-size: 18px; font-weight: 300;");
    connectionLayout->addWidget(choosePortForconnection, 0, 0);
    comboPort = new QComboBox();
    comboPort->addItems({"15", "14"});
    comboPort->setStyleSheet("QComboBox { background-color: white; font-size: 15px; font-weight: 400;}"
                             "QComboBox QAbstractItemView{ background-color: white; font-size: 15px; font-weight: 400;}"
                             );
    connectionLayout->addWidget(comboPort, 0, 1);
    connectionLayout->addItem(new QSpacerItem(5,10,QSizePolicy::Expanding,QSizePolicy::Minimum), 0, 2);

    QLabel *chooseSpeedFotSendData = new QLabel("Выбор скорости передачи данных, Б/c:");
    chooseSpeedFotSendData->setStyleSheet("font-size: 18px; font-weight: 400;");
    connectionLayout->addWidget(chooseSpeedFotSendData, 0, 3);
    comboBaud = new QComboBox();
    comboBaud->setStyleSheet(
        "QComboBox { background-color: white; font-size: 15px; font-weight: 400; }"
        "QComboBox QAbstractItemView { background-color: white; font-size: 15px; font-weight: 400; }"
        );
    comboBaud->addItems({"115200","9600", "19200", "38400", "57600"});
    connectionLayout->addWidget(comboBaud, 0, 4);
    connectionLayout->addItem(new QSpacerItem(10,90,QSizePolicy::Expanding,QSizePolicy::Minimum), 1, 0);
    btnConnect = new QPushButton("Открыть\nпорт"); // Создание кнопки соединения
    connectionLayout->addWidget(btnConnect, 1, 0, 2, 7, Qt::AlignCenter);

    mainLayout->addWidget(connectionGroup);

    /*-----------------------------------Выбор интерфейса--------------------------------------------*/

    QLabel *chooseInterface = new QLabel("Выбор интерфейса для работы с МИУС:", this);
    chooseInterface->setStyleSheet("font-size: 18px; font-weight: 400;");
    mainLayout->addWidget(chooseInterface);
    QGroupBox *fbmGroup = new QGroupBox();
    QGridLayout *fbmLayout = new QGridLayout(fbmGroup);
    fbmLayout->addItem(new QSpacerItem(10,50,QSizePolicy::Expanding,QSizePolicy::Minimum), 0, 0);

    QLabel *chooseInterfaceText = new QLabel("Интерфейс:");
    chooseInterfaceText->setStyleSheet("font-size: 18px; font-weight: 300;");
    fbmLayout->addWidget(chooseInterfaceText, 0, 0);

    comboInterface = new QComboBox();
    comboInterface->setStyleSheet(
        "QComboBox { background-color: white; font-size: 15px; font-weight: 400; }"
        "QComboBox QAbstractItemView { background-color: white; font-size: 15px; font-weight: 400; }"
        );
    comboInterface->addItems({"RS-485", "SpaceWire"});
    fbmLayout->addWidget(comboInterface, 0, 1);

    QLabel *chooseAdressBvu = new QLabel("Адрес БВУ:");
    chooseAdressBvu->setStyleSheet("font-size: 18px; font-weight: 300;");
    fbmLayout->addWidget(chooseAdressBvu, 0, 3);

    spinAddressBvu = new QSpinBox();
    spinAddressBvu->setRange(1, 32);
    spinAddressBvu->setValue(32);
    fbmLayout->addWidget(spinAddressBvu, 0, 4);
    fbmLayout->addItem(new QSpacerItem(4,10,QSizePolicy::Expanding,QSizePolicy::Minimum),2,2);

    QLabel *chooseAdressMius = new QLabel("Адрес МИУС:");
    chooseAdressMius->setStyleSheet("font-size: 18px; font-weight: 300;");
    fbmLayout->addWidget(chooseAdressMius, 1, 0);

    spinAddressMius = new QSpinBox();
    spinAddressMius->setRange(1, 32);
    spinAddressMius->setValue(16);

    fbmLayout->addWidget(spinAddressMius, 1, 1);

    fbmLayout->addItem(new QSpacerItem(4,10,QSizePolicy::Expanding,QSizePolicy::Minimum),1,2);

    QLabel *chooseFrequency = new QLabel("Частота опроса МИУС:");
    chooseFrequency->setStyleSheet("font-size: 18px; font-weight: 300;");
    fbmLayout->addWidget(chooseFrequency, 1, 3);

    spinFrequency = new QSpinBox();
    spinFrequency->setRange(1, 500);
    spinFrequency->setValue(1);
    fbmLayout->addWidget(spinFrequency, 1, 4);

    fbmLayout->addWidget(new QLabel("Гц"), 1, 5);
    fbmLayout->addItem(new QSpacerItem(10,90,QSizePolicy::Expanding,QSizePolicy::Minimum), 1, 5);
    btnSend = new QPushButton("Поключиться\nк\nМИУС");

    fbmLayout->addWidget(btnSend, 3, 0, 1, 7,Qt::AlignCenter);
    mainLayout->addWidget(fbmGroup);

    /*-----------------------------Параметры опроса--------------------------------------------------*/

    spinAddress = new QSpinBox();
    spinAddress->setRange(0, 255);

    QLabel *poolInterface = new QLabel("Выбор параметров опроса МИУС:", this);
    poolInterface->setStyleSheet("font-size: 18px; font-weight: 400;");
    mainLayout->addWidget(poolInterface);
    QHBoxLayout *pollLayout = new QHBoxLayout();
    btnStartPoll = new QPushButton("Начать\nопрос\nМИУС");
    btnStopPoll = new QPushButton("Закончить\nопрос\nМИУС");
    btnStartPoll->setMaximumWidth(450);
    btnStartPoll->setMinimumHeight(40);
    durationValueS = new QSpinBox();
    durationValueS->setStyleSheet("font-size: 15px; font-weight: 300;");
    durationValueS->setMinimum(1);
    durationValueS->setMaximum(604800);
    durationValueS->setValue(60);
    comboDurationUnit = new QComboBox();
    comboDurationUnit->setStyleSheet(
        "QComboBox { background-color: white; font-size: 15px; font-weight: 400; }"
        "QComboBox QAbstractItemView { background-color: white; font-size: 15px; font-weight: 400; }"
        );
    comboDurationUnit->addItems({"сек", "мин", "час"});
    comboDurationUnit->setStyleSheet(
        "QComboBox { background-color: white; font-size: 15px; font-weight: 400; }"
        "QComboBox QAbstractItemView { background-color: white; font-size: 15px; font-weight: 400; }"
        );

    QLabel *duration = new QLabel("Длительность:");
    duration->setStyleSheet("font-size: 18px; font-weight: 300;");
    pollLayout->addWidget(duration);
    pollLayout->addWidget(durationValueS);
    pollLayout->addWidget(comboDurationUnit);
    pollLayout->addWidget(btnStartPoll);
    pollLayout->addWidget(btnStopPoll);

    spinAddressBvu->setStyleSheet("QSpinBox { font-size: 15px; }");
    spinAddressMius->setStyleSheet("QSpinBox { font-size: 15px; }");
    spinFrequency->setStyleSheet("QSpinBox { font-size: 15px; }");
    spinAddress->setStyleSheet("QSpinBox { font-size: 15px; }");
    btnConnect->setMinimumHeight(50);
    btnConnect->setMinimumWidth(100);
    btnConnect->setStyleSheet(
        "QPushButton {"
        "   font-size: 15px;"
        "   font-weight: 400;"
        "   background: #ADD8E6;"
        "   color: black;"
        "   border-radius: 20px;"
        "   padding: 5px;"
        "}"
        "QPushButton:hover {"
        " border: 2px solid #5D9CA6;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #5FA9C9;"
        "   transmission: background-color 0.9s;"
        "}"
        );
    btnSend->setStyleSheet("QPushButton { font-size: 17px; font-weight: 400; }");
    btnSend->setMinimumHeight(65);
    btnSend->setMinimumWidth(125);
    btnSend->setStyleSheet(
        "QPushButton {"
        "   font-size: 15px;"
        "   font-weight: 400;"
        "   background: #ADD8E6;"
        "   color: black;"
        "   border-radius: 25px;"
        "   padding: 5px;"
        "}"
        "QPushButton:hover {"
        " border: 2px solid #5D9CA6;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #5FA9C9;"
        "   transmission: background-color 0.9s;"
        "}"
        );
    btnStartPoll->setStyleSheet("QPushButton { font-size: 17px; font-weight: 400; }");
    btnStartPoll->setMinimumHeight(65);
    btnStartPoll->setMinimumWidth(135);
    btnStartPoll->setStyleSheet(
        "QPushButton {"
        "   font-size: 15px;"
        "   font-weight: 400;"
        "   background-color: #90EE90;"
        "   color: black;"
        "   border-radius: 20px;"
        "   padding: 5px;"
        "   border: 1px solid #50C878;"
        "}"
        "QPushButton:hover {"
        "   background-color: #90EE90;"
        "   border: 2px solid #3CB371;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #7AC79A;"
        "   border: 2px solid #7AC79A;"
        "   transition: background-color 0.3s;"
        "}"
        );
    btnStopPoll->setStyleSheet("QPushButton { font-size: 17px; font-weight: 400; }");
    btnStopPoll->setMinimumHeight(65);
    btnStopPoll->setMinimumWidth(135);
    btnStopPoll->setStyleSheet(
        "QPushButton {"
        "   font-size: 15px;"
        "   font-weight: 400;"
        "   background-color: #FFA07A;"
        "   color: black;"
        "   border-radius: 20px;"
        "   padding: 5px;"
        "   border: 1px solid #50C878;"
        "}"
        "QPushButton:hover {"
        "   border: 2px solid red;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #DF805A;"
        "   transmission: background-color 0.9s;"
        "}"
        );
    mainLayout->addLayout(pollLayout);

    // Было до 20.06
//    miusPoller = new MiusPoller(m_pult, 15, this);

    // Изменил переачу порта 20.06
    miusPoller = new MiusPoller(m_pult, 15, this);

    commandWindow->setMiusPoller(miusPoller); // Передаю туда, чтобы оттула задать ему тройку
    connect(miusPoller, &MiusPoller::log, this, &MainWindow::onLogMessage);
    connect(commandWindow, &CommandWindow::onLogMessage, this, &MainWindow::onLogMessage);
    connect(btnStartPoll, &QPushButton::clicked, this, &MainWindow::startMiusPolling);
    connect(btnStopPoll, &QPushButton::clicked, this, &MainWindow::stopMiusPolling);
    connect(m_pult, &pultUdpSend::deliveryFailed, this, &MainWindow::onDeliveryFailed);

    /*-----------------------------Окно сообщений--------------------------------------------------*/

    QLabel *logInterface = new QLabel("Окно сообщений:", this);
    logInterface->setStyleSheet("font-size: 18px; font-weight: 400;");
    mainLayout->addWidget(logInterface);
    logText = new QTextEdit();
    logText->setReadOnly(true);
    mainLayout->addWidget(logText);

}

void MainWindow::startMiusPolling() {
    portForPoller = comboPort->currentText().toInt(); // 20.06
    qDebug() << "portForPoller" <<portForPoller;
    miusPoller->setPort(portForPoller); // 20.06
    qDebug() << "comboFrequencyHz" << spinFrequency->value();
    freqHz = spinFrequency->value();
    if (freqHz <= 0) {
        onLogMessage("❌ Недопустимая частота");
        return;
    }
    miusPoller->setFreq(freqHz);
    int intervalMs = 1000 / freqHz;

    int durationValue = durationValueS->value();
    QString durationUnit = comboDurationUnit->currentText();
    int totalMs = durationValue * (durationUnit == "сек" ? 1000 :
                                       durationUnit == "мин" ? 60000 :
                                       3600000);

    QMetaObject::invokeMethod(miusPoller, [=]() {
        miusPoller->start(intervalMs, totalMs);
    }, Qt::QueuedConnection);
}

void MainWindow::stopMiusPolling() {

    QMetaObject::invokeMethod(miusPoller, "stop", Qt::QueuedConnection);
}

QByteArray MainWindow::buildTestCommand() {

    userDisplayValue = spinAddressMius->value();
    addressByte = userDisplayValue;
    switch (userDisplayValue) {
    case 16:
        addressByte = 0x16;
        break;
    case 17:
        addressByte = 0x17;
        break;
    default:
        addressByte = 0x16;  // по умолчанию — прямое преобразование
    }
    QByteArray command;
    command.append(0xAA); // Старт
    command.append(0xC4); // Флаг
    command.append(static_cast<char>(addressByte));
    command.append((char)0x00); // Размер младший байт
    command.append(0x01); // Размер старший байт
    command.append(0x03); // Полезная нагрузка
    QByteArray crcData = command.mid(1);
    command.append(calculateCRC8(crcData));
    return command;
}

uint8_t MainWindow::calculateCRC8(const QByteArray &data) {

    static const uint8_t CRC_TABLE[256] = {
        0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15,
        0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
        0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65,
        0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
        0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5,
        0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
        0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85,
        0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
        0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,
        0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
        0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2,
        0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
        0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32,
        0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
        0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42,
        0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
        0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C,
        0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
        0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC,
        0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
        0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C,
        0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
        0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C,
        0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
        0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B,
        0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
        0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,
        0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
        0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB,
        0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
        0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB,
        0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
    };
    uint8_t crc = 0;
    for (char byte : data) {
        crc = CRC_TABLE[crc ^ static_cast<uint8_t>(byte)];
    }
    return crc;
}

void MainWindow::onLogMessage(const QString &message) {

    logText->setReadOnly(true);
    logText->setFont(QFont("Courier", 12));
    logText->append(message);
}

void MainWindow::onResponseReceived(const QByteArray &data, QObject *origin) {

    if (origin == this) {
            // logText->append("Ответ на нашу команду: " + data.toHex(' '));
        logText->append(QString("✅ МИУС адр. 16 успешно подключен"));
            // Обработка ответа
        }
}

void MainWindow::onDeliveryFailed(QObject *origin) {

    if (origin == this) {
        logText->append(QString("❌ Нет связи с МИУС адр. 16"));
    }
}

void MainWindow::connectToPortImitation() {
    portForPoller = comboPort->currentText().toInt(); // 20.06
    qDebug() << "portForPoller" <<portForPoller; // 20.06
    miusPoller->setPort(portForPoller); // 20.06
    onLogMessage("Порт открыт!");
}

void MainWindow::closeEvent(QCloseEvent* event) {

    if (commandWindow) {
        commandWindow->close();
    }
    QMainWindow::closeEvent(event);
}
void MainWindow::setResultsViewer(ResultsViewer *resultsViewer) {

    this->resultsViewer = resultsViewer;
    qDebug() << "MainWindow take resultsViewer for connect";
    qDebug() << " set resultsViewer" << resultsViewer;
    qDebug() << " con resultsViewer" << resultsViewer;
    connect(miusPoller,&MiusPoller::newQuaternionData, resultsViewer, &ResultsViewer::addQuaternionData);
    connect(miusPoller,&MiusPoller::newVSKData, resultsViewer, &ResultsViewer::addVSKData);
}


