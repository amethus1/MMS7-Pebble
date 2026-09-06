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

// --- Parametric (scalable) renderer for large displays ---
// Generalizes the hand-drawn fixed-size art to any box size. The formulas
// below reproduce paint_seg_41 EXACTLY at 26x41 (t=6, g=2): trapezoid
// top/bottom bars, a diamond-ended middle bar, and verticals with 45-degree
// outer corners and pointed tips toward the middle, all separated by the
// same diagonal corner gaps as the original art.

static int iabs(int v) { return v < 0 ? -v : v; }

void digits_paint_box(GContext* ctx, int digit, int w, int h, GPoint pos) {
    if (digit < 0 || digit > 9) return;
    const uint8_t* segs = s_digit_segments[digit];

    int t = (h * 6 + 20) / 41;   // segment thickness, same 6/41 ratio as the 41px art
    if (t < 2) t = 2;
    if (t > w / 3) t = w / 3;    // keep verticals from colliding on narrow boxes
    int g = (t + 1) / 3;         // diagonal corner gap (2 at t=6, like the original)
    if (g < 1) g = 1;
    int m0 = (h - t + 1) / 2;    // top row of the middle bar
    int jpeak = t / 2 - 1;       // column where the vertical tips peak
    if (jpeak < 0) jpeak = 0;
    int px = pos.x, py = pos.y;

    if (segs[0]) {  // 1 top bar: narrows downward
        for (int i = 0; i < t; i++)
            graphics_draw_line(ctx, GPoint(px + g + i, py + i), GPoint(px + w - 1 - g - i, py + i));
    }
    if (segs[6]) {  // 7 bottom bar: narrows upward
        for (int i = 0; i < t; i++)
            graphics_draw_line(ctx, GPoint(px + g + i, py + h - 1 - i), GPoint(px + w - 1 - g - i, py + h - 1 - i));
    }
    if (segs[3]) {  // 4 middle bar: diamond-pointed ends
        for (int k = 0; k < t; k++) {
            int inset = 2 * g + iabs(2 * k - (t - 1)) / 2;
            graphics_draw_line(ctx, GPoint(px + inset, py + m0 + k), GPoint(px + w - 1 - inset, py + m0 + k));
        }
    }
    for (int j = 0; j < t; j++) {  // verticals: 45-degree outer corners, pointed inner tips
        int tip = jpeak - iabs(j - jpeak);
        int top_start = g + j;
        int top_end   = m0 - 1 + tip;
        int bot_start = m0 + t - tip;
        int bot_end   = h - 1 - g - j;
        if (segs[1]) graphics_draw_line(ctx, GPoint(px + j,         py + top_start), GPoint(px + j,         py + top_end));  // 2 top-left
        if (segs[2]) graphics_draw_line(ctx, GPoint(px + w - 1 - j, py + top_start), GPoint(px + w - 1 - j, py + top_end));  // 3 top-right
        if (segs[4]) graphics_draw_line(ctx, GPoint(px + j,         py + bot_start), GPoint(px + j,         py + bot_end));  // 5 bottom-left
        if (segs[5]) graphics_draw_line(ctx, GPoint(px + w - 1 - j, py + bot_start), GPoint(px + w - 1 - j, py + bot_end));  // 6 bottom-right
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
