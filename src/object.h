/**
 * Provides an interface for updating all objects' state within the game
 */
#ifndef OBJECT_H
#define OBJECT_H

#include "action.h"
#include "peripheral.h"

//-
//- Definitions
//-

#define ObjectColumnMax 4
#define ObjectRowMax 4

#define ObjectSelectorCount 1
#define ObjectCount (ObjectColumnMax * ObjectRowMax) + ObjectSelectorCount

//-
//- Public Functions
//-

void ObjectInit(void);

//- Sends render commands to the render subsystem for all objects
void ObjectRender(void);

void ObjectUpdate(enum Controller player, struct Action * action);

#endif
