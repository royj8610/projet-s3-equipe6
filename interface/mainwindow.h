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

    void sendMessage(QString msg);//à changer
    void setUpdateRate(int rateMs);

    void onPeriodicUpdate();
    void onMessageReceived(QString);//à changer

private slots:
    void receiveFromSerial(QString);
    void resetButtonClicked(); //changé
    void startButtonClicked(); //changé
    void stopButtonClicked(); //changé
    void stateMachine();

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





protected:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
