#include "objectLayout.h"
#include "object.h"

//-
//- Definitions
//-

struct LayoutProps {
    unsigned xMargin;
    unsigned yMargin;
    unsigned playableWidth;
    unsigned playableHeight;
    unsigned xSpacer;
    unsigned ySpacer;
    unsigned objectWidth;
    unsigned objectHeight;
    unsigned objectMargin;
};

//-
//- Static data
//-

static struct RenderAction RenderCommands[ObjectCount];
static struct LayoutProps Layout;

//-
//- Private functions
//-

//-
//- Public functions
//-

void ObjectLayoutInit(void) {
    //- Setup dependencies
    RenderInit();

    //- Setup dimensions
    Layout.xMargin = 16;
    Layout.yMargin = 16;
    Layout.playableWidth = RenderScreenWidth() - (Layout.xMargin * 2);
    Layout.playableHeight = RenderScreenHeight() - (Layout.yMargin * 2);

    Layout.xSpacer = 20;
    Layout.ySpacer = 20;
    Layout.objectWidth = (
        Layout.playableWidth - ((ObjectColumnMax - 1) * Layout.xSpacer)
        ) / ObjectColumnMax;
    Layout.objectHeight = (
        Layout.playableHeight - ((ObjectRowMax - 1) * Layout.ySpacer)
        ) / ObjectRowMax;
    Layout.objectMargin = 5;

    //- Setup render commands
    //  Selector
    for (unsigned ii = 0, colIdx = 0, rowIdx = 0; ii < ObjectSelectorCount;) {
        RenderCommands[ii].command = DrawFilledRect_e;
        ObjectLayoutMove(ii, rowIdx, colIdx);

        // Compute here instead of the `for` header for easier reading
        ++ii;
        colIdx = ii % ObjectColumnMax;
        rowIdx = ii / ObjectColumnMax;
    }

    //  Objects
    for (unsigned ii = ObjectSelectorCount, jj = 0, colIdx = 0, rowIdx = 0;
        ii < ObjectCount;) {
        RenderCommands[ii].command = DrawFilledRect_e;
        ObjectLayoutMove(ii, rowIdx, colIdx);

        // Compute here instead of the `for` header for easier reading
        ++ii;
        ++jj;
        colIdx = jj % ObjectColumnMax;
        rowIdx = jj / ObjectColumnMax;
    }
}

void ObjectLayoutMove(unsigned index, unsigned row, unsigned column) {
    if (index < ObjectSelectorCount) {
        struct RectProperties * rect = &RenderCommands[index].data.filledRect;

        rect->topLeftX = Layout.xMargin
            + (Layout.objectWidth * column)
            + (Layout.xSpacer * column);
        rect->topLeftY = Layout.yMargin
            + (Layout.objectHeight * row)
            + (Layout.ySpacer * row);
        rect->width = Layout.objectWidth;
        rect->height = Layout.objectHeight;
    } else {
        struct RectProperties * rect = &RenderCommands[index].data.filledRect;

        rect->topLeftX = Layout.xMargin
            + (Layout.objectWidth * column)
            + (Layout.xSpacer * column)
            + Layout.objectMargin;
        rect->topLeftY = Layout.yMargin
            + (Layout.objectHeight * row)
            + (Layout.ySpacer * row)
            + Layout.objectMargin;
        rect->width = Layout.objectWidth - (Layout.objectMargin * 2);
        rect->height = Layout.objectHeight - (Layout.objectMargin * 2);
    }
}

void ObjectLayoutRender(unsigned index, enum Color color) {
    if (index >= ObjectCount) { return; }

    RenderCommands[index].data.filledRect.color = color;
    RenderAddAction(&RenderCommands[index]);
}
