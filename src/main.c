#include <libdragon.h>

enum BinaryColor {
    Black = 0x00000000,
    White = 0xffffffff
};

void drawAcknowledgement(display_context_t * disp) {
    // Grab a render buffer
    while(!(*disp = display_lock()));

    // Fill the screen
    graphics_fill_screen(*disp, Black);

    // Set the text color
    graphics_set_color(White, Black);

    // Draw the text
    graphics_draw_text(*disp, 20, 20, "For Oliver");

    // Force backbuffer flip?
    display_show(*disp);
}

int main(void) {
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, FILTERS_RESAMPLE);

    static display_context_t disp = 0;

    drawAcknowledgement(&disp);

    // Main loop
    while (1) {}

    return 0;
}
