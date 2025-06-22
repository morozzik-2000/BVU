#ifndef COMMANDWINDOW_H
#define COMMANDWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "serialportmanager.h"
#include "packetbuilder.h"
#include "worktripledialog.h"
#include "matrixinputdialog.h"
#include "quaternionrewritedialog.h"
#include "freqtabledialog.h"
#include "dusdatatablewindow.h"
#include "miusunprocesseddatatablewindow.h"
#include "hardwaremiusdatatabewindow.h"
#include "pultudpsend.h"
#include "comparisontablewindow.h"
#include "resultsviewer.h"
#include "miusdataviewer.h"
#include "miuspoller.h"



class CommandWindow : public QWidget {
    Q_OBJECT
public:
    explicit CommandWindow(pultUdpSend *pult, uint8_t interfaceId, QWidget *parent = nullptr);
    ~CommandWindow();
    // int m_addressByte;
    pultUdpSend *m_pult;
    uint8_t m_interfaceId;
    QString ip = "192.168.2.200";
    quint16 port = 32000;
    uint32_t baseAddress = BASE_UART_ADDRESS;
    void setFreqHz(int freq);
    void setMiusPoller(MiusPoller *poller);

signals:
    void sendCommand(const QByteArray &command);
    void onLogMessage(const QString &message);
    void resultsViewerCreated(ResultsViewer *viewer); // эимтим сигнал с указателем на таблицу чтобы закинуть ее  в мейн

private slots:

    void onResponseReceived(const QByteArray &data, QObject *origin);
    void onDeliveryFailed(QObject *origin);
    void onLockCommand(QObject *origin);

private:
    void setupUI();

    struct PendingCommand {
        QObject* origin;
        int expectedLength;
    };

    // Зона 1: Команды управления режимом МИУС
    QPushButton *btnCriticalMode;
    QPushButton *btnWorkTriple;
    QPushButton *btnRewriteMatrix;
    QPushButton *btnRewriteQuaternion;
    QPushButton *btnCriticalModeOff;
    QPushButton *btnFreq;
    void addCriticalModeCommand();
    void addWorkTripleCommand();
    void addRewriteMatrixCommand();
    void addRewriteQuaternionCommand();
    void addCriticalModeOffCommand();
    void addFreq();
    void rewriteTripleCommand();
    int tripleNumberDUS; // номер тройки в функции выьора тройки для заполнения в таблицы
    void processTripleResponse(const QByteArray &packet);
    void rewriteMatrixCommand();
    QByteArray createMatrixRewriteData(int tripleNum, const QVector<int32_t> &values);
    void processMatrixResponse(const QByteArray &packet);
    void rewriteQuaternionCommand();
    void processQuaternionResponse(const QByteArray &packet);

    // Зона 2: Команды чтения обработанных данных МИУС
    QPushButton *btnReadQuaternion;
    QPushButton *btnReadAngularVelocity;
    QPushButton *btnReadQuaternionAndAngularVelocity;
    QPushButton *btnReadAngles;
    QPushButton *shareTableMiusQuaternionResponseTwice;
    QPushButton *btnCompeteMiusData;
    void sendQuaternionRequest();
    void sendAngularVelocityRequest();
    void sendQuaternionAndAngularVelocityRequest();
    void sendAnglesRequest();
    void showTableMiusQuaternionResponseTwice();
    void processMiusDataResponse(const QByteArray &packet);
    void competeMiusData();

    // Зона 3: Команды чтения сервисных данных ДУС
    QPushButton *btnReadAngularVelocityDus;
    QPushButton *btnReadTemperaturesDus;
    QPushButton *btnReadUptimeDus;
    QPushButton *btnReadTripleNumberDus;
    QPushButton *btnReadAnglesDus;
    QPushButton *shareTableDusServisDataResponse;
    QPushButton *shareTableDusResponse;
    QPushButton *btnCompeteDusServisData;
    void addReadAngularVelocityDusCommand();
    void addReadTemperaturesDusCommand();
    void addReadUptimeDusCommand();
    void addReadTripleNumberDusCommand();
    void addReadAnglesDusCommand();
    void processDusServisDataResponse(const QByteArray &packet);
    void showTableDusDataResponse();
    void competeDusServisData();

    // Зона 4: Команды чтения сервисных данных МИУС
    QPushButton *btnReadAngularVelocityMiuss;
    QPushButton *btnReadTemperatureMiuss;
    QPushButton *btnReadStatusesDus;
    QPushButton *btnReadStatusesMicrochipsMiuss;
    QPushButton *btnReadAnglesMiuss;
    QPushButton *shareTableMiusServisDatAndMicroDataResponse;
    QPushButton *btnCompeteMiusServisData;
    void addReadAngularVelocityMiussCommand();
    void addReadTemperatureMiussCommand();
    void addReadStatusesDusCommand();
    void addReadStatusesMicrochipsMiussCommand();
    void addReadAnglesMiussCommand();
    void processMiusServisDataResponse(const QByteArray &packet);
    void showTableMiusServisDatAndMicroDataResponse();
    void competeMiusServisData();

    enum CommandType {
        None,
        Triple,
        Matrix,
        QQuaternion,
        Frequensy,
        ProcessedQuaternionRequest,
        ProcessedAngularVelocityRequest,
        ProcessedQuaternionAndVelocityRequest,
        ProcessedAnglesRequest,
        ServisAngularVelocityDusRequest,
        ServisTemperaturesDusRequest,
        ServisUptimeDusRequest,
        ServisTripleNumberDusRequest,
        ServisAnglesDusRequest,
        ServisAngularVelocityMiussRequest,
        ServisTemperatureMiussRequest,
        ServisReadStatusesDus,
        ServisReadStatusesMicrochipsMiuss,
        ServisAnglesMiuss
    };
    CommandType lastCommand = CommandType::None;

//    int sensorAddresses[20][3] = {
//        {1, 2, 3}, {1, 2, 4}, {1, 2, 5}, {1, 2, 6}, {1, 3, 4},
//        {1, 3, 5}, {1, 3, 6}, {1, 4, 5}, {1, 4, 6}, {1, 5, 6},
//        {2, 3, 4}, {2, 3, 5}, {2, 3, 6}, {2, 4, 5}, {2, 4, 6},
//        {2, 5, 6}, {3, 4, 5}, {3, 4, 6}, {3, 5, 6}, {4, 5, 6}
//    };
    int sensorAddresses[20][3] = {
        {1, 3, 5}, {1, 3, 6}, {1, 4, 5}, {1, 4, 6}, {1, 5, 3},
        {1, 5, 4}, {1, 6, 3}, {1, 6, 4}, {2, 3, 5}, {2, 3, 6},
        {2, 4, 5}, {2, 4, 6}, {2, 5, 3}, {2, 5, 4}, {2, 6, 3},
        {3, 1, 5}, {3, 1, 6}, {3, 2, 5}, {4, 1, 6}, {6, 2, 3}
    };
    int index;
    int addrX;
    int addrY;
    int addrZ;
    void chooseAdresOfTriple(); // Выбор адресов ДУСов выбранной тройки

    // -----*    Для таблицы  обработанных данных       *----
     ResultsViewer *resultsViewer;
    struct MiusDataBuffer {
        bool hasSpeed = false;
        bool hasAngle = false;
        qint32 oxSpeed = 0, oySpeed = 0, ozSpeed = 0;
        qint32 oxAngle = 0, oyAngle = 0, ozAngle = 0;
    };
    QMap<int, MiusDataBuffer> miusBufferMap; // ключ — tripleNumberDUS
    double frequency = 10; // Частота опроса МИУС

    // -----*    Для таблицы  данных  ДУС     *----
    DusDataTableWindow *dusTableWindow;
    struct DusDataBuffer {
        bool hasSpeed = false;
        bool hasAngle = false;
        bool hasTime = false;
        bool hasTemperature = false;
        qint32 oxSpeed = 0, oySpeed = 0, ozSpeed = 0;
        qint32 oxAngle = 0, oyAngle = 0, ozAngle = 0;
        qint32 oxTime = 0, oyTime = 0, ozTime = 0;
        qint32 oxTemperature = 0, oyTemperature = 0, ozTemperature = 0;
    };
    QMap<int, DusDataBuffer> dusBufferMap; // ключ — tripleNumberDUS


    // -----*    Для таблицы  необработанных данных  МИУС     *----
    MiusDataViewer *miusHard;
    struct MiusUnProcessedDataBuffer {
        bool hasSpeed = false;
        bool hasAngle = false;
        bool hasState = false;
        qint32 oxSpeed = 0, oySpeed = 0, ozSpeed = 0;
        qint32 oxAngle = 0, oyAngle = 0, ozAngle = 0;
        QString oxState = 0, oyState = 0, ozState = 0;
    };
    QMap<int, MiusUnProcessedDataBuffer> miusUnProcessedBufferMap; // ключ — tripleNumberDUS
    QString getDusStateByAdress(uint8_t addr, const bool sensorStatus[6]);

    int freqHz = 1;
    MiusPoller *miusPoller = nullptr;


};

#endif // COMMANDWINDOW_H
