/**
 * Provides data types representing player input and helper functions for
 * their use.
 */
#ifndef ACTION_H
#define ACTION_H

#include "peripheral.h"
#include "render.h"

//-
//- Definitions
//-

enum ActionType {
    Move_e = 0,
    Paint_e
};

enum ActionDirection {
    North_e = 0,
    East_e,
    South_e,
    West_e,
    ActionDirectionSize_e
};

union ActionData {
    enum ActionDirection direction;
    enum Color color;
};

struct Action {
    enum ActionType type;
    union ActionData data;
};

//-
//- Public Functions
//-

//- Translates a button press into a player action. `action` is not modified if
// button cannot be translated into an action.
void ActionTranslate(enum Button button, struct Action * action);

#endif
