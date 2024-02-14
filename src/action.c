#include "action.h"
#include "log.h"
#include <stddef.h>

//-
//- Private Functions
//-

enum ActionDirection ActionButtonToDirection(enum Button button) {
    switch (button) {
        case DPadUp_e: {
            return North_e;
        }
        case DPadRight_e: {
            return East_e;
        }
        case DPadDown_e: {
            return South_e;
        }
        case DPadLeft_e: {
            return West_e;
        }
        default: {
            Log("%u\n", button);
            return ActionDirectionSize_e;
        }
    }
}

enum Color ActionButtonToColor(enum Button button) {
    switch (button) {
        case Start_e: {
            return Red_e;
        }
        case A_e: {
            return Blue_e;
        }
        case B_e: {
            return Green_e;
        }
        case CUp_e:
        case CRight_e:
        case CDown_e:
        case CLeft_e: {
            return Yellow_e;
        }
        case LeftShoulder_e:
        case RightShoulder_e:
        case Z_e: {
            return Grey_e;
        }
        default: {
            Log("%u\n", button);
            return ColorSize_e;
        }
    }
}

//-
//- Public Functions
//-

void ActionTranslate(enum Button button, struct Action * action) {
    unsigned const buttonIdx = PeripheralButtonToIdx(button);
    if ((buttonIdx >= ButtonSize_e) || (action == NULL)) {
        Log("invalid condition %u\n", button);
        return;
    }

    if (button >= DPadUp_e) {
        action->type = Move_e;
        action->data.direction = ActionButtonToDirection(button);
    } else {
        action->type = Paint_e;
        action->data.color = ActionButtonToColor(button);
    }
}
