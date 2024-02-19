#include "peripheral.h"
#include "log.h"
#include <libdragon.h>

#include <string.h>
#include <stdbool.h>

//-
//- Definitions
//-

enum ButtonIndex {
    StartIdx_e = 0,
    AIdx_e,
    BIdx_e,
    CUpIdx_e,
    CLeftIdx_e,
    CRightIdx_e,
    CDownIdx_e,
    LeftShoulderIdx_e,
    RightShoulderIdx_e,
    ZIdx_e,
    DPadUpIdx_e,
    DPadLeftIdx_e,
    DPadRightIdx_e,
    DPadDownIdx_e,
    ButtonIdxSize_e
};

enum ButtonState {
    Released_e = 0,
    Down_e,
    Pressed_e
};

//-
//- Static Data
//-

static bool Initialized = false;

static char const * const ButtonText[ButtonSize_e] = {
    "Start",
    "A",
    "B",
    "CUp",
    "CLeft",
    "CRight",
    "CDown",
    "LeftShoulder",
    "RightShoulder",
    "Z",
    "DPadUp",
    "DPadLeft",
    "DPadRight",
    "DPadDown"
};

#define ControllerCount 4
static bool ButtonPressed[ControllerCount][ButtonSize_e];
static enum Button ButtonReleased[ControllerCount][ButtonSize_e];
static unsigned ButtonReleasedBack[ControllerCount] = { 0, 0, 0, 0 };

//-
//- Private Functions
//-

void ResetButtonReleased(enum Controller controller) {
    memset(ButtonReleased[controller], ButtonSize_e, sizeof(enum Button) * ButtonSize_e);
    ButtonReleasedBack[controller] = 0;
}

void ResetAllState(enum Controller controller) {
    memset(ButtonPressed[controller], false, sizeof(bool) * ButtonSize_e);
    ResetButtonReleased(controller);
}

//-
//- Public Functions
//-

void PeripheralInit(void) {
    if (Initialized) { return; }

    for (unsigned ii = 0; ii < ControllerSize_e; ++ii) {
        ResetAllState(ii);
    }

    controller_init();

    Initialized = true;
}

void PeripheralUpdateButtonState(void) {
    for (unsigned ii = 0; ii < ControllerSize_e; ++ii) {
        ResetButtonReleased(ii);
    }

    controller_scan();
    int const pluggedInControllers = get_controllers_present();
    struct controller_data const keysDown = get_keys_down();
    struct controller_data const keysUp = get_keys_up();

    for (int ii = 0; ii < ControllerCount; ++ii) {
        int controllerId = 0;
        switch (ii) {
            case 0:
                controllerId = CONTROLLER_1_INSERTED;
                break;
            case 1:
                controllerId = CONTROLLER_2_INSERTED;
                break;
            case 2:
                controllerId = CONTROLLER_3_INSERTED;
                break;
            case 3:
                controllerId = CONTROLLER_4_INSERTED;
                break;
            default:
                Log("invalid controller %d\n", ii);
                break;
        }

        if (pluggedInControllers & controllerId) {
            // Check for buttons currently pressed
            if(keysDown.c[ii].start) { ButtonPressed[ii][StartIdx_e] = true; }
            if(keysDown.c[ii].A) { ButtonPressed[ii][AIdx_e] = true; }
            if(keysDown.c[ii].B) { ButtonPressed[ii][BIdx_e] = true; }
            if(keysDown.c[ii].C_up) { ButtonPressed[ii][CUpIdx_e] = true; }
            if(keysDown.c[ii].C_left) { ButtonPressed[ii][CLeftIdx_e] = true; }
            if(keysDown.c[ii].C_right) { ButtonPressed[ii][CRightIdx_e] = true; }
            if(keysDown.c[ii].C_down) { ButtonPressed[ii][CDownIdx_e] = true; }
            if(keysDown.c[ii].L) { ButtonPressed[ii][LeftShoulderIdx_e] = true; }
            if(keysDown.c[ii].R) { ButtonPressed[ii][RightShoulderIdx_e] = true; }
            if(keysDown.c[ii].Z) { ButtonPressed[ii][ZIdx_e] = true; }
            if(keysDown.c[ii].up) { ButtonPressed[ii][DPadUpIdx_e] = true; }
            if(keysDown.c[ii].left) { ButtonPressed[ii][DPadLeftIdx_e] = true; }
            if(keysDown.c[ii].right) { ButtonPressed[ii][DPadRightIdx_e] = true; }
            if(keysDown.c[ii].down) { ButtonPressed[ii][DPadDownIdx_e] = true; }

            // Check for every button that's been released. We're not going
            // to worry about the case where someone presses and releases a
            // button in the same frame.
            if(keysUp.c[ii].start && ButtonPressed[ii][StartIdx_e]) {
                ButtonReleased[ii][ButtonReleasedBack[ii]++] = Start_e;
                ButtonPressed[ii][StartIdx_e] = false;
            }
            if(keysUp.c[ii].A && ButtonPressed[ii][AIdx_e]) {
                ButtonReleased[ii][ButtonReleasedBack[ii]++] = A_e;
                ButtonPressed[ii][AIdx_e] = false;
            }
            if(keysUp.c[ii].B && ButtonPressed[ii][BIdx_e]) {
                ButtonReleased[ii][ButtonReleasedBack[ii]++] = B_e;
                ButtonPressed[ii][BIdx_e] = false;
            }
            if(keysUp.c[ii].C_up && ButtonPressed[ii][CUpIdx_e]) {
                ButtonReleased[ii][ButtonReleasedBack[ii]++] = CUp_e;
                ButtonPressed[ii][CUpIdx_e] = false;
            }
            if(keysUp.c[ii].C_left && ButtonPressed[ii][CLeftIdx_e]) {
                ButtonReleased[ii][ButtonReleasedBack[ii]++] = CLeft_e;
                ButtonPressed[ii][CLeftIdx_e] = false;
            }
            if(keysUp.c[ii].C_right && ButtonPressed[ii][CRightIdx_e]) {
                ButtonReleased[ii][ButtonReleasedBack[ii]++] = CRight_e;
                ButtonPressed[ii][CRightIdx_e] = false;
            }
            if(keysUp.c[ii].C_down && ButtonPressed[ii][CDownIdx_e]) {
                ButtonReleased[ii][ButtonReleasedBack[ii]++] = CDown_e;
                ButtonPressed[ii][CDownIdx_e] = false;
            }
            if(keysUp.c[ii].L && ButtonPressed[ii][LeftShoulderIdx_e]) {
                ButtonReleased[ii][ButtonReleasedBack[ii]++] = LeftShoulder_e;
                ButtonPressed[ii][LeftShoulderIdx_e] = false;
            }
            if(keysUp.c[ii].R && ButtonPressed[ii][RightShoulderIdx_e]) {
                ButtonReleased[ii][ButtonReleasedBack[ii]++] = RightShoulder_e;
                ButtonPressed[ii][RightShoulderIdx_e] = false;
            }
            if(keysUp.c[ii].Z && ButtonPressed[ii][ZIdx_e]) {
                ButtonReleased[ii][ButtonReleasedBack[ii]++] = Z_e;
                ButtonPressed[ii][ZIdx_e] = false;
            }
            if(keysUp.c[ii].up && ButtonPressed[ii][DPadUpIdx_e]) {
                ButtonReleased[ii][ButtonReleasedBack[ii]++] = DPadUp_e;
                ButtonPressed[ii][DPadUpIdx_e] = false;
            }
            if(keysUp.c[ii].left && ButtonPressed[ii][DPadLeftIdx_e]) {
                ButtonReleased[ii][ButtonReleasedBack[ii]++] = DPadLeft_e;
                ButtonPressed[ii][DPadLeftIdx_e] = false;
            }
            if(keysUp.c[ii].right && ButtonPressed[ii][DPadRightIdx_e]) {
                ButtonReleased[ii][ButtonReleasedBack[ii]++] = DPadRight_e;
                ButtonPressed[ii][DPadRightIdx_e] = false;
            }
            if(keysUp.c[ii].down && ButtonPressed[ii][DPadDownIdx_e]) {
                ButtonReleased[ii][ButtonReleasedBack[ii]++] = DPadDown_e;
                ButtonPressed[ii][DPadDownIdx_e] = false;
            }
        }
    }
}

ButtonFlags PeripheralButtonsPressed(enum Controller controller) {
    ButtonFlags result = 0;

    if (controller < ControllerSize_e) {
        for (unsigned ii = 0; ii < ButtonReleasedBack[controller]; ++ii) {
            result |= ButtonReleased[controller][ii];
        }
    } else {
        Log("invalid controller %u\n", controller);
    }

    return result;
}

inline unsigned PeripheralButtonToIdx(enum Button button) {
    unsigned result = ButtonSize_e;

    for (unsigned ii = 0; ii < ButtonSize_e; ++ii) {
        if (button == (0x0001 << ii)) {
            result = ii;
            break;
        }
    }

    if (result == ButtonSize_e) {
        Log("invalid button %u\n", button);
    }

    return result;
}

inline char const * const PeripheralButtonText(enum Button button) {
    unsigned idx = PeripheralButtonToIdx(button);

    if (idx != ButtonSize_e) {
        return ButtonText[idx];
    } else {
        return NULL;
    }
}
