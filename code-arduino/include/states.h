#ifndef STATES_H
#define STATES_H

enum class States
{
    Idle,
    Swing,
    MoveToX,
    Stabilize,
    Drop,

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

    case States::Drop:
        return "DROP";

    default:
        return "UNKNOWN";
    }
}

#endif

// QString jsonValue = mainWindow->receiver()->temperature();