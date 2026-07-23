#ifndef STATES_H
#define STATES_H

enum class States
{
    Idle,
    Swing,
    MoveToX,
    Stabilize,

};

inline const char *stateToString(States state)
{
    switch (state)
    {
    case States::Idle:
        return "IDLE";

    case States::Swing:
        return "SWING";

    case States::MoveToX:
        return "MOVE_TO_X";

    case States::Stabilize:
        return "STABILIZE";

    default:
        return "UNKNOWN";
    }
}

#endif