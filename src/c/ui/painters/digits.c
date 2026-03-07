#include "digits.h"

/*
    ----   <-- 1
  |      | <-- 3
  | <-- 2|
    ----   <-- 4
  | <-- 5|
  |      | <-- 6
    ----   <-- 7
*/

// Lookup table for segment activation (0=off, 1=on)
static const uint8_t s_digit_segments[10][7] = {
  {1, 1, 1, 0, 1, 1, 1}, // 0
  {0, 0, 1, 0, 0, 1, 0}, // 1
  {1, 0, 1, 1, 1, 0, 1}, // 2
  {1, 0, 1, 1, 0, 1, 1}, // 3
  {0, 1, 1, 1, 0, 1, 0}, // 4
  {1, 1, 0, 1, 0, 1, 1}, // 5
  {1, 1, 0, 1, 1, 1, 1}, // 6
  {1, 0, 1, 0, 0, 1, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 0, 1, 1}  // 9
};

// Internal helpers (declarations)
static void paint_15(GContext* ctx, const uint8_t* segs, GPoint pos);
static void paint_41(GContext* ctx, const uint8_t* segs, GPoint pos);
static void paint_seg_15(GContext* ctx, int seg, GPoint pos);
static void paint_seg_41(GContext* ctx, int seg, GPoint pos);

void digits_paint(GContext* ctx, int digit, int size, GPoint pos) {
  if (digit < 0 || digit > 9) return;
  const uint8_t *segs = s_digit_segments[digit];

  if (size == 15) {
      paint_15(ctx, segs, pos);
  } else if (size == 41) {
      paint_41(ctx, segs, pos);
  }
}

void digits_clear(GContext* ctx, int size, GPoint pos) {
    if (size == 15) {
        graphics_fill_rect(ctx, GRect(pos.x, pos.y, 10, 15), 0, GCornerNone);
    } else if (size == 41) {
        graphics_fill_rect(ctx, GRect(pos.x, pos.y, 26, 41), 0, GCornerNone);
    }
}

// --- Implementation Details ---

static void paint_15(GContext* ctx, const uint8_t* segs, GPoint pos) {
    for (int i = 0; i < 7; i++) {
        if (segs[i]) paint_seg_15(ctx, i+1, pos);
    }
}

static void paint_41(GContext* ctx, const uint8_t* segs, GPoint pos) {
    for (int i = 0; i < 7; i++) {
        if (segs[i]) paint_seg_41(ctx, i+1, pos);
    }
}

static void paint_seg_15(GContext* ctx, int seg, GPoint pos) {
    switch(seg) {
        case 1: 
            graphics_draw_line(ctx, GPoint(1+pos.x, 0+pos.y), GPoint(8+pos.x, 0+pos.y));
            graphics_draw_line(ctx, GPoint(2+pos.x, 1+pos.y), GPoint(7+pos.x, 1+pos.y));
            break;
        case 2:
            graphics_draw_line(ctx, GPoint(0+pos.x, 1+pos.y), GPoint(0+pos.x, 6+pos.y));
            graphics_draw_line(ctx, GPoint(1+pos.x, 2+pos.y), GPoint(1+pos.x, 5+pos.y));
            break;
        case 3:
            graphics_draw_line(ctx, GPoint(9+pos.x, 1+pos.y), GPoint(9+pos.x, 6+pos.y));
            graphics_draw_line(ctx, GPoint(8+pos.x, 2+pos.y), GPoint(8+pos.x, 5+pos.y));
            break;
        case 4:
            graphics_draw_line(ctx, GPoint(2+pos.x, 6+pos.y), GPoint(7+pos.x, 6+pos.y));
            graphics_draw_line(ctx, GPoint(1+pos.x, 7+pos.y), GPoint(8+pos.x, 7+pos.y));
            break;
        case 5:
            graphics_draw_line(ctx, GPoint(0+pos.x, 8+pos.y), GPoint(0+pos.x, 13+pos.y));
            graphics_draw_line(ctx, GPoint(1+pos.x, 9+pos.y), GPoint(1+pos.x, 12+pos.y));
            break;
        case 6:
            graphics_draw_line(ctx, GPoint(9+pos.x, 8+pos.y), GPoint(9+pos.x, 13+pos.y));
            graphics_draw_line(ctx, GPoint(8+pos.x, 9+pos.y), GPoint(8+pos.x, 12+pos.y));
            break;
        case 7:
            graphics_draw_line(ctx, GPoint(1+pos.x, 14+pos.y), GPoint(8+pos.x, 14+pos.y));
            graphics_draw_line(ctx, GPoint(2+pos.x, 13+pos.y), GPoint(7+pos.x, 13+pos.y));
            break;
    }
}

static void paint_seg_41(GContext* ctx, int seg, GPoint pos) {
    switch(seg) {
        case 1:
            graphics_draw_line(ctx, GPoint(2+pos.x, 0+pos.y), GPoint(23+pos.x, 0+pos.y));
            graphics_draw_line(ctx, GPoint(3+pos.x, 1+pos.y), GPoint(22+pos.x, 1+pos.y));
            graphics_draw_line(ctx, GPoint(4+pos.x, 2+pos.y), GPoint(21+pos.x, 2+pos.y));
            graphics_draw_line(ctx, GPoint(5+pos.x, 3+pos.y), GPoint(20+pos.x, 3+pos.y));
            graphics_draw_line(ctx, GPoint(6+pos.x, 4+pos.y), GPoint(19+pos.x, 4+pos.y));
            graphics_draw_line(ctx, GPoint(7+pos.x, 5+pos.y), GPoint(18+pos.x, 5+pos.y));
            break;
        case 2:
            graphics_draw_line(ctx, GPoint(0+pos.x, 2+pos.y), GPoint(0+pos.x, 17+pos.y));
            graphics_draw_line(ctx, GPoint(1+pos.x, 3+pos.y), GPoint(1+pos.x, 18+pos.y));
            graphics_draw_line(ctx, GPoint(2+pos.x, 4+pos.y), GPoint(2+pos.x, 19+pos.y));
            graphics_draw_line(ctx, GPoint(3+pos.x, 5+pos.y), GPoint(3+pos.x, 18+pos.y));
            graphics_draw_line(ctx, GPoint(4+pos.x, 6+pos.y), GPoint(4+pos.x, 17+pos.y));
            graphics_draw_line(ctx, GPoint(5+pos.x, 7+pos.y), GPoint(5+pos.x, 16+pos.y));
            break;
        case 3:
            graphics_draw_line(ctx, GPoint(25+pos.x, 2+pos.y), GPoint(25+pos.x, 17+pos.y));
            graphics_draw_line(ctx, GPoint(24+pos.x, 3+pos.y), GPoint(24+pos.x, 18+pos.y));
            graphics_draw_line(ctx, GPoint(23+pos.x, 4+pos.y), GPoint(23+pos.x, 19+pos.y));
            graphics_draw_line(ctx, GPoint(22+pos.x, 5+pos.y), GPoint(22+pos.x, 18+pos.y));
            graphics_draw_line(ctx, GPoint(21+pos.x, 6+pos.y), GPoint(21+pos.x, 17+pos.y));
            graphics_draw_line(ctx, GPoint(20+pos.x, 7+pos.y), GPoint(20+pos.x, 16+pos.y));
            break;
        case 4:
            graphics_draw_line(ctx, GPoint(4+pos.x, 20+pos.y), GPoint(21+pos.x, 20+pos.y));
            graphics_draw_line(ctx, GPoint(4+pos.x, 21+pos.y), GPoint(21+pos.x, 21+pos.y));
            graphics_draw_line(ctx, GPoint(5+pos.x, 19+pos.y), GPoint(20+pos.x, 19+pos.y));
            graphics_draw_line(ctx, GPoint(6+pos.x, 18+pos.y), GPoint(19+pos.x, 18+pos.y));
            graphics_draw_line(ctx, GPoint(5+pos.x, 22+pos.y), GPoint(20+pos.x, 22+pos.y));
            graphics_draw_line(ctx, GPoint(6+pos.x, 23+pos.y), GPoint(19+pos.x, 23+pos.y));
            break;
        case 5:
            graphics_draw_line(ctx, GPoint(0+pos.x, 38+pos.y), GPoint(0+pos.x, 24+pos.y));
            graphics_draw_line(ctx, GPoint(1+pos.x, 37+pos.y), GPoint(1+pos.x, 23+pos.y));
            graphics_draw_line(ctx, GPoint(2+pos.x, 36+pos.y), GPoint(2+pos.x, 22+pos.y));
            graphics_draw_line(ctx, GPoint(3+pos.x, 35+pos.y), GPoint(3+pos.x, 23+pos.y));
            graphics_draw_line(ctx, GPoint(4+pos.x, 34+pos.y), GPoint(4+pos.x, 24+pos.y));
            graphics_draw_line(ctx, GPoint(5+pos.x, 33+pos.y), GPoint(5+pos.x, 25+pos.y));
            break;
        case 6:
            graphics_draw_line(ctx, GPoint(25+pos.x, 38+pos.y), GPoint(25+pos.x, 24+pos.y));
            graphics_draw_line(ctx, GPoint(24+pos.x, 37+pos.y), GPoint(24+pos.x, 23+pos.y));
            graphics_draw_line(ctx, GPoint(23+pos.x, 36+pos.y), GPoint(23+pos.x, 22+pos.y));
            graphics_draw_line(ctx, GPoint(22+pos.x, 35+pos.y), GPoint(22+pos.x, 23+pos.y));
            graphics_draw_line(ctx, GPoint(21+pos.x, 34+pos.y), GPoint(21+pos.x, 24+pos.y));
            graphics_draw_line(ctx, GPoint(20+pos.x, 33+pos.y), GPoint(20+pos.x, 25+pos.y));
            break;
        case 7:
            graphics_draw_line(ctx, GPoint(2+pos.x, 40+pos.y), GPoint(23+pos.x, 40+pos.y));
            graphics_draw_line(ctx, GPoint(3+pos.x, 39+pos.y), GPoint(22+pos.x, 39+pos.y));
            graphics_draw_line(ctx, GPoint(4+pos.x, 38+pos.y), GPoint(21+pos.x, 38+pos.y));
            graphics_draw_line(ctx, GPoint(5+pos.x, 37+pos.y), GPoint(20+pos.x, 37+pos.y));
            graphics_draw_line(ctx, GPoint(6+pos.x, 36+pos.y), GPoint(19+pos.x, 36+pos.y));
            graphics_draw_line(ctx, GPoint(7+pos.x, 35+pos.y), GPoint(18+pos.x, 35+pos.y));
            break;
    }
}

