#ifndef STATES_H
#define STATES_H

enum class States
{
    Idle,
    Swing,
    Stabilize,
    MoveBack
};

inline const char *stateToString(States state)
{
    switch (state)
    {
    case States::Idle:
        return "IDLE";

    case States::Swing:
        return "SWING";

    case States::MoveBack:
        return "MOVE_BACK";

    case States::Stabilize:
        return "STABILIZE";

    default:
        return "UNKNOWN";
    }
}

#endif

// QString jsonValue = mainWindow->receiver()->temperature();