#include "render.h"
#include <graphics.h>

#include <stddef.h>
#include <string.h>

//-
//- Defintions
//-

#define ACTION_QUEUE_MAX_COUNT 16

struct ActionQueue {
    unsigned size;
    struct RenderAction queue[ACTION_QUEUE_MAX_COUNT];
};

//-
//- Static Data
//-

//- This initialization is directly tied to the layout of the Color enum. If it
//  changes, then this must too.
static color_t const ColorLookup[ColorSize_e] = {
    { 0x00, 0x00, 0x00, 0xff },
    { 0x80, 0x80, 0x80, 0xff },
    { 0xff, 0xff, 0xff, 0xff },
    { 0xff, 0x00, 0x00, 0xff },
    { 0x00, 0xff, 0x00, 0xff },
    { 0x00, 0x00, 0xff, 0xff },
    { 0xff, 0xff, 0x00, 0xff }
};

static surface_t * display = NULL;

static struct ActionQueue actionQueue;

static unsigned frameTick = 0;

//-
//- Private Functions
//-

bool ActionQueueAdd(struct RenderAction * action) {
    bool result = false;

    if (actionQueue.size < ACTION_QUEUE_MAX_COUNT) {
        memcpy(
            &actionQueue.queue[actionQueue.size],
            action,
            sizeof(struct RenderAction)
        );
        ++actionQueue.size;

        result = true;
    }

    return result;
}

void ActionQueueClear(void) {
    memset(actionQueue.queue, 0, sizeof(struct RenderAction) * actionQueue.size);
    actionQueue.size = 0;
}

void RenderBlankScreen(struct BlankScreenProperties * properties) {
    if (properties->color >= ColorSize_e) { return; }

    graphics_fill_screen(
        display,
        graphics_convert_color(ColorLookup[properties->color])
    );
}

void RenderText(struct TextProperties * properties) {
    if (
        (properties->textColor >= ColorSize_e)
        || (properties->bgColor >= ColorSize_e)
    ) {
        return;
    }

    graphics_set_color(
        graphics_convert_color(ColorLookup[properties->textColor]),
        graphics_convert_color(ColorLookup[properties->bgColor])
    );
    graphics_draw_text(
        display,
        properties->x,
        properties->y,
        properties->text
    );
}

void RenderRect(struct RectProperties * properties) {
    if (properties->color >= ColorSize_e) { return; }

    graphics_draw_box(
        display,
        properties->topLeftX,
        properties->topLeftY,
        properties->width,
        properties->height,
        graphics_convert_color(ColorLookup[properties->color])
    );
}

//-
//- Public Functions
//-

void RenderInit(void) {
    static uint32_t const BufferCount = 2;

    display_init(
        RESOLUTION_320x240, // progressive (TODO: investigate interlaced jitter)
        DEPTH_16_BPP,
        BufferCount,
        GAMMA_NONE,
        FILTERS_RESAMPLE
    );

    display = NULL;

    ActionQueueClear();
}

void RenderStart(void) {
    while(!(display = display_lock()));
    ++frameTick;
}

bool RenderAddAction(struct RenderAction * action) {
    static unsigned frameCheck = 0;

    if (frameCheck != frameTick) {
        frameCheck = frameTick;
        ActionQueueClear();
    }

    return ActionQueueAdd(action);
}

bool RenderIsBuilding(void) {
    return display != NULL;
}

void RenderFinish(void) {
    if (display) {
        for (unsigned ii = 0; ii < actionQueue.size; ++ii) {
            switch (actionQueue.queue[ii].command) {
                case BlankScreen_e:
                    RenderBlankScreen(&actionQueue.queue[ii].data.blankScreen);
                    break;
                case DrawText_e:
                    RenderText(&actionQueue.queue[ii].data.text);
                    break;
                case DrawFilledRect_e:
                    RenderRect(&actionQueue.queue[ii].data.filledRect);
                    break;
                default:
                    break;
            }
        }

        display_show(display);
        display = NULL;
    }
}
