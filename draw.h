#include <efi.h>
#include <efilib.h>
#include <efiprot.h>

#define abs(x) (x < 0 ? x * -1 : x)

typedef struct {
  unsigned int width;
  unsigned int height;
  unsigned int mode;
} GraphicsSettings;

// Primitives
void draw_pixel(void *fb_base_addr, int x, int y, uint32_t color, GraphicsSettings *settings);
void draw_line(void *fb_base_addr, int x0, int y0, int x1, int y1, uint32_t color, GraphicsSettings *settings);

// Frame functions

// Draws an entire frame
void draw_full(void *fb_base, int x, int y, int player_y, GraphicsSettings *settings);

// Only redraws necessary areas for ball & player
void draw_delta(void *fb_base, int x0, int y0, int x1, int y1, int player_y0, int player_y1, GraphicsSettings *settings);