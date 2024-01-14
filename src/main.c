#include "clock.h"
#include <debug.h>
#include <libdragon.h>
#include <n64sys.h>
#include "peripheral.h"
#include "render.h"
#include <timer.h>

#include <string.h>

//-
//- Static Data
//-

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

    //- Initialize subsystems. Never freed.
    debug_init_isviewer();

    ClockInit(FPS60_e);
    RenderInit();
    PeripheralInit();
    timer_init();

    //- Pre-game loop splash screen
    RenderStart();
    drawAcknowledgement(5);
    RenderFinish();

    //- Main loop
    while (true) {
        ClockTick const startTick = ClockStartFrame();

        RenderStart();

        if (!GameStatePaused && !lateFrame) {
            //- Get controller state
            PeripheralUpdateButtonState();

            //- Send commands to graphics system
        }

        //- Make sure this is the last thing rendered
        if (lateFrame) {
            struct RenderAction blank;
            blank.command = BlankScreen_e;
            blank.data.blankScreen.color = Red_e;
            RenderPushAction(&blank);

            lateFrame = false;
        }

        RenderFinish();

        ClockTick remainingFrameTicks = ClockEndFrame(startTick);
        if (remainingFrameTicks != ClockOverflow) {
            wait_ticks(remainingFrameTicks);
        } else {
            //- We're late! Make it obvious we missed our frame and immediately
            //  start on the next frame.
            lateFrame = true;
        }
    }

    return 0;
}
