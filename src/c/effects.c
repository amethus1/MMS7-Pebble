#include "effects.h"

uint8_t GlobalInverterColor;
uint8_t GlobalBkgColor;

static void set_pixel(uint8_t* bitmap_data, int bytes_per_row, int y, int x, uint8_t color) {
#ifdef PBL_COLOR
    bitmap_data[y * bytes_per_row + x] = color;
#else
    bitmap_data[y * bytes_per_row + x / 8] ^= (-color ^ bitmap_data[y * bytes_per_row + x / 8]) & (1 << (x % 8));
#endif
}

static uint8_t get_pixel(uint8_t* bitmap_data, int bytes_per_row, int y, int x) {
#ifdef PBL_COLOR
    return bitmap_data[y * bytes_per_row + x];
#else
    return (bitmap_data[y * bytes_per_row + x / 8] >> (x % 8)) & 1;
#endif
}

void effect_invert_color(GContext* ctx, GRect position, void* param) {
    GBitmap* fb = graphics_capture_frame_buffer(ctx);
    if (!fb) {
        return;
    }

    uint8_t* bitmap_data = gbitmap_get_data(fb);
    int bytes_per_row = gbitmap_get_bytes_per_row(fb);

#ifdef PBL_COLOR
    uint8_t inverter_color = (uintptr_t)param;
    uint8_t background_color = 0;
    if (inverter_color == 0) {
        inverter_color = GlobalInverterColor;
        background_color = GlobalBkgColor;
    }
#endif

    for (int y = 0; y < position.size.h; y++) {
        for (int x = 0; x < position.size.w; x++) {
#ifdef PBL_COLOR
            if ((get_pixel(bitmap_data, bytes_per_row, y + position.origin.y, x + position.origin.x) & 0b00111111) != background_color) {
                set_pixel(bitmap_data, bytes_per_row, y + position.origin.y, x + position.origin.x, background_color);
            } else {
                set_pixel(bitmap_data, bytes_per_row, y + position.origin.y, x + position.origin.x, inverter_color);
            }
#else
            set_pixel(bitmap_data, bytes_per_row, y + position.origin.y, x + position.origin.x,
                      1 - get_pixel(bitmap_data, bytes_per_row, y + position.origin.y, x + position.origin.x));
#endif
        }
    }

    graphics_release_frame_buffer(ctx, fb);
}
