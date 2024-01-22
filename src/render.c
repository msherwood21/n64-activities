#include "render.h"
#include <debug.h>
#include <graphics.h>

#include <stddef.h>
#include <string.h>

//-
//- Defintions
//-

//- Conditionally prints debug messages depending on whether or not data has
//  been LogThisFrame this frame
static bool LogThisFrame = false;
#define DEBUG(args...) if(LogThisFrame){debugf(args);}

#define ACTION_QUEUE_MAX_COUNT 16

struct RenderQueue {
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

static struct RenderQueue actionQueue;

static struct RenderQueue pushQueue;

static unsigned frameTick = 0;

//-
//- Private Functions
//-

bool RenderQueueAdd(struct RenderQueue * this, struct RenderAction * action) {
    bool result = false;

    if (this->size < ACTION_QUEUE_MAX_COUNT) {
        memcpy(
            &this->queue[this->size],
            action,
            sizeof(struct RenderAction)
        );
        ++this->size;

        DEBUG(
            "RenderQueueAdd: %p added %d (%u)\n",
            this,
            action->command,
            this->size
        );

        result = true;
    } else {
        DEBUG("RenderQueueAdd: %p is full\n", this);
    }

    return result;
}

void RenderQueueClear(struct RenderQueue * this) {
    memset(this->queue, 0, sizeof(struct RenderAction) * this->size);
    this->size = 0;

    DEBUG("RenderQueueClear: Cleared %p\n", this);
}

void RenderBlankScreen(struct BlankScreenProperties * properties) {
    if (properties->color >= ColorSize_e) { return; }

    graphics_fill_screen(
        display,
        graphics_convert_color(ColorLookup[properties->color])
    );

    DEBUG("RenderBlankScreen: %u\n", properties->color);
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

    DEBUG("RenderText: %s\n", properties->text);
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

    DEBUG("RenderRect: %u / %u / %u\n", properties->color, properties->topLeftX, properties->topLeftY);
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

    RenderQueueClear(&actionQueue);
    RenderQueueClear(&pushQueue);

    LogThisFrame = true;

    DEBUG("RenderInit: finished\n");
}

void RenderStart(void) {
    if (!display) {
        while(!(display = display_lock()));
        ++frameTick;
    }

    DEBUG("RenderStart: finished\n");
}

bool RenderAddAction(struct RenderAction * action) {
    static unsigned frameCheck = 0;
    LogThisFrame = true;
    bool result = false;

    if (frameCheck != frameTick) {
        frameCheck = frameTick;
        RenderQueueClear(&actionQueue);
    }

    result = RenderQueueAdd(&actionQueue, action);

    DEBUG(
        "RenderAddAction: Added 0x%p (%u / %u)\n",
        action,
        action->command,
        actionQueue.size
    );

    return result;
}

bool RenderPushAction(struct RenderAction * action) {
    LogThisFrame = true;
    bool result = RenderQueueAdd(&pushQueue, action);

    DEBUG(
        "RenderPushAction: Added 0x%p (%u / %u)\n",
        action,
        action->command,
        pushQueue.size
    );

    return result;
}

bool RenderIsBuilding(void) {
    return display != NULL;
}

void RenderFinish(void) {
    if (!display) { return; }

    struct RenderQueue * queue = pushQueue.size == 0 ? &actionQueue : &pushQueue;
    DEBUG("RenderFinish: Rendering push? (%u)\n", pushQueue.size != 0);

    for (unsigned ii = 0; ii < queue->size; ++ii) {
        switch (queue->queue[ii].command) {
            case BlankScreen_e:
                RenderBlankScreen(&queue->queue[ii].data.blankScreen);
                break;
            case DrawText_e:
                RenderText(&queue->queue[ii].data.text);
                break;
            case DrawFilledRect_e:
                RenderRect(&queue->queue[ii].data.filledRect);
                break;
            default:
                break;
        }
    }

    RenderQueueClear(&pushQueue);

    display_show(display);
    display = NULL;

    DEBUG("RenderFinish: finished\n");
    LogThisFrame = false;
}

unsigned RenderScreenHeight(void) {
    return 240;
}

unsigned RenderScreenWidth(void) {
    return 320;
}
