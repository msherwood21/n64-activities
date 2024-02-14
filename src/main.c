#include "action.h"
#include "clock.h"
#include "log.h"
#include "object.h"
#include "peripheral.h"
#include "render.h"
#include <timer.h>

#include <string.h>
#include <stdio.h>

//-
//- Static Data
//-

static bool GameStatePaused = true;

//-
//- Private Functions
//-

void DrawButtons(ButtonFlags buttons) {
    static char buttonString[2048] = { 'B', 'u', 't', 't', 'o', 'n', 's', ':', ' ', 0 };
    char * stringPos = buttonString + 9;

    if (!buttons) {
        *stringPos = '\0';
    } else {
        for (unsigned ii = 0; ii < ButtonSize_e; ++ii) {
            unsigned const button = buttons & (0x0001 << ii);
            if (button) {
                char const * const currentButton = PeripheralButtonText(button);

                sprintf(stringPos, "%s ", currentButton);
                stringPos += strlen(currentButton) + 2; // null terminator + space
                *stringPos = '\0';
            }
        }
    }

    struct RenderAction renderText;
    renderText.command = DrawText_e;
    strcpy(renderText.data.text.text, buttonString);
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
    Log("timer fired\n");
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

    Log("timer started\n");
}

int main(void) {
    //- Initialize subsystems. Never freed. LogInit must come first.
    LogInit();

    ClockInit(FPS60_e);
    ObjectInit();
    PeripheralInit();
    RenderInit();
    timer_init();

    struct ClockMarker fpsDiag;
    ClockMarkerCtor(&fpsDiag);

    Log("initialized\n");

    //- Pre-game loop splash screen
    RenderStart();
    DrawAcknowledgement(1);
    RenderFinish();

    //- Main loop
    ButtonFlags lastButtons = 0;
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
                Log("input received %x\n", pressedButtons);
                for (unsigned ii = 0; ii < ButtonSize_e; ++ii) {
                    unsigned const button = pressedButtons & (0x0001 << ii);
                    if (button) {
                        struct Action action;
                        ActionTranslate(button, &action);
                        ObjectUpdate(Controller1_e, &action);
                    }
                }

                lastButtons = pressedButtons;
            }

            // Render the world
            ObjectRender();
            // DrawButtons(lastButtons);
            // DrawDiag(ClockMarkMs(&fpsDiag));
        }

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
