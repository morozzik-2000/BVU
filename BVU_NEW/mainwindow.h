#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include "serialportmanager.h"
#include "commandwindow.h"
#include "packetbuilder.h"
#include "miuspoller.h"
#include <QThread>
#include "pultudpsend.h"
#include "resultsviewer.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int userDisplayValue;
    int addressByte;
    void onSendClicked();
    pultUdpSend *m_pult;
    void connectToPortImitation();
    void setResultsViewer(ResultsViewer *resultsViewer);

signals:

private slots:

    void onLogMessage(const QString &message);
    void onResponseReceived(const QByteArray &data, QObject *origin);
    void onDeliveryFailed(QObject *origin);


private:
    QTextEdit *logText;
    QPushButton *btnConnect;
    QPushButton *btnSend;
    QComboBox *comboPort;
    QComboBox *comboBaud;
    QSpinBox *spinAddress;

    void setupUI();
    QByteArray buildTestCommand();
    uint8_t calculateCRC8(const QByteArray &data);

    QPushButton *btnStartPoll;
    QPushButton *btnStopPoll;
    QComboBox *comboIntervalValue;
    QComboBox *comboIntervalUnit;
    QComboBox *comboDuration;
    MiusPoller *miusPoller;
    void startMiusPolling();
    void stopMiusPolling();
    QComboBox *comboFrequencyHz;
    QComboBox *comboDurationUnit;
    CommandWindow *commandWindow = nullptr;
    QSpinBox *durationValueS;
    QComboBox *comboInterface;
    QSpinBox *spinAddressBvu;
    QSpinBox *spinAddressMius;
    QSpinBox *spinFrequency;

    QString ip = "192.168.2.200";
    quint16 port = 32000;
    uint32_t baseAddress = BASE_UART_ADDRESS;

    int freqHz;

    void closeEvent(QCloseEvent* event) override;
    ResultsViewer *resultsViewer = nullptr;


    int portForPoller; // 20.06


};

#endif // MAINWINDOW_H
