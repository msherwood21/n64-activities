#include <graphics.h>
#include <libdragon.h>

//-
//- Static Data
//-

// No transparency
static color_t const Black = { 0x00, 0x00, 0x00, 0xff };
static color_t const White = { 0xff, 0xff, 0xff, 0xff };

//-
//- Private Functions
//-

void drawAcknowledgement() {
    surface_t * display = NULL; // non-owning

    // Grab a render buffer
    while(!(display = display_lock()));

    // Fill the screen
    graphics_fill_screen(display, graphics_convert_color(Black));

    // Set the text color
    graphics_set_color(graphics_convert_color(White), graphics_convert_color(Black));

    // Draw the text
    graphics_draw_text(display, 290, 240, "For Oliver");

    // Draw to our buffer
    display_show(display);
}

int main(void) {
    static uint32_t const BufferCount = 2;

    display_init(
        RESOLUTION_640x480, // interlaced
        DEPTH_16_BPP,
        BufferCount,
        GAMMA_NONE,
        FILTERS_DISABLED
    );

    drawAcknowledgement();

    // Main loop
    while (true) {}

    return 0;
}
