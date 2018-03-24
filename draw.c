#include "draw.h"

void draw_pixel(void *fb_base_addr, int x, int y, uint32_t color, GraphicsSettings *settings)
{
  int32_t *addr = fb_base_addr + (x * 4) + (y * 4 * settings->width);
  *addr = color | 0xff000000;
}

void draw_line(void *fb_base_addr, int x0, int y0, int x1, int y1, uint32_t color, GraphicsSettings *settings) {
  int x = x0, y = y0;

  draw_pixel(fb_base_addr, x, y, color, settings);

  do {
    int dx = x1 - x;
    int dy = y1 - y;

    if (abs(dx) > abs(dy)) x += abs(dx) / dx;
    else y += abs(dy) / dy;

    draw_pixel(fb_base_addr, x, y, color, settings);
  } while(x != x1 || y != y1);
}

// NOT FULLY IMPLEMENTED, needs to clear appropriate parts and draw new positions instead
void draw_delta(void *fb_base, int x0, int y0, int x1, int y1, int player_y0, int player_y1, GraphicsSettings *settings) {

  // clear & draw ball & player
  for (int i = 0; i < 10; i++)
  {
    draw_line(fb_base, x0, y0 + i, x0 + 10, y0 + i, 0x000000, settings);
    //draw_line(fb_base, 30 + i, player_y0, 30 + i, player_y0 + 50, 0x000000, settings);

    draw_line(fb_base, x1, y1 + i, x1 + 10, y1 + i, 0xffffff, settings);
    //draw_line(fb_base, 30 + i, player_y1, 30 + i, player_y1 + 50, 0x00ff00, settings);
  }

}

void draw_full(void *fb_base, int x, int y, int player_y, GraphicsSettings *settings) {

  int W = settings->width, H = settings->height;

  // fill screen
  for (int y = 0; y < H; y++)
  {
    for (int x = 0; x < W; x++)
      draw_pixel(fb_base, x, y, 0, settings);
  }

  // draw frame, ball & player
  for (int i = 0; i < 10; i++)
  {
    draw_line(fb_base, 10 + i, 10 + i, W - 10 - i, 10 + i, 0x00ffff, settings);
    draw_line(fb_base, 10 + i, H - 10 - i, W - 10 - i, H - 10 - i, 0x00ffff, settings);
    draw_line(fb_base, 10 + i, 10 + i, 10 + i, H - 10 - i, 0xff0000, settings);
    draw_line(fb_base, W - 10 - i, 10 + i, W - 10 - i, H - 10 - i, 0xff0000, settings);

    draw_line(fb_base, x, y + i, x + 10, y + i, 0xffffff, settings);

    draw_line(fb_base, 30 + i, player_y, 30 + i, player_y + 50, 0x00ff00, settings);
  }

}
