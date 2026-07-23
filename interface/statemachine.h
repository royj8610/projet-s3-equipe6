#ifndef STATEMACHINE_H
#define STATEMACHINE_H

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


class StateMachine
{
public:
    StateMachine();
    void update(QString state);
    QString getState();
    double getTargetX();
private :
    QString nextState_ = "";
    double nextTargetX_ = 0;

};

#endif // STATEMACHINE_H
