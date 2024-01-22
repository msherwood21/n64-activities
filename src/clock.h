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

struct ClockMarker {
    uint32_t lastTick;
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
uint32_t ClockEndFrame(ClockTick frameStart);

//- Initializes a ClockMarker struct for use with this module
void ClockMarkerCtor(struct ClockMarker * marker);

//- Returns the number of ticks since this function or ClockMarkerCtor was
//  called. ClockOverflow is returned if marker is NULL.
uint32_t ClockMark(struct ClockMarker * marker);

//- Returns the time, in milliseconds, since this function or ClockMarkerCtor
//  was called. ClockOverflow is returned if marker is NULL.
uint32_t ClockMarkMs(struct ClockMarker * marker);

#endif
