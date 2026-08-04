#include "statemachine.h"

//-------------------------------------------
// Constructeur
//-------------------------------------------
StateMachine::StateMachine()
{
    // Constructeur

}

//-------------------------------------------
// Functions publics
//-------------------------------------------
void StateMachine::update(QString state, double position, double speed, double angle, double angularSpeed, bool hasExtended){

    if(state == "IDLE"
            && shouldStart_)
    {
        nextState_ = "STABILIZE";
        nextTargetX_ = 0.5;
        shouldStart_ = false;
    }
    else if(state == "STABILIZE"
            && position > 0.4
            && nextTargetX_ == 0.5)
    { 
            nextState_ = "SWING";
    }
    else if(state == "SWING"
            && (position > 0.7 ))
    {

            nextState_ = "STABILIZE";
            nextTargetX_ = 1.2;

    }
    else if(state == "STABILIZE"
            && (qAbs(position - 1.2) <= POSITION_TOLERANCE)
            && (qAbs(angularSpeed - 0) <= SPEED_TOLERANCE)
            && (qAbs(angle - 0) <= ANGLE_TOLERANCE))
    {

        nextState_ = "DROP";
        qDebug() << "Changed to drop";
    }
    else if(state == "DROP"
            && hasExtended)
    {
        qDebug() << "Changed to moveback";
        nextState_ = "MOVE_BACK";
        nextTargetX_ = 0;
    }
    else if (state == "MOVE_BACK"
             && (qAbs(position - nextTargetX_) <= POSITION_TOLERANCE)
             && (qAbs(speed - 0) <= SPEED_TOLERANCE))
    {
        treeNum_ += 1;

        nextState_ = "IDLE";
        shouldStart_ = false;
    }
    /*
    else
    {
        qDebug() << "State inconnu! On revient IDLE";
        nextState_ = "IDLE";
    }*/

    if(lastState_ != state)
    {
        qDebug()
                << "Last State" << lastState_
                << "New State" << state
                << "Position" << position
                << "Vitesse"    << speed
                << "Angle"    << angle
                << "AngVel"    << angularSpeed
                << "Extended" << hasExtended;
        lastState_ = state;
    }


}

void StateMachine::sendButtonCommand(StateMachine::ButtonType buttonType)
{
    // Modifie l'état du StateMachine selon le type de Button appuyé
    switch(buttonType){
    case StateMachine::ButtonType::START:{
        shouldStart_ = true;
        break;
    }
    case StateMachine::ButtonType::STOP:{
        shouldStart_ = false;
        break;
    }
    case StateMachine::ButtonType::RESET:{
        shouldStart_ = false;
        break;
    }
    }
}

//-------------------------------------------
// Getters
//-------------------------------------------
QString StateMachine::getState (){
    return nextState_;

}

double StateMachine::getTargetX(){
    return nextTargetX_;
}

uint8_t StateMachine::getTreeNum(){

    return treeNum_;
}
