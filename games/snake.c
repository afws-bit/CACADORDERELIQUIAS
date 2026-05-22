// =============================================================================
// LOGIC SNAKE - Snake com Raciocínio Lógico para SPANE Engine
// =============================================================================
// Compile: gcc -shared -fPIC -O3 -o logic_snake.so logic_snake.c
// =============================================================================

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

// SDK types
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

// =============================================================================
// DRAWING PRIMITIVES
// =============================================================================

static inline int in_bounds(int x, int y) {
    return x >= 0 && x < GAME_AREA_WIDTH && y >= 0 && y < GAME_AREA_HEIGHT;
}

static inline void fb_pixel(Framebuffer* fb, int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    if (!in_bounds(x, y)) return;
    int ax = GAME_AREA_X + x;
    int ay = GAME_AREA_Y + y;
    int i = (ay * MAIN_WINDOW_WIDTH + ax) * 4;
    fb->pixels[i] = r;
    fb->pixels[i+1] = g;
    fb->pixels[i+2] = b;
    fb->pixels[i+3] = 255;
}

static void fb_fill(Framebuffer* fb, int x, int y, int w, int h, unsigned char r, unsigned char g, unsigned char b) {
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > GAME_AREA_WIDTH) w = GAME_AREA_WIDTH - x;
    if (y + h > GAME_AREA_HEIGHT) h = GAME_AREA_HEIGHT - y;
    if (w <= 0 || h <= 0) return;
    for (int dy = 0; dy < h; dy++)
        for (int dx = 0; dx < w; dx++)
            fb_pixel(fb, x + dx, y + dy, r, g, b);
}

static void fb_rect(Framebuffer* fb, int x, int y, int w, int h, unsigned char r, unsigned char g, unsigned char b) {
    if (w <= 0 || h <= 0) return;
    for (int dx = 0; dx < w; dx++) {
        fb_pixel(fb, x + dx, y, r, g, b);
        if (h > 1) fb_pixel(fb, x + dx, y + h - 1, r, g, b);
    }
    for (int dy = 1; dy < h - 1; dy++) {
        fb_pixel(fb, x, y + dy, r, g, b);
        fb_pixel(fb, x + w - 1, y + dy, r, g, b);
    }
}

static void fb_char(Framebuffer* fb, int x, int y, char c, unsigned char r, unsigned char g, unsigned char b) {
    if (c < 32 || c > 126) return;
    const unsigned char* glyph = font_5x7[c - 32];
    for (int row = 0; row < 7; row++)
        for (int col = 0; col < 5; col++)
            if (glyph[col] & (1 << row))
                fb_pixel(fb, x + col, y + row, r, g, b);
}

static void fb_text(Framebuffer* fb, int x, int y, const char* s, unsigned char r, unsigned char g, unsigned char b) {
    if (!s) return;
    for (int i = 0; s[i] && i < 200; i++)
        fb_char(fb, x + i * 6, y, s[i], r, g, b);
}

static void fb_text_center(Framebuffer* fb, int cx, int y, const char* s, unsigned char r, unsigned char g, unsigned char b) {
    if (!s) return;
    int len = (int)strlen(s);
    if (len > 200) len = 200;
    fb_text(fb, cx - (len * 6) / 2, y, s, r, g, b);
}

// =============================================================================
// GAME CONSTANTS
// =============================================================================

#define SG 24          // Grid size (bigger for better visibility)
#define SML 400        // Max snake length
#define SAW 672        // Snake area width (28 cols)
#define SAH 504        // Snake area height (21 rows)
#define SOX 15         // Offset X
#define SOY 60         // Offset Y
#define SNC (SAW/SG)   // 28 cols
#define SNR (SAH/SG)   // 21 rows

// Logic gate types
#define GATE_AND    0
#define GATE_OR     1
#define GATE_NOT    2
#define GATE_IMPLIES 3

typedef struct { int x, y; } Pt;

// Each food item represents a logical expression: input1 GATE input2 = result
typedef struct {
    int x, y;
    int gate;       // Which gate (AND, OR, NOT, IMPLIES)
    int input1;     // P value: 0=False, 1=True
    int input2;     // Q value: 0=False, 1=True, -1=unused (for NOT)
    int result;     // Expected result: 0=False, 1=True
    int correct;    // Is this the target food?
    int active;
} Food;

typedef struct {
    // Snake data
    Pt s[SML];
    int len, dir, ndir;
    int go, fc, spd;
    int score;
    int lives;
    int invincible;
    int invincible_timer;
    
    // Food
    Food foods[8];
    int num_foods;
    
    // Challenge
    int target_gate;
    int target_input1;
    int target_input2;
    int target_result;
    int challenge_active;
    int challenge_timer;
    int level;
    int foods_eaten;
    
    // Display
    int frame_count;
    int message_timer;
    char message[128];
} SnakeData;

// =============================================================================
// LOGIC EVALUATION
// =============================================================================

static int evaluate_gate(int gate, int a, int b) {
    switch (gate) {
        case GATE_AND:     return a && b;        // P E Q
        case GATE_OR:      return a || b;        // P OU Q
        case GATE_NOT:     return !a;            // NAO P
        case GATE_IMPLIES: return (!a) || b;     // SE P ENTAO Q
        default: return 0;
    }
}

static const char* gate_name(int gate) {
    switch (gate) {
        case GATE_AND:     return "E (AND)";
        case GATE_OR:      return "OU (OR)";
        case GATE_NOT:     return "NAO (NOT)";
        case GATE_IMPLIES: return "SE...ENTAO";
        default: return "";
    }
}

static const char* gate_symbol(int gate) {
    switch (gate) {
        case GATE_AND:     return "E";
        case GATE_OR:      return "OU";
        case GATE_NOT:     return "NAO";
        case GATE_IMPLIES: return "->";
        default: return "?";
    }
}

static int gate_color(int gate) {
    switch (gate) {
        case GATE_AND:     return 0;  // Verde
        case GATE_OR:      return 1;  // Azul
        case GATE_NOT:     return 2;  // Vermelho
        case GATE_IMPLIES: return 3;  // Amarelo
        default: return 0;
    }
}

// =============================================================================
// FOOD SPAWN
// =============================================================================

static void spawn_food(SnakeData* d, int index) {
    if (index < 0 || index >= 8) return;
    Food* f = &d->foods[index];
    
    int valid;
    do {
        valid = 1;
        f->x = rand() % SNC;
        f->y = rand() % SNR;
        
        // Don't spawn on snake
        for (int i = 0; i < d->len; i++) {
            if (d->s[i].x == f->x && d->s[i].y == f->y) {
                valid = 0;
                break;
            }
        }
        
        // Don't spawn on other food
        for (int j = 0; j < d->num_foods; j++) {
            if (j != index && d->foods[j].active &&
                d->foods[j].x == f->x && d->foods[j].y == f->y) {
                valid = 0;
                break;
            }
        }
    } while (!valid);
    
    // Set logic values
    f->gate = rand() % 4;
    f->input1 = rand() % 2;
    f->input2 = (f->gate == GATE_NOT) ? -1 : rand() % 2;
    f->result = evaluate_gate(f->gate, f->input1, f->input2);
    f->correct = 0;
    f->active = 1;
}

static void generate_challenge(SnakeData* d) {
    // Pick random target
    d->target_gate = rand() % 4;
    d->target_input1 = rand() % 2;
    d->target_input2 = (d->target_gate == GATE_NOT) ? -1 : rand() % 2;
    d->target_result = evaluate_gate(d->target_gate, d->target_input1, d->target_input2);
    d->challenge_active = 1;
    d->challenge_timer = 1200; // 20 seconds
    d->foods_eaten = 0;
    
    // Spawn foods - one correct, rest random
    d->num_foods = 4 + d->level;
    if (d->num_foods > 8) d->num_foods = 8;
    
    for (int i = 0; i < d->num_foods; i++) {
        d->foods[i].active = 0;
        spawn_food(d, i);
    }
    
    // Make the first food the correct one
    int correct_idx = rand() % d->num_foods;
    d->foods[correct_idx].gate = d->target_gate;
    d->foods[correct_idx].input1 = d->target_input1;
    d->foods[correct_idx].input2 = d->target_input2;
    d->foods[correct_idx].result = d->target_result;
    d->foods[correct_idx].correct = 1;
    
    // Format message
    const char* vp = d->target_input1 ? "V" : "F";
    const char* vq = d->target_input2 == 1 ? "V" : d->target_input2 == 0 ? "F" : "";
    const char* vr = d->target_result ? "V" : "F";
    const char* sym = gate_symbol(d->target_gate);
    
    if (d->target_gate == GATE_NOT) {
        snprintf(d->message, sizeof(d->message), 
                 "Encontre: NAO %s = %s  (%s)", vp, vr, gate_name(d->target_gate));
    } else {
        snprintf(d->message, sizeof(d->message), 
                 "Encontre: %s %s %s = %s  (%s)", vp, sym, vq, vr, gate_name(d->target_gate));
    }
    d->message_timer = 180;
}

// =============================================================================
// GAME INIT
// =============================================================================

static void sn_init(Game* game) {
    SnakeData* d = (SnakeData*)calloc(1, sizeof(SnakeData));
    if (!d) return;
    
    srand((unsigned int)time(NULL));
    
    d->len = 4;
    int sx = SNC / 2, sy = SNR / 2;
    for (int i = 0; i < d->len; i++) {
        d->s[i].x = sx - i;
        d->s[i].y = sy;
    }
    d->dir = 0;
    d->ndir = 0;
    d->spd = 8;
    d->score = 0;
    d->lives = 5;
    d->go = 0;
    d->level = 1;
    d->invincible = 0;
    d->invincible_timer = 0;
    
    d->num_foods = 0;
    for (int i = 0; i < 8; i++) d->foods[i].active = 0;
    
    generate_challenge(d);
    
    game->data = d;
    game->active = 1;
}

static void sn_handle_key(Game* game, int key_code, int pressed) {
    if (!game || !game->data || !pressed) return;
    SnakeData* d = (SnakeData*)game->data;
    
    if (key_code == 82) { // R to restart anytime
        free(game->data);
        sn_init(game);
        return;
    }
    
    if (d->go) return;
    
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

// =============================================================================
// UPDATE
// =============================================================================

static void sn_update(Game* game) {
    if (!game || !game->data) return;
    SnakeData* d = (SnakeData*)game->data;
    
    if (d->go) return;
    
    d->frame_count++;
    
    if (d->message_timer > 0) d->message_timer--;
    if (d->invincible_timer > 0) {
        d->invincible_timer--;
        if (d->invincible_timer == 0) d->invincible = 0;
    }
    
    // Challenge timer
    if (d->challenge_active) {
        d->challenge_timer--;
        if (d->challenge_timer <= 0) {
            d->lives--;
            snprintf(d->message, sizeof(d->message), "TEMPO ESGOTADO! -1 vida");
            d->message_timer = 120;
            if (d->lives <= 0) {
                d->go = 1;
                return;
            }
            d->invincible = 1;
            d->invincible_timer = 60;
            generate_challenge(d);
            return;
        }
    }
    
    // Speed control
    if (++d->fc < d->spd) return;
    d->fc = 0;
    d->dir = d->ndir;
    
    // New head
    Pt nh = d->s[0];
    switch (d->dir) {
        case 0: nh.x++; break;
        case 1: nh.y++; break;
        case 2: nh.x--; break;
        case 3: nh.y--; break;
    }
    
    // Wall collision
    if (nh.x < 0 || nh.x >= SNC || nh.y < 0 || nh.y >= SNR) {
        if (!d->invincible) {
            d->lives--;
            snprintf(d->message, sizeof(d->message), "BATEU NA PAREDE! -1 vida");
            d->message_timer = 90;
            d->invincible = 1;
            d->invincible_timer = 60;
        }
        if (d->lives <= 0) { d->go = 1; return; }
        nh.x = SNC / 2; nh.y = SNR / 2;
        d->dir = 0; d->ndir = 0;
        return;
    }
    
    // Self collision
    for (int i = 0; i < d->len; i++) {
        if (d->s[i].x == nh.x && d->s[i].y == nh.y) {
            if (!d->invincible) {
                d->lives--;
                snprintf(d->message, sizeof(d->message), "BATEU EM SI MESMO! -1 vida");
                d->message_timer = 90;
                d->invincible = 1;
                d->invincible_timer = 60;
            }
            if (d->lives <= 0) { d->go = 1; return; }
            nh.x = SNC / 2; nh.y = SNR / 2;
            d->dir = 0; d->ndir = 0;
            return;
        }
    }
    
    // Check food collision
    int ate = -1;
    for (int i = 0; i < d->num_foods; i++) {
        if (d->foods[i].active && d->foods[i].x == nh.x && d->foods[i].y == nh.y) {
            ate = i;
            break;
        }
    }
    
    // Move snake
    for (int i = d->len - 1; i > 0; i--) d->s[i] = d->s[i-1];
    d->s[0] = nh;
    
    if (ate >= 0) {
        Food* f = &d->foods[ate];
        
        if (f->correct) {
            // CORRECT! Ate a comida certa
            d->challenge_active = 0;
            d->len += 2;
            d->score += 50 + d->level * 10;
            d->level++;
            if (d->spd > 3) d->spd--;
            
            const char* vp = f->input1 ? "V" : "F";
            const char* vq = f->input2 == 1 ? "V" : f->input2 == 0 ? "F" : "";
            const char* vr = f->result ? "V" : "F";
            
            if (f->gate == GATE_NOT) {
                snprintf(d->message, sizeof(d->message), 
                         "CORRETO! NAO %s = %s! Nivel %d!", vp, vr, d->level);
            } else {
                snprintf(d->message, sizeof(d->message), 
                         "CORRETO! %s %s %s = %s! Nivel %d!", 
                         vp, gate_symbol(f->gate), vq, vr, d->level);
            }
            d->message_timer = 150;
            
            for (int i = 0; i < d->num_foods; i++) d->foods[i].active = 0;
            generate_challenge(d);
        } else {
            // WRONG! Comeu comida errada
            d->lives--;
            d->len++;
            d->score += 5;
            
            const char* vp = f->input1 ? "V" : "F";
            const char* vq = f->input2 == 1 ? "V" : f->input2 == 0 ? "F" : "";
            const char* vr = f->result ? "V" : "F";
            
            if (f->gate == GATE_NOT) {
                snprintf(d->message, sizeof(d->message), 
                         "ERRADO! NAO %s = %s nao e o alvo! -1 vida", vp, vr);
            } else {
                snprintf(d->message, sizeof(d->message), 
                         "ERRADO! %s %s %s = %s nao e o alvo! -1 vida", 
                         vp, gate_symbol(f->gate), vq, vr);
            }
            d->message_timer = 120;
            d->invincible = 1;
            d->invincible_timer = 60;
            f->active = 0;
            
            if (d->lives <= 0) { d->go = 1; return; }
            
            // Spawn replacement food
            spawn_food(d, ate);
        }
    }
}

// =============================================================================
// RENDER
// =============================================================================

static void render_food(Framebuffer* fb, Food* f) {
    int fx = SOX + f->x * SG;
    int fy = SOY + f->y * SG;
    int cx = fx + SG/2;
    int cy = fy + SG/2;
    int r = SG/2 - 3;
    
    // Color by gate type
    unsigned char cr, cg, cb, tr, tg, tb;
    switch (f->gate) {
        case GATE_AND:
            cr = 0x00; cg = 0xBB; cb = 0x00;  // Verde
            tr = 0x00; tg = 0x44; tb = 0x00;
            break;
        case GATE_OR:
            cr = 0x33; cg = 0x99; cb = 0xFF;  // Azul
            tr = 0x11; tg = 0x33; tb = 0x66;
            break;
        case GATE_NOT:
            cr = 0xEE; cg = 0x44; cb = 0x44;  // Vermelho
            tr = 0x66; tg = 0x11; tb = 0x11;
            break;
        case GATE_IMPLIES:
            cr = 0xFF; cg = 0xCC; cb = 0x00;  // Amarelo
            tr = 0x66; tg = 0x44; tb = 0x00;
            break;
        default:
            cr = 0xAA; cg = 0xAA; cb = 0xAA;
            tr = 0x33; tg = 0x33; tb = 0x33;
    }
    
    // Draw circle
    for (int dy = -r; dy <= r; dy++)
        for (int dx = -r; dx <= r; dx++)
            if (dx*dx + dy*dy <= r*r)
                fb_pixel(fb, cx + dx, cy + dy, cr, cg, cb);
    
    // Border
    for (int dy = -r; dy <= r; dy++)
        for (int dx = -r; dx <= r; dx++)
            if (dx*dx + dy*dy >= (r-2)*(r-2) && dx*dx + dy*dy <= r*r)
                fb_pixel(fb, cx + dx, cy + dy, tr, tg, tb);
    
    // Show gate symbol in center (BIGGER, CLEARER)
    const char* sym = gate_symbol(f->gate);
    int slen = (int)strlen(sym) * 6;
    
    // Black background for text
    fb_fill(fb, cx - slen/2 - 2, cy - 5, slen + 4, 10, 0x00, 0x00, 0x00);
    fb_text(fb, cx - slen/2, cy - 4, sym, 0xFF, 0xFF, 0xFF);
    
    // Show result at bottom (V or F)
    const char* res = f->result ? "V" : "F";
    fb_fill(fb, cx - 5, cy + r - 10, 10, 10, 0x00, 0x00, 0x00);
    fb_text(fb, cx - 3, cy + r - 8, res, f->result ? 0x00 : 0xFF, f->result ? 0xFF : 0x00, 0x00);
    
    // If this is the target, draw golden border
    if (f->correct) {
        fb_rect(fb, fx - 1, fy - 1, SG + 2, SG + 2, 0xFF, 0xDD, 0x00);
        fb_rect(fb, fx - 3, fy - 3, SG + 6, SG + 6, 0xFF, 0xAA, 0x00);
    }
    
    // Input indicators (P, Q) in small circles at top
    // P value (left)
    const char* pv = f->input1 ? "V" : "F";
    fb_fill(fb, fx + 2, fy - 2, 10, 10, f->input1 ? 0x00 : 0x33, f->input1 ? 0x88 : 0x00, 0x00);
    fb_text(fb, fx + 3, fy + 1, pv, 0xFF, 0xFF, 0xFF);
    
    // Q value (right, only if not NOT)
    if (f->gate != GATE_NOT) {
        const char* qv = f->input2 ? "V" : "F";
        fb_fill(fb, fx + SG - 12, fy - 2, 10, 10, f->input2 ? 0x00 : 0x33, f->input2 ? 0x88 : 0x00, 0x00);
        fb_text(fb, fx + SG - 11, fy + 1, qv, 0xFF, 0xFF, 0xFF);
    }
}

static void sn_render(Game* game, Framebuffer* fb) {
    if (!game || !game->data || !fb) return;
    SnakeData* d = (SnakeData*)game->data;
    
    // Background
    fb_fill(fb, 0, 0, GAME_AREA_WIDTH, GAME_AREA_HEIGHT, 0x0A, 0x0A, 0x18);
    
    // ===== GAME AREA =====
    fb_fill(fb, SOX, SOY, SAW, SAH, 0x11, 0x11, 0x22);
    fb_rect(fb, SOX - 1, SOY - 1, SAW + 2, SAH + 2, 0x44, 0x44, 0x66);
    
    // Grid (subtle)
    for (int i = 0; i <= SNC; i++)
        fb_fill(fb, SOX + i * SG, SOY, 1, SAH, 0x18, 0x18, 0x28);
    for (int i = 0; i <= SNR; i++)
        fb_fill(fb, SOX, SOY + i * SG, SAW, 1, 0x18, 0x18, 0x28);
    
    // Draw foods
    for (int i = 0; i < d->num_foods; i++)
        if (d->foods[i].active)
            render_food(fb, &d->foods[i]);
    
    // Draw snake
    for (int i = 0; i < d->len; i++) {
        int sx = SOX + d->s[i].x * SG + 2;
        int sy = SOY + d->s[i].y * SG + 2;
        int ss = SG - 4;
        
        if (d->invincible && (d->frame_count % 6 < 3)) {
            fb_fill(fb, sx, sy, ss, ss, 0x88, 0x88, 0x88);
        } else {
            unsigned char sr = (i == 0) ? 0x00 : 0x11;
            unsigned char sg = (i == 0) ? 0xEE : 0x99;
            unsigned char sb = 0x22;
            fb_fill(fb, sx, sy, ss, ss, sr, sg, sb);
        }
        
        // Eyes on head
        if (i == 0) {
            int ex1, ey1, ex2, ey2;
            switch (d->dir) {
                case 0: ex1=sx+ss-5; ey1=sy+3; ex2=sx+ss-5; ey2=sy+ss-6; break;
                case 1: ex1=sx+3; ey1=sy+ss-5; ex2=sx+ss-6; ey2=sy+ss-5; break;
                case 2: ex1=sx+2; ey1=sy+3; ex2=sx+2; ey2=sy+ss-6; break;
                case 3: ex1=sx+3; ey1=sy+2; ex2=sx+ss-6; ey2=sy+2; break;
                default: ex1=sx+ss-5; ey1=sy+3; ex2=sx+ss-5; ey2=sy+ss-6;
            }
            fb_fill(fb, ex1, ey1, 3, 3, 0xFF, 0xFF, 0xFF);
            fb_fill(fb, ex2, ey2, 3, 3, 0xFF, 0xFF, 0xFF);
        }
    }
    
    // ===== TOP BAR =====
    fb_fill(fb, 0, 0, GAME_AREA_WIDTH, SOY - 5, 0x0D, 0x0D, 0x1E);
    fb_text(fb, 10, 8, "LOGIC SNAKE", 0xFF, 0xCC, 0x00);
    
    char info[128];
    snprintf(info, sizeof(info), "Nivel: %d  |  Score: %d  |  Vidas: ", d->level, d->score);
    fb_text(fb, 160, 8, info, 0xFF, 0xFF, 0xFF);
    
    // Hearts for lives
    for (int i = 0; i < d->lives; i++)
        fb_char(fb, 160 + (int)strlen(info) * 6 + i * 8, 8, 3, 0xFF, 0x33, 0x33); // Heart symbol
    
    snprintf(info, sizeof(info), "Tamanho: %d  |  Vel: %d", d->len, 11 - d->spd);
    fb_text(fb, 500, 8, info, 0x88, 0xCC, 0x88);
    
    // Timer bar
    if (d->challenge_active) {
        int bar_w = 150;
        int bar_x = 630;
        int bar_fill = (d->challenge_timer * bar_w) / 1200;
        fb_fill(fb, bar_x, 6, bar_w, 10, 0x22, 0x22, 0x22);
        fb_fill(fb, bar_x, 6, bar_fill, 10, 
                d->challenge_timer < 300 ? 0xFF : 0x00,
                d->challenge_timer < 300 ? 0x44 : 0xCC,
                0x00);
        fb_rect(fb, bar_x, 6, bar_w, 10, 0x66, 0x66, 0x66);
        
        char ttext[16];
        snprintf(ttext, sizeof(ttext), "%ds", d->challenge_timer / 60);
        fb_text(fb, bar_x + bar_w + 5, 8, ttext, 0xFF, 0xFF, 0x00);
    }
    
    // ===== BOTTOM BAR - Challenge Info =====
    int by = SOY + SAH + 5;
    fb_fill(fb, 0, by, GAME_AREA_WIDTH, GAME_AREA_HEIGHT - by, 0x0D, 0x0D, 0x1E);
    
    // Target expression (BIG and CLEAR)
    const char* vp = d->target_input1 ? "V" : "F";
    const char* vq = d->target_input2 == 1 ? "V" : d->target_input2 == 0 ? "F" : "";
    const char* vr = d->target_result ? "V" : "F";
    
    fb_text(fb, 10, by + 5, "ALVO:", 0xFF, 0xCC, 0x00);
    
    char expr[64];
    if (d->target_gate == GATE_NOT) {
        snprintf(expr, sizeof(expr), "%s %s = %s", gate_symbol(d->target_gate), vp, vr);
    } else {
        snprintf(expr, sizeof(expr), "%s %s %s = %s", vp, gate_symbol(d->target_gate), vq, vr);
    }
    
    // Bigger expression display
    int expr_len = (int)strlen(expr);
    int expr_x = 90;
    
    // Expression background
    fb_fill(fb, expr_x - 5, by + 2, expr_len * 6 + 10, 18, 0x00, 0x00, 0x00);
    fb_rect(fb, expr_x - 5, by + 2, expr_len * 6 + 10, 18, 0xFF, 0xDD, 0x00);
    fb_text(fb, expr_x, by + 5, expr, 0xFF, 0xFF, 0x00);
    
    // Gate name
    fb_text(fb, expr_x + expr_len * 6 + 20, by + 5, gate_name(d->target_gate), 0x00, 0xFF, 0xFF);
    
    // Legend
    int lx = 450;
    fb_text(fb, lx, by + 5, "LEGENDA:", 0x88, 0x88, 0x88);
    
    const char* leg[] = {"E=Verde", "OU=Azul", "NAO=Verm", "->=Amar"};
    for (int i = 0; i < 4; i++)
        fb_text(fb, lx + 80 + i * 85, by + 5, leg[i], 0xAA, 0xAA, 0xAA);
    
    // Message
    if (d->message_timer > 0)
        fb_text(fb, 10, by + 25, d->message, 0xFF, 0xFF, 0x00);
    
    // Food labels explanation
    fb_text(fb, 10, by + 25 + (d->message_timer > 0 ? 0 : -20) + 20, 
            "Cada comida mostra: VALORES de entrada e RESULTADO. Coma a que corresponde ao ALVO!", 
            0x66, 0x66, 0x66);
    
    // ===== GAME OVER =====
    if (d->go) {
        fb_fill(fb, SOX + 100, SOY + 180, 470, 120, 0x00, 0x00, 0x00);
        fb_rect(fb, SOX + 100, SOY + 180, 470, 120, 0xFF, 0x00, 0x00);
        fb_text_center(fb, SOX + SAW/2, SOY + 205, "GAME OVER", 0xFF, 0x33, 0x33);
        char final[64];
        snprintf(final, sizeof(final), "Score: %d | Nivel: %d | Pressione R", d->score, d->level);
        fb_text_center(fb, SOX + SAW/2, SOY + 240, final, 0xFF, 0xFF, 0xFF);
        fb_text_center(fb, SOX + SAW/2, SOY + 265, "Setas/WASD para mover. Coma a comida certa!", 0xAA, 0xAA, 0xAA);
    }
}

static void sn_cleanup(Game* game) {
    if (game && game->data) {
        free(game->data);
        game->data = NULL;
    }
}

// =============================================================================
// EXPORT
// =============================================================================

__attribute__((visibility("default"))) Game* create_game() {
    Game* game = (Game*)calloc(1, sizeof(Game));
    if (!game) return NULL;
    
    strcpy(game->name, "Logic Snake");
    game->active = 0;
    game->init = sn_init;
    game->handle_key = sn_handle_key;
    game->handle_click = sn_handle_click;
    game->update = sn_update;
    game->render = sn_render;
    game->cleanup = sn_cleanup;
    
    return game;
}