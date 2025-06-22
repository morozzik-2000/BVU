#include "commandwindow.h"
#include <QDebug>
#include <QGroupBox>

constexpr uint8_t REQUEST_READING = 0xC4; // Чтение (абонент отправляет данные)
constexpr uint8_t REQUEST_RECORDING = 0x84; // Запись (абонент записывает данные)

CommandWindow::CommandWindow(pultUdpSend *pult, uint8_t interfaceId, QWidget *parent) :
    QWidget(parent), m_pult(pult), m_interfaceId(interfaceId){
    setupUI();
    connect(m_pult, &pultUdpSend::responseReceived, this, &CommandWindow::onResponseReceived);
    connect(m_pult, &pultUdpSend::deliveryFailed, this, &CommandWindow::onDeliveryFailed);
    connect(m_pult, &pultUdpSend::lockCommand, this, &CommandWindow::onLockCommand);
}
CommandWindow::~CommandWindow() {

}

void CommandWindow::setupUI() {
    setWindowTitle("Окно 2. Оператор МИУС");
    adjustSize();

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFont titleFont;
    titleFont.setPointSize(12);
    titleFont.setBold(false);
    QFont buttonFont;
    buttonFont.setPointSize(10);

    // ЗОНА 1: Настройки
    QGroupBox *groupSettings = new QGroupBox("Настройки");
    groupSettings->setFont(titleFont);
    groupSettings->setMaximumWidth(450);
    QVBoxLayout *settingsLayout = new QVBoxLayout(groupSettings);
    auto setupButton = [&](QPushButton* &btn, const QString &text) {
        btn = new QPushButton(text, this);
        btn->setFont(buttonFont);
        btn->setStyleSheet(
            "QPushButton {"
            "  background-color: #FFA07A;"
            "  border: 1px solid #888;"
            "  padding: 5px 10px 5px 30px;"
            "  border-radius: 15px;"
            "  background-position: left 5px center;"
            "  background-repeat: no-repeat;"
            "}"
            "QPushButton:hover {"
            "   border: 2px solid red;"   //  Красная рамка при наведении
            "}"
            "QPushButton:pressed {"
            "   background-color: #DF805A;" // Красный градиент при нажатии
            "   transmission: background-color 0.9s;"
            "}"
            );

        settingsLayout->addWidget(btn);
    };

    setupButton(btnCriticalMode, "Включить критический режим");
    setupButton(btnWorkTriple, "Выбрать рабочую тройку");
    setupButton(btnRewriteMatrix, "Перезапись матрицы поворота");
    setupButton(btnRewriteQuaternion, "Перезапись кватерниона");
    setupButton(btnFreq, "Частота опроса ДУС");
    setupButton(btnCriticalModeOff, "Выключить критический режим");

    mainLayout->addWidget(groupSettings);

    // ЗОНА 2: Команды управления
    QGroupBox *groupCommands = new QGroupBox("Команды управления");
    groupCommands->setFont(titleFont);
    QHBoxLayout *commandsLayout = new QHBoxLayout(groupCommands);
    auto createSubGroup = [&](const QString &title, const std::vector<QPushButton*> &buttons) {
        QGroupBox *subGroup = new QGroupBox(title);
        subGroup->setFont(titleFont);
        QVBoxLayout *layout = new QVBoxLayout(subGroup);
        for (QPushButton* btn : buttons) {
            btn->setFont(buttonFont);
            btn->setStyleSheet(
                "QPushButton {"
                "  border: 1px solid #888;"
                "  padding: 5px 10px 5px 30px;"
                "  border-radius: 15px;"
                "  background-position: left 5px center;"
                "  background-repeat: no-repeat;"
                "}"
                "QPushButton:hover {"
                 "   border: 2px solid #707070;"
                 "}"
                "QPushButton:pressed {"
                 "   background-color: #999999;"
                // D4D4D4
                 "   transmission: background-color 0.9s;"
                "}"

            );
            layout->addWidget(btn);
        }
        return subGroup;
    };

    // Зона 2: Запросы данных
    btnReadQuaternion = new QPushButton("Запрос кватерниона", this);
    btnReadAngularVelocity = new QPushButton("Запрос угловых скоростей", this);
    btnReadQuaternionAndAngularVelocity = new QPushButton("Запрос кватернионов и угловых скоростей", this);
    btnReadAngles = new QPushButton("Запрос углов", this);
    shareTableMiusQuaternionResponseTwice = new QPushButton("Показать таблицу обработанных данных", this);
    btnCompeteMiusData = new QPushButton("Заполнить таблицу\nданных МИУС", this);
    commandsLayout->addWidget(createSubGroup("Обработанные данные МИУС", {
                              btnReadQuaternion, btnReadAngularVelocity, btnReadQuaternionAndAngularVelocity,
                              btnReadAngles,
                              shareTableMiusQuaternionResponseTwice, btnCompeteMiusData
                              }));
    btnCompeteMiusData->setStyleSheet(
        "QPushButton {"
        "  background-color: #ADD8E6;"
        "  border: 1px solid #888;"
        "  padding: 5px 10px 5px 30px;"
        "  border-radius: 25px;"
        "  background-position: left 5px center;"
        "  background-repeat: no-repeat;"
        "}"
        "QPushButton:hover {"
         "   border: 2px solid #5D9CA6;"
         "}"
        "QPushButton:pressed {"
         "   background-color: #5FA9C9;"
         "   transmission: background-color 0.9s;"
        "}"
        );
    shareTableMiusQuaternionResponseTwice->setStyleSheet(
        "QPushButton {"
        "  background-color: #ADD8E6;"
        "  border: 1px solid #888;"
        "  padding: 5px 10px 5px 30px;"
        "  border-radius: 15px;"
        "  background-position: left 5px center;"
        "  background-repeat: no-repeat;"
        "}"
        "QPushButton:hover {"
         "   border: 2px solid #5D9CA6;"
         "}"
        "QPushButton:pressed {"
         "   background-color: #5FA9C9;"
         "   transmission: background-color 0.9s;"
        "}"
        );
    // Зона 3: Сервис ДУС
    btnReadAngularVelocityDus = new QPushButton("Запрос угловых скоростей ДУС", this);
    btnReadTemperaturesDus = new QPushButton("Запрос температур ДУС", this);
    btnReadUptimeDus = new QPushButton("Запрос времени с момента включения ДУС", this);
    btnReadTripleNumberDus = new QPushButton("Запрос номера тройки ДУС", this);
    btnReadAnglesDus = new QPushButton("Запрос углов", this);
    shareTableDusResponse = new QPushButton("Показать таблицу данных ДУС", this);
    btnCompeteDusServisData = new QPushButton("Заполнить таблицу\nсервисных данных ДУС", this);
    commandsLayout->addWidget(createSubGroup("Сервисные данные ДУС", {
                              btnReadAngularVelocityDus, btnReadTemperaturesDus, btnReadUptimeDus,
                              btnReadTripleNumberDus, btnReadAnglesDus, shareTableDusResponse,
                              btnCompeteDusServisData
                              }));
    shareTableDusResponse->setStyleSheet(
        "QPushButton {"
        "  background-color: #ADD8E6;"
        "  border: 1px solid #888;"
        "  padding: 5px 10px 5px 30px;"
        "  border-radius: 15px;"
        "  background-position: left 5px center;"
        "  background-repeat: no-repeat;"
        "}"
        "QPushButton:hover {"
         "   border: 2px solid #5D9CA6;"
         "}"
        "QPushButton:pressed {"
         "   background-color: #5FA9C9;"
         "   transmission: background-color 0.9s;"
        "}"
        );
    btnCompeteDusServisData->setStyleSheet(
        "QPushButton {"
        "  background-color: #ADD8E6;"
        "  border: 1px solid #888;"
        "  padding: 5px 10px 5px 30px;"
        "  border-radius: 25px;"
        "  background-position: left 5px center;"
        "  background-repeat: no-repeat;"
        "}"
        "QPushButton:hover {"
         "   border: 2px solid #5D9CA6;"
         "}"
        "QPushButton:pressed {"
         "   background-color: #5FA9C9;"
         "   transmission: background-color 0.9s;"
        "}"
        );
    // Зона 4: Служебные данные МИУС
    btnReadAngularVelocityMiuss = new QPushButton("Запрос угловых скоростей МИУС", this);
    btnReadTemperatureMiuss = new QPushButton("Запрос температуры МИУС", this);
    btnReadStatusesDus = new QPushButton("Запрос статусов ДУС", this);
    btnReadStatusesMicrochipsMiuss = new QPushButton("Запрос статусов микросхем МИУС", this);
    btnReadAnglesMiuss = new QPushButton("Запрос углов МИУС", this);
     btnCompeteMiusServisData = new QPushButton("Заполнить таблицу\nнеобработанных данных МИУС", this);
    shareTableMiusServisDatAndMicroDataResponse = new QPushButton("Показать таблицу необработанных данных,\nсостояния микросхем МИУС", this);
    commandsLayout->addWidget(createSubGroup("Служебные данные МИУС", {
                              btnReadAngularVelocityMiuss, btnReadTemperatureMiuss, btnReadStatusesDus,
                              btnReadStatusesMicrochipsMiuss, btnReadAnglesMiuss,
                              shareTableMiusServisDatAndMicroDataResponse, btnCompeteMiusServisData
                              }));
    mainLayout->addWidget(groupCommands);
    shareTableMiusServisDatAndMicroDataResponse->setStyleSheet(
        "QPushButton {"
        "  background-color: #ADD8E6;"
        "  border: 1px solid #888;"
        "  padding: 5px 10px 5px 30px;"
        "  border-radius: 25px;"
        "  background-position: left 5px center;"
        "  background-repeat: no-repeat;"
        "}"
        "QPushButton:hover {"
         "   border: 2px solid #5D9CA6;"
         "}"
        "QPushButton:pressed {"
         "   background-color: #5FA9C9;"
         "   transmission: background-color 0.9s;"
        "}"
        );
    btnCompeteMiusServisData->setStyleSheet(
        "QPushButton {"
        "  background-color: #ADD8E6;"
        "  border: 1px solid #888;"
        "  padding: 5px 10px 5px 30px;"
        "  border-radius: 25px;"
        "  background-position: left 5px center;"
        "  background-repeat: no-repeat;"
        "}"
        "QPushButton:hover {"
         "   border: 2px solid #5D9CA6;"
         "}"
        "QPushButton:pressed {"
         "   background-color: #5FA9C9;"
         "   transmission: background-color 0.9s;"
        "}"
        );
    // Подключаем сигналы кнопок с команд управления режимом МИУС
    connect(btnCriticalMode, &QPushButton::clicked, this, &CommandWindow::addCriticalModeCommand);
    connect(btnWorkTriple, &QPushButton::clicked, this, &CommandWindow::addWorkTripleCommand);
    connect(btnRewriteMatrix, &QPushButton::clicked, this, &CommandWindow::addRewriteMatrixCommand);
    connect(btnRewriteQuaternion, &QPushButton::clicked, this, &CommandWindow::addRewriteQuaternionCommand);
    connect(btnCriticalModeOff, &QPushButton::clicked, this, &CommandWindow::addCriticalModeOffCommand);
    connect(btnFreq, &QPushButton::clicked, this, &CommandWindow::addFreq);

    // Подключаем сигналы для кнопок с командами чтения обработанных данных МИУС
    connect(btnReadQuaternion, &QPushButton::clicked, this, &CommandWindow::sendQuaternionRequest);
    connect(btnReadAngularVelocity, &QPushButton::clicked, this, &CommandWindow::sendAngularVelocityRequest);
    connect(btnReadQuaternionAndAngularVelocity, &QPushButton::clicked, this, &CommandWindow::sendQuaternionAndAngularVelocityRequest);
    connect(btnReadAngles, &QPushButton::clicked, this, &CommandWindow::sendAnglesRequest);
    connect(shareTableMiusQuaternionResponseTwice, &QPushButton::clicked, this, &CommandWindow::showTableMiusQuaternionResponseTwice);
    connect(btnCompeteMiusData, &QPushButton::clicked, this, &CommandWindow::competeMiusData);


    // Подключаем сигналы для кнопок с сервисными данными ДУС
    connect(btnReadAngularVelocityDus, &QPushButton::clicked, this, &CommandWindow::addReadAngularVelocityDusCommand);
    connect(btnReadTemperaturesDus, &QPushButton::clicked, this, &CommandWindow::addReadTemperaturesDusCommand);
    connect(btnReadUptimeDus, &QPushButton::clicked, this, &CommandWindow::addReadUptimeDusCommand);
    connect(btnReadTripleNumberDus, &QPushButton::clicked, this, &CommandWindow::addReadTripleNumberDusCommand);
    connect(btnReadAnglesDus,&QPushButton::clicked, this, &CommandWindow::addReadAnglesDusCommand);
    connect(shareTableDusResponse, &QPushButton::clicked, this, &CommandWindow::showTableDusDataResponse);
    connect(btnCompeteDusServisData, &QPushButton::clicked, this, &CommandWindow::competeDusServisData);


    // Подключаем сигналы для кнопок с сервисными данными МИУС
    connect(btnReadAngularVelocityMiuss, &QPushButton::clicked, this, &CommandWindow::addReadAngularVelocityMiussCommand);
    connect(btnReadTemperatureMiuss, &QPushButton::clicked, this, &CommandWindow::addReadTemperatureMiussCommand);
    connect(btnReadStatusesDus, &QPushButton::clicked, this, &CommandWindow::addReadStatusesDusCommand);
    connect(btnReadStatusesMicrochipsMiuss, &QPushButton::clicked, this, &CommandWindow::addReadStatusesMicrochipsMiussCommand);
    connect(btnReadAnglesMiuss, &QPushButton::clicked, this, &CommandWindow::addReadAnglesMiussCommand);
    connect(shareTableMiusServisDatAndMicroDataResponse, &QPushButton::clicked, this, &CommandWindow::showTableMiusServisDatAndMicroDataResponse);
    connect(btnCompeteMiusServisData, &QPushButton::clicked, this, &CommandWindow::competeMiusServisData);





}

/*--------------------------------- Зона 1: Команды управления режимом МИУС --------------------------------- */
void CommandWindow::addCriticalModeCommand() {
    qDebug() << "port" << m_interfaceId;
    QByteArray packet = PacketBuilder::createLevel1Packet(REQUEST_RECORDING, 0x9F);
    PultErrors errors;
    onLogMessage(" >> Отправляем команду на включение критического режима << ");
    if (m_pult->sendData(m_interfaceId, packet, this, errors)) {
       //onLogMessage("Data sent successfully.");
    } else {
         onLogMessage("Send error: " + errors.toString());
        return;
    }
}

void CommandWindow::addWorkTripleCommand() {

    QByteArray packet = PacketBuilder::createLevel1Packet(REQUEST_RECORDING, 0x81);
    PultErrors errors;
    if (m_pult->sendData(m_interfaceId, packet, this, errors)) {
//         onLogMessage("Data sent successfully.");
    } else {
         onLogMessage("Send error: " + errors.toString());
        return;
    }
    onLogMessage("Отправляем запрос на разрешение выбора тройки");
}

void CommandWindow::addRewriteMatrixCommand() {

    QByteArray packet = PacketBuilder::createLevel1Packet(REQUEST_RECORDING, 0x82);
    PultErrors errors;
    if (m_pult->sendData(m_interfaceId, packet, this, errors)) {
//         onLogMessage("Data sent successfully.");
    } else {
         onLogMessage("Send error: " + errors.toString());
        return;
    }
    onLogMessage("Отправляем запрос на разрешение записи матрицы поворота");
}

void CommandWindow::addRewriteQuaternionCommand() {

    QByteArray packet = PacketBuilder::createLevel1Packet(REQUEST_RECORDING, 0x84);
    PultErrors errors;
    if (m_pult->sendData(m_interfaceId, packet, this, errors)) {
//         onLogMessage("Data sent successfully.");
    } else {
         onLogMessage("Send error: " + errors.toString());
        return;
    }
}

void CommandWindow::addCriticalModeOffCommand() {

    QByteArray packet = PacketBuilder::createLevel1Packet(REQUEST_RECORDING, 0x80);
    PultErrors errors;
    if (m_pult->sendData(m_interfaceId, packet, this, errors)) {
//         onLogMessage("Data sent successfully.");
    } else {
         onLogMessage("Send error: " + errors.toString());
        return;
    }
    onLogMessage("Выключить критический режим");
}

void CommandWindow::addFreq() {
    FreqTableDialog dialog(this);
     dialog.setFixedSize(350,150);
        if (dialog.exec() != QDialog::Accepted) {
            onLogMessage("Ввод частоты отменён.");
            return;
        }
        quint16 freqWord = dialog.getFreqWord();
        if (freqWord == 0) {
            onLogMessage("Некорректное значение частоты.");
            return;
        }
        // Формируем payload (0x16 + частота в little endian)
        QByteArray payload;
        payload.append(static_cast<char>((freqWord >> 8) & 0xFF)); // старший байт
        payload.append(static_cast<char>(freqWord & 0xFF));        // младший байт
        QByteArray packet;
        packet.append(0xAA); // Старт
        packet.append(REQUEST_RECORDING); // Флаг
        packet.append(0x16); // Адрес
        packet.append((char)0x00); // Размер младший байт
        packet.append(0x02); // Размер старший байт
        packet.append(payload);
        QByteArray crcData = packet.mid(1);
        packet.append(PacketBuilder::calculateCRC8(crcData));
        PultErrors errors;
        if (m_pult->sendData(m_interfaceId, packet, this, errors)) {
//             onLogMessage("Data sent successfully.");
            qDebug() << "freq"<< this;
        } else {
             onLogMessage("Send error: " + errors.toString());
            return;
        }
        onLogMessage(QString("Отправлен запрос на установку частоты: %1 Гц").arg(freqWord & 0x1FFF));
}

void CommandWindow::onResponseReceived(const QByteArray &data, QObject *origin) {

    if (origin == this) {
        uint16_t dataSize = (static_cast<uint8_t>(data[3]) << 8) | static_cast<uint8_t>(data[4]);
        QByteArray payload = data.mid(5, dataSize);
        QString payloadHex;
        for (char byte : payload) {
            payloadHex += QString::asprintf("%02X ", static_cast<uint8_t>(byte));
        }
        if (lastCommand == CommandType::Triple) {
            processTripleResponse(data);
            return;
        } else  if (lastCommand == CommandType::Matrix) {
            processMatrixResponse(data);
            return;
        } else  if (lastCommand == CommandType::QQuaternion) {
            processQuaternionResponse(data);
            return;
        } else  if (lastCommand == CommandType::ProcessedQuaternionRequest) {
            processMiusDataResponse(data);
            return;
        } else  if (lastCommand == CommandType::ProcessedAngularVelocityRequest) {
            processMiusDataResponse(data);
            return;
        } else  if (lastCommand == CommandType::ProcessedQuaternionAndVelocityRequest) {
            processMiusDataResponse(data);
            return;
        } else  if (lastCommand == CommandType::ProcessedAnglesRequest) {
            processMiusDataResponse(data);
            return;
        } else  if (lastCommand == CommandType::ServisAngularVelocityDusRequest) {
            processDusServisDataResponse(data);
            return;
        } else  if (lastCommand == CommandType::ServisTemperaturesDusRequest) {
            processDusServisDataResponse(data);
            return;
        } else  if (lastCommand == CommandType::ServisUptimeDusRequest) {
            processDusServisDataResponse(data);
            return;
        } else  if (lastCommand == CommandType::ServisTripleNumberDusRequest) {
            processDusServisDataResponse(data);
            return;
        } else  if (lastCommand == CommandType::ServisAnglesDusRequest) {
            processDusServisDataResponse(data);
            return;
        } else  if (lastCommand == CommandType::ServisAngularVelocityMiussRequest) {
            processMiusServisDataResponse(data);
            return;
        } else  if (lastCommand == CommandType::ServisTemperatureMiussRequest) {
            processMiusServisDataResponse(data);
            return;
        } else  if (lastCommand == CommandType::ServisReadStatusesDus) {
            processMiusServisDataResponse(data);
            return;
        }  else  if (lastCommand == CommandType::ServisReadStatusesMicrochipsMiuss) {
            processMiusServisDataResponse(data);
            return;
        }  else  if (lastCommand == CommandType::ServisAnglesMiuss) {
            processMiusServisDataResponse(data);
            return;
        } else {
//            onLogMessage("Неизвестный тип команды, либо команда управления");
        }
        lastCommand = CommandType::None;
        uint8_t commandByte = static_cast<uint8_t>(payload.at(0));
        if (commandByte == 0x80) {
            onLogMessage("✅ Команда подтверждена: ОТКЛЮЧИТЬ КРИТИЧЕСКИЙ РЕЖИМ!");
        } else if (commandByte == 0x81) {
            onLogMessage("✅ Команда подтверждена: ВЫБОР РАБОЧЕЙ ТРОЙКИ!");
            rewriteTripleCommand();
        } else if (commandByte == 0x82) {
            onLogMessage("✅ Команда подтверждена: ЗАПИСЬ МАТРИЦЫ ПОВОРОТА!");
            rewriteMatrixCommand();
        } else if (commandByte == 0x84) {
            onLogMessage("✅ Команда подтверждена: ЗАПИСЬ КВАТЕРНИОНА!");
            rewriteQuaternionCommand();
        } else if (commandByte == 0x9F) {
            onLogMessage("✅ Команда подтверждена: ВКЛЮЧИТЬ КРИТИЧЕСКИЙ РЕЖИМ!");
        } else if (commandByte == 0x30) {
            onLogMessage("✅ Команда подтверждена: УСТАНОВИЛ ЧАСТОТУ ДУС!");
        }  else if (commandByte == 0x00) {
            onLogMessage("❌ Команда подтверждена: ЗАПРЕТИЛ УСТАНОВКУ ЧАСТОТЫ ДУС!");
        }
        else {
           onLogMessage("⚠️ Команда подтверждена, но тип неизвестен: 0x" +
                                   QString::asprintf("%02X", commandByte));
        }
    }
}

void CommandWindow::rewriteTripleCommand() {

    onLogMessage("Разрешение выбора тройки. Ожидается выбор пользователя...");
    WorkTripleDialog dialog(this);
    dialog.setFixedSize(1130,160);
    if (dialog.exec() == QDialog::Accepted) {
        tripleNumberDUS = dialog.selectedTriple();
        if (tripleNumberDUS < 1 || tripleNumberDUS > 20) {
            onLogMessage("❌ Ошибка: некорректный номер тройки.");
            return;
        }
        // Формируем команду с кодом от 0xA1 для первой тройки
        uint8_t code = 0xA0 + static_cast<uint8_t>(tripleNumberDUS); // Используем A1 для первой, A2 для второй и так далее
        QByteArray packet = PacketBuilder::createLevel1Packet(REQUEST_RECORDING, code);
        PultErrors errors;
        if (m_pult->sendData(m_interfaceId, packet, this, errors)) {
//             onLogMessage("Data sent successfully.");
        } else {
             onLogMessage("Send error: " + errors.toString());
            return;
        }

        onLogMessage(QString("📤 Отправка команды выбора тройки №%1 (код 0x%2)")
                                                                      .arg(tripleNumberDUS)
                                                                      .arg(code, 2, 16, QChar('0')).toUpper());
        chooseAdresOfTriple(); // Вызываем, чтобы закинуть poller тройку
        lastCommand = CommandType::Triple;
    } else {
        onLogMessage("Выбор тройки отменён пользователем.");
        lastCommand = CommandType::None;
    }
}

void CommandWindow::processTripleResponse(const QByteArray &packet) {

    if (packet.size() < 6) return;
    uint8_t cmdType = static_cast<uint8_t>(packet[1]);
    uint8_t subType = static_cast<uint8_t>(packet[2]);
    if (cmdType == 0x04 && subType == 0x32) {
        uint8_t result = static_cast<uint8_t>(packet[5]);
        if (result == 0x20) {
            onLogMessage("✅ МИУС разрешил выбор тройки.");
        } else if (result == 0x00) {
           onLogMessage("❌ МИУС запретил выбор тройки.");
        } else {
            onLogMessage("⚠️ Неизвестный ответ от МИУС по тройке.");
        }
    }
    lastCommand = CommandType::None;
}

void CommandWindow::rewriteMatrixCommand() {

    MatrixInputDialog dialog(this);
    dialog.setFixedSize(430,200);
    if (dialog.exec() == QDialog::Accepted) {
        int tripleNum = dialog.tripleNumber();
        QVector<int32_t> values = dialog.matrixValues();
        QByteArray payload = createMatrixRewriteData(tripleNum, values);
        QByteArray packet = PacketBuilder::createLevel1Packet(REQUEST_RECORDING,  QByteArray(payload));
        PultErrors errors;
        if (m_pult->sendData(m_interfaceId, packet, this, errors)) {
//             onLogMessage("Data sent successfully.");
            qDebug() << "matr"<< this;
        } else {
             onLogMessage("Send error: " + errors.toString());
            return;
        }

        lastCommand = CommandType::Matrix;
    } else {
        onLogMessage("Перезапись матрицы отменена пользователем.");
        lastCommand = CommandType::None;
        onLogMessage("CommandType::None - Отменили запись матрицы");
    }
}

QByteArray CommandWindow::createMatrixRewriteData(int tripleNum, const QVector<int32_t> &values) {

    if (values.size() != 9) {
        onLogMessage("❌ Ошибка: нужно 9 значений матрицы.");
        return {};
    }
    // Добавляем номер тройки
    QByteArray payload;
    uint8_t tripleNumber = tripleNum;
    uint8_t code = 0xC0 + static_cast<uint8_t>(tripleNumber);
    payload.append(static_cast<char>(code)); // Номер тройки
    onLogMessage(QString("📤 Отправка команды перезаписи матрицы №%1 (код 0x%2)")
                                                          .arg(tripleNumber)
                                                          .arg(code, 2, 16, QChar('0')).toUpper());

    for (int32_t val : values) {
        for (int i = 3; i >= 0; --i) {
            payload.append(static_cast<char>((val >> (8 * i)) & 0xFF)); // little-endian
        }
    }
    return payload;
}

void CommandWindow::processMatrixResponse(const QByteArray &packet) {

    if (packet.size() < 6) return;

    uint8_t cmdType = static_cast<uint8_t>(packet[1]);
    uint8_t subType = static_cast<uint8_t>(packet[2]);
    if (cmdType == 0x04 && subType == 0x32) {
        uint8_t result = static_cast<uint8_t>(packet[5]);
        if (result == 0x40) {
            onLogMessage("✅ МИУС разрешил запись матрицы поворота.");
        } else if (result == 0x00) {
            onLogMessage("❌ МИУС запретил запись матрицы поворота.");
        } else {
           onLogMessage("⚠️ Неизвестный ответ от МИУС по матрице.");
        }
    }
    lastCommand = CommandType::None;

}

void CommandWindow::rewriteQuaternionCommand() {

    onLogMessage("Перезапись кватерниона. Ожидается выбор пользователя...");

    QuaternionRewriteDialog dialog(this);
    dialog.setFixedSize(400,170);
    if (dialog.exec() == QDialog::Accepted) {

        QByteArray payload = dialog.generatePayload();

        if (payload.isEmpty()) {
            onLogMessage("❌ Ошибка: пустая полезная нагрузка.");
            return;
        }

        QByteArray packet = PacketBuilder::createLevel1Packet(REQUEST_RECORDING,  QByteArray(payload));

        int tripleNumber = dialog.tripleNumberSpinBox->value();
        uint8_t commandCode = 0xE0 + tripleNumber;
        onLogMessage(QString("📤 Отправка команды перезаписи кватерниона №%1 (код 0x%2)")
                                                              .arg(tripleNumber)
                                                              .arg(commandCode, 2, 16, QChar('0')).toUpper());
        PultErrors errors;
        if (m_pult->sendData(m_interfaceId, packet, this, errors)) {
//             onLogMessage("Data sent successfully.");
        } else {
             onLogMessage("Send error: " + errors.toString());
            return;
        }
        lastCommand = CommandType::QQuaternion;

    } else {
        onLogMessage("Перезапись кватерниона отменена пользователем.");
        lastCommand = CommandType::None;
        onLogMessage("CommandType::None - Отменили запись");
    }

}

void CommandWindow::processQuaternionResponse(const QByteArray &packet) {

    if (packet.size() < 6) return;

    uint8_t cmdType = static_cast<uint8_t>(packet[1]);
    uint8_t subType = static_cast<uint8_t>(packet[2]);
    if (cmdType == 0x04 && subType == 0x32) {
        uint8_t result = static_cast<uint8_t>(packet[5]);
        if (result == 0x60) {
            onLogMessage("✅ МИУС разрешил запись кватерниона.");
        } else if (result == 0x00) {
            onLogMessage("❌ МИУС запретил запись матрицы поворота.");
        } else {
            onLogMessage("⚠️ Неизвестный ответ от МИУС по кватерниону.");
        }
    }
    lastCommand = CommandType::None;
}

//=============================================Запросы данных============================================
/*---------------------------------  Зона 2: Команды чтения обработанных данных МИУС --------------------------------- */

void CommandWindow::sendQuaternionRequest() {

    lastCommand = CommandType::ProcessedQuaternionRequest;
    QByteArray packet = PacketBuilder::createLevel1Packet(REQUEST_READING, 0x18);
    PultErrors errors;
    if (m_pult->sendData(m_interfaceId, packet, this, errors)) {
//         onLogMessage("Data sent successfully.");
    } else {
         onLogMessage("Send error: " + errors.toString());
        return;
    }
    onLogMessage("📤 Запрос кватерниона");
}

void CommandWindow::sendAngularVelocityRequest() {

    lastCommand = CommandType::ProcessedAngularVelocityRequest;
    QByteArray packet = PacketBuilder::createLevel1Packet(REQUEST_READING, 0x14);
    PultErrors errors;
    if (m_pult->sendData(m_interfaceId, packet, this, errors)) {
//         onLogMessage("Data sent successfully.");
    } else {
         onLogMessage("Send error: " + errors.toString());
        return;
    }
    onLogMessage("📤 Запрос угловых скоростей");
}

void CommandWindow::sendQuaternionAndAngularVelocityRequest() {

    lastCommand = CommandType::ProcessedQuaternionAndVelocityRequest;
    QByteArray packet = PacketBuilder::createLevel1Packet(REQUEST_READING, 0x1C);
    PultErrors errors;
    if (m_pult->sendData(m_interfaceId, packet, this, errors)) {
//         onLogMessage("Data sent successfully.");
    } else {
         onLogMessage("Send error: " + errors.toString());
        return;
    }
    onLogMessage("📤 Запрос кватерниона и угловых скоростей");
}

void CommandWindow::sendAnglesRequest() {

    lastCommand = CommandType::ProcessedAnglesRequest;
    QByteArray packet = PacketBuilder::createLevel1Packet(REQUEST_READING, 0x12);
    PultErrors errors;
    if (m_pult->sendData(m_interfaceId, packet, this, errors)) {
//         onLogMessage("Data sent successfully.");
    } else {
         onLogMessage("Send error: " + errors.toString());
        return;
    }
    onLogMessage("📤 Запрос углов поворота");
}

void CommandWindow::processMiusDataResponse(const QByteArray &packet) {

    // -----*    Для таблицы  обработанных данных       *----
    chooseAdresOfTriple(); // Сопоставление адресов ДУСов выбранной тройки
    MiusDataBuffer &buffer = miusBufferMap[tripleNumberDUS]; // получаем буфер по номеру тройки

    // Используем байты с индексами 3 и 4 для извлечения dataSize
    uint8_t dataSize = static_cast<uint8_t>(packet[4]);

    // Извлечение команды (lsb_read) — это 5-й байт
    uint8_t lsb_read = static_cast<uint8_t>(packet[5]);

    // Извлечение полезной нагрузки (payload)
    const QByteArray payload = packet.mid(6, dataSize);

    QString payloadHex;
    for (char byte : payload) {
        payloadHex += QString::asprintf("%02X ", static_cast<uint8_t>(byte));
    }

    // Обработка запрещенной операции
    if ((lsb_read == 0x18 || lsb_read == 0x14 || lsb_read == 0x1C || lsb_read == 0x12) && payload.size() <= 8) {
        onLogMessage("\n❌ **Запрещенная операция** МИУС.");
        lastCommand = CommandType::None;
        onLogMessage("CommandType::None - Запрещенная операция** МИУС");
        return;
    }

    // Обработка случая lsb_read = 0
    if (lsb_read == 0) {
        onLogMessage("\n⚠️ **Обнаружено lsb_read = 0**. Требуется дополнительная проверка.");
        onLogMessage(QString("  ▸ **Данные пакета**: %1").arg(QString(packet.toHex())));
        onLogMessage("------------------------------------");
        lastCommand = CommandType::None; // 0905
        return;
    }

    auto bytesToSignedLong = [](const QByteArray &arr, int offset) -> qint32 {
        return static_cast<qint32>((arr[offset] << 24) |
                                   ((uint8_t)arr[offset + 1] << 16) |
                                   ((uint8_t)arr[offset + 2] << 8) |
                                   ((uint8_t)arr[offset + 3]));
    };

    // Обработка кватерниона (lsb_read == 0x18)
    if (lsb_read == 0x18 && payload.size() >= 16) { // Кватернион
        qint32 q0 = bytesToSignedLong(payload, 0);
        qint32 q1 = bytesToSignedLong(payload, 4);
        qint32 q2 = bytesToSignedLong(payload, 8);
        qint32 q3 = bytesToSignedLong(payload, 12);
        // -----*         Для таблицы             *----
        if (resultsViewer) {
            resultsViewer->addQuaternionData(
                tripleNumberDUS,
                addrX, addrY, addrZ,
                q0, q1, q2, q3
                );
        } else {
            onLogMessage("⚠️Откройте таблицу!");
        }
         onLogMessage("\n🧭 **Кватернион**:");
         onLogMessage(QString("  ▸ **q0** = %1").arg(q0));
         onLogMessage(QString("  ▸ **q1** = %1").arg(q1));
         onLogMessage(QString("  ▸ **q2** = %1").arg(q2));
         onLogMessage(QString("  ▸ **q3** = %1").arg(q3));
         onLogMessage("------------------------------------");
    }
    // Обработка угловых скоростей (lsb_read == 0x14)
    else if (lsb_read == 0x14 && payload.size() >= 12) { // Угловая скорость

        onLogMessage("\n💫 **Угловые скорости**:");
        onLogMessage(QString("  ▸ **ωx** = %1 grad/s").arg(bytesToSignedLong(payload, 0)));
        onLogMessage(QString("  ▸ **ωy** = %1 grad/s").arg(bytesToSignedLong(payload, 4)));
        onLogMessage(QString("  ▸ **ωz** = %1 grad/s").arg(bytesToSignedLong(payload, 8)));

        // -----*         Для таблицы             *----
        buffer.oxSpeed = bytesToSignedLong(payload, 0);
        buffer.oySpeed = bytesToSignedLong(payload, 4);
        buffer.ozSpeed = bytesToSignedLong(payload, 8);
        buffer.hasSpeed = true;

        if (resultsViewer) {
            resultsViewer->addVSKData(
            tripleNumberDUS,
            addrX, addrY, addrZ,
            buffer.oxSpeed, buffer.oySpeed, buffer.ozSpeed,
            freqHz);
        } else {
            onLogMessage("⚠️Откройте таблицу!");
        }
        buffer = MiusDataBuffer();
        onLogMessage("------------------------------------");
    }

    // Обработка кватерниона + угловых скоростей (lsb_read == 0x1C)
    else if (lsb_read == 0x1C && payload.size() >= 20) { // Кватернион + угловая скорость
        qint32 q0 = bytesToSignedLong(payload, 0);
        qint32 q1 = bytesToSignedLong(payload, 4);
        qint32 q2 = bytesToSignedLong(payload, 8);
        qint32 q3 = bytesToSignedLong(payload, 12);
        qint32 wx = bytesToSignedLong(payload, 16);
        qint32 wy = bytesToSignedLong(payload, 20);
        qint32 wz = bytesToSignedLong(payload, 24);

        onLogMessage("\n🧭 **Кватернион**:");
        onLogMessage(QString("  ▸ **q0** = %1").arg(q0));
        onLogMessage(QString("  ▸ **q1** = %1").arg(q1));
        onLogMessage(QString("  ▸ **q2** = %1").arg(q2));
        onLogMessage(QString("  ▸ **q3** = %1").arg(q3));

        onLogMessage("\n💫 **Угловые скорости**:");
        onLogMessage(QString("  ▸ **ωx** = %1 grad/s").arg(wx));
        onLogMessage(QString("  ▸ **ωy** = %1 grad/s").arg(wy));
        onLogMessage(QString("  ▸ **ωz** = %1 grad/s").arg(wz));
        onLogMessage("------------------------------------");
    }
    // Обработка углов (lsb_read == 0x12)
    else if (lsb_read == 0x12 && payload.size() >= 12) { // Углы (φx, φy, φz)

        onLogMessage("\n🌀 **Углы**:");
        onLogMessage(QString("  ▸ **φx** = %1°").arg(bytesToSignedLong(payload, 0)));
        onLogMessage(QString("  ▸ **φy** = %1°").arg(bytesToSignedLong(payload, 4)));
        onLogMessage(QString("  ▸ **φz** = %1°").arg(bytesToSignedLong(payload, 8)));
        onLogMessage("------------------------------------");

    }
    else {
        onLogMessage("\n⚠️ **Неизвестный тип ответа или неверный размер данных.**");
        onLogMessage("------------------------------------");
    }
    lastCommand = CommandType::None;
}

/*---------------------------------   Зона 3: Команды чтения сервисных данных ДУС --------------------------------- */

void CommandWindow::addReadAngularVelocityDusCommand() {

    lastCommand = CommandType::ServisAngularVelocityDusRequest;
    QByteArray packet = PacketBuilder::createLevel1Packet(REQUEST_READING, 0x08);
    PultErrors errors;
    if (m_pult->sendData(m_interfaceId, packet, this, errors)) {
//         onLogMessage("Data sent successfully.");
    } else {
         onLogMessage("Send error: " + errors.toString());
        return;
    }
    onLogMessage("📤 Запрос угловых скоростей");
}

void CommandWindow::addReadTemperaturesDusCommand() {

    lastCommand = CommandType::ServisTemperaturesDusRequest;
    QByteArray packet = PacketBuilder::createLevel1Packet(REQUEST_READING, 0x09);
    PultErrors errors;
    if (m_pult->sendData(m_interfaceId, packet, this, errors)) {
//         onLogMessage("Data sent successfully.");
    } else {
         onLogMessage("Send error: " + errors.toString());
        return;
    }
    onLogMessage("📤 Запрос температур");
}

void CommandWindow::addReadUptimeDusCommand() {

    lastCommand = CommandType::ServisUptimeDusRequest;
    QByteArray packet = PacketBuilder::createLevel1Packet(REQUEST_READING, 0x0A);
    PultErrors errors;
    if (m_pult->sendData(m_interfaceId, packet, this, errors)) {
//         onLogMessage("Data sent successfully.");
    } else {
         onLogMessage("Send error: " + errors.toString());
        return;
    }
    onLogMessage("📤 Запрос времени от вкл.");
}

void CommandWindow::addReadTripleNumberDusCommand() {

    lastCommand = CommandType::ServisTripleNumberDusRequest;
    QByteArray packet = PacketBuilder::createLevel1Packet(REQUEST_READING, 0x0B);
    PultErrors errors;
    if (m_pult->sendData(m_interfaceId, packet, this, errors)) {
//         onLogMessage("Data sent successfully.");
    } else {
         onLogMessage("Send error: " + errors.toString());
        return;
    }
    onLogMessage("📤 Запрос номера тройки");
}

void CommandWindow::addReadAnglesDusCommand() {

    lastCommand = CommandType:: ServisAnglesDusRequest;
    QByteArray packet = PacketBuilder::createLevel1Packet(REQUEST_READING, 0x0C);
    PultErrors errors;
    if (m_pult->sendData(m_interfaceId, packet, this, errors)) {
//         onLogMessage("Data sent successfully.");
    } else {
         onLogMessage("Send error: " + errors.toString());
        return;
    }
    onLogMessage("📤 Запрос углов поворота");
}

void CommandWindow::processDusServisDataResponse(const QByteArray &packet) {

    // -----*    Для таблицы  данных  ДУС     *----
    chooseAdresOfTriple(); // Сопоставление адресов ДУСов выбранной тройки
    DusDataBuffer &buffer = dusBufferMap[tripleNumberDUS]; // получаем буфер по номеру тройки

    uint8_t dataSize = static_cast<uint8_t>(packet[4]);
    uint8_t lsb_read = static_cast<uint8_t>(packet[5]);
    QByteArray payload = packet.mid(6, dataSize - 1);

    if (packet.toHex() == "aa043200010827" || packet.toHex() == "aa043200010920"
     || packet.toHex() == "aa043200010a29" || packet.toHex() == "aa043200010b2e"
     || packet.toHex() == "aa043200010c3b") {

        onLogMessage("\n❌ **Запрещенная операция**.");
        lastCommand = CommandType::None;
        return;
    }

    auto bytesToSignedLong = [](const QByteArray &arr, int offset) -> qint32 {
        return static_cast<qint32>((arr[offset] << 24) |
                                   ((uint8_t)arr[offset + 1] << 16) |
                                   ((uint8_t)arr[offset + 2] << 8) |
                                   ((uint8_t)arr[offset + 3]));
    };

    // Обработка угловых скоростей (lsb_read == 0x08)
    if (lsb_read == 0x08 && payload.size() >= 12) {

        // -----*         Для таблицы             *----
        buffer.oxSpeed = bytesToSignedLong(payload, 0);
        buffer.oySpeed = bytesToSignedLong(payload, 4);
        buffer.ozSpeed = bytesToSignedLong(payload, 8);
        buffer.hasSpeed = true;

        onLogMessage("\n💫 **Угловые скорости**:");
        onLogMessage(QString("  ▸ **ωx** = %1 grad/s").arg(bytesToSignedLong(payload, 0), 10));
        onLogMessage(QString("  ▸ **ωy** = %1 grad/s").arg(bytesToSignedLong(payload, 4), 10));
        onLogMessage(QString("  ▸ **ωz** = %1 grad/s").arg(bytesToSignedLong(payload, 8), 10));
        onLogMessage("------------------------------------");
    }

    // Обработка температур (lsb_read == 0x09)
    else if (lsb_read == 0x09 && payload.size() >= 12) {

        // -----*         Для таблицы             *----
        buffer.oxTemperature = bytesToSignedLong(payload, 0);
        buffer.oyTemperature = bytesToSignedLong(payload, 4);
        buffer.ozTemperature = bytesToSignedLong(payload, 8);
        buffer.hasTemperature = true;

        onLogMessage("\n🌡 **Температуры**:");
        onLogMessage(QString("  ▸ **Tx** = %1 °C").arg(bytesToSignedLong(payload, 0), 10));
        onLogMessage(QString("  ▸ **Ty** = %1 °C").arg(bytesToSignedLong(payload, 4), 10));
        onLogMessage(QString("  ▸ **Tz** = %1 °C").arg(bytesToSignedLong(payload, 8), 10));
        onLogMessage("------------------------------------");
    }

    // Обработка времени от включения (lsb_read == 0x0A)
    else if (lsb_read == 0x0A && payload.size() >= 12) {

        // -----*         Для таблицы             *----
        buffer.oxTime = bytesToSignedLong(payload, 0);
        buffer.oyTime = bytesToSignedLong(payload, 4);
        buffer.ozTime = bytesToSignedLong(payload, 8);
        buffer.hasTime = true;

        onLogMessage("\n⏱ **Время от включения**:");
        onLogMessage(QString("  ▸ **tx** = %1 s").arg(bytesToSignedLong(payload, 0), 10));
        onLogMessage(QString("  ▸ **ty** = %1 s").arg(bytesToSignedLong(payload, 4), 10));
        onLogMessage(QString("  ▸ **tz** = %1 s").arg(bytesToSignedLong(payload, 8), 10));
        onLogMessage("------------------------------------");
    }

    // Обработка номера тройки (lsb_read == 0x0B)
    else if (lsb_read == 0x0B && payload.size() >= 1) {
        uint8_t tripleId = static_cast<uint8_t>(payload[0]);
        onLogMessage("\n🔢 **Номер тройки**:");
        onLogMessage(QString("  ▸ **ID** = %1").arg(tripleId));
        onLogMessage("------------------------------------");
    }

    // Обработка углов (lsb_read == 0x0C)
    else if (lsb_read == 0x0C && payload.size() >= 12) {

        // -----*         Для таблицы             *----
        buffer.oxAngle = bytesToSignedLong(payload, 0);
        buffer.oyAngle = bytesToSignedLong(payload, 4);
        buffer.ozAngle = bytesToSignedLong(payload, 8);
        buffer.hasAngle = true;

        onLogMessage("\n🌀 **Углы**:");
        onLogMessage(QString("  ▸ **φx** = %1°").arg(bytesToSignedLong(payload, 0), 10));
        onLogMessage(QString("  ▸ **φy** = %1°").arg(bytesToSignedLong(payload, 4), 10));
        onLogMessage(QString("  ▸ **φz** = %1°").arg(bytesToSignedLong(payload, 8), 10));
        onLogMessage("------------------------------------");
    }

    // Обработка кватерниона (lsb_read == 0x18)
    else if (lsb_read == 0x18 && payload.size() >= 16) {
        qint32 q0 = bytesToSignedLong(payload, 0);
        qint32 q1 = bytesToSignedLong(payload, 4);
        qint32 q2 = bytesToSignedLong(payload, 8);
        qint32 q3 = bytesToSignedLong(payload, 12);

        onLogMessage("\n🧭 **Кватернион**:");
        onLogMessage(QString("  ▸ **q0** = %1").arg(q0, 10));
        onLogMessage(QString("  ▸ **q1** = %1").arg(q1, 10));
        onLogMessage(QString("  ▸ **q2** = %1").arg(q2, 10));
        onLogMessage(QString("  ▸ **q3** = %1").arg(q3, 10));
        onLogMessage("------------------------------------");
    }

    else {
        onLogMessage("\n⚠️ **Неизвестный тип ответа или размер**.");
        onLogMessage("------------------------------------");
    }

    if (dusTableWindow) {
        // -----*    Для таблицы  обработанных данных       *----
         if (buffer.hasSpeed && buffer.hasAngle && buffer.hasTemperature && buffer.hasTemperature) {
            dusTableWindow->addDusDataRow(tripleNumberDUS, addrX, "OX", buffer.oxSpeed, buffer.oxAngle, buffer.oxTime, buffer.oxTemperature);
            dusTableWindow->addDusDataRow(tripleNumberDUS, addrY, "OY", buffer.oySpeed, buffer.oyAngle, buffer.oyTime, buffer.oyTemperature);
            dusTableWindow->addDusDataRow(tripleNumberDUS, addrZ, "OZ", buffer.ozSpeed, buffer.ozAngle, buffer.ozTime, buffer.ozTemperature);
            buffer = DusDataBuffer();
         }
    } else {
        onLogMessage("⚠️Откройте таблицу!");
    }
    lastCommand = CommandType::None;
}
/*---------------------------------  // Зона 4: Команды чтения сервисных данных МИУС --------------------------------- */

void CommandWindow::addReadAngularVelocityMiussCommand() {

    lastCommand = CommandType::ServisAngularVelocityMiussRequest;
    QByteArray packet = PacketBuilder::createLevel1Packet(REQUEST_READING, 0x00);
    PultErrors errors;
    if (m_pult->sendData(m_interfaceId, packet, this, errors)) {
//         onLogMessage("Data sent successfully.");
    } else {
         onLogMessage("Send error: " + errors.toString());
        return;
    }
    onLogMessage("📤 Запрос накопленных угловых скоростей");
}

void CommandWindow::addReadTemperatureMiussCommand() {

    lastCommand = CommandType::ServisTemperatureMiussRequest;
    QByteArray packet = PacketBuilder::createLevel1Packet(REQUEST_READING, 0x01);
    PultErrors errors;
    if (m_pult->sendData(m_interfaceId, packet, this, errors)) {
//         onLogMessage("Data sent successfully.");
    } else {
         onLogMessage("Send error: " + errors.toString());
        return;
    }
    onLogMessage("📤 Запрос температуры МИУС");
}

void CommandWindow::addReadStatusesDusCommand() {

    lastCommand = CommandType::ServisReadStatusesDus;
    QByteArray packet = PacketBuilder::createLevel1Packet(REQUEST_READING, 0x02);
    PultErrors errors;
    if (m_pult->sendData(m_interfaceId, packet, this, errors)) {
//         onLogMessage("Data sent successfully.");
    } else {
         onLogMessage("Send error: " + errors.toString());
        return;
    }
    onLogMessage("📤 Запрос статусов ДУС");
}

void CommandWindow::addReadStatusesMicrochipsMiussCommand() {

    lastCommand = CommandType::ServisReadStatusesMicrochipsMiuss;
    QByteArray packet = PacketBuilder::createLevel1Packet(REQUEST_READING, 0x03);
    PultErrors errors;
    if (m_pult->sendData(m_interfaceId, packet, this, errors)) {
//         onLogMessage("Data sent successfully.");
    } else {
         onLogMessage("Send error: " + errors.toString());
        return;
    }
    onLogMessage("📤 Запрос статусов микросхем");
}

void CommandWindow::addReadAnglesMiussCommand() {

    lastCommand = CommandType:: ServisAnglesMiuss;
    QByteArray packet = PacketBuilder::createLevel1Packet(REQUEST_READING, 0x04);
    PultErrors errors;
    if (m_pult->sendData(m_interfaceId, packet, this, errors)) {
//         onLogMessage("Data sent successfully.");
    } else {
         onLogMessage("Send error: " + errors.toString());
        return;
    }
    onLogMessage("📤 Запрос углов");
}

void CommandWindow::processMiusServisDataResponse(const QByteArray &packet) {

    // -----*    Для таблицы  обработанных данных       *----
    chooseAdresOfTriple(); // Сопоставление адресов ДУСов выбранной тройки
    MiusUnProcessedDataBuffer &buffer = miusUnProcessedBufferMap[tripleNumberDUS]; // получаем буфер по номеру тройки

    uint8_t dataSize = static_cast<uint8_t>(packet[4]);
    uint8_t lsb_read = static_cast<uint8_t>(packet[5]);
    QByteArray payload = packet.mid(6, dataSize);

    QString payloadHex;
    for (char byte : payload) {
        payloadHex += QString::asprintf("%02X ", static_cast<uint8_t>(byte));
    }
    auto bytesToSignedLong = [](const QByteArray &arr, int offset) -> qint32 {

        return static_cast<qint32>((arr[offset] << 24) |
                                   ((uint8_t)arr[offset + 1] << 16) |
                                   ((uint8_t)arr[offset + 2] << 8) |
                                   ((uint8_t)arr[offset + 3]));
    };

    if (packet.toHex() == "aa04320001001f" || packet.toHex() == "aa043200010118"
     || packet.toHex() == "aa043200010211" || packet.toHex() == "aa043200010316"
     || packet.toHex() == "aa043200010403") {

        onLogMessage("\n❌ **Запрещенная операция**.");
        lastCommand = CommandType::None;
        return;
    }
    // Обработка запроса накопленных угловых скоростей (lsb_read == 0x00)
    if (lsb_read == 0x00 && payload.size() >= 12) {

        // -----*         Для таблицы необработанных данных        *----
        buffer.oxSpeed = bytesToSignedLong(payload, 0);
        buffer.oySpeed = bytesToSignedLong(payload, 4);
        buffer.ozSpeed = bytesToSignedLong(payload, 8);
        buffer.hasSpeed = true;

        onLogMessage("\n💫 **Накопленные угловые скорости**:");
        onLogMessage(QString("  ▸ **ωx** = %1 grad/s").arg(bytesToSignedLong(payload, 0), 10));
        onLogMessage(QString("  ▸ **ωy** = %1 grad/s").arg(bytesToSignedLong(payload, 4), 10));
        onLogMessage(QString("  ▸ **ωz** = %1 grad/s").arg(bytesToSignedLong(payload, 8), 10));
        onLogMessage("------------------------------------");
    }


    // Обработка запроса температуры МИУС (lsb_read == 0x01)
    else if (lsb_read == 0x01 && payload.size() >= 2) {
        int16_t temperature = static_cast<int16_t>((payload[0] << 8) | payload[1]);
        onLogMessage("\n🌡 **Температура МИУС**:");
        onLogMessage(QString("  ▸ **Температура** = %1").arg(temperature));
        onLogMessage("------------------------------------");
    }

    // Обработка запроса статусов ДУС (lsb_read == 0x02)
    else if (lsb_read == 0x02 && payload.size() >= 1) {
        onLogMessage("\n🔧 **Статус ДУС**:");
        onLogMessage("------------------------------------");
        uint8_t dusStatus = payload[0];
        bool sensorStatus[6] = {false};
        for (int i = 0; i < 6; ++i) {
             sensorStatus[i] = (dusStatus >> i) & 0x01;
             onLogMessage(QString("  ▸ **Датчик %1 ** = %2")
                          .arg(i+1)
                          .arg(sensorStatus[i] ? "Исправен" : "Неисправен"));
        }
        QString stateX = getDusStateByAdress(addrX, sensorStatus);
        QString stateY = getDusStateByAdress(addrY, sensorStatus);
        QString stateZ = getDusStateByAdress(addrZ, sensorStatus);
        qDebug() <<"addrX:" << addrX << "->" << stateX;
        qDebug() <<"addrY:" << addrY << "->" << stateY;
        qDebug() <<"addrZ:" << addrZ << "->" << stateZ;

        buffer.oxState = stateX;
        buffer.oyState = stateY;
        buffer.ozState = stateZ;
        buffer.hasState = true;
    }

    // Обработка запроса статусов микросхем (lsb_read == 0x03)
    else if (lsb_read == 0x03 && payload.size() >= 2) {
        payload = packet.mid(6, dataSize - 1);
        QString payloadHex;
        uint16_t twoPayloadBytes = (static_cast<uint16_t>(payload[0]) << 8) | payload[1];
        bool micro[7] = {false};
        micro[0] = (twoPayloadBytes >> 15) & 0x1;
        micro[1] = (twoPayloadBytes >> 14) & 0x1;
        micro[2] = (twoPayloadBytes >> 13) & 0x1;
        micro[3] = (twoPayloadBytes >> 12) & 0x1;
        micro[4] = (twoPayloadBytes >> 11) & 0x1;
        micro[5] = (twoPayloadBytes >> 1) & 0x1;
        micro[6] = (twoPayloadBytes >> 0) & 0x1;
        onLogMessage("\n🔌 **Статусы микросхем**:");
        const char* chipNames[] = {"flashStatus", "sramStatus","rs485_2Status","rs485_1Status",
                                  "spacewire_3Status", "spacewire_2Status", "spacewire_1Status"};
        for (int i = 0; i < 7; ++i){
            onLogMessage(QString("Микросхема  %1 (%2) = (%3)")
                         .arg(i+1)
                         .arg(chipNames[i])
                         .arg(micro[i]?"Исправна":"Неисправна"));
        }
        if(miusHard) {
            if (!micro[0]) {
                miusHard->addHardwareData("flash", "Неисправность");
            } else {
                 miusHard->addHardwareData("flash", "Норма");
            }
            if (!micro[1]) {
                miusHard->addHardwareData("SRAM", "Неисправность");
            } else {
                miusHard->addHardwareData("SRAM", "Норма");
            }
            if (!micro[3]) {
                miusHard->addHardwareData("RS_1", "Неисправность");
            } else {
                 miusHard->addHardwareData("RS_1", "Норма");
            }
            if (!micro[2]) {
                 miusHard->addHardwareData("RS_2", "Неисправность");
            } else {
                miusHard->addHardwareData("RS_2", "Норма");
            }
            if (!micro[4]) {
               miusHard->addHardwareData("spacewire_3", "Неисправность");
            } else {
                miusHard->addHardwareData("spacewire_3", "Норма");
            }
            if (!micro[5]) {
                miusHard->addHardwareData("spacewire_2", "Неисправность");
            } else {
                miusHard->addHardwareData("spacewire_2", "Норма");
            }
            if (!micro[6]) {
                miusHard->addHardwareData("spacewire_1", "Неисправность");
            } else {
                miusHard->addHardwareData("spacewire_1", "Норма");
            }
        } else {
            onLogMessage("⚠️Откройте таблицу!");
        }

         onLogMessage("------------------------------------");
    }

    // Обработка углов (lsb_read == 0x04)
    else if (lsb_read == 0x04 && payload.size() >= 12) {

        // -----*         Для таблицы необработанных данных        *----
        buffer.oxAngle = bytesToSignedLong(payload, 0);
        buffer.oyAngle = bytesToSignedLong(payload, 4);
        buffer.ozAngle = bytesToSignedLong(payload, 8);
        buffer.hasAngle = true;

        onLogMessage("\n🌀 **Углы**:");
        onLogMessage(QString("  ▸ **φx** = %1°").arg(bytesToSignedLong(payload, 0), 10));
        onLogMessage(QString("  ▸ **φy** = %1°").arg(bytesToSignedLong(payload, 4), 10));
        onLogMessage(QString("  ▸ **φz** = %1°").arg(bytesToSignedLong(payload, 8), 10));
        onLogMessage("------------------------------------");
    }
    else {
         onLogMessage("\n⚠️ **Неизвестный тип ответа или размер**.");
         onLogMessage("------------------------------------");
    }
    if (miusHard) {
        // -----*    Для таблицы  обработанных данных       *----
        if (buffer.hasSpeed && buffer.hasAngle && buffer.hasState) {

            miusHard->addUnProcessedData(tripleNumberDUS, addrX, "OX", buffer.oxSpeed, buffer.oxAngle, buffer.oxState);
            miusHard->addUnProcessedData(tripleNumberDUS, addrY, "OY", buffer.oySpeed, buffer.oyAngle, buffer.oyState);
            miusHard->addUnProcessedData(tripleNumberDUS, addrZ, "OZ", buffer.ozSpeed, buffer.ozAngle, buffer.ozState);
            buffer = MiusUnProcessedDataBuffer();
        }
    } else {
        onLogMessage("⚠️Откройте таблицу!");
    }

    lastCommand = CommandType::None;
}

void CommandWindow::chooseAdresOfTriple() { // Выбор адресов выбранной тройки

    index = tripleNumberDUS - 1;
    if (index >= 0 && index < 20) {
        addrX = sensorAddresses[index][0];
        addrY = sensorAddresses[index][1];
        addrZ = sensorAddresses[index][2];
    }
    if (miusPoller) {
        miusPoller->setTripeInfo(index + 1, addrX, addrY, addrZ);
    } else {
        qDebug() << "Не передали указатель на опросника в окно команд или его вообще нет";
    }

}
QString CommandWindow::getDusStateByAdress(uint8_t addr, const bool sensorStatus[6]) {

    if (addr >= 1 && addr <= 6) {
        return sensorStatus[addr - 1] ? "Исправен" : "Неисправен";
    } else {
        return "Неизвестен";
    }
}
void CommandWindow::onDeliveryFailed(QObject *origin) {

    if (origin == this) {
        onLogMessage(QString("❌ Нет связи с МИУС адр. 16"));
        lastCommand = CommandType::None; // 20.06
    }
}
void CommandWindow::onLockCommand(QObject *origin) {

    if (origin == this) {
        onLogMessage(QString("❌ Была отправлена несуществующая команда или неправильные данные"));
        lastCommand = CommandType::None; // 20.06
    }
}
void CommandWindow::showTableDusDataResponse() {

    if (!dusTableWindow) {
        dusTableWindow = new DusDataTableWindow();
    }
    dusTableWindow->show();
}

void CommandWindow::showTableMiusQuaternionResponseTwice() {

    if (!resultsViewer) {
        resultsViewer = new ResultsViewer();
         qDebug() << " new resultsViewer" << resultsViewer;
        emit resultsViewerCreated(resultsViewer);
    }
    resultsViewer->show();
}

void CommandWindow::showTableMiusServisDatAndMicroDataResponse() {

    if (!miusHard) {
        miusHard = new MiusDataViewer();
    }
    miusHard->show();
}

void CommandWindow::setFreqHz(int freq) {

    freqHz = freq;
}

void CommandWindow::competeMiusServisData() {

    addReadAnglesMiussCommand();
    QTimer::singleShot(200,this,[this](){
        addReadStatusesMicrochipsMiussCommand();
        QTimer::singleShot(200,this,[this](){
            addReadStatusesDusCommand();
            QTimer::singleShot(200,this,[this](){
                addReadAngularVelocityMiussCommand();

            });
        });
    });
}
void CommandWindow::competeDusServisData() {

    addReadAngularVelocityDusCommand();
    QTimer::singleShot(200,this,[this](){
        addReadTemperaturesDusCommand();
        QTimer::singleShot(200,this,[this](){
            addReadUptimeDusCommand();
            QTimer::singleShot(200,this,[this](){
                addReadAnglesDusCommand();

            });
        });
    });
}

void CommandWindow::competeMiusData() {

    sendQuaternionRequest();
    QTimer::singleShot(200,this,[this](){
        sendAngularVelocityRequest();
        });
}

void CommandWindow::setMiusPoller(MiusPoller *poller) {
    this->miusPoller = poller;
    qDebug() << "CommandWindow take poller for send 3 him";
}
