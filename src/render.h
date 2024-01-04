/**
 * Provides the following basic rendering services:
 *
 * - Screen blank
 * - Text
 * - Rectangle primitives
 *
 */
#ifndef RENDER_H
#define RENDER_H

#include <stdbool.h>

//-
//- Definitions
//-

enum Color {
    Black_e = 0,
    Grey_e,
    White_e,
    Red_e,
    Green_e,
    Blue_e,
    Yellow_e,
    ColorSize_e     // Size property
};

struct BlankScreenProperties {
    enum Color color;
};

struct TextProperties {
    char text[256];
    unsigned x;
    unsigned y;
    enum Color textColor;
    enum Color bgColor;
};

struct RectProperties {
    unsigned topLeftX;
    unsigned topLeftY;
    unsigned width;
    unsigned height;
    enum Color color;
};

enum RenderCommand {
    BlankScreen_e = 0,
    DrawText_e,
    DrawFilledRect_e
};

union RenderActionData {
    struct BlankScreenProperties blankScreen;
    struct TextProperties text;
    struct RectProperties filledRect;
};

struct RenderAction {
    enum RenderCommand command;
    union RenderActionData data;
};

//-
//- Public Functions
//-

void RenderInit(void);

void RenderStart(void);

bool RenderAddAction(struct RenderAction * action);

bool RenderIsBuilding(void);

void RenderFinish(void);

#endif
