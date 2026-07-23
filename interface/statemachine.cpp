#include "statemachine.h"

StateMachine::StateMachine()
{


}

void StateMachine::update(QString state){


    if(state == "IDLE"){
        qDebug() << "Le robot est en attente";
        nextState_ = "SWING";

    }

    else if(state == "SWING"){
        qDebug() << "Le robot fait osciller le pendule.";
        nextState_ = "MOVE_TO_X";
    }

    else if(state == "MOVE_TO_X"){
        qDebug() << "Prochain target" << nextTargetX_;
        nextState_ = "STABILIZE";

    }

    else if(state == "STABILIZE"){
        qDebug() << "Le robot se stabilise";
        nextState_ = "DROP";

    }

    else if (state == "DROP"){
         qDebug() << "Le robot depose la charge";
         nextState_ = "IDLE";
    }


}
QString StateMachine::getState (){
    return nextState_;

}

double StateMachine::getTargetX(){
    return nextTargetX_;
}
