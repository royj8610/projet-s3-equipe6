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
    double r_angle = angle *PI /180.0;
    double h_arbre = std::cos(r_angle) * H_PENDULE;

    if(state == "IDLE"
            && shouldStart_)
    {
        nextState_ = "SWING";
    }
    else if(state == "SWING"
            && h_arbre < H_OBSTACLE)
    {
        nextState_ = "MOVE_TO_X";
    }
    else if(state == "MOVE_TO_X"
            && position >= TARGET_X_DROP)
    {
        nextState_ = "STABILIZE";
    }
    else if(state == "STABILIZE")
    {
        ////hmmmmmmm
        nextState_ = "DROP";

    }
    else if (state == "DROP")
    {
        //hmmmmmmmmmmm
        qDebug() << "Le robot depose la charge";
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
    }
    case StateMachine::ButtonType::STOP:{
        shouldStart_ = false;
    }
    case StateMachine::ButtonType::RESET:{
        shouldStart_ = false;
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
