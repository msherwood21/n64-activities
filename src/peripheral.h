/**
 * Provides per frame controller and accessory updates
 */
#ifndef PERIPHERAL_H
#define PERIPHERAL_H

#include <stdint.h>

//-
//- Definitions
//-

enum Button {
    Start_e = 0x0001,
    A_e = 0x0002,
    B_e = 0x0004,
    CUp_e = 0x0008,
    CLeft_e = 0x0010,
    CRight_e = 0x0020,
    CDown_e = 0x0040,
    LeftShoulder_e = 0x0080,
    RightShoulder_e = 0x0100,
    Z_e = 0x0200,
    DPadUp_e = 0x0400,
    DPadLeft_e = 0x0800,
    DPadRight_e = 0x1000,
    DPadDown_e = 0x2000,
    ButtonSize_e = 14
};

typedef uint16_t ButtonFlags;

enum Controller {
    Controller1_e = 0,
    Controller2_e,
    Controller3_e,
    Controller4_e,
    ControllerSize_e
};

//-
//- Public Functions
//-

void PeripheralInit(void);

void PeripheralUpdateButtonState(void);

ButtonFlags PeripheralButtonsPressed(enum Controller controller);

unsigned PeripheralButtonToIdx(enum Button button);

char const * const PeripheralButtonText(enum Button button);

#endif
