/**
 * Provides additional clock and timing services
 */
#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>

//-
//- Definitions
//-

typedef uint32_t ClockTick;

static ClockTick const ClockOverflow = 0xffffffff;

enum ClockFrameDuration {
    FPS30_e = 0,
    FPS60_e,
    ClockFrameDurationSize_e
};

//-
//- Public Functions
//-

//- Initializes the Clock subsystem. `duration` informs the subsystem how long
//  a frame should last.
void ClockInit(enum ClockFrameDuration duration);

//- Returns the starting time of a frame
ClockTick ClockStartFrame(void);

//- Returns the remaining time in a frame or ClockOverflow
ClockTick ClockEndFrame(ClockTick frameStart);

#endif
