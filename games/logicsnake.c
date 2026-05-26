// =============================================================================
// LOGIC SNAKE - Snake com Questões de Raciocínio Lógico para SPANE Engine
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

#define SG 24          // Snake grid cell size (aumentado para 24)
#define SML 400        // Max snake length
#define SAW 720        // Snake area width (30 cols)
#define SAH 552        // Snake area height (23 rows)
#define SOX 40         // Snake area offset X (centralizado)
#define SOY 24         // Snake area offset Y
#define SNC (SAW/SG)   // Number of columns (30)
#define SNR (SAH/SG)   // Number of rows (23)

// Question categories - ALL about logical reasoning
#define QCAT_TABELA          0  // Tabela-verdade
#define QCAT_INFERENCIA      1  // Inferência lógica
#define QCAT_EQUIVALENCIA    2  // Equivalência lógica
#define QCAT_SILOGISMO       3  // Silogismos
#define QCAT_QUANTIFICADORES 4  // Quantificadores lógicos
#define QCAT_FALACIAS        5  // Falácias lógicas
#define QCAT_ARGUMENTOS      6  // Validade de argumentos
#define QCAT_DIAGRAMAS       7  // Diagramas de Venn
#define QCAT_SEQUENCIAS      8  // Sequências lógicas
#define QCAT_ANALOGIAS       9  // Analogias
#define QCAT_CONDICIONAIS   10  // Raciocínio condicional
#define QCAT_PARADOXOS      11  // Paradoxos lógicos
#define QCAT_DEDUCAO        12  // Dedução lógica
#define QCAT_CONJUNTOS      13  // Teoria dos conjuntos
#define QCAT_PROBABILIDADE  14  // Raciocínio probabilístico
#define QCAT_PUZZLES        15  // Puzzles lógicos

#define MAX_QUESTIONS 100

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
// QUESTION BANK - 100 Questions of Logical Reasoning
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
    
    // ==========================================
    // TABELAS-VERDADE (10 questões)
    // ==========================================
    add_question(d, "Qual o resultado de V E F?", 
                 "Verdadeiro", "Falso", "Depende", "Indeterminado", 1, QCAT_TABELA);
    add_question(d, "Qual o resultado de F OU V?", 
                 "Falso", "Verdadeiro", "Depende", "Indeterminado", 1, QCAT_TABELA);
    add_question(d, "Qual o resultado de NAO V?", 
                 "Verdadeiro", "Falso", "Depende", "Indeterminado", 1, QCAT_TABELA);
    add_question(d, "Qual o resultado de V -> F?", 
                 "Verdadeiro", "Falso", "Depende", "Indeterminado", 1, QCAT_TABELA);
    add_question(d, "Qual o resultado de F -> V?", 
                 "Verdadeiro", "Falso", "Depende", "Indeterminado", 0, QCAT_TABELA);
    add_question(d, "Qual o resultado de V <-> F?", 
                 "Verdadeiro", "Falso", "Depende", "Indeterminado", 1, QCAT_TABELA);
    add_question(d, "Qual o resultado de V XOR V?", 
                 "Verdadeiro", "Falso", "Depende", "Indeterminado", 1, QCAT_TABELA);
    add_question(d, "Qual o resultado de F XOR F?", 
                 "Verdadeiro", "Falso", "Depende", "Indeterminado", 1, QCAT_TABELA);
    add_question(d, "Qual o resultado de NAO(V E V)?", 
                 "Verdadeiro", "Falso", "Depende", "Indeterminado", 1, QCAT_TABELA);
    add_question(d, "Qual o resultado de (V -> F) E (F -> V)?", 
                 "Verdadeiro", "Falso", "Depende", "Indeterminado", 1, QCAT_TABELA);
    
    // ==========================================
    // INFERÊNCIA LÓGICA (10 questões)
    // ==========================================
    add_question(d, "Se P->Q e P=V, entao Q=?", 
                 "Falso", "Verdadeiro", "Depende", "Indeterminado", 1, QCAT_INFERENCIA);
    add_question(d, "Se P->Q e Q=F, entao P=?", 
                 "Verdadeiro", "Depende", "Falso", "Indeterminado", 2, QCAT_INFERENCIA);
    add_question(d, "Modus Ponens: Se P entao Q. P e verdade. Logo:", 
                 "P e falso", "Q e verdade", "Nada conclui", "P e Q falsos", 1, QCAT_INFERENCIA);
    add_question(d, "Modus Tollens: Se P entao Q. Q e falso. Logo:", 
                 "P e falso", "P e verdade", "Q e verdade", "Nada conclui", 0, QCAT_INFERENCIA);
    add_question(d, "Se chove, a rua molha. A rua nao molhou. Logo:", 
                 "Choveu", "Nao choveu", "A rua esta seca", "Nada conclui", 1, QCAT_INFERENCIA);
    add_question(d, "Se estudo, passo. Se passo, me formo. Estudei. Logo:", 
                 "Nao me formo", "Me formo", "Passei mas nao formo", "Nada conclui", 1, QCAT_INFERENCIA);
    add_question(d, "Dilema: Se P->R e Q->R e P ou Q, entao:", 
                 "R e falso", "R e verdade", "P e Q sao falsos", "Nada conclui", 1, QCAT_INFERENCIA);
    add_question(d, "Contrapositiva de 'Se tem fogo, ha fumaca':", 
                 "Se ha fumaca, tem fogo", "Se nao ha fumaca, nao tem fogo", 
                 "Se tem fogo, nao ha fumaca", "Fumaca causa fogo", 1, QCAT_INFERENCIA);
    add_question(d, "Se A->B e B->C, podemos concluir:", 
                 "C->A", "A->C", "B->A", "C->B", 1, QCAT_INFERENCIA);
    add_question(d, "Reducao ao absurdo: Se assumir NAO P leva a contradicao, entao:", 
                 "P e falso", "P e verdade", "Nada conclui", "NAO P e verdade", 1, QCAT_INFERENCIA);
    
    // ==========================================
    // EQUIVALÊNCIA LÓGICA (8 questões)
    // ==========================================
    add_question(d, "NAO(NAO P) equivale a:", 
                 "NAO P", "P", "P E P", "P OU P", 1, QCAT_EQUIVALENCIA);
    add_question(d, "NAO(P E Q) equivale a (De Morgan):", 
                 "NAO P E NAO Q", "P OU Q", "NAO P OU NAO Q", "P E Q", 2, QCAT_EQUIVALENCIA);
    add_question(d, "P->Q equivale a:", 
                 "Q->P", "NAO P -> NAO Q", "NAO Q -> NAO P", "P E NAO Q", 2, QCAT_EQUIVALENCIA);
    add_question(d, "NAO(P OU Q) equivale a (De Morgan):", 
                 "NAO P OU NAO Q", "NAO P E NAO Q", "P E Q", "P OU Q", 1, QCAT_EQUIVALENCIA);
    add_question(d, "P<->Q equivale a:", 
                 "P->Q", "(P->Q) E (Q->P)", "P OU Q", "NAO P OU NAO Q", 1, QCAT_EQUIVALENCIA);
    add_question(d, "P->Q equivale a:", 
                 "NAO P OU Q", "P E NAO Q", "NAO P E Q", "P OU NAO Q", 0, QCAT_EQUIVALENCIA);
    add_question(d, "A dupla negacao de P e equivalente a:", 
                 "NAO P", "P E P", "P OU P", "P", 3, QCAT_EQUIVALENCIA);
    add_question(d, "(P E Q)->R equivale a:", 
                 "P->(Q->R)", "(P->R) E (Q->R)", "P->(Q E R)", "NAO R -> NAO(P E Q)", 0, QCAT_EQUIVALENCIA);
    
    // ==========================================
    // SILOGISMOS (8 questões)
    // ==========================================
    add_question(d, "Todo A e B. Todo B e C. Logo:", 
                 "Nenhum A e C", "Todo A e C", "Algum A nao e C", "Nada conclui", 1, QCAT_SILOGISMO);
    add_question(d, "Todo gato e mamifero. Todo mamifero e vertebrado. Logo:", 
                 "Nenhum gato e vertebrado", "Todo gato e vertebrado", 
                 "Algum gato nao e vertebrado", "Nada conclui", 1, QCAT_SILOGISMO);
    add_question(d, "Nenhum A e B. Todo C e A. Logo:", 
                 "Todo C e B", "Algum C e B", "Nenhum C e B", "Nada conclui", 2, QCAT_SILOGISMO);
    add_question(d, "Algum A e B. Todo B e C. Logo:", 
                 "Todo A e C", "Nenhum A e C", "Algum A e C", "Nada conclui", 2, QCAT_SILOGISMO);
    add_question(d, "Todo peixe nada. Nenhum passaro nada. Logo:", 
                 "Nenhum peixe e passaro", "Todo passaro e peixe", 
                 "Algum peixe e passaro", "Nada conclui", 0, QCAT_SILOGISMO);
    add_question(d, "Se todo X e Y e algum Z e X:", 
                 "Todo Z e Y", "Algum Z e Y", "Nenhum Z e Y", "Todo Y e Z", 1, QCAT_SILOGISMO);
    add_question(d, "Nenhum reptil tem pelo. Cobra e reptil. Logo:", 
                 "Cobra tem pelo", "Cobra nao tem pelo", "Alguma cobra tem pelo", "Nada conclui", 1, QCAT_SILOGISMO);
    add_question(d, "Todo numero primo >2 e impar. 7 e primo >2. Logo:", 
                 "7 e par", "7 e impar", "7 nao e primo", "Nada conclui", 1, QCAT_SILOGISMO);
    
    // ==========================================
    // QUANTIFICADORES LÓGICOS (8 questões)
    // ==========================================
    add_question(d, "A negacao de 'Todo A e B' e:", 
                 "Nenhum A e B", "Algum A nao e B", "Todo A nao e B", "Algum B nao e A", 1, QCAT_QUANTIFICADORES);
    add_question(d, "A negacao de 'Existe x tal que P(x)' e:", 
                 "Existe x tal que NAO P(x)", "Para todo x, P(x)", 
                 "Para todo x, NAO P(x)", "Nao existe x", 2, QCAT_QUANTIFICADORES);
    add_question(d, "Qual a negacao de 'Todos os gatos sao pretos'?", 
                 "Nenhum gato e preto", "Existe gato que nao e preto", 
                 "Todos os gatos sao brancos", "Alguns gatos sao pretos", 1, QCAT_QUANTIFICADORES);
    add_question(d, "Para todo x(P(x)->Q(x)) significa:", 
                 "Existe x com P e Q", "Para todo x, se P entao Q", 
                 "Todo x tem P e Q", "Nenhum x tem P", 1, QCAT_QUANTIFICADORES);
    add_question(d, "Existe x(P(x) e Q(x)) significa:", 
                 "Todo x tem P e Q", "Existe x com P e Q", 
                 "Nenhum x tem P ou Q", "Existe x com P ou Q", 1, QCAT_QUANTIFICADORES);
    add_question(d, "A negacao de 'Existe x Para todo y P(x,y)' e:", 
                 "Para todo x Existe y NAO P(x,y)", "Existe x Para todo y NAO P(x,y)", 
                 "Para todo x Para todo y P(x,y)", "Existe x Existe y NAO P(x,y)", 0, QCAT_QUANTIFICADORES);
    add_question(d, "Se 'todo A e B' e falso, podemos afirmar:", 
                 "Nenhum A e B", "Algum A nao e B", "Algum B nao e A", "Todo B e A", 1, QCAT_QUANTIFICADORES);
    add_question(d, "Contrapositiva de 'Para todo x(P(x)->Q(x))':", 
                 "Para todo x(Q(x)->P(x))", "Para todo x(NAO Q(x)->NAO P(x))", 
                 "Existe x(P(x) e NAO Q(x))", "Para todo x(NAO P(x)->NAO Q(x))", 1, QCAT_QUANTIFICADORES);
    
    // ==========================================
    // FALÁCIAS LÓGICAS (8 questões)
    // ==========================================
    add_question(d, "'Se estudo, passo. Passei, logo estudei.' Que falacia?", 
                 "Afirmacao do consequente", "Negacao do antecedente", 
                 "Peticao de principio", "Falso dilema", 0, QCAT_FALACIAS);
    add_question(d, "'Se chove, molha. Nao choveu, logo nao molhou.' Que falacia?", 
                 "Afirmacao do consequente", "Negacao do antecedente", 
                 "Generalizacao apressada", "Ad hominem", 1, QCAT_FALACIAS);
    add_question(d, "'Einstein era fisico. Logo, todo fisico e genio.' Que falacia?", 
                 "Composicao", "Divisao", "Generalizacao apressada", "Falsa causa", 2, QCAT_FALACIAS);
    add_question(d, "'O time e excelente. Logo, cada jogador e excelente.' Que falacia?", 
                 "Composicao", "Divisao", "Peticao de principio", "Non sequitur", 1, QCAT_FALACIAS);
    add_question(d, "'Voce nao pode opinar sobre politica, nao e formado.' Que falacia?", 
                 "Ad hominem", "Apelo a autoridade", "Falso dilema", "Espantalho", 1, QCAT_FALACIAS);
    add_question(d, "'Ou voce me ama ou me odeia!' Que falacia?", 
                 "Falso dilema", "Inclinacao escorregadia", "Peticao de principio", "Ad populum", 0, QCAT_FALACIAS);
    add_question(d, "'Isso sempre foi assim, logo e correto.' Que falacia?", 
                 "Apelo a tradicao", "Apelo a natureza", "Non sequitur", "Post hoc", 0, QCAT_FALACIAS);
    add_question(d, "'Se A ocorreu depois de B, entao B causou A.' Que falacia?", 
                 "Post hoc ergo propter hoc", "Falsa dicotomia", 
                 "Generalizacao", "Circularidade", 0, QCAT_FALACIAS);
    
    // ==========================================
    // VALIDADE DE ARGUMENTOS (6 questões)
    // ==========================================
    add_question(d, "Premissas: P->Q, P. Conclusao: Q. O argumento e:", 
                 "Valido", "Invalido", "Valido so se P=V", "Sofisma", 0, QCAT_ARGUMENTOS);
    add_question(d, "Premissas: P->Q, Q. Conclusao: P. O argumento e:", 
                 "Valido", "Invalido", "Valido se Q=V", "Sempre valido", 1, QCAT_ARGUMENTOS);
    add_question(d, "Se as premissas sao verdadeiras e conclusao falsa, o argumento e:", 
                 "Valido", "Invalido", "Solido", "Cogente", 1, QCAT_ARGUMENTOS);
    add_question(d, "Um argumento valido com premissas verdadeiras e chamado:", 
                 "Sofisma", "Argumento solido", "Falacia", "Inducao", 1, QCAT_ARGUMENTOS);
    add_question(d, "Argumento: 'Todos homens sao mortais. Socrates e homem. Logo...'", 
                 "Invalido", "Valido mas nao solido", "Valido e solido", "Falacioso", 2, QCAT_ARGUMENTOS);
    add_question(d, "Se P ou Q e NAO P, podemos concluir Q. Isso e:", 
                 "Silogismo disjuntivo", "Modus ponens", "Falacia", "Dilema", 0, QCAT_ARGUMENTOS);
    
    // ==========================================
    // DIAGRAMAS DE VENN (5 questões)
    // ==========================================
    add_question(d, "Se A esta contido em B, no diagrama de Venn:", 
                 "A esta dentro de B", "B esta dentro de A", 
                 "A e B sao disjuntos", "A e B se intersectam", 0, QCAT_DIAGRAMAS);
    add_question(d, "Se A intersecao B = vazio, no diagrama de Venn:", 
                 "A e B se sobrepoem", "A e B nao se intersectam", 
                 "A contem B", "B contem A", 1, QCAT_DIAGRAMAS);
    add_question(d, "Quantas regioes tem o diagrama de Venn com 2 conjuntos?", 
                 "2", "3", "4", "5", 2, QCAT_DIAGRAMAS);
    add_question(d, "A uniao B representa no diagrama:", 
                 "Apenas intersecao", "A uniao de A e B", 
                 "Apenas A", "Apenas B", 1, QCAT_DIAGRAMAS);
    add_question(d, "A intersecao B representa:", 
                 "Elementos comuns a A e B", "Todos elementos de A e B", 
                 "Elementos so de A", "Elementos so de B", 0, QCAT_DIAGRAMAS);
    
    // ==========================================
    // SEQUÊNCIAS LÓGICAS (6 questões)
    // ==========================================
    add_question(d, "Complete: 2, 4, 8, 16, ?", 
                 "24", "32", "20", "18", 1, QCAT_SEQUENCIAS);
    add_question(d, "Complete: 1, 1, 2, 3, 5, ?", 
                 "7", "8", "6", "10", 1, QCAT_SEQUENCIAS);
    add_question(d, "Complete: 1, 4, 9, 16, ?", 
                 "20", "25", "36", "24", 1, QCAT_SEQUENCIAS);
    add_question(d, "Complete: A, C, E, G, ?", 
                 "H", "I", "J", "K", 1, QCAT_SEQUENCIAS);
    add_question(d, "Complete: Z, X, V, T, ?", 
                 "S", "R", "Q", "P", 1, QCAT_SEQUENCIAS);
    add_question(d, "Complete: 3, 6, 12, 24, ?", 
                 "36", "48", "30", "42", 1, QCAT_SEQUENCIAS);
    
    // ==========================================
    // ANALOGIAS (5 questões)
    // ==========================================
    add_question(d, "Cao esta para latido assim como gato esta para:", 
                 "Ronrono", "Miado", "Uivo", "Canto", 1, QCAT_ANALOGIAS);
    add_question(d, "Medico esta para hospital como professor para:", 
                 "Livraria", "Escola", "Laboratorio", "Escritorio", 1, QCAT_ANALOGIAS);
    add_question(d, "Quente esta para frio assim como Alto esta para:", 
                 "Medio", "Baixo", "Grande", "Pequeno", 1, QCAT_ANALOGIAS);
    add_question(d, "Peixe esta para agua assim como Passaro esta para:", 
                 "Terra", "Fogo", "Ar", "Ninho", 2, QCAT_ANALOGIAS);
    add_question(d, "Dia esta para sol assim como Noite esta para:", 
                 "Estrela", "Lua", "Escuro", "Sono", 1, QCAT_ANALOGIAS);
    
    // ==========================================
    // RACIOCÍNIO CONDICIONAL (6 questões)
    // ==========================================
    add_question(d, "Em 'Se P entao Q', P e condicao:", 
                 "Necessaria para Q", "Suficiente para Q", 
                 "Necessaria e suficiente", "Irrelevante", 1, QCAT_CONDICIONAIS);
    add_question(d, "Em 'P somente se Q', Q e condicao:", 
                 "Suficiente para P", "Necessaria para P", 
                 "Irrelevante", "Impossivel", 1, QCAT_CONDICIONAIS);
    add_question(d, "'P se e somente se Q' significa:", 
                 "P->Q", "Q->P", "P<->Q", "P ou Q", 2, QCAT_CONDICIONAIS);
    add_question(d, "Se 'P e suficiente para Q', entao:", 
                 "P->Q", "Q->P", "P<->Q", "NAO P -> NAO Q", 0, QCAT_CONDICIONAIS);
    add_question(d, "Se 'P e necessario para Q', entao:", 
                 "P->Q", "Q->P", "P<->Q", "NAO P -> Q", 1, QCAT_CONDICIONAIS);
    add_question(d, "A reciproca de P->Q e:", 
                 "Q->P", "NAO P -> NAO Q", "NAO Q -> NAO P", "P<->Q", 0, QCAT_CONDICIONAIS);
    
    // ==========================================
    // PARADOXOS LÓGICOS (4 questões)
    // ==========================================
    add_question(d, "O paradoxo do mentiroso: 'Esta frase e falsa'. Isso e:", 
                 "Verdadeiro", "Falso", "Paradoxal", "Indecidivel", 2, QCAT_PARADOXOS);
    add_question(d, "Se um barbeiro faz a barba de todos que nao se barbeiam, ele se barbeia?", 
                 "Sim", "Nao", "E um paradoxo", "Depende do barbeiro", 2, QCAT_PARADOXOS);
    add_question(d, "O paradoxo de Zenao sugere que o movimento:", 
                 "E impossivel", "E ilusorio", "E paradoxal", "E real", 2, QCAT_PARADOXOS);
    add_question(d, "Se 'tudo que digo e mentira' e dito por um mentiroso:", 
                 "E verdade", "E mentira", "E paradoxal", "E irrelevante", 2, QCAT_PARADOXOS);
    
    // ==========================================
    // DEDUÇÃO LÓGICA (6 questões)
    // ==========================================
    add_question(d, "Se A>B e B>C, entao:", 
                 "A=C", "A>C", "C>A", "A<C", 1, QCAT_DEDUCAO);
    add_question(d, "Ana e mais alta que Bia. Bia e mais alta que Carla. Quem e a mais baixa?", 
                 "Ana", "Bia", "Carla", "Nao sei", 2, QCAT_DEDUCAO);
    add_question(d, "Se A esta a leste de B, e B a leste de C, A esta ___ de C:", 
                 "Norte", "Sul", "Leste", "Oeste", 2, QCAT_DEDUCAO);
    add_question(d, "Em uma corrida, cheguei atras do 2o. Em que posicao estou?", 
                 "1o", "2o", "3o", "4o", 2, QCAT_DEDUCAO);
    add_question(d, "Se Maria e irma de Joao, e Joao e irmao de Pedro. Maria e ___ de Pedro:", 
                 "Irma", "Mae", "Prima", "Nao sei", 0, QCAT_DEDUCAO);
    add_question(d, "Tres pessoas: A sempre mente, B sempre diz verdade, C alterna. Quem e quem se A diz 'Eu minto'?", 
                 "A=mentiroso", "A=verdadeiro", "E paradoxal", "A=alternante", 2, QCAT_DEDUCAO);
    
    // ==========================================
    // TEORIA DOS CONJUNTOS (6 questões)
    // ==========================================
    add_question(d, "Se A={1,2,3} e B={3,4,5}, A uniao B e:", 
                 "{1,2,3}", "{3,4,5}", "{1,2,3,4,5}", "{3}", 2, QCAT_CONJUNTOS);
    add_question(d, "Se A={1,2,3} e B={3,4,5}, A intersecao B e:", 
                 "{1,2}", "{3}", "{1,2,3,4,5}", "{4,5}", 1, QCAT_CONJUNTOS);
    add_question(d, "O conjunto vazio e subconjunto de:", 
                 "Nenhum conjunto", "Apenas de si mesmo", "Todo conjunto", "Conjuntos nao vazios", 2, QCAT_CONJUNTOS);
    add_question(d, "Se A contido em B e B contido em C, entao:", 
                 "C contido em A", "A contido em C", "A=C", "A intersecao C = vazio", 1, QCAT_CONJUNTOS);
    add_question(d, "O complementar de A em relacao ao universo U e:", 
                 "U uniao A", "U-A", "A-U", "U intersecao A", 1, QCAT_CONJUNTOS);
    add_question(d, "|A uniao B| = |A| + |B| - |A intersecao B| para conjuntos:", 
                 "Quaisquer", "Disjuntos", "Infinitos", "Unitarios", 0, QCAT_CONJUNTOS);
    
    // ==========================================
    // RACIOCÍNIO PROBABILÍSTICO (4 questões)
    // ==========================================
    add_question(d, "Qual a probabilidade de cara em uma moeda honesta?", 
                 "1/4", "1/3", "1/2", "2/3", 2, QCAT_PROBABILIDADE);
    add_question(d, "Probabilidade de soma 7 em dois dados:", 
                 "1/6", "1/12", "1/36", "1/3", 0, QCAT_PROBABILIDADE);
    add_question(d, "Se P(A)=0.4 e P(B)=0.3, e sao independentes, P(A e B):", 
                 "0.7", "0.12", "0.1", "0.58", 1, QCAT_PROBABILIDADE);
    add_question(d, "Em um baralho de 52 cartas, probabilidade de As:", 
                 "1/52", "4/52", "13/52", "1/4", 1, QCAT_PROBABILIDADE);
    
    // ==========================================
    // PUZZLES LÓGICOS (10 questões)
    // ==========================================
    add_question(d, "5 maquinas fazem 5 pecas em 5 min. 100 maquinas fazem 100 pecas em:", 
                 "5 min", "100 min", "1 min", "20 min", 0, QCAT_PUZZLES);
    add_question(d, "Se uma bola e um taco custam R$1,10 e o taco custa R$1 a mais, a bola custa:", 
                 "R$0,10", "R$0,05", "R$1,00", "R$0,01", 1, QCAT_PUZZLES);
    add_question(d, "Em um lago, vitorias-regias dobram a cada dia. Se cobrem o lago em 48 dias, em quantos dias cobrem metade?", 
                 "24", "47", "12", "36", 1, QCAT_PUZZLES);
    add_question(d, "Quantos quadrados ha em um tabuleiro de xadrez 8x8?", 
                 "64", "128", "204", "100", 2, QCAT_PUZZLES);
    add_question(d, "Se 3 gatos matam 3 ratos em 3 min, 100 gatos matam 100 ratos em:", 
                 "3 min", "100 min", "33 min", "1 min", 0, QCAT_PUZZLES);
    add_question(d, "Qual o proximo: 1, 11, 21, 1211, 111221, ?", 
                 "312211", "123121", "1112221", "112211", 0, QCAT_PUZZLES);
    add_question(d, "Um fazendeiro tem 17 ovelhas. Morrem todas menos 9. Quantas restam?", 
                 "8", "9", "0", "17", 1, QCAT_PUZZLES);
    add_question(d, "Quantos meses tem 28 dias?", 
                 "1", "2", "6", "12", 3, QCAT_PUZZLES);
    add_question(d, "Se voce corre numa corrida e ultrapassa o 2o, fica em:", 
                 "1o", "2o", "3o", "Ultimo", 1, QCAT_PUZZLES);
    add_question(d, "O pai de Maria tem 5 filhas: Nana, Nene, Nini, Nono. Qual a 5a?", 
                 "Nunu", "Nana", "Maria", "Nono", 2, QCAT_PUZZLES);
    
    printf("Banco de questoes carregado: %d questoes de raciocinio logico\n", d->num_questions);
}

// =============================================================================
// CATEGORY NAME HELPER
// =============================================================================

static const char* category_name(int cat) {
    switch (cat) {
        case QCAT_TABELA: return "TABELA-VERDADE";
        case QCAT_INFERENCIA: return "INFERENCIA LOGICA";
        case QCAT_EQUIVALENCIA: return "EQUIVALENCIA LOGICA";
        case QCAT_SILOGISMO: return "SILOGISMOS";
        case QCAT_QUANTIFICADORES: return "QUANTIFICADORES";
        case QCAT_FALACIAS: return "FALACIAS LOGICAS";
        case QCAT_ARGUMENTOS: return "ARGUMENTOS";
        case QCAT_DIAGRAMAS: return "DIAGRAMAS DE VENN";
        case QCAT_SEQUENCIAS: return "SEQUENCIAS LOGICAS";
        case QCAT_ANALOGIAS: return "ANALOGIAS";
        case QCAT_CONDICIONAIS: return "RACIOCINIO CONDICIONAL";
        case QCAT_PARADOXOS: return "PARADOXOS LOGICOS";
        case QCAT_DEDUCAO: return "DEDUCAO LOGICA";
        case QCAT_CONJUNTOS: return "TEORIA DOS CONJUNTOS";
        case QCAT_PROBABILIDADE: return "RACIOCINIO PROBABILISTICO";
        case QCAT_PUZZLES: return "PUZZLES LOGICOS";
        default: return "RACIOCINIO LOGICO";
    }
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

static void sn_render(Game* game, Framebuffer* fb) {
    if (!game || !game->data || !fb) return;
    SnakeData* d = (SnakeData*)game->data;
    
    // Background
    fb_fill(fb, 0, 0, GAME_AREA_WIDTH, GAME_AREA_HEIGHT, 0x0A, 0x0A, 0x18);
    
    // ===== GAME AREA =====
    fb_fill(fb, SOX, SOY, SAW, SAH, 0x11, 0x11, 0x22);
    fb_rect(fb, SOX - 1, SOY - 1, SAW + 2, SAH + 2, 0x44, 0x44, 0x66);
    fb_rect(fb, SOX - 2, SOY - 2, SAW + 4, SAH + 4, 0x33, 0x33, 0x55);
    
    // Grid
    for (int i = 0; i <= SNC; i++)
        fb_fill(fb, SOX + i * SG, SOY, 1, SAH, 0x18, 0x18, 0x28);
    for (int i = 0; i <= SNR; i++)
        fb_fill(fb, SOX, SOY + i * SG, SAW, 1, 0x18, 0x18, 0x28);
    
    // Food (red apple with glow effect)
    int fx = SOX + d->food.x * SG + SG/2;
    int fy = SOY + d->food.y * SG + SG/2;
    int r = SG/2 - 2;
    
    // Glow
    for (int dy = -r-2; dy <= r+2; dy++)
        for (int dx = -r-2; dx <= r+2; dx++)
            if (dx*dx + dy*dy <= (r+2)*(r+2) && dx*dx + dy*dy > r*r)
                fb_pixel(fb, fx + dx, fy + dy, 0x44, 0x00, 0x00);
    
    // Apple body
    for (int dy = -r; dy <= r; dy++)
        for (int dx = -r; dx <= r; dx++)
            if (dx*dx + dy*dy <= r*r)
                fb_pixel(fb, fx + dx, fy + dy, 0xFF, 0x33, 0x33);
    
    // Highlight
    for (int dy = -r+2; dy <= -r+4; dy++)
        for (int dx = -2; dx <= 2; dx++)
            if (dx*dx + dy*dy <= 4)
                fb_pixel(fb, fx + dx, fy + dy, 0xFF, 0x88, 0x88);
    
    // Stem
    fb_fill(fb, fx - 1, fy - r - 3, 2, 5, 0x00, 0x88, 0x00);
    
    // Snake
    for (int i = 0; i < d->len; i++) {
        int sx = SOX + d->s[i].x * SG + 2;
        int sy = SOY + d->s[i].y * SG + 2;
        int ss = SG - 4;
        
        // Body with gradient
        unsigned char sr = i == 0 ? 0x00 : 0x11 + (i * 2 > 255 ? 255 : i * 2);
        unsigned char sg = i == 0 ? 0xEE : 0x99 + (i > 100 ? 100 : i);
        unsigned char sb = 0x22 + (i % 3) * 10;
        
        fb_fill(fb, sx, sy, ss, ss, sr, sg, sb);
        fb_rect(fb, sx-1, sy-1, ss+2, ss+2, 0x00, 0x44, 0x00);
        
        // Pattern on body
        if (i > 0 && i % 2 == 0) {
            fb_fill(fb, sx+ss/2-2, sy+ss/2-2, 4, 4, 0x00, 0x66, 0x00);
        }
        
        // Eyes on head
        if (i == 0) {
            int ex1, ey1, ex2, ey2;
            switch (d->dir) {
                case 0: ex1=sx+ss-6; ey1=sy+4; ex2=sx+ss-6; ey2=sy+ss-7; break;
                case 1: ex1=sx+4; ey1=sy+ss-6; ex2=sx+ss-7; ey2=sy+ss-6; break;
                case 2: ex1=sx+3; ey1=sy+4; ex2=sx+3; ey2=sy+ss-7; break;
                case 3: ex1=sx+4; ey1=sy+3; ex2=sx+ss-7; ey2=sy+3; break;
                default: ex1=sx+ss-6; ey1=sy+4; ex2=sx+ss-6; ey2=sy+ss-7;
            }
            // Eye whites
            fb_fill(fb, ex1-1, ey1-1, 5, 5, 0xFF, 0xFF, 0xFF);
            fb_fill(fb, ex2-1, ey2-1, 5, 5, 0xFF, 0xFF, 0xFF);
            // Pupils
            fb_fill(fb, ex1+1, ey1+1, 2, 2, 0x00, 0x00, 0x00);
            fb_fill(fb, ex2+1, ey2+1, 2, 2, 0x00, 0x00, 0x00);
        }
    }
    
    // ===== TOP BAR =====
    fb_fill(fb, 0, 0, GAME_AREA_WIDTH, SOY - 5, 0x0D, 0x0D, 0x1E);
    fb_rect(fb, 0, SOY-5, GAME_AREA_WIDTH, 1, 0x33, 0x55, 0x77);
    
    fb_text(fb, 10, 5, "LOGIC SNAKE", 0xFF, 0xCC, 0x00);
    
    char info[128];
    snprintf(info, sizeof(info), "Score: %d ", d->score);
    fb_text(fb, 160, 5, info, 0xFF, 0xFF, 0xFF);
    
    int info_len = (int)strlen(info);
    for (int i = 0; i < d->lives; i++)
        fb_char(fb, 160 + info_len * 6 + i * 8, 5, 3, 0xFF, 0x33, 0x33);
    
    snprintf(info, sizeof(info), "Tamanho: %d  |  Combo: %dx", d->len, d->combo);
    fb_text(fb, 480, 5, info, 0x88, 0xCC, 0x88);
    
    // ===== BOTTOM BAR =====
    int by = SOY + SAH + 5;
    fb_fill(fb, 0, by, GAME_AREA_WIDTH, GAME_AREA_HEIGHT - by, 0x0D, 0x0D, 0x1E);
    fb_rect(fb, 0, by, GAME_AREA_WIDTH, 1, 0x33, 0x55, 0x77);
    
    if (d->message_timer > 0) {
        fb_text(fb, 20, by + 8, d->message, 0xFF, 0xFF, 0x00);
    } else if (!d->question_active) {
        fb_text(fb, 20, by + 8, "Coma a comida para receber uma questao de logica!", 0x88, 0x88, 0x88);
    }
    
    fb_text(fb, 20, by + 25, "SETAS/WASD: Mover  |  ENTER: Confirmar  |  R: Reiniciar", 0x66, 0x66, 0x66);
    
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
        int qx = 60;
        int qy = 80;
        int qw = 680;
        int qh = 420;
        
        // Shadow
        fb_fill(fb, qx + 4, qy + 4, qw, qh, 0x00, 0x00, 0x00);
        
        // Main box
        fb_fill(fb, qx, qy, qw, qh, 0x15, 0x15, 0x28);
        fb_rect(fb, qx, qy, qw, qh, 0x44, 0x66, 0xAA);
        fb_rect(fb, qx + 2, qy + 2, qw - 4, qh - 4, 0x22, 0x44, 0x88);
        
        // Title bar
        fb_fill(fb, qx + 4, qy + 4, qw - 8, 28, 0x0A, 0x2A, 0x4A);
        
        // Category
        char cat_str[64];
        snprintf(cat_str, sizeof(cat_str), "[ %s ]", category_name(q->category));
        fb_text(fb, qx + 15, qy + 12, cat_str, 0x00, 0xCC, 0xFF);
        
        // Paused indicator
        fb_text(fb, qx + qw - 150, qy + 12, "JOGO PAUSADO", 0xFF, 0x66, 0x00);
        
        // Separator
        fb_fill(fb, qx + 15, qy + 38, qw - 30, 2, 0x33, 0x55, 0x77);
        
        // Question number
        char qnum[32];
        snprintf(qnum, sizeof(qnum), "Questao #%d de %d", d->question_index + 1, d->num_questions);
        fb_text(fb, qx + 25, qy + 52, qnum, 0x88, 0x88, 0x88);
        
        // Question text
        fb_text(fb, qx + 30, qy + 80, q->question, 0xFF, 0xFF, 0xFF);
        
        // Options
        int opt_start = qy + 150;
        for (int i = 0; i < 4; i++) {
            int oy = opt_start + i * 58;
            int sel = (i == d->selected_option);
            int show_correct = d->answered && (i == q->correct);
            int show_wrong = d->answered && sel && !d->correct_answer;
            
            unsigned char bg_r = 0x1A, bg_g = 0x1A, bg_b = 0x2A;
            if (show_correct) { bg_r = 0x00; bg_g = 0x44; bg_b = 0x00; }
            else if (show_wrong) { bg_r = 0x44; bg_g = 0x00; bg_b = 0x00; }
            else if (sel) { bg_r = 0x22; bg_g = 0x22; bg_b = 0x55; }
            
            fb_fill(fb, qx + 35, oy, qw - 70, 45, bg_r, bg_g, bg_b);
            
            unsigned char br = sel ? 0x00 : 0x33;
            unsigned char bg = sel ? 0x88 : 0x33;
            unsigned char bb = sel ? 0xFF : 0x55;
            if (show_correct) { br = 0x00; bg = 0xFF; bb = 0x00; }
            if (show_wrong) { br = 0xFF; bg = 0x00; bb = 0x00; }
            fb_rect(fb, qx + 35, oy, qw - 70, 45, br, bg, bb);
            
            // Option number circle
            int cx = qx + 55;
            int cy = oy + 22;
            for (int dy = -8; dy <= 8; dy++)
                for (int dx = -8; dx <= 8; dx++)
                    if (dx*dx + dy*dy <= 64)
                        fb_pixel(fb, cx + dx, cy + dy, sel ? 0xFF : 0x88, sel ? 0xCC : 0x88, 0x00);
            
            char opt_num[4];
            snprintf(opt_num, sizeof(opt_num), "%d", i + 1);
            fb_text(fb, cx - 3, cy - 3, opt_num, 0x00, 0x00, 0x00);
            
            fb_text(fb, qx + 75, oy + 14, q->options[i], sel ? 0xFF : 0xCC, sel ? 0xFF : 0xCC, 0xCC);
            
            if (show_correct) fb_text(fb, qx + qw - 110, oy + 14, "CORRETO!", 0x00, 0xFF, 0x00);
            if (show_wrong) fb_text(fb, qx + qw - 110, oy + 14, "ERRADO", 0xFF, 0x00, 0x00);
        }
        
        // Instructions
        if (!d->answered) {
            fb_text_center(fb, GAME_AREA_WIDTH/2, qy + qh - 35, 
                          "SETAS: Navegar  |  ENTER: Confirmar  |  Clique na opcao",
                          0x88, 0x88, 0x88);
        } else {
            const char* feedback_text = d->correct_answer ? 
                "CORRETO! Pressione ENTER para continuar" : 
                "ERRADO! Pressione ENTER para continuar";
            fb_text_center(fb, GAME_AREA_WIDTH/2, qy + qh - 35, feedback_text,
                          d->correct_answer ? 0x00 : 0xFF, 
                          d->correct_answer ? 0xFF : 0x00, 0x00);
        }
        
        // Score update
        if (d->answered) {
            char points[64];
            if (d->correct_answer) {
                snprintf(points, sizeof(points), "+%d pontos! Combo atual: %dx", 
                         20 + (d->combo - 1) * 5, d->combo);
            } else {
                snprintf(points, sizeof(points), "+5 pontos. Combo perdido! Resposta correta: %d", 
                         q->correct + 1);
            }
            fb_text_center(fb, GAME_AREA_WIDTH/2, qy + qh - 60, points, 0xFF, 0xCC, 0x00);
        }
    }
    
    // ===== GAME OVER =====
    if (d->go) {
        // Darken
        for (int y = SOY; y < SOY + SAH; y++) {
            for (int x = SOX; x < SOX + SAW; x++) {
                int i = ((GAME_AREA_Y + y) * MAIN_WINDOW_WIDTH + (GAME_AREA_X + x)) * 4;
                fb->pixels[i] = fb->pixels[i] / 2;
                fb->pixels[i+1] = fb->pixels[i+1] / 2;
                fb->pixels[i+2] = fb->pixels[i+2] / 2;
            }
        }
        
        // Game over box
        int gox = SOX + SAW/2 - 250;
        int goy = SOY + SAH/2 - 80;
        
        fb_fill(fb, gox + 4, goy + 4, 500, 160, 0x00, 0x00, 0x00);
        fb_fill(fb, gox, goy, 500, 160, 0x1A, 0x0A, 0x0A);
        fb_rect(fb, gox, goy, 500, 160, 0xFF, 0x00, 0x00);
        fb_rect(fb, gox + 2, goy + 2, 496, 156, 0x88, 0x00, 0x00);
        
        fb_text_center(fb, SOX + SAW/2, goy + 30, "GAME OVER", 0xFF, 0x33, 0x33);
        
        char final[64];
        snprintf(final, sizeof(final), "Score Final: %d  |  Maior Combo: %dx", d->score, d->combo);
        fb_text_center(fb, SOX + SAW/2, goy + 65, final, 0xFF, 0xFF, 0xFF);
        
        char questions_answered[64];
        snprintf(questions_answered, sizeof(questions_answered), "Questoes respondidas: %d", 
                 d->score / 5); // Aproximado
        fb_text_center(fb, SOX + SAW/2, goy + 90, questions_answered, 0xCC, 0xCC, 0xCC);
        
        fb_text_center(fb, SOX + SAW/2, goy + 120, "Pressione R para recomecar", 0xFF, 0xCC, 0x00);
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