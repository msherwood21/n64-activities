#include "clock.h"
#include "log.h"
#include <libdragon.h>

#include <string.h>

//-
//- Definitions
//-

//-
//- Static Data
//-

static bool Initialized = false;

//- Precomputed durations of the form TICKS_PER_SECOND / x
static uint32_t const DurationTicks[ClockFrameDurationSize_e] = {
    1562500, // (93750000 / 2) / 30
    781250   // (93750000 / 2) / 60
};

static unsigned DurationSelection = 0;

//-
//- Private Functions
//-

inline ClockTick ClockCurrentTick(void) {
    //- Get start ticks. Ticks for COProcessor 0 happen at half max
    //  instruction rate (46.875 MHz). This will overflow often.
    return TICKS_READ();
}

inline uint32_t ClockCalculateTickDelta(ClockTick startTick, ClockTick endTick) {
    ClockTick result = 0;

    if (TICKS_BEFORE(endTick, startTick)) {
        //- Correct for overflow

        //- We can do this beacuse it's unsigned
        uint32_t const overflow = endTick;
        uint32_t const temp = 0xffffffff - startTick;

        result = temp + overflow + 1;
    } else {
        result = TICKS_DISTANCE(startTick, endTick);
    }

    return result;
}

inline uint32_t ClockTicksToMs(uint32_t ticks) {
    return TIMER_MICROS_LL(ticks) / 1000;
}

//-
//- Public Functions
//-

void ClockInit(enum ClockFrameDuration duration) {
    if (Initialized) { return; }

    if (duration >= ClockFrameDurationSize_e) {
        duration = FPS60_e;
    }

    DurationSelection = duration;
    Initialized = true;
}

ClockTick ClockStartFrame(void) {
    return ClockCurrentTick();
}

uint32_t ClockEndFrame(ClockTick frameStart) {
    uint32_t result = ClockCalculateTickDelta(frameStart, ClockCurrentTick());

    int32_t const frameDelta = DurationTicks[DurationSelection] - result;
    if (frameDelta < 0) {
        Log(
            "LATE frameTicks %lu / frameDelta %ld / frameStart %lu\n",
            result,
            frameDelta,
            frameStart
        );

        // We overflowed which signals we missed our frame
        result = ClockOverflow;
    } else {
        result = (uint32_t)frameDelta;
    }

    return result;
}

void ClockMarkerCtor(struct ClockMarker * marker) {
    if (marker != NULL) {
        marker->lastTick = ClockCurrentTick();
    }
}

uint32_t ClockMark(struct ClockMarker * marker) {
    uint32_t result = ClockOverflow;

    if (marker != NULL) {
        result = ClockCalculateTickDelta(marker->lastTick, ClockCurrentTick());
        marker->lastTick = ClockCurrentTick();
    }

    return result;
}

uint32_t ClockMarkMs(struct ClockMarker * marker) {
    uint32_t result = ClockMark(marker);

    if (result != ClockOverflow) {
        result = ClockTicksToMs(result);
    }

    return result;
}
