// =============================================================================
// RPG GAME - Compile to shared library for SPANE Engine
// =============================================================================
// Compile: gcc -shared -fPIC -O3 -o rpg.so rpg.c
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
// RPG GAME DATA
// =============================================================================

#define TILE_SIZE     32
#define MAP_WIDTH     25
#define MAP_HEIGHT    18
#define PLAYER_SPEED  5

typedef struct {
    int map[18][25];
    int px, py, dir, frame, moving;
    int ku, kd, kl, kr;
} RPGData;

static int rpg_map[18][25] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,3,3,3,0,0,0,0,0,0,4,0,0,5,0,0,0,0,0,0,0,0,1},
    {1,0,0,3,3,3,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,2,2,2,0,1},
    {1,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,3,3,3,2,2,2,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,5,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,3,3,3,0,0,0,0,0,0,5,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,3,3,3,0,0,0,0,0,0,0,0,0,0,4,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

static int rpg_walkable(RPGData* d, int x, int y) {
    return (x>=0 && x<MAP_WIDTH && y>=0 && y<MAP_HEIGHT && (d->map[y][x]==0 || d->map[y][x]==3));
}

static void rpg_tile(Framebuffer* fb, int x, int y, int t) {
    unsigned char r,g,b;
    switch(t) {
        case 0: r=0x33; g=0xAA; b=0x33; break;
        case 1: r=0x66; g=0x66; b=0x66; break;
        case 2: r=0x33; g=0x55; b=0xAA; break;
        case 3: r=0xCC; g=0xAA; b=0x66; break;
        case 4: r=0x22; g=0x66; b=0x22; break;
        case 5: r=0x88; g=0x44; b=0x22; break;
        default: r=g=b=0;
    }
    fb_fill(fb, x, y, TILE_SIZE, TILE_SIZE, r, g, b);
    if (t == 4) {
        fb_circle(fb, x+16, y, TILE_SIZE/2+2, 0x00, 0xCC, 0x00);
        fb_fill(fb, x+13, y+16, 6, 16, 0x66, 0x44, 0x22);
    }
    if (t == 5) {
        fb_fill(fb, x+4, y+8, 24, 24, 0xCC, 0x33, 0x33);
        for (int i=0; i<12; i++)
            fb_fill(fb, x+12-i, y+8-i, i*2+4, 1, 0x88, 0x44, 0x22);
    }
}

static void rpg_player(Framebuffer* fb, int x, int y, int dir, int frame) {
    fb_fill(fb, x+8, y+12, 16, 14, 0x33, 0x66, 0xFF);
    fb_circle(fb, x+16, y+10, 8, 0xFF, 0xCC, 0x99);
    if (dir == 1) fb_fill(fb, x+9, y+8, 3, 3, 0, 0, 0);
    else if (dir == 2) fb_fill(fb, x+20, y+8, 3, 3, 0, 0, 0);
    else { fb_fill(fb, x+12, y+8, 3, 3, 0, 0, 0); fb_fill(fb, x+19, y+8, 3, 3, 0, 0, 0); }
    if (frame%20 < 10) {
        fb_fill(fb, x+9, y+26, 5, 8, 0, 0, 0x88);
        fb_fill(fb, x+18, y+26, 5, 8, 0, 0, 0x88);
    } else {
        fb_fill(fb, x+10, y+26, 5, 8, 0, 0, 0x88);
        fb_fill(fb, x+17, y+26, 5, 8, 0, 0, 0x88);
    }
}

// =============================================================================
// RPG GAME FUNCTIONS
// =============================================================================

static void rpg_init(Game* game) {
    RPGData* d = calloc(1, sizeof(RPGData));
    memcpy(d->map, rpg_map, sizeof(rpg_map));
    d->px = 5 * TILE_SIZE;
    d->py = 5 * TILE_SIZE;
    game->data = d;
    printf("RPG Adventure initialized\n");
}

static void rpg_handle_key(Game* game, int key_code, int pressed) {
    RPGData* d = game->data;
    switch (key_code) {
        case 38: case 87: d->ku = pressed; if (pressed) d->dir = 3; break;
        case 40: case 83: d->kd = pressed; if (pressed) d->dir = 0; break;
        case 37: case 65: d->kl = pressed; if (pressed) d->dir = 1; break;
        case 39: case 68: d->kr = pressed; if (pressed) d->dir = 2; break;
    }
}

static void rpg_handle_click(Game* game, int x, int y) {
    RPGData* d = game->data;
    if (x >= GAME_AREA_X && x < GAME_AREA_X+GAME_AREA_WIDTH && 
        y >= GAME_AREA_Y && y < GAME_AREA_Y+GAME_AREA_HEIGHT) {
        int nx = x - GAME_AREA_X - 16;
        int ny = y - GAME_AREA_Y - 16;
        if (rpg_walkable(d, nx/TILE_SIZE, ny/TILE_SIZE) && 
            rpg_walkable(d, (nx+31)/TILE_SIZE, (ny+31)/TILE_SIZE)) {
            d->px = nx;
            d->py = ny;
        }
    }
}

static void rpg_update(Game* game) {
    RPGData* d = game->data;
    d->moving = 0;
    int nx = d->px, ny = d->py;
    
    if (d->ku) { ny -= PLAYER_SPEED; d->moving = 1; }
    if (d->kd) { ny += PLAYER_SPEED; d->moving = 1; }
    if (d->kl) { nx -= PLAYER_SPEED; d->moving = 1; }
    if (d->kr) { nx += PLAYER_SPEED; d->moving = 1; }
    
    if (d->moving && rpg_walkable(d, nx/TILE_SIZE, ny/TILE_SIZE) && 
        rpg_walkable(d, (nx+31)/TILE_SIZE, ny/TILE_SIZE) &&
        rpg_walkable(d, nx/TILE_SIZE, (ny+31)/TILE_SIZE) && 
        rpg_walkable(d, (nx+31)/TILE_SIZE, (ny+31)/TILE_SIZE)) {
        d->px = nx;
        d->py = ny;
    }
    
    if (d->px < 0) d->px = 0;
    if (d->py < 0) d->py = 0;
    if (d->px > (MAP_WIDTH-1)*TILE_SIZE) d->px = (MAP_WIDTH-1)*TILE_SIZE;
    if (d->py > (MAP_HEIGHT-1)*TILE_SIZE) d->py = (MAP_HEIGHT-1)*TILE_SIZE;
    
    if (d->moving) d->frame++;
    else d->frame = 0;
}

static void rpg_render(Game* game, Framebuffer* fb) {
    RPGData* d = game->data;
    fb_fill(fb, GAME_AREA_X, GAME_AREA_Y, GAME_AREA_WIDTH, GAME_AREA_HEIGHT, 0, 0, 0);
    
    for (int y = 0; y < MAP_HEIGHT; y++)
        for (int x = 0; x < MAP_WIDTH; x++)
            rpg_tile(fb, GAME_AREA_X + x*TILE_SIZE, GAME_AREA_Y + y*TILE_SIZE, d->map[y][x]);
    
    rpg_player(fb, GAME_AREA_X + d->px, GAME_AREA_Y + d->py, d->dir, d->frame);
    
    char s[200];
    snprintf(s, sizeof(s), "RPG Adventure - Pos:(%d,%d) - ArrowKeys/WASD", 
             d->px/TILE_SIZE, d->py/TILE_SIZE);
    fb_text(fb, GAME_AREA_X+10, GAME_AREA_Y+GAME_AREA_HEIGHT-10, s, 0xFF, 0xFF, 0xFF);
}

static void rpg_cleanup(Game* game) {
    free(game->data);
}

// =============================================================================
// EXPORTED GAME CREATOR - Creates NEW instance each time
// =============================================================================

Game* create_game() {
    Game* game = calloc(1, sizeof(Game));
    if (!game) return NULL;
    
    strcpy(game->name, "RPG Adventure");
    game->active = 1;
    game->init = rpg_init;
    game->handle_key = rpg_handle_key;
    game->handle_click = rpg_handle_click;
    game->update = rpg_update;
    game->render = rpg_render;
    game->cleanup = rpg_cleanup;
    
    return game;
}