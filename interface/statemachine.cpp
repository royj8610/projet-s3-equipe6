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
void StateMachine::update(QString state, double position, double speed, double angle, double angularSpeed){

    if(state == "IDLE"
            && shouldStart_)
    {
        nextState_ = "STABILIZE";
        nextTargetX_ = 0.5;


    }
    else if(state == "STABILIZE"
            && position > 0.4)
    { 
            nextState_ = "SWING";
    }
    else if(state == "SWING"
            && (qAbs(position - 0.7) <= POSITION_TOLERANCE) )
    {

            nextState_ = "STABILIZE";
            nextTargetX_ = 1.2;

    }
    else if(state == "STABILIZE"
            && (qAbs(position - nextTargetX_) <= POSITION_TOLERANCE)
            && (qAbs(angularSpeed - 0) <= SPEED_TOLERANCE)
            && (qAbs(angle - 0) <= ANGLE_TOLERANCE))
    {

        nextState_ = "MOVE_BACK";
        nextTargetX_ = 0;

    }
    else if (state == "MOVE_BACK"
             && (qAbs(position - nextTargetX_) <= POSITION_TOLERANCE)
             && (qAbs(speed - 0) <= SPEED_TOLERANCE))
    {
        treeNum_ += 1;

        nextState_ = "IDLE";
    }
    else
    {
        qDebug() << "State inconnu! On revient IDLE";
        nextState_ = "IDLE";
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
