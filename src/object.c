#include "object.h"
// #include "debug.h"
#include "objectLayout.h"
#include "render.h"
#include <stddef.h>

//-
//- Definitions
//-

struct Position {
    unsigned row;
    unsigned column;
};

struct ObjectState {
    struct Position pos;
    enum Color color;
};

//-
//- Static data
//-

static bool Initialized = false;

static struct ObjectState Objects[ObjectCount];

//-
//- Private functions
//-

unsigned ObjectControllerToSelector(enum Controller controller) {
    unsigned result = controller;

    if (controller >= ObjectSelectorCount) {
        result = 0;
    }

    return result;
}

struct ObjectState * ObjectGetSelectedObject(unsigned selectorIdx) {
    struct ObjectState * result = NULL;

    for (unsigned ii = ObjectSelectorCount; ii < ObjectCount; ++ii) {
        if ((Objects[selectorIdx].pos.row == Objects[ii].pos.row)
            && (Objects[selectorIdx].pos.column == Objects[ii].pos.column)) {
            result = &Objects[ii];
            break;
        }
    }

    return result;
}

void ObjectMove(unsigned objIdx, enum ActionDirection direction) {
    switch (direction) {
        case North_e:
            if (--Objects[objIdx].pos.row >= ObjectRowMax) {
                Objects[objIdx].pos.row = ObjectRowMax - 1;
            }
            break;
        case East_e:
            if (++Objects[objIdx].pos.column >= ObjectColumnMax) {
                Objects[objIdx].pos.column = 0;
            }
            break;
        case South_e:
            if (++Objects[objIdx].pos.row >= ObjectRowMax) {
                Objects[objIdx].pos.row = 0;
            }
            break;
        case West_e:
            if (--Objects[objIdx].pos.column >= ObjectColumnMax) {
                Objects[objIdx].pos.column = ObjectColumnMax - 1;
            }
            break;
        default:
            break;
    }

    ObjectLayoutMove(
        objIdx,
        Objects[objIdx].pos.row,
        Objects[objIdx].pos.column
    );
    // debugf("ObjectMove %d %d %d", objIdx, Objects[objIdx].pos.row, Objects[objIdx].pos.column);
}

//-
//- Public functions
//-

void ObjectInit(void) {
    if (Initialized) { return; }

    //- Setup dependencies
    ObjectLayoutInit();

    //- Setup positions and initial colors
    for (unsigned ii = 0, colIdx = 0, rowIdx = 0; ii < ObjectSelectorCount;) {
        Objects[ii].pos.row = rowIdx;
        Objects[ii].pos.column = colIdx;
        Objects[ii].color = White_e;

        // Compute here instead of the `for` header for easier reading
        ++ii;
        colIdx = ii % ObjectColumnMax;
        rowIdx = ii / ObjectColumnMax;
    }
    for (unsigned ii = ObjectSelectorCount, jj = 0, colIdx = 0, rowIdx = 0;
        ii < ObjectCount;) {
        Objects[ii].pos.row = rowIdx;
        Objects[ii].pos.column = colIdx;
        Objects[ii].color = Grey_e;

        // Compute here instead of the `for` header for easier reading
        ++ii;
        ++jj;
        colIdx = jj % ObjectColumnMax;
        rowIdx = jj / ObjectColumnMax;
    }

    Initialized = true;
}

void ObjectRender(void) {
    for (unsigned ii = 0; ii < ObjectCount; ++ii) {
        ObjectLayoutRender(ii, Objects[ii].color);
    }
}

void ObjectUpdate(enum Controller player, struct Action * action) {
    if (action == NULL) {
        return;
    }

    switch (action->type) {
        case Move_e:
            ObjectMove(
                ObjectControllerToSelector(player),
                action->data.direction
            );
            break;
        case Paint_e:
            ;   //- This semi-colon makes us pre-C23 compliant and I'm not even
                //  joking. https://stackoverflow.com/a/57742042
            struct ObjectState * object = ObjectGetSelectedObject(
                ObjectControllerToSelector(player)
            );
            if (object) {
                object->color = action->data.color;
            }
            break;
        default:
            break;
    }
}
