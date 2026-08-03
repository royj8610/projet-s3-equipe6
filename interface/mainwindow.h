#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QCloseEvent>
#include <QDebug>
#include <QtWidgets>
#include <QJsonObject>
#include <QJsonDocument>
#include <QSerialPortInfo>

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts>

// Propres librairies
#include "serialprotocol.h"
#include "statemachine.h"

// Classe definissant l'application
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    const qint32 BAUD_RATE = 115200;

    explicit MainWindow(int updateRate, QWidget *parent = nullptr);
    explicit MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow() override;
    void closeEvent(QCloseEvent *event) override;

    void sendMessage(QString msg);
    void sendButton(QString msg);
    void setUpdateRate(int rateMs);

    void onPeriodicUpdate();
    void onMessageReceived(QString);//à changer

    QString getState() const;
    double getPosition() const;
    double getAngle() const;
    double getTime () const;


private slots:
    void receiveFromSerial(QString);
    void resetButtonClicked(); //changé
    void startButtonClicked(); //changé
    void stopButtonClicked(); //changé

    void startSerialCom(QString);

private:
    void connectTimers(int updateRate);
    void connectButtons();
    void connectSerialPortRead();
    void connectComboBox();
    void portCensus();

    bool record = false;
    QTimer updateTimer_;
    QString msgReceived_{""};
    QString msgBuffer_{""};
    SerialProtocol* serialCom_=nullptr;

//changé
    QChart chartA_;
    QLineSeries seriesApos_;
    QLineSeries seriesAangle_;

    QChart chartB_;
    QLineSeries seriesBpos_;
    QLineSeries seriesBangle_;

    QString state = "Unknown";
    double position = 0.0;
    double angle = 0.0;
    double time = 0.0;
    double speed =0.0;
    double angularSpeed = 0.0;
    double acceleraionX = 0.0;
    double targetPosition = 0.0;

    bool slipDetected = false;
    bool arduinoReady = false;

    StateMachine stateMachine_;






protected:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
