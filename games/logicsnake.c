// =============================================================================
// LOGIC SNAKE - Snake com Questões de Lógica para SPANE Engine
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
// DRAWING PRIMITIVES (relative to game area 0,0 top-left)
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

#define SG 20          // Snake grid cell size
#define SML 400        // Max snake length
#define SAW 640        // Snake area width (32 cols)
#define SAH 480        // Snake area height (24 rows)
#define SOX 10         // Snake area offset X
#define SOY 50         // Snake area offset Y
#define SNC (SAW/SG)   // Number of columns (32)
#define SNR (SAH/SG)   // Number of rows (24)

// Question categories
#define QCAT_TABELA     0  // Tabela-verdade
#define QCAT_INFERENCIA 1  // Inferência lógica
#define QCAT_EQUIVALENCIA 2 // Equivalência lógica
#define QCAT_PROGRAMACAO 3 // Lógica de programação

#define MAX_QUESTIONS 30

typedef struct { int x, y; } Pt;

typedef struct {
    char question[256];
    char options[4][64];
    int correct;        // 0-3
    int category;
} Question;

typedef struct {
    // Snake
    Pt s[SML];
    int len, dir, ndir;
    int go, fc, spd;
    int score;
    int lives;
    
    // Food
    Pt food;
    
    // Question system
    Question questions[MAX_QUESTIONS];
    int num_questions;
    int question_active;    // 1 = showing question, snake paused
    int question_index;
    int selected_option;
    int answered;
    int correct_answer;
    int combo;              // Consecutive correct answers
    
    // Display
    int frame_count;
    int message_timer;
    char message[128];
    int screen;             // 0=playing, 1=game_over
} SnakeData;

// =============================================================================
// QUESTION BANK
// =============================================================================

static void add_question(SnakeData* d, const char* q, 
                         const char* o0, const char* o1, const char* o2, const char* o3,
                         int correct, int category) {
    if (d->num_questions >= MAX_QUESTIONS) return;
    Question* qp = &d->questions[d->num_questions];
    strncpy(qp->question, q, 255);
    strncpy(qp->options[0], o0, 63);
    strncpy(qp->options[1], o1, 63);
    strncpy(qp->options[2], o2, 63);
    strncpy(qp->options[3], o3, 63);
    qp->correct = correct;
    qp->category = category;
    d->num_questions++;
}

static void init_questions(SnakeData* d) {
    d->num_questions = 0;
    
    // Tabelas-verdade
    add_question(d, "Qual o resultado de V E F?",
                 "Verdadeiro", "Falso", "Depende", "Nenhuma", 1, QCAT_TABELA);
    add_question(d, "Qual o resultado de F OU V?",
                 "Falso", "Verdadeiro", "Depende", "Nenhuma", 1, QCAT_TABELA);
    add_question(d, "Qual o resultado de NAO V?",
                 "Verdadeiro", "Falso", "Depende", "Nenhuma", 1, QCAT_TABELA);
    add_question(d, "Qual o resultado de V -> F?",
                 "Verdadeiro", "Falso", "Depende", "Nenhuma", 1, QCAT_TABELA);
    add_question(d, "Qual o resultado de F -> V?",
                 "Verdadeiro", "Falso", "Depende", "Nenhuma", 0, QCAT_TABELA);
    add_question(d, "Qual o resultado de V OU V?",
                 "Falso", "Depende", "Verdadeiro", "Nenhuma", 2, QCAT_TABELA);
    add_question(d, "Qual o resultado de F E F?",
                 "Verdadeiro", "Depende", "Nenhuma", "Falso", 3, QCAT_TABELA);
    
    // Inferência lógica
    add_question(d, "Se P->Q e P=V, entao Q=?",
                 "Falso", "Verdadeiro", "Depende", "Nao sei", 1, QCAT_INFERENCIA);
    add_question(d, "Se P->Q e Q=F, entao P=?",
                 "Verdadeiro", "Depende", "Falso", "Nao sei", 2, QCAT_INFERENCIA);
    add_question(d, "Modus Ponens: Se P entao Q. P e verdade. Logo:",
                 "P e falso", "Q e verdade", "Nada conclui", "P e Q falsos", 1, QCAT_INFERENCIA);
    add_question(d, "Modus Tollens: Se P entao Q. Q e falso. Logo:",
                 "P e falso", "P e verdade", "Q e verdade", "Nada conclui", 0, QCAT_INFERENCIA);
    add_question(d, "Silogismo: Todo A e B. Todo B e C. Logo:",
                 "Nenhum A e C", "Todo A e C", "Algum A nao e C", "Nada conclui", 1, QCAT_INFERENCIA);
    
    // Equivalência lógica
    add_question(d, "~~P equivale a:",
                 "~P", "P", "P^P", "PvP", 1, QCAT_EQUIVALENCIA);
    add_question(d, "~(P^Q) equivale a (De Morgan):",
                 "~P^~Q", "PvQ", "~Pv~Q", "P^Q", 2, QCAT_EQUIVALENCIA);
    add_question(d, "P->Q equivale a:",
                 "Q->P", "~P->~Q", "~Q->~P", "P^~Q", 2, QCAT_EQUIVALENCIA);
    add_question(d, "~(PvQ) equivale a (De Morgan):",
                 "~Pv~Q", "~P^~Q", "P^Q", "PvQ", 1, QCAT_EQUIVALENCIA);
    
    // Lógica de programação
    add_question(d, "if (x>5 && x<10), qual x satisfaz?",
                 "x=3", "x=7", "x=12", "x=15", 1, QCAT_PROGRAMACAO);
    add_question(d, "if (!(a||b)), quando e verdade?",
                 "a=V,b=V", "a=V,b=F", "a=F,b=V", "a=F,b=F", 3, QCAT_PROGRAMACAO);
    add_question(d, "while(x<10) executa quantas vezes se x=7?",
                 "2 vezes", "3 vezes", "4 vezes", "Loop infinito", 1, QCAT_PROGRAMACAO);
    add_question(d, "!(A && B) equivale em programacao a:",
                 "!A && !B", "!A || !B", "A || B", "!A && B", 1, QCAT_PROGRAMACAO);
    add_question(d, "if(a>b)?a:b retorna o que?",
                 "Sempre a", "Sempre b", "O maior valor", "O menor valor", 2, QCAT_PROGRAMACAO);
    add_question(d, "Para que serve o operador % em C?",
                 "Divisao", "Resto da divisao", "Porcentagem", "Potencia", 1, QCAT_PROGRAMACAO);
    add_question(d, "Qual o valor de x apos: x=5; x+=3;?",
                 "5", "3", "8", "53", 2, QCAT_PROGRAMACAO);
    add_question(d, "O que significa o operador != em programacao?",
                 "Igual", "Diferente", "Atribuicao", "Maior ou igual", 1, QCAT_PROGRAMACAO);
}

// =============================================================================
// FOOD SPAWN
// =============================================================================

static void spawn_food(SnakeData* d) {
    int valid;
    do {
        valid = 1;
        d->food.x = rand() % SNC;
        d->food.y = rand() % SNR;
        for (int i = 0; i < d->len; i++) {
            if (d->s[i].x == d->food.x && d->s[i].y == d->food.y) {
                valid = 0;
                break;
            }
        }
    } while (!valid);
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
    d->screen = 0;
    d->question_active = 0;
    d->combo = 0;
    d->selected_option = 0;
    
    init_questions(d);
    spawn_food(d);
    
    game->data = d;
    game->active = 1;
}

// =============================================================================
// QUESTION SYSTEM
// =============================================================================

static void show_random_question(SnakeData* d) {
    d->question_index = rand() % d->num_questions;
    d->selected_option = 0;
    d->answered = 0;
    d->correct_answer = 0;
    d->question_active = 1;
}

// =============================================================================
// KEY HANDLER
// =============================================================================

static void sn_handle_key(Game* game, int key_code, int pressed) {
    if (!game || !game->data || !pressed) return;
    SnakeData* d = (SnakeData*)game->data;
    
    // Restart
    if (key_code == 82) { // R key
        free(game->data);
        sn_init(game);
        return;
    }
    
    if (d->go) return;
    
    // If question is active, handle question input
    if (d->question_active) {
        if (d->answered) {
            // Already answered, press any key to continue
            if (key_code == 13 || key_code == 32) { // ENTER or SPACE
                d->question_active = 0;
                spawn_food(d);
                snprintf(d->message, sizeof(d->message), "Continue jogando! Combo: %dx", d->combo);
                d->message_timer = 60;
            }
            return;
        }
        
        // Navigate options
        switch (key_code) {
            case 38: case 87: // UP or W
                d->selected_option--;
                if (d->selected_option < 0) d->selected_option = 3;
                break;
            case 40: case 83: // DOWN or S
                d->selected_option++;
                if (d->selected_option > 3) d->selected_option = 0;
                break;
            case 13: case 32: // ENTER or SPACE - confirm answer
                d->answered = 1;
                Question* q = &d->questions[d->question_index];
                if (d->selected_option == q->correct) {
                    d->correct_answer = 1;
                    d->score += 20 + d->combo * 5;
                    d->combo++;
                    d->len += 2;
                    if (d->spd > 3) d->spd--;
                    snprintf(d->message, sizeof(d->message), "CORRETO! +%d pontos! Combo: %dx", 
                             20 + (d->combo - 1) * 5, d->combo);
                } else {
                    d->correct_answer = 0;
                    d->combo = 0;
                    d->lives--;
                    d->len++;
                    d->score += 5;
                    snprintf(d->message, sizeof(d->message), "ERRADO! Resposta correta: opcao %d. -1 vida", 
                             q->correct + 1);
                }
                d->message_timer = 90;
                
                if (d->lives <= 0) {
                    d->go = 1;
                    d->screen = 1;
                }
                return;
        }
        return;
    }
    
    // Normal snake controls
    switch (key_code) {
        case 38: case 87: if (d->dir != 1) d->ndir = 3; break;
        case 40: case 83: if (d->dir != 3) d->ndir = 1; break;
        case 37: case 65: if (d->dir != 0) d->ndir = 2; break;
        case 39: case 68: if (d->dir != 2) d->ndir = 0; break;
    }
}

static void sn_handle_click(Game* game, int x, int y) {
    if (!game || !game->data) return;
    SnakeData* d = (SnakeData*)game->data;
    
    int gx = x - GAME_AREA_X;
    int gy = y - GAME_AREA_Y;
    
    if (d->question_active) {
        if (d->answered) {
            d->question_active = 0;
            spawn_food(d);
            snprintf(d->message, sizeof(d->message), "Continue jogando! Combo: %dx", d->combo);
            d->message_timer = 60;
            return;
        }
        
        // Option click areas
        int opt_start_y = 350;
        for (int i = 0; i < 4; i++) {
            int oy = opt_start_y + i * 45;
            if (gx >= 60 && gx <= 740 && gy >= oy && gy <= oy + 35) {
                d->selected_option = i;
                // Auto-confirm on click
                d->answered = 1;
                Question* q = &d->questions[d->question_index];
                if (d->selected_option == q->correct) {
                    d->correct_answer = 1;
                    d->score += 20 + d->combo * 5;
                    d->combo++;
                    d->len += 2;
                    if (d->spd > 3) d->spd--;
                    snprintf(d->message, sizeof(d->message), "CORRETO! +%d pontos! Combo: %dx",
                             20 + (d->combo - 1) * 5, d->combo);
                } else {
                    d->correct_answer = 0;
                    d->combo = 0;
                    d->lives--;
                    d->len++;
                    d->score += 5;
                    snprintf(d->message, sizeof(d->message), "ERRADO! Resposta correta: opcao %d. -1 vida",
                             q->correct + 1);
                }
                d->message_timer = 90;
                if (d->lives <= 0) {
                    d->go = 1;
                    d->screen = 1;
                }
                return;
            }
        }
    }
}

// =============================================================================
// UPDATE
// =============================================================================

static void sn_update(Game* game) {
    if (!game || !game->data) return;
    SnakeData* d = (SnakeData*)game->data;
    
    if (d->go) return;
    
    // Don't move if question is active
    if (d->question_active) {
        d->frame_count++;
        if (d->message_timer > 0) d->message_timer--;
        return;
    }
    
    d->frame_count++;
    if (d->message_timer > 0) d->message_timer--;
    
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
        d->lives--;
        snprintf(d->message, sizeof(d->message), "BATEU NA PAREDE! -1 vida");
        d->message_timer = 90;
        if (d->lives <= 0) { d->go = 1; d->screen = 1; return; }
        nh.x = SNC / 2; nh.y = SNR / 2;
        d->dir = 0; d->ndir = 0;
        return;
    }
    
    // Self collision
    for (int i = 0; i < d->len; i++) {
        if (d->s[i].x == nh.x && d->s[i].y == nh.y) {
            d->lives--;
            snprintf(d->message, sizeof(d->message), "BATEU EM SI MESMO! -1 vida");
            d->message_timer = 90;
            if (d->lives <= 0) { d->go = 1; d->screen = 1; return; }
            nh.x = SNC / 2; nh.y = SNR / 2;
            d->dir = 0; d->ndir = 0;
            return;
        }
    }
    
    // Food collision
    int ate = (nh.x == d->food.x && nh.y == d->food.y);
    
    // Move snake
    for (int i = d->len - 1; i > 0; i--) d->s[i] = d->s[i-1];
    d->s[0] = nh;
    
    if (ate) {
        // Show question and pause snake
        show_random_question(d);
        d->fc = 0;
    }
}

// =============================================================================
// RENDER
// =============================================================================

static const char* category_name(int cat) {
    switch (cat) {
        case QCAT_TABELA: return "TABELA-VERDADE";
        case QCAT_INFERENCIA: return "INFERENCIA LOGICA";
        case QCAT_EQUIVALENCIA: return "EQUIVALENCIA LOGICA";
        case QCAT_PROGRAMACAO: return "LOGICA DE PROGRAMACAO";
        default: return "";
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
    
    // Grid
    for (int i = 0; i <= SNC; i++)
        fb_fill(fb, SOX + i * SG, SOY, 1, SAH, 0x18, 0x18, 0x28);
    for (int i = 0; i <= SNR; i++)
        fb_fill(fb, SOX, SOY + i * SG, SAW, 1, 0x18, 0x18, 0x28);
    
    // Food (simple red apple)
    int fx = SOX + d->food.x * SG + SG/2;
    int fy = SOY + d->food.y * SG + SG/2;
    int r = SG/2 - 2;
    for (int dy = -r; dy <= r; dy++)
        for (int dx = -r; dx <= r; dx++)
            if (dx*dx + dy*dy <= r*r)
                fb_pixel(fb, fx + dx, fy + dy, 0xFF, 0x33, 0x33);
    // Stem
    fb_fill(fb, fx - 1, fy - r - 2, 2, 4, 0x00, 0x88, 0x00);
    
    // Snake
    for (int i = 0; i < d->len; i++) {
        int sx = SOX + d->s[i].x * SG + 2;
        int sy = SOY + d->s[i].y * SG + 2;
        int ss = SG - 4;
        
        fb_fill(fb, sx, sy, ss, ss,
                i == 0 ? 0x00 : 0x11,
                i == 0 ? 0xEE : 0x99,
                0x22);
        
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
    snprintf(info, sizeof(info), "Score: %d  |  Vidas: ", d->score);
    fb_text(fb, 160, 8, info, 0xFF, 0xFF, 0xFF);
    
    int info_len = (int)strlen(info);
    for (int i = 0; i < d->lives; i++)
        fb_char(fb, 160 + info_len * 6 + i * 8, 8, 3, 0xFF, 0x33, 0x33);
    
    snprintf(info, sizeof(info), "Tamanho: %d  |  Combo: %dx", d->len, d->combo);
    fb_text(fb, 450, 8, info, 0x88, 0xCC, 0x88);
    
    // ===== BOTTOM BAR =====
    int by = SOY + SAH + 5;
    fb_fill(fb, 0, by, GAME_AREA_WIDTH, GAME_AREA_HEIGHT - by, 0x0D, 0x0D, 0x1E);
    
    if (d->message_timer > 0) {
        fb_text(fb, 10, by + 5, d->message, 0xFF, 0xFF, 0x00);
    } else if (!d->question_active) {
        fb_text(fb, 10, by + 5, "Coma a comida para receber uma questao de logica!", 0x88, 0x88, 0x88);
    }
    
    fb_text(fb, 10, by + 22, "SETAS/WASD: Mover  |  R: Reiniciar", 0x66, 0x66, 0x66);
    
    // ===== QUESTION OVERLAY =====
    if (d->question_active) {
        // Darken background
        for (int y = 0; y < GAME_AREA_HEIGHT; y++) {
            for (int x = 0; x < GAME_AREA_WIDTH; x++) {
                int ax = GAME_AREA_X + x;
                int ay = GAME_AREA_Y + y;
                int i = (ay * MAIN_WINDOW_WIDTH + ax) * 4;
                fb->pixels[i] = fb->pixels[i] / 3;
                fb->pixels[i+1] = fb->pixels[i+1] / 3;
                fb->pixels[i+2] = fb->pixels[i+2] / 3;
            }
        }
        
        Question* q = &d->questions[d->question_index];
        
        // Question box
        int qx = 40;
        int qy = 120;
        int qw = 720;
        int qh = 340;
        
        fb_fill(fb, qx, qy, qw, qh, 0x15, 0x15, 0x28);
        fb_rect(fb, qx, qy, qw, qh, 0x44, 0x66, 0xAA);
        fb_rect(fb, qx + 2, qy + 2, qw - 4, qh - 4, 0x22, 0x44, 0x88);
        
        // Category
        char cat_str[64];
        snprintf(cat_str, sizeof(cat_str), "[%s]", category_name(q->category));
        fb_text(fb, qx + 20, qy + 15, cat_str, 0x00, 0xCC, 0xFF);
        
        // Paused indicator
        fb_text(fb, qx + qw - 140, qy + 15, "JOGO PAUSADO", 0xFF, 0x66, 0x00);
        
        // Separator
        fb_fill(fb, qx + 20, qy + 35, qw - 40, 2, 0x33, 0x55, 0x77);
        
        // Question
        fb_text(fb, qx + 30, qy + 55, "PERGUNTA:", 0xFF, 0xFF, 0x00);
        fb_text(fb, qx + 30, qy + 78, q->question, 0xFF, 0xFF, 0xFF);
        
        // Options
        int opt_start = qy + 130;
        for (int i = 0; i < 4; i++) {
            int oy = opt_start + i * 48;
            int sel = (i == d->selected_option);
            int show_correct = d->answered && (i == q->correct);
            int show_wrong = d->answered && sel && !d->correct_answer;
            
            unsigned char bg_r = 0x1A, bg_g = 0x1A, bg_b = 0x2A;
            if (show_correct) { bg_r = 0x00; bg_g = 0x44; bg_b = 0x00; }
            else if (show_wrong) { bg_r = 0x44; bg_g = 0x00; bg_b = 0x00; }
            else if (sel) { bg_r = 0x22; bg_g = 0x22; bg_b = 0x44; }
            
            fb_fill(fb, qx + 30, oy, qw - 60, 38, bg_r, bg_g, bg_b);
            
            unsigned char br = sel ? 0x00 : 0x33;
            unsigned char bg = sel ? 0x88 : 0x33;
            unsigned char bb = sel ? 0xFF : 0x55;
            if (show_correct) { br = 0x00; bg = 0xFF; bb = 0x00; }
            if (show_wrong) { br = 0xFF; bg = 0x00; bb = 0x00; }
            fb_rect(fb, qx + 30, oy, qw - 60, 38, br, bg, bb);
            
            char opt_label[8];
            snprintf(opt_label, sizeof(opt_label), "%d.", i + 1);
            fb_text(fb, qx + 45, oy + 10, opt_label, 0xFF, 0xCC, 0x00);
            fb_text(fb, qx + 65, oy + 10, q->options[i], sel ? 0xFF : 0xCC, sel ? 0xFF : 0xCC, 0xCC);
            
            if (show_correct) fb_text(fb, qx + qw - 100, oy + 10, "CORRETO!", 0x00, 0xFF, 0x00);
            if (show_wrong) fb_text(fb, qx + qw - 100, oy + 10, "ERRADO", 0xFF, 0x00, 0x00);
        }
        
        // Instructions
        if (!d->answered) {
            fb_text_center(fb, GAME_AREA_WIDTH/2, qy + qh - 30, 
                          "SETAS: Navegar  |  ENTER: Confirmar  |  Clique na opcao",
                          0x88, 0x88, 0x88);
        } else {
            const char* feedback_text = d->correct_answer ? 
                "CORRETO! Pressione ENTER para continuar" : 
                "ERRADO! Pressione ENTER para continuar";
            fb_text_center(fb, GAME_AREA_WIDTH/2, qy + qh - 30, feedback_text,
                          d->correct_answer ? 0x00 : 0xFF, 
                          d->correct_answer ? 0xFF : 0x00, 0x00);
        }
        
        // Score update
        if (d->answered) {
            char points[32];
            if (d->correct_answer) {
                snprintf(points, sizeof(points), "+%d pontos! Combo: %dx", 
                         20 + (d->combo - 1) * 5, d->combo);
            } else {
                snprintf(points, sizeof(points), "+5 pontos. Combo perdido!");
            }
            fb_text_center(fb, GAME_AREA_WIDTH/2, qy + qh - 55, points, 0xFF, 0xCC, 0x00);
        }
    }
    
    // ===== GAME OVER =====
    if (d->go) {
        fb_fill(fb, SOX + 80, SOY + 160, 480, 130, 0x00, 0x00, 0x00);
        fb_rect(fb, SOX + 80, SOY + 160, 480, 130, 0xFF, 0x00, 0x00);
        fb_text_center(fb, SOX + SAW/2, SOY + 185, "GAME OVER", 0xFF, 0x33, 0x33);
        char final[64];
        snprintf(final, sizeof(final), "Score Final: %d  |  Maior Combo: %dx", d->score, d->combo);
        fb_text_center(fb, SOX + SAW/2, SOY + 220, final, 0xFF, 0xFF, 0xFF);
        fb_text_center(fb, SOX + SAW/2, SOY + 250, "Pressione R para recomecar", 0xFF, 0xCC, 0x00);
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