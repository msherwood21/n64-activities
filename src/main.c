#include "action.h"
#include "clock.h"
#include <debug.h>
#include <libdragon.h>
#include <n64sys.h>
#include "object.h"
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

void DrawButtons(char * text) {
    struct RenderAction renderText;
    renderText.command = DrawText_e;
    strcpy(renderText.data.text.text, text);
    renderText.data.text.x = 10;
    renderText.data.text.y = RenderScreenHeight() - 55;
    renderText.data.text.textColor = White_e;
    renderText.data.text.bgColor = Black_e;

    RenderAddAction(&renderText);
}

void DrawDiag(uint32_t frameEstimateMs) {
    //- Estimate fps
    double fps = (double)1000 / (double)frameEstimateMs;

    //- Write to char buffer. Add some extra bytes in case we mess up later...
    //  NOTE: Layout is "FPS: xxx.xxx\0"
    char fpsStr[24] = { '\0' };
    memset(fpsStr, 0, sizeof(char) * 24);
    sprintf(fpsStr, "FPS: %03.3f", fps);

    //- Write to screen

    struct RenderAction text;
    text.command = DrawText_e;
    memcpy(text.data.text.text, fpsStr, sizeof(char) * 24);
    text.data.text.x = 10;
    text.data.text.y = RenderScreenHeight() - 40;
    text.data.text.textColor = White_e;
    text.data.text.bgColor = Black_e;
    RenderPushAction(&text);
}

void PauseGame(void) {
    GameStatePaused = true;
}

void ResumeGame(int ovfl) {
    GameStatePaused = false;
}

void DrawAcknowledgement(unsigned secondsToPauseGame) {
    // Make sure the game doesn't render over us
    PauseGame();

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
        ResumeGame
    );
}

int main(void) {
    //- Initialize subsystems. Never freed.
    debug_init_isviewer();

    ClockInit(FPS60_e);
    ObjectInit();
    PeripheralInit();
    RenderInit();
    timer_init();

    struct ClockMarker fpsDiag;
    ClockMarkerCtor(&fpsDiag);

    //- Pre-game loop splash screen
    RenderStart();
    DrawAcknowledgement(1);
    RenderFinish();

    // char buttonString[2048] = { 'B', 'u', 't', 't', 'o', 'n', 's', ':', ' ', 0 };

    //- Main loop
    while (true) {
        ClockTick const startTick = ClockStartFrame();

        RenderStart();

        if (!GameStatePaused) {
            // Draw background
            struct RenderAction bg;
            bg.command = BlankScreen_e;
            bg.data.blankScreen.color = Black_e;
            RenderAddAction(&bg);

            //- Get controller state
            PeripheralUpdateButtonState();
            ButtonFlags pressedButtons = PeripheralButtonsPressed(Controller1_e);

            //- Send commands to objects
            if (pressedButtons) {
                // "Buttons: "
                // char * stringPos = buttonString + 9;
                // memset(stringPos, 0, sizeof(char) * strlen(buttonString));

                for (unsigned ii = 0; ii < ButtonSize_e; ++ii) {
                    unsigned const button = pressedButtons & (0x0001 << ii);
                    if (button) {
                        // char const * const currentButton = PeripheralButtonText(button);

                        // sprintf(stringPos, "%s ", currentButton);
                        // stringPos += strlen(currentButton) + 1;

                        struct Action action;
                        ActionTranslate(button, &action);
                        ObjectUpdate(Controller1_e, &action);
                    }
                }
            }

            // Render the world
            ObjectRender();
            // DrawButtons(buttonString);
            // DrawDiag(ClockMarkMs(&fpsDiag));
        }

        //- Make sure this is the last thing rendered
        // if (lateFrame) {
        //     struct RenderAction blank;
        //     blank.command = BlankScreen_e;
        //     blank.data.blankScreen.color = Red_e;
        //     RenderPushAction(&blank);

        //     lateFrame = false;
        // }

        RenderFinish();

        uint32_t remainingFrameTicks = ClockEndFrame(startTick);
        if (remainingFrameTicks != ClockOverflow) {
            wait_ticks(remainingFrameTicks);
        } else {
            //- We're late! Immediately start on the next frame.
        }
    }

    return 0;
}
