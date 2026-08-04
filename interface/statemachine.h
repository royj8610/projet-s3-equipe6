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
#include <cmath>


class StateMachine
{
public:
    // ENUM
    enum ButtonType {
      START,
      STOP,
      RESET
    };

    // Constants
    static constexpr double H_PENDULE = 0;
    static constexpr double H_OBSTACLE = 0;
    static constexpr double TARGET_X_DROP = 1.2; //m
    static constexpr double TARGET_X_PICKUP = 0;
    static constexpr double PI = 3.14159265;
    static constexpr double POSITION_TOLERANCE = 0.05;
    static constexpr double ANGLE_TOLERANCE = 0.05;
    static constexpr double SPEED_TOLERANCE = 0.05;


    // Méthodes
    StateMachine();
    void update(QString state, double angle, double position, double speed, double angularSpeed);
    QString getState();
    double getTargetX();
    uint8_t getTreeNum();
    void sendButtonCommand(StateMachine::ButtonType buttonType);
    bool verifyTolerance(double valeur);

private :
    QString nextState_ = "";
    double nextTargetX_ = 0;
    bool shouldStart_ = false;
    uint8_t treeNum_ = 0;

};

#endif // STATEMACHINE_H
