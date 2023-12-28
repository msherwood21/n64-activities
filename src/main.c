#include <graphics.h>
#include <libdragon.h>
#include <timer.h>

//-
//- Static Data
//-

// No transparency
static color_t const Black = { 0x00, 0x00, 0x00, 0xff };
static color_t const Grey = { 0x80, 0x80, 0x80, 0xff };
static color_t const White = { 0xff, 0xff, 0xff, 0xff };
static color_t const Red = { 0xff, 0x00, 0x00, 0xff };
static color_t const Green = { 0x00, 0xff, 0x00, 0xff };
static color_t const Blue = { 0x00, 0x00, 0xff, 0xff };
static color_t const Yellow = { 0xff, 0xff, 0x00, 0xff };

//-
//- Private Functions
//-

void resetBoard(int ovfl) {
    surface_t * display = NULL; // non-owning

    // Grab a render buffer
    while(!(display = display_lock()));

    // Fill the screen
    graphics_fill_screen(display, graphics_convert_color(Black));

    // Draw selector
    graphics_draw_box(
        display,
        10,
        5,
        50,
        40,
        graphics_convert_color(White)
    );

    // Draw squares
    graphics_draw_box(
        display,
        15,
        10,
        40,
        30,
        graphics_convert_color(Red)
    );
    graphics_draw_box(
        display,
        65,
        10,
        40,
        30,
        graphics_convert_color(Green)
    );
    graphics_draw_box(
        display,
        115,
        10,
        40,
        30,
        graphics_convert_color(Blue)
    );
    graphics_draw_box(
        display,
        165,
        10,
        40,
        30,
        graphics_convert_color(Yellow)
    );
    graphics_draw_box(
        display,
        215,
        10,
        40,
        30,
        graphics_convert_color(Grey)
    );

    // Draw to our buffer
    display_show(display);
}

void drawAcknowledgement(void) {
    surface_t * display = NULL; // non-owning

    // Grab a render buffer
    while(!(display = display_lock()));

    // Fill the screen
    graphics_fill_screen(display, graphics_convert_color(Black));

    // Set the text color
    graphics_set_color(graphics_convert_color(White), graphics_convert_color(Black));

    // Draw the text
    graphics_draw_text(display, 125, 120, "For Oliver");

    // Draw to our buffer
    display_show(display);
}

int main(void) {
    static uint32_t const BufferCount = 2;

    // Never freed
    display_init(
        RESOLUTION_320x240, // progressive (TODO: investigate interlaced jitter)
        DEPTH_16_BPP,
        BufferCount,
        GAMMA_NONE,
        FILTERS_RESAMPLE
    );
    timer_init();

    drawAcknowledgement();

    new_timer(
        TIMER_TICKS(5000000),   // 5 seconds
        TF_ONE_SHOT,
        resetBoard
    );

    // Main loop
    while (true) {}

    return 0;
}
