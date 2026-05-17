// =============================================================================
// RPG GAME - Enhanced Graphics Edition (SPANE Compatible)
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

// Enhanced font data
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

// Simple sine approximation using lookup table (avoids math library)
static int fast_sine(int angle) {
    // Sine lookup table for 256 angles (0-255 maps to 0-2π)
    static const int sine_table[256] = {
        0,6,12,18,24,30,36,42,48,54,59,65,70,75,80,85,
        89,94,98,102,105,108,112,114,117,119,121,122,123,124,124,124,
        123,122,121,119,117,114,112,108,105,102,98,94,89,85,80,75,
        70,65,59,54,48,42,36,30,24,18,12,6,0,-6,-12,-18,
        -24,-30,-36,-42,-48,-54,-59,-65,-70,-75,-80,-85,-89,-94,-98,-102,
        -105,-108,-112,-114,-117,-119,-121,-122,-123,-124,-124,-124,-123,-122,-121,-119,
        -117,-114,-112,-108,-105,-102,-98,-94,-89,-85,-80,-75,-70,-65,-59,-54,
        -48,-42,-36,-30,-24,-18,-12,-6,0,6,12,18,24,30,36,42,
        48,54,59,65,70,75,80,85,89,94,98,102,105,108,112,114,
        117,119,121,122,123,124,124,124,123,122,121,119,117,114,112,108,
        105,102,98,94,89,85,80,75,70,65,59,54,48,42,36,30,
        24,18,12,6,0,-6,-12,-18,-24,-30,-36,-42,-48,-54,-59,-65,
        -70,-75,-80,-85,-89,-94,-98,-102,-105,-108,-112,-114,-117,-119,-121,-122,
        -123,-124,-124,-124,-123,-122,-121,-119,-117,-114,-112,-108,-105,-102,-98,-94,
        -89,-85,-80,-75,-70,-65,-59,-54,-48,-42,-36,-30,-24,-18,-12,-6
    };
    return sine_table[angle & 255];
}

static int fast_cosine(int angle) {
    return fast_sine(angle + 64); // Cosine is sine shifted by 90 degrees
}

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
            if (dx*dx+dy*dy <= radius*radius) 
                fb_pixel(fb, cx+dx, cy+dy, R, G, B);
}

static void fb_circle_outline(Framebuffer* fb, int cx, int cy, int radius, unsigned char R, unsigned char G, unsigned char B) {
    for (int dy=-radius; dy<=radius; dy++)
        for (int dx=-radius; dx<=radius; dx++) {
            int dist = dx*dx+dy*dy;
            if (dist <= radius*radius && dist >= (radius-2)*(radius-2))
                fb_pixel(fb, cx+dx, cy+dy, R, G, B);
        }
}

static void fb_rect(Framebuffer* fb, int x, int y, int w, int h, unsigned char r, unsigned char g, unsigned char b) {
    for (int i=0; i<w; i++) {
        fb_pixel(fb, x+i, y, r, g, b);
        fb_pixel(fb, x+i, y+h-1, r, g, b);
    }
    for (int i=0; i<h; i++) {
        fb_pixel(fb, x, y+i, r, g, b);
        fb_pixel(fb, x+w-1, y+i, r, g, b);
    }
}

static void fb_char(Framebuffer* fb, int x, int y, char c, unsigned char r, unsigned char g, unsigned char b) {
    if (c<32||c>126) return;
    const unsigned char* glyph = font_5x7[c-32];
    for (int row=0; row<7; row++)
        for (int col=0; col<5; col++)
            if (glyph[col] & (1<<row)) 
                fb_pixel(fb, x+col, y+row, r, g, b);
}

static void fb_text(Framebuffer* fb, int x, int y, const char* s, unsigned char r, unsigned char g, unsigned char b) {
    for (int i=0; s[i]; i++) 
        fb_char(fb, x+i*6, y, s[i], r, g, b);
}

static void fb_text_shadow(Framebuffer* fb, int x, int y, const char* s, unsigned char r, unsigned char g, unsigned char b) {
    fb_text(fb, x+1, y+1, s, 0, 0, 0);
    fb_text(fb, x, y, s, r, g, b);
}

static void fb_text_center(Framebuffer* fb, int x, int y, int w, const char* s, unsigned char r, unsigned char g, unsigned char b) {
    fb_text(fb, x+(w-(int)strlen(s)*6)/2, y, s, r, g, b);
}

// =============================================================================
// RPG GAME DATA
// =============================================================================

#define TILE_SIZE     32
#define MAP_WIDTH     25
#define MAP_HEIGHT    18
#define PLAYER_SPEED  4
#define PARTICLE_MAX  100
#define NPC_COUNT     5
#define ITEM_COUNT    8

typedef struct {
    float x, y;
    float vx, vy;
    int life;
    unsigned char r, g, b;
} Particle;

typedef struct {
    int x, y;
    int type;
    int dir;
    int frame;
    int move_timer;
    char name[20];
    char dialog[50];
} NPC;

typedef struct {
    int x, y;
    int type;
    int active;
    int frame;
} Item;

typedef struct {
    int map[18][25];
    int px, py, dir, frame, moving;
    int ku, kd, kl, kr;
    int health, max_health;
    int coins;
    int anim_timer;
    int day_timer;
    Particle particles[PARTICLE_MAX];
    NPC npcs[NPC_COUNT];
    Item items[ITEM_COUNT];
    int item_count;
    int show_minimap;
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

static void add_particle(RPGData* d, int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    for (int i=0; i<PARTICLE_MAX; i++) {
        if (d->particles[i].life <= 0) {
            d->particles[i].x = x;
            d->particles[i].y = y;
            d->particles[i].vx = (rand() % 100 - 50) / 30.0f;
            d->particles[i].vy = (rand() % 100 - 50) / 30.0f - 1.5f;
            d->particles[i].life = 20 + rand() % 15;
            d->particles[i].r = r;
            d->particles[i].g = g;
            d->particles[i].b = b;
            break;
        }
    }
}

// Enhanced tile rendering with animations
static void rpg_tile(Framebuffer* fb, int x, int y, int t, int anim) {
    unsigned char r,g,b;
    
    switch(t) {
        case 0: // Grass
            r = 0x33 + ((fast_sine(anim + x/2) * 10) >> 7); 
            g = 0xAA + ((fast_sine(anim + y/2) * 10) >> 7); 
            b = 0x33;
            fb_fill(fb, x, y, TILE_SIZE, TILE_SIZE, r, g, b);
            break;
            
        case 1: // Stone wall
            fb_fill(fb, x, y, TILE_SIZE, TILE_SIZE, 0x55, 0x55, 0x55);
            for (int i=0; i<TILE_SIZE; i+=8)
                for (int j=0; j<TILE_SIZE; j+=8)
                    if ((i+j)%16==0)
                        fb_fill(fb, x+i, y+j, 4, 4, 0x66, 0x66, 0x66);
            break;
            
        case 2: // Water with animation
            fb_fill(fb, x, y, TILE_SIZE, TILE_SIZE, 0x33, 0x55, 0xAA);
            for (int i=0; i<3; i++) {
                int wx = x + ((fast_sine(anim*3 + i*85) * 8) >> 7) + 8;
                int wy = y + ((fast_cosine(anim*3 + i*85) * 8) >> 7) + 16;
                fb_pixel(fb, wx, wy, 0xAA, 0xCC, 0xFF);
            }
            break;
            
        case 3: // Sand/Path
            fb_fill(fb, x, y, TILE_SIZE, TILE_SIZE, 0xCC, 0xAA, 0x66);
            break;
            
        case 4: // Dark grass with tree
            fb_fill(fb, x, y, TILE_SIZE, TILE_SIZE, 0x22, 0x66, 0x22);
            fb_circle(fb, x+16, y, TILE_SIZE/2+2, 0x00, 0xAA, 0x00);
            fb_fill(fb, x+13, y+16, 6, 16, 0x66, 0x44, 0x22);
            fb_circle(fb, x+12, y-2, 6, 0x44, 0xCC, 0x44);
            break;
            
        case 5: // Dirt with house
            fb_fill(fb, x, y, TILE_SIZE, TILE_SIZE, 0x88, 0x44, 0x22);
            fb_fill(fb, x+4, y+8, 24, 24, 0xDD, 0xBB, 0x88);
            fb_rect(fb, x+4, y+8, 24, 24, 0x88, 0x66, 0x44);
            fb_fill(fb, x+12, y+20, 8, 12, 0x66, 0x44, 0x22);
            fb_pixel(fb, x+17, y+26, 0xFF, 0xDD, 0x00);
            fb_fill(fb, x+8, y+12, 6, 6, 0x88, 0xCC, 0xFF);
            fb_fill(fb, x+18, y+12, 6, 6, 0x88, 0xCC, 0xFF);
            break;
            
        default:
            fb_fill(fb, x, y, TILE_SIZE, TILE_SIZE, 0, 0, 0);
    }
}

// Enhanced player rendering
static void rpg_player(Framebuffer* fb, int x, int y, int dir, int frame, int moving) {
    // Shadow
    fb_circle(fb, x+16, y+30, 10, 0x00, 0x00, 0x00);
    
    // Body
    fb_fill(fb, x+8, y+12, 16, 14, 0x33, 0x66, 0xFF);
    fb_rect(fb, x+8, y+12, 16, 14, 0x22, 0x55, 0xAA);
    
    // Armor detail
    fb_fill(fb, x+12, y+18, 8, 2, 0xFF, 0xDD, 0x44);
    
    // Head
    fb_circle(fb, x+16, y+8, 8, 0xFF, 0xCC, 0x99);
    fb_circle_outline(fb, x+16, y+8, 8, 0xCC, 0x99, 0x66);
    
    // Hair
    fb_fill(fb, x+10, y+1, 12, 5, 0x66, 0x33, 0x11);
    
    // Eyes
    if (dir == 1) {
        fb_fill(fb, x+9, y+7, 3, 3, 0, 0, 0);
    } else if (dir == 2) {
        fb_fill(fb, x+20, y+7, 3, 3, 0, 0, 0);
    } else {
        fb_fill(fb, x+11, y+7, 3, 3, 0, 0, 0);
        fb_fill(fb, x+19, y+7, 3, 3, 0, 0, 0);
    }
    
    // Legs with walking animation
    if (moving) {
        int leg_offset = (fast_sine(frame * 40) * 4) >> 7;
        fb_fill(fb, x+9 + leg_offset, y+26, 5, 8, 0x22, 0x22, 0x66);
        fb_fill(fb, x+18 - leg_offset, y+26, 5, 8, 0x22, 0x22, 0x66);
    } else {
        fb_fill(fb, x+9, y+26, 5, 8, 0x22, 0x22, 0x66);
        fb_fill(fb, x+18, y+26, 5, 8, 0x22, 0x22, 0x66);
    }
}

// Render NPCs
static void rpg_npc(Framebuffer* fb, NPC* npc, int anim) {
    int x = npc->x, y = npc->y;
    
    // Shadow
    fb_circle(fb, x+8, y+30, 8, 0x00, 0x00, 0x00);
    
    switch(npc->type) {
        case 0: // Villager
            fb_fill(fb, x+4, y+16, 8, 12, 0xAA, 0x88, 0x66);
            fb_circle(fb, x+8, y+12, 6, 0xFF, 0xCC, 0x99);
            break;
        case 1: // Merchant
            fb_fill(fb, x+3, y+14, 10, 14, 0x88, 0x44, 0x88);
            fb_circle(fb, x+8, y+10, 7, 0xFF, 0xDD, 0xAA);
            fb_fill(fb, x+3, y+5, 10, 4, 0x66, 0x22, 0x66);
            break;
        case 2: // Guard
            fb_fill(fb, x+4, y+14, 8, 14, 0x88, 0x88, 0x88);
            fb_circle(fb, x+8, y+10, 7, 0xFF, 0xCC, 0x99);
            fb_fill(fb, x+4, y+6, 8, 5, 0xCC, 0xCC, 0xCC);
            fb_fill(fb, x+12, y+6, 2, 20, 0x88, 0x66, 0x44);
            fb_fill(fb, x+11, y+4, 4, 4, 0xCC, 0xCC, 0xCC);
            break;
    }
}

// Render minimap
static void rpg_minimap(Framebuffer* fb, RPGData* d) {
    int mm_x = GAME_AREA_X + GAME_AREA_WIDTH - 120;
    int mm_y = GAME_AREA_Y + 10;
    int mm_size = 100;
    int cell_w = mm_size / MAP_WIDTH;
    int cell_h = mm_size / MAP_HEIGHT;
    
    fb_fill(fb, mm_x-2, mm_y-2, mm_size+4, mm_size+4, 0x00, 0x00, 0x00);
    fb_fill(fb, mm_x, mm_y, mm_size, mm_size, 0x22, 0x22, 0x22);
    
    for (int y=0; y<MAP_HEIGHT; y++)
        for (int x=0; x<MAP_WIDTH; x++)
            if (d->map[y][x] != 0) {
                unsigned char r=0x44, g=0x44, b=0x44;
                switch(d->map[y][x]) {
                    case 1: r=0x66; g=0x66; b=0x66; break;
                    case 2: r=0x33; g=0x55; b=0xAA; break;
                    case 3: r=0xCC; g=0xAA; b=0x66; break;
                    case 4: r=0x22; g=0x66; b=0x22; break;
                    case 5: r=0x88; g=0x44; b=0x22; break;
                }
                fb_fill(fb, mm_x+x*cell_w, mm_y+y*cell_h, cell_w, cell_h, r, g, b);
            }
    
    int px = mm_x + d->px * mm_size / (MAP_WIDTH * TILE_SIZE);
    int py = mm_y + d->py * mm_size / (MAP_HEIGHT * TILE_SIZE);
    fb_circle(fb, px, py, 3, 0xFF, 0x00, 0x00);
    fb_circle_outline(fb, px, py, 4, 0xFF, 0xFF, 0x00);
}

// =============================================================================
// RPG GAME FUNCTIONS
// =============================================================================

static void rpg_init(Game* game) {
    RPGData* d = calloc(1, sizeof(RPGData));
    memcpy(d->map, rpg_map, sizeof(rpg_map));
    d->px = 5 * TILE_SIZE;
    d->py = 5 * TILE_SIZE;
    d->health = 100;
    d->max_health = 100;
    d->coins = 0;
    d->show_minimap = 1;
    
    // Initialize NPCs
    d->npcs[0] = (NPC){10*TILE_SIZE+16, 8*TILE_SIZE+16, 0, 1, 0, 0, "Elder", "Welcome, hero!"};
    d->npcs[1] = (NPC){15*TILE_SIZE+16, 12*TILE_SIZE+16, 1, 2, 0, 0, "Merchant", "Buy my wares!"};
    d->npcs[2] = (NPC){20*TILE_SIZE+16, 6*TILE_SIZE+16, 2, 0, 0, 0, "Guard", "Halt! State your business."};
    d->npcs[3] = (NPC){8*TILE_SIZE+16, 14*TILE_SIZE+16, 0, 3, 0, 0, "Farmer", "Nice weather today."};
    d->npcs[4] = (NPC){12*TILE_SIZE+16, 4*TILE_SIZE+16, 1, 1, 0, 0, "Traveler", "Beware of the forest."};
    
    // Initialize items
    d->items[0] = (Item){8*TILE_SIZE+16, 5*TILE_SIZE+16, 0, 1, 0};
    d->items[1] = (Item){18*TILE_SIZE+16, 10*TILE_SIZE+16, 1, 1, 0};
    d->items[2] = (Item){14*TILE_SIZE+16, 15*TILE_SIZE+16, 2, 1, 0};
    d->items[3] = (Item){6*TILE_SIZE+16, 12*TILE_SIZE+16, 1, 1, 0};
    d->items[4] = (Item){22*TILE_SIZE+16, 3*TILE_SIZE+16, 0, 1, 0};
    d->items[5] = (Item){11*TILE_SIZE+16, 9*TILE_SIZE+16, 1, 1, 0};
    d->items[6] = (Item){19*TILE_SIZE+16, 14*TILE_SIZE+16, 0, 1, 0};
    d->items[7] = (Item){16*TILE_SIZE+16, 7*TILE_SIZE+16, 2, 1, 0};
    d->item_count = 8;
    
    game->data = d;
    printf("RPG Adventure Enhanced initialized\n");
}

static void rpg_handle_key(Game* game, int key_code, int pressed) {
    RPGData* d = game->data;
    switch (key_code) {
        case 38: case 87: d->ku = pressed; if (pressed) d->dir = 3; break;
        case 40: case 83: d->kd = pressed; if (pressed) d->dir = 0; break;
        case 37: case 65: d->kl = pressed; if (pressed) d->dir = 1; break;
        case 39: case 68: d->kr = pressed; if (pressed) d->dir = 2; break;
        case 77: if (pressed) d->show_minimap = !d->show_minimap; break;
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
            for (int i=0; i<10; i++)
                add_particle(d, x, y, 0xFF, 0xFF, 0x00);
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
        if (d->frame % 5 == 0)
            add_particle(d, GAME_AREA_X + d->px + 16, GAME_AREA_Y + d->py + 30, 
                        0xAA, 0xAA, 0xAA);
    }
    
    if (d->px < 0) d->px = 0;
    if (d->py < 0) d->py = 0;
    if (d->px > (MAP_WIDTH-1)*TILE_SIZE) d->px = (MAP_WIDTH-1)*TILE_SIZE;
    if (d->py > (MAP_HEIGHT-1)*TILE_SIZE) d->py = (MAP_HEIGHT-1)*TILE_SIZE;
    
    // Update animations
    d->anim_timer = (d->anim_timer + 1) & 255;
    d->day_timer = (d->day_timer + 1) % 256;
    
    if (d->moving) d->frame++;
    else d->frame = 0;
    
    // Update particles
    for (int i=0; i<PARTICLE_MAX; i++) {
        if (d->particles[i].life > 0) {
            d->particles[i].x += d->particles[i].vx;
            d->particles[i].y += d->particles[i].vy;
            d->particles[i].life--;
        }
    }
    
    // Update NPCs
    for (int i=0; i<NPC_COUNT; i++) {
        d->npcs[i].frame++;
        d->npcs[i].move_timer++;
        if (d->npcs[i].move_timer > 60) {
            d->npcs[i].move_timer = 0;
            d->npcs[i].dir = rand() % 4;
        }
    }
    
    // Check item pickup
    for (int i=0; i<d->item_count; i++) {
        if (d->items[i].active) {
            int dx = abs(d->px - d->items[i].x);
            int dy = abs(d->py - d->items[i].y);
            if (dx < TILE_SIZE && dy < TILE_SIZE) {
                d->items[i].active = 0;
                if (d->items[i].type == 0) d->health = d->max_health;
                else if (d->items[i].type == 1) d->coins += 10;
                else if (d->items[i].type == 2) d->coins += 50;
                
                for (int j=0; j<15; j++)
                    add_particle(d, GAME_AREA_X + d->items[i].x, GAME_AREA_Y + d->items[i].y,
                               d->items[i].type == 0 ? 0xFF : 0xFF,
                               d->items[i].type == 2 ? 0xFF : 0xDD,
                               0x00);
            }
        }
    }
}

static void rpg_render(Game* game, Framebuffer* fb) {
    RPGData* d = game->data;
    
    // Dynamic sky color based on time of day
    int day_val = d->day_timer;
    unsigned char sky_r = 0x44 + ((fast_sine(day_val) * 0x44) >> 7);
    unsigned char sky_g = 0x44 + ((fast_sine(day_val) * 0x44) >> 7);
    unsigned char sky_b = 0x88 + ((fast_sine(day_val) * 0x44) >> 7);
    
    fb_fill(fb, GAME_AREA_X, GAME_AREA_Y, GAME_AREA_WIDTH, GAME_AREA_HEIGHT, sky_r, sky_g, sky_b);
    
    // Render map
    for (int y = 0; y < MAP_HEIGHT; y++)
        for (int x = 0; x < MAP_WIDTH; x++)
            rpg_tile(fb, GAME_AREA_X + x*TILE_SIZE, GAME_AREA_Y + y*TILE_SIZE, 
                    d->map[y][x], d->anim_timer);
    
    // Render items
    for (int i=0; i<d->item_count; i++) {
        if (d->items[i].active) {
            int ix = GAME_AREA_X + d->items[i].x;
            int iy = GAME_AREA_Y + d->items[i].y;
            int bob = (fast_sine(d->anim_timer * 3 + i * 50) * 3) >> 7;
            
            switch(d->items[i].type) {
                case 0: // Heart
                    fb_circle(fb, ix-4, iy-4 + bob, 4, 0xFF, 0x33, 0x33);
                    fb_circle(fb, ix+4, iy-4 + bob, 4, 0xFF, 0x33, 0x33);
                    fb_fill(fb, ix-6, iy-2 + bob, 12, 8, 0xFF, 0x33, 0x33);
                    break;
                case 1: // Coin
                    fb_circle(fb, ix, iy + bob, 5, 0xFF, 0xDD, 0x00);
                    fb_circle_outline(fb, ix, iy + bob, 5, 0xCC, 0xAA, 0x00);
                    fb_text(fb, ix-2, iy-2 + bob, "$", 0xAA, 0x88, 0x00);
                    break;
                case 2: // Gem
                    fb_fill(fb, ix-3, iy-6 + bob, 6, 10, 0x88, 0x00, 0xFF);
                    fb_fill(fb, ix-5, iy-2 + bob, 10, 6, 0xAA, 0x44, 0xFF);
                    fb_pixel(fb, ix, iy-4 + bob, 0xFF, 0xFF, 0xFF);
                    break;
            }
        }
    }
    
    // Render NPCs
    for (int i=0; i<NPC_COUNT; i++)
        rpg_npc(fb, &d->npcs[i], d->anim_timer);
    
    // Render particles
    for (int i=0; i<PARTICLE_MAX; i++)
        if (d->particles[i].life > 0) {
            int alpha = d->particles[i].life * 8;
            if (alpha > 255) alpha = 255;
            int px = (int)d->particles[i].x;
            int py = (int)d->particles[i].y;
            if (px >= 0 && px < MAIN_WINDOW_WIDTH && py >= 0 && py < MAIN_WINDOW_HEIGHT) {
                int idx = (py * MAIN_WINDOW_WIDTH + px) * 4;
                fb->pixels[idx] = (d->particles[i].r * alpha + fb->pixels[idx] * (255 - alpha)) >> 8;
                fb->pixels[idx+1] = (d->particles[i].g * alpha + fb->pixels[idx+1] * (255 - alpha)) >> 8;
                fb->pixels[idx+2] = (d->particles[i].b * alpha + fb->pixels[idx+2] * (255 - alpha)) >> 8;
            }
        }
    
    // Render player
    rpg_player(fb, GAME_AREA_X + d->px, GAME_AREA_Y + d->py, d->dir, d->frame, d->moving);
    
    // Render minimap
    if (d->show_minimap)
        rpg_minimap(fb, d);
    
    // HUD - Health bar
    fb_fill(fb, GAME_AREA_X+10, GAME_AREA_Y+10, 204, 24, 0x00, 0x00, 0x00);
    fb_fill(fb, GAME_AREA_X+12, GAME_AREA_Y+12, 200, 20, 0x44, 0x00, 0x00);
    int health_width = 200 * d->health / d->max_health;
    fb_fill(fb, GAME_AREA_X+12, GAME_AREA_Y+12, health_width, 20, 0xFF, 0x22, 0x22);
    fb_rect(fb, GAME_AREA_X+12, GAME_AREA_Y+12, 200, 20, 0xFF, 0xFF, 0xFF);
    
    char hp_text[20];
    snprintf(hp_text, sizeof(hp_text), "HP: %d/%d", d->health, d->max_health);
    fb_text_shadow(fb, GAME_AREA_X+20, GAME_AREA_Y+15, hp_text, 0xFF, 0xFF, 0xFF);
    
    // Coin display
    char coin_text[20];
    snprintf(coin_text, sizeof(coin_text), "Gold: %d", d->coins);
    fb_text_shadow(fb, GAME_AREA_X+220, GAME_AREA_Y+15, coin_text, 0xFF, 0xDD, 0x00);
    
    // Status text
    char s[200];
    snprintf(s, sizeof(s), "RPG Adventure - Pos:(%d,%d) - ArrowKeys/WASD | M=Map", 
             d->px/TILE_SIZE, d->py/TILE_SIZE);
    fb_text_shadow(fb, GAME_AREA_X+10, GAME_AREA_Y+GAME_AREA_HEIGHT-10, s, 0xFF, 0xFF, 0xFF);
    
    // Time indicator
    char time_text[10];
    if (d->day_timer < 64) snprintf(time_text, sizeof(time_text), "Dawn");
    else if (d->day_timer < 128) snprintf(time_text, sizeof(time_text), "Day");
    else if (d->day_timer < 192) snprintf(time_text, sizeof(time_text), "Dusk");
    else snprintf(time_text, sizeof(time_text), "Night");
    fb_text_shadow(fb, GAME_AREA_X+GAME_AREA_WIDTH-80, GAME_AREA_Y+15, time_text, 0xFF, 0xFF, 0xFF);
    
    // NPC dialog when nearby
    for (int i=0; i<NPC_COUNT; i++) {
        int dx = abs(d->px - d->npcs[i].x);
        int dy = abs(d->py - d->npcs[i].y);
        if (dx < TILE_SIZE*2 && dy < TILE_SIZE*2) {
            int nx = GAME_AREA_X + d->npcs[i].x;
            int ny = GAME_AREA_Y + d->npcs[i].y - 20;
            fb_fill(fb, nx-50, ny-15, 100, 25, 0x00, 0x00, 0x00);
            fb_rect(fb, nx-50, ny-15, 100, 25, 0xFF, 0xFF, 0xFF);
            fb_text(fb, nx-48, ny-12, d->npcs[i].name, 0xFF, 0xDD, 0x00);
            fb_text(fb, nx-48, ny-4, d->npcs[i].dialog, 0xCC, 0xCC, 0xCC);
        }
    }
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
    
    strcpy(game->name, "RPG Adventure Enhanced");
    game->active = 1;
    game->init = rpg_init;
    game->handle_key = rpg_handle_key;
    game->handle_click = rpg_handle_click;
    game->update = rpg_update;
    game->render = rpg_render;
    game->cleanup = rpg_cleanup;
    
    return game;
}