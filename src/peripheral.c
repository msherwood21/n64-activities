#include "peripheral.h"
#include <controller.h>

#include <string.h>
#include <stdbool.h>

//-
//- Definitions
//-

enum ButtonState {
    Released_e = 0,
    Down_e,
    Pressed_e
};

//-
//- Static Data
//-

#define ControllerCount 4
static bool ButtonPressed[ControllerCount][ButtonSize_e];
static enum Button ButtonReleased[ControllerCount][ButtonSize_e];
static unsigned ButtonReleasedBack[ControllerCount] = { 0, 0, 0, 0 };

//-
//- Public Functions
//-

void PeripheralInit(void) {
    for (unsigned ii = 0; ii < ControllerCount; ++ii) {
        memset(ButtonPressed[ii], false, sizeof(bool) * ButtonSize_e);
        memset(ButtonReleased[ii], ButtonSize_e, sizeof(enum Button) * ButtonSize_e);
    }

    controller_init();
}

void PeripheralUpdateButtonState(void) {
    controller_scan();
    int const pluggedInControllers = get_controllers_present();
    struct controller_data const keysDown = get_keys_down();
    struct controller_data const keysUp = get_keys_up();
    for (int ii = 0; ii < ControllerCount; ++ii) {
        // 0xf000, 0x0f00, 0x00f0 and 0x000f correspond to controller 1, 2,
        // 3 and 4. See the CONTROLLER_1_INSERTED, etc. macros.
        int const controllerId = (0xf << (3 - ii));
        if (pluggedInControllers & controllerId) {
            // Check for buttons currently pressed
            if(keysDown.c[ii].start) { ButtonPressed[ii][Start_e] = true; }
            if(keysDown.c[ii].A) { ButtonPressed[ii][A_e] = true; }
            if(keysDown.c[ii].B) { ButtonPressed[ii][B_e] = true; }
            if(keysDown.c[ii].C_up) { ButtonPressed[ii][CUp_e] = true; }
            if(keysDown.c[ii].C_left) { ButtonPressed[ii][CLeft_e] = true; }
            if(keysDown.c[ii].C_right) { ButtonPressed[ii][CRight_e] = true; }
            if(keysDown.c[ii].C_down) { ButtonPressed[ii][CDown_e] = true; }
            if(keysDown.c[ii].L) { ButtonPressed[ii][LeftShoulder_e] = true; }
            if(keysDown.c[ii].R) { ButtonPressed[ii][RightShoulder_e] = true; }
            if(keysDown.c[ii].Z) { ButtonPressed[ii][Z_e] = true; }
            if(keysDown.c[ii].up) { ButtonPressed[ii][DPadUp_e] = true; }
            if(keysDown.c[ii].left) { ButtonPressed[ii][DPadLeft_e] = true; }
            if(keysDown.c[ii].right) { ButtonPressed[ii][DPadRight_e] = true; }
            if(keysDown.c[ii].down) { ButtonPressed[ii][DPadDown_e] = true; }

            // Check for every button that's been released. We're not going
            // to worry about the case where someone presses and releases a
            // button in the same frame.
            if(keysUp.c[ii].start && ButtonPressed[ii][Start_e]) {
                ButtonReleased[ii][ButtonReleasedBack[ii]++] = Start_e;
                ButtonPressed[ii][Start_e] = false;
            }
            if(keysUp.c[ii].A && ButtonPressed[ii][A_e]) {
                ButtonReleased[ii][ButtonReleasedBack[ii]++] = A_e;
                ButtonPressed[ii][A_e] = false;
            }
            if(keysUp.c[ii].B && ButtonPressed[ii][B_e]) {
                ButtonReleased[ii][ButtonReleasedBack[ii]++] = B_e;
                ButtonPressed[ii][B_e] = false;
            }
            if(keysUp.c[ii].C_up && ButtonPressed[ii][CUp_e]) {
                ButtonReleased[ii][ButtonReleasedBack[ii]++] = CUp_e;
                ButtonPressed[ii][CUp_e] = false;
            }
            if(keysUp.c[ii].C_left && ButtonPressed[ii][CLeft_e]) {
                ButtonReleased[ii][ButtonReleasedBack[ii]++] = CLeft_e;
                ButtonPressed[ii][CLeft_e] = false;
            }
            if(keysUp.c[ii].C_right && ButtonPressed[ii][CRight_e]) {
                ButtonReleased[ii][ButtonReleasedBack[ii]++] = CRight_e;
                ButtonPressed[ii][CRight_e] = false;
            }
            if(keysUp.c[ii].C_down && ButtonPressed[ii][CDown_e]) {
                ButtonReleased[ii][ButtonReleasedBack[ii]++] = CDown_e;
                ButtonPressed[ii][CDown_e] = false;
            }
            if(keysUp.c[ii].L && ButtonPressed[ii][LeftShoulder_e]) {
                ButtonReleased[ii][ButtonReleasedBack[ii]++] = LeftShoulder_e;
                ButtonPressed[ii][LeftShoulder_e] = false;
            }
            if(keysUp.c[ii].R && ButtonPressed[ii][RightShoulder_e]) {
                ButtonReleased[ii][ButtonReleasedBack[ii]++] = RightShoulder_e;
                ButtonPressed[ii][RightShoulder_e] = false;
            }
            if(keysUp.c[ii].Z && ButtonPressed[ii][Z_e]) {
                ButtonReleased[ii][ButtonReleasedBack[ii]++] = Z_e;
                ButtonPressed[ii][Z_e] = false;
            }
            if(keysUp.c[ii].up && ButtonPressed[ii][DPadUp_e]) {
                ButtonReleased[ii][ButtonReleasedBack[ii]++] = DPadUp_e;
                ButtonPressed[ii][DPadUp_e] = false;
            }
            if(keysUp.c[ii].left && ButtonPressed[ii][DPadLeft_e]) {
                ButtonReleased[ii][ButtonReleasedBack[ii]++] = DPadLeft_e;
                ButtonPressed[ii][DPadLeft_e] = false;
            }
            if(keysUp.c[ii].right && ButtonPressed[ii][DPadRight_e]) {
                ButtonReleased[ii][ButtonReleasedBack[ii]++] = DPadRight_e;
                ButtonPressed[ii][DPadRight_e] = false;
            }
            if(keysUp.c[ii].down && ButtonPressed[ii][DPadDown_e]) {
                ButtonReleased[ii][ButtonReleasedBack[ii]++] = DPadDown_e;
                ButtonPressed[ii][DPadDown_e] = false;
            }
        }
    }
}
