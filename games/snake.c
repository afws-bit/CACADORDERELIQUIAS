// =============================================================================
// SNAKE GAME - Compile to shared library for SPANE Engine
// =============================================================================
// Compile: gcc -shared -fPIC -O3 -o snake.so snake.c
// =============================================================================

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// SDK types (must match engine)
#define MAIN_WINDOW_WIDTH  1000
#define MAIN_WINDOW_HEIGHT 700
#define GAME_AREA_WIDTH    800
#define GAME_AREA_HEIGHT   600
#define GAME_AREA_X        180
#define GAME_AREA_Y        50

typedef struct Framebuffer {
    unsigned char pixels[MAIN_WINDOW_WIDTH * MAIN_WINDOW_HEIGHT * 4];
} Framebuffer;

typedef struct Game {
    char name[64];
    char path[512];
    void* data;
    void* handle;
    int active;
    void (*init)(struct Game* game);
    void (*handle_key)(struct Game* game, int key_code, int pressed);
    void (*handle_click)(struct Game* game, int x, int y);
    void (*update)(struct Game* game);
    void (*render)(struct Game* game, Framebuffer* fb);
    void (*cleanup)(struct Game* game);
} Game;

// Font data
static const unsigned char font_5x7[][5] = {
    {0x00,0x00,0x00,0x00,0x00},{0x00,0x00,0x5f,0x00,0x00},{0x00,0x07,0x00,0x07,0x00},
    {0x14,0x7f,0x14,0x7f,0x14},{0x24,0x2a,0x7f,0x2a,0x12},{0x23,0x13,0x08,0x64,0x62},
    {0x36,0x49,0x55,0x22,0x50},{0x00,0x05,0x03,0x00,0x00},{0x00,0x1c,0x22,0x41,0x00},
    {0x00,0x41,0x22,0x1c,0x00},{0x08,0x2a,0x1c,0x2a,0x08},{0x08,0x08,0x3e,0x08,0x08},
    {0x00,0x50,0x30,0x00,0x00},{0x08,0x08,0x08,0x08,0x08},{0x00,0x60,0x60,0x00,0x00},
    {0x20,0x10,0x08,0x04,0x02},{0x3e,0x51,0x49,0x45,0x3e},{0x00,0x42,0x7f,0x40,0x00},
    {0x42,0x61,0x51,0x49,0x46},{0x21,0x41,0x45,0x4b,0x31},{0x18,0x14,0x12,0x7f,0x10},
    {0x27,0x45,0x45,0x45,0x39},{0x3c,0x4a,0x49,0x49,0x30},{0x01,0x71,0x09,0x05,0x03},
    {0x36,0x49,0x49,0x49,0x36},{0x06,0x49,0x49,0x29,0x1e},{0x00,0x36,0x36,0x00,0x00},
    {0x00,0x56,0x36,0x00,0x00},{0x00,0x08,0x14,0x22,0x41},{0x14,0x14,0x14,0x14,0x14},
    {0x41,0x22,0x14,0x08,0x00},{0x02,0x01,0x51,0x09,0x06},{0x32,0x49,0x79,0x41,0x3e},
    {0x7e,0x11,0x11,0x11,0x7e},{0x7f,0x49,0x49,0x49,0x36},{0x3e,0x41,0x41,0x41,0x22},
    {0x7f,0x41,0x41,0x22,0x1c},{0x7f,0x49,0x49,0x49,0x41},{0x7f,0x09,0x09,0x01,0x01},
    {0x3e,0x41,0x41,0x51,0x32},{0x7f,0x08,0x08,0x08,0x7f},{0x00,0x41,0x7f,0x41,0x00},
    {0x20,0x40,0x41,0x3f,0x01},{0x7f,0x08,0x14,0x22,0x41},{0x7f,0x40,0x40,0x40,0x40},
    {0x7f,0x02,0x04,0x02,0x7f},{0x7f,0x04,0x08,0x10,0x7f},{0x3e,0x41,0x41,0x41,0x3e},
    {0x7f,0x09,0x09,0x09,0x06},{0x3e,0x41,0x51,0x21,0x5e},{0x7f,0x09,0x19,0x29,0x46},
    {0x46,0x49,0x49,0x49,0x31},{0x01,0x01,0x7f,0x01,0x01},{0x3f,0x40,0x40,0x40,0x3f},
    {0x1f,0x20,0x40,0x20,0x1f},{0x7f,0x20,0x18,0x20,0x7f},{0x63,0x14,0x08,0x14,0x63},
    {0x03,0x04,0x78,0x04,0x03},{0x61,0x51,0x49,0x45,0x43},{0x00,0x00,0x7f,0x41,0x41},
    {0x02,0x04,0x08,0x10,0x20},{0x41,0x41,0x7f,0x00,0x00},{0x04,0x02,0x01,0x02,0x04},
    {0x40,0x40,0x40,0x40,0x40},{0x00,0x01,0x02,0x04,0x00},{0x20,0x54,0x54,0x54,0x78},
    {0x7f,0x48,0x44,0x44,0x38},{0x38,0x44,0x44,0x44,0x20},{0x38,0x44,0x44,0x48,0x7f},
    {0x38,0x54,0x54,0x54,0x18},{0x08,0x7e,0x09,0x01,0x02},{0x08,0x14,0x54,0x54,0x3c},
    {0x7f,0x08,0x04,0x04,0x78},{0x00,0x44,0x7d,0x40,0x00},{0x20,0x40,0x44,0x3d,0x00},
    {0x00,0x7f,0x10,0x28,0x44},{0x00,0x41,0x7f,0x40,0x00},{0x7c,0x04,0x18,0x04,0x78},
    {0x7c,0x08,0x04,0x04,0x78},{0x38,0x44,0x44,0x44,0x38},{0x7c,0x14,0x14,0x14,0x08},
    {0x08,0x14,0x14,0x18,0x7c},{0x7c,0x08,0x04,0x04,0x08},{0x48,0x54,0x54,0x54,0x20},
    {0x04,0x3f,0x44,0x40,0x20},{0x3c,0x40,0x40,0x20,0x7c},{0x1c,0x20,0x40,0x20,0x1c},
    {0x3c,0x40,0x30,0x40,0x3c},{0x44,0x28,0x10,0x28,0x44},{0x0c,0x50,0x50,0x50,0x3c},
    {0x44,0x64,0x54,0x4c,0x44},{0x00,0x08,0x36,0x41,0x00},{0x00,0x00,0x7f,0x00,0x00},
    {0x00,0x41,0x36,0x08,0x00},{0x08,0x08,0x2a,0x1c,0x08}
};

// Drawing primitives
static inline void fb_pixel(Framebuffer* fb, int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    if (x < 0 || x >= MAIN_WINDOW_WIDTH || y < 0 || y >= MAIN_WINDOW_HEIGHT) return;
    int i = (y * MAIN_WINDOW_WIDTH + x) * 4;
    fb->pixels[i]=r; fb->pixels[i+1]=g; fb->pixels[i+2]=b; fb->pixels[i+3]=255;
}

static void fb_fill(Framebuffer* fb, int x, int y, int w, int h, unsigned char r, unsigned char g, unsigned char b) {
    for (int dy=0; dy<h; dy++)
        for (int dx=0; dx<w; dx++)
            fb_pixel(fb, x+dx, y+dy, r, g, b);
}

static void fb_rect(Framebuffer* fb, int x, int y, int w, int h, unsigned char r, unsigned char g, unsigned char b) {
    for (int dx=0; dx<w; dx++) { fb_pixel(fb,x+dx,y,r,g,b); fb_pixel(fb,x+dx,y+h-1,r,g,b); }
    for (int dy=1; dy<h-1; dy++) { fb_pixel(fb,x,y+dy,r,g,b); fb_pixel(fb,x+w-1,y+dy,r,g,b); }
}

static void fb_circle(Framebuffer* fb, int cx, int cy, int radius, unsigned char R, unsigned char G, unsigned char B) {
    for (int dy=-radius; dy<=radius; dy++)
        for (int dx=-radius; dx<=radius; dx++)
            if (dx*dx+dy*dy <= radius*radius) fb_pixel(fb, cx+dx, cy+dy, R, G, B);
}

static void fb_char(Framebuffer* fb, int x, int y, char c, unsigned char r, unsigned char g, unsigned char b) {
    if (c<32||c>126) return;
    const unsigned char* glyph = font_5x7[c-32];
    for (int row=0; row<7; row++)
        for (int col=0; col<5; col++)
            if (glyph[col] & (1<<row)) fb_pixel(fb, x+col, y+row, r, g, b);
}

static void fb_text(Framebuffer* fb, int x, int y, const char* s, unsigned char r, unsigned char g, unsigned char b) {
    for (int i=0; s[i]; i++) fb_char(fb, x+i*6, y, s[i], r, g, b);
}

// =============================================================================
// SNAKE GAME DATA
// =============================================================================

#define SG 20
#define SML 500
#define SAW 800
#define SAH 600
#define SNC (SAW/SG)
#define SNR (SAH/SG)

typedef struct { int x, y; } Pt;

typedef struct {
    Pt s[SML];
    int len, dir, ndir, score, go, fc, spd;
    Pt food;
} SnakeData;

static void sn_food(SnakeData* d) {
    int v;
    do {
        v = 1;
        d->food.x = rand() % SNC;
        d->food.y = rand() % SNR;
        for (int i = 0; i < d->len; i++) {
            if (d->s[i].x == d->food.x && d->s[i].y == d->food.y) {
                v = 0;
                break;
            }
        }
    } while (!v);
}

// =============================================================================
// SNAKE GAME FUNCTIONS
// =============================================================================

static void sn_init(Game* game) {
    SnakeData* d = calloc(1, sizeof(SnakeData));
    d->len = 3;
    int sx = SNC/2, sy = SNR/2;
    for (int i = 0; i < d->len; i++) {
        d->s[i].x = sx - i;
        d->s[i].y = sy;
    }
    d->spd = 10;
    sn_food(d);
    game->data = d;
    printf("Snake initialized\n");
}

static void sn_handle_key(Game* game, int key_code, int pressed) {
    SnakeData* d = game->data;
    if (!pressed) return;
    
    if (d->go) {
        if (key_code == 82) { // R key
            d->len = 3;
            int sx = SNC/2, sy = SNR/2;
            for (int i = 0; i < d->len; i++) {
                d->s[i].x = sx - i;
                d->s[i].y = sy;
            }
            d->dir = 0;
            d->ndir = 0;
            d->score = 0;
            d->go = 0;
            d->fc = 0;
            sn_food(d);
        }
        return;
    }
    
    switch (key_code) {
        case 38: case 87: if (d->dir != 1) d->ndir = 3; break;
        case 40: case 83: if (d->dir != 3) d->ndir = 1; break;
        case 37: case 65: if (d->dir != 0) d->ndir = 2; break;
        case 39: case 68: if (d->dir != 2) d->ndir = 0; break;
    }
}

static void sn_handle_click(Game* game, int x, int y) {
    (void)game; (void)x; (void)y;
}

static void sn_update(Game* game) {
    SnakeData* d = game->data;
    if (d->go) return;
    
    if (++d->fc < d->spd) return;
    d->fc = 0;
    d->dir = d->ndir;
    
    Pt nh = d->s[0];
    switch (d->dir) {
        case 0: nh.x++; break;
        case 1: nh.y++; break;
        case 2: nh.x--; break;
        case 3: nh.y--; break;
    }
    
    if (nh.x < 0 || nh.x >= SNC || nh.y < 0 || nh.y >= SNR) {
        d->go = 1;
        return;
    }
    
    for (int i = 0; i < d->len; i++) {
        if (d->s[i].x == nh.x && d->s[i].y == nh.y) {
            d->go = 1;
            return;
        }
    }
    
    int ate = (nh.x == d->food.x && nh.y == d->food.y);
    for (int i = d->len - 1; i > 0; i--) d->s[i] = d->s[i-1];
    d->s[0] = nh;
    
    if (ate) {
        d->len++;
        d->score += 10;
        if (d->spd > 3) d->spd--;
        sn_food(d);
    }
}

static void sn_render(Game* game, Framebuffer* fb) {
    SnakeData* d = game->data;
    fb_fill(fb, GAME_AREA_X, GAME_AREA_Y, SAW, SAH, 0x1A, 0x1A, 0x2E);
    
    // Grid lines
    for (int i = 0; i <= SNC; i++)
        fb_fill(fb, GAME_AREA_X + i*SG, GAME_AREA_Y, 1, SAH, 0x25, 0x25, 0x40);
    for (int i = 0; i <= SNR; i++)
        fb_fill(fb, GAME_AREA_X, GAME_AREA_Y + i*SG, SAW, 1, 0x25, 0x25, 0x40);
    
    // Food
    int fx = GAME_AREA_X + d->food.x * SG;
    int fy = GAME_AREA_Y + d->food.y * SG;
    fb_circle(fb, fx + SG/2, fy + SG/2, SG/2 - 2, 0xFF, 0x33, 0x33);
    
    // Snake
    for (int i = 0; i < d->len; i++) {
        int sx = GAME_AREA_X + d->s[i].x * SG + 1;
        int sy = GAME_AREA_Y + d->s[i].y * SG + 1;
        fb_fill(fb, sx, sy, SG-2, SG-2, 
                i == 0 ? 0x00 : 0x00, 
                i == 0 ? 0xFF : 0xAA, 
                0x00);
    }
    
    char t[50];
    snprintf(t, sizeof(t), "Score: %d", d->score);
    fb_text(fb, GAME_AREA_X + 10, GAME_AREA_Y + 20, t, 0xFF, 0xFF, 0xFF);
    
    if (d->go) {
        fb_fill(fb, GAME_AREA_X + 250, GAME_AREA_Y + 250, 300, 100, 0, 0, 0);
        fb_rect(fb, GAME_AREA_X + 250, GAME_AREA_Y + 250, 300, 100, 0xFF, 0, 0);
        fb_text(fb, GAME_AREA_X + 340, GAME_AREA_Y + 270, "GAME OVER!", 0xFF, 0, 0);
        fb_text(fb, GAME_AREA_X + 320, GAME_AREA_Y + 300, "Press R to restart", 0xFF, 0xFF, 0xFF);
    }
    
    snprintf(t, sizeof(t), "Snake - Arrows/WASD - Length:%d", d->len);
    fb_text(fb, GAME_AREA_X + 10, GAME_AREA_Y + SAH - 10, t, 0x88, 0x88, 0x88);
}

static void sn_cleanup(Game* game) {
    free(game->data);
}

// =============================================================================
// EXPORTED GAME CREATOR - Creates NEW instance each time
// =============================================================================

Game* create_game() {
    Game* game = calloc(1, sizeof(Game));
    if (!game) return NULL;
    
    strcpy(game->name, "Snake");
    game->active = 1;
    game->init = sn_init;
    game->handle_key = sn_handle_key;
    game->handle_click = sn_handle_click;
    game->update = sn_update;
    game->render = sn_render;
    game->cleanup = sn_cleanup;
    
    return game;
}