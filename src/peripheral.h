/**
 * Provides per frame controller and accessory updates
 */
#ifndef PERIPHERAL_H
#define PERIPHERAL_H

//-
//- Definitions
//-

enum Button {
    Start_e = 0x00,
    A_e,
    B_e,
    CUp_e,
    CLeft_e,
    CRight_e,
    CDown_e,
    LeftShoulder_e,
    RightShoulder_e,
    Z_e,
    DPadUp_e,
    DPadLeft_e,
    DPadRight_e,
    DPadDown_e,
    ButtonSize_e
};

//-
//- Public Functions
//-

void PeripheralInit(void);

void PeripheralUpdateButtonState(void);

#endif
