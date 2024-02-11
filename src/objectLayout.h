/**
 * Provides layout calculations for the Object subsystem
 */
#ifndef OBJECT_LAYOUT_H
#define OBJECT_LAYOUT_H

#include "render.h"
#include <stdbool.h>

//-
//- Definitions
//-

//-
//- Public Functions
//-

void ObjectLayoutInit(void);

void ObjectLayoutMove(unsigned index, unsigned row, unsigned column);

void ObjectLayoutRender(unsigned index, enum Color color);

#endif
