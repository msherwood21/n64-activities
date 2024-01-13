#include "clock.h"
#include <debug.h>
#include <controller.h>
#include <libdragon.h>
#include <n64sys.h>
#include "render.h"
#include <timer.h>

#include <string.h>

//-
//- Types
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

static bool GameStatePaused = true;

//-
//- Private Functions
//-

void resetBoard(int ovfl) {
    if (!RenderIsBuilding()) { RenderStart(); }

    struct RenderAction bg;
    bg.command = BlankScreen_e;
    bg.data.blankScreen.color = Black_e;
    RenderAddAction(&bg);

    //- Draw selector

    struct RenderAction rect;
    rect.command = DrawFilledRect_e;
    rect.data.filledRect.topLeftX = 10;
    rect.data.filledRect.topLeftY = 5;
    rect.data.filledRect.width = 50;
    rect.data.filledRect.height = 40;
    rect.data.filledRect.color = White_e;
    RenderAddAction(&rect);

    //- Draw squares

    rect.data.filledRect.topLeftX = 15;
    rect.data.filledRect.topLeftY = 10;
    rect.data.filledRect.width = 40;
    rect.data.filledRect.height = 30;
    rect.data.filledRect.color = Red_e;
    RenderAddAction(&rect);

    rect.data.filledRect.topLeftX = 65;
    rect.data.filledRect.topLeftY = 10;
    rect.data.filledRect.width = 40;
    rect.data.filledRect.height = 30;
    rect.data.filledRect.color = Green_e;
    RenderAddAction(&rect);

    rect.data.filledRect.topLeftX = 115;
    rect.data.filledRect.topLeftY = 10;
    rect.data.filledRect.width = 40;
    rect.data.filledRect.height = 30;
    rect.data.filledRect.color = Blue_e;
    RenderAddAction(&rect);

    rect.data.filledRect.topLeftX = 165;
    rect.data.filledRect.topLeftY = 10;
    rect.data.filledRect.width = 40;
    rect.data.filledRect.height = 30;
    rect.data.filledRect.color = Yellow_e;
    RenderAddAction(&rect);

    rect.data.filledRect.topLeftX = 215;
    rect.data.filledRect.topLeftY = 10;
    rect.data.filledRect.width = 40;
    rect.data.filledRect.height = 30;
    rect.data.filledRect.color = Grey_e;
    RenderAddAction(&rect);

    // Make sure we're actually rendering the game
    GameStatePaused = false;
}

void drawAcknowledgement(unsigned secondsToPauseGame) {
    // Make sure the game doesn't render over us
    GameStatePaused = true;

    if (!RenderIsBuilding()) { RenderStart(); }

    struct RenderAction blank;
    blank.command = BlankScreen_e;
    blank.data.blankScreen.color = Black_e;
    RenderAddAction(&blank);

    struct RenderAction text;
    text.command = DrawText_e;
    memcpy(text.data.text.text, "For Oliver", sizeof(char) * 11);
    text.data.text.x = 125;
    text.data.text.y = 120;
    text.data.text.textColor = White_e;
    text.data.text.bgColor = Black_e;
    RenderAddAction(&text);

    new_timer(
        TICKS_PER_SECOND * secondsToPauseGame,
        TF_ONE_SHOT,
        resetBoard
    );
}

int main(void) {
    bool lateFrame = false;

    // Initialize global static data
    for (unsigned ii = 0; ii < ControllerCount; ++ii) {
        memset(ButtonPressed[ii], false, sizeof(bool) * ButtonSize_e);
        memset(ButtonReleased[ii], ButtonSize_e, sizeof(enum Button) * ButtonSize_e);
    }

    // Initialize subsystems. Never freed.
    debug_init_isviewer();

    ClockInit(FPS60_e);
    controller_init();
    RenderInit();
    timer_init();

    // Pre-game loop splash screen
    RenderStart();
    drawAcknowledgement(5);
    RenderFinish();

    // Main loop
    while (true) {
        ClockTick const startTick = ClockStartFrame();

        // Open us up to render something
        RenderStart();

        if (!GameStatePaused && !lateFrame) {
            // Get controller state
            controller_scan();
            int const pluggedInControllers = get_controllers_present();
            struct controller_data const keysDown = get_keys_down();
            struct controller_data const keysUp = get_keys_up();
            for (int ii = 0; ii < 4; ++ii) {
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

                    // Send commands to the graphics system
                }
            }

            // Send commands to graphics system

            // Render a frame
        }

        //- Make sure this is the last thing rendered
        if (lateFrame) {
            struct RenderAction blank;
            blank.command = BlankScreen_e;
            blank.data.blankScreen.color = Red_e;
            RenderPushAction(&blank);

            lateFrame = false;
        }

        // Render our frame
        RenderFinish();

        ClockTick remainingFrameTicks = ClockEndFrame(startTick);
        if (remainingFrameTicks != ClockOverflow) {
            wait_ticks(remainingFrameTicks);
        } else {
            // We're late! Make it obvious we missed our frame and immediately
            // start on the next frame.
            lateFrame = true;
        }
    }

    return 0;
}
