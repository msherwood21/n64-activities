#include "clock.h"
#include <debug.h>
#include <timer.h>

//-
//- Definitions
//-

//-
//- Static Data
//-

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

//-
//- Public Functions
//-

void ClockInit(enum ClockFrameDuration duration) {
    if (duration >= ClockFrameDurationSize_e) {
        duration = FPS60_e;
    }

    DurationSelection = duration;
}

ClockTick ClockStartFrame() {
    return ClockCurrentTick();
}

ClockTick ClockEndFrame(ClockTick frameStart) {
    ClockTick result = 0;

    ClockTick const currentTick = ClockCurrentTick();
    uint32_t frameTicks = 0;
    if (TICKS_BEFORE(currentTick, frameStart)) {
        //- Correct for overflow

        //- We can do this beacuse it's unsigned
        uint32_t const overflow = currentTick;
        uint32_t const temp = 0xffffffff - frameStart;

        frameTicks = temp + overflow + 1;
    } else {
        frameTicks = TICKS_DISTANCE(frameStart, currentTick);
    }

    int32_t const frameDelta = DurationTicks[DurationSelection] - frameTicks;
    if (frameDelta < 0) {
        debugf(
            "ClockEndFrame: frameTicks %lu / frameDelta %ld (%lu / %lu)\n",
            frameTicks,
            frameDelta,
            frameStart,
            currentTick
        );

        // We overflowed which signals we missed our frame
        result = ClockOverflow;
    } else {
        result = (uint32_t)frameDelta;
    }

    return result;
}
