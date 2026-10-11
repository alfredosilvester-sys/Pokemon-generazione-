#include <gba.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SCREEN_W 240
#define SCREEN_H 160
#define TILE 16
#define MAP_W 15
#define MAP_H 10

typedef enum { MODE_WORLD, MODE_BATTLE, MODE_INFO } GameMode;
static GameMode mode = MODE_WORLD;
static int px = 7, py = 5;
static int encounterSteps = 0;
static int playerHP = 20, wildHP = 12;
static int messageTimer = 0;
static char message[48] = "Benvenuto a Borgo Alba!";
static unsigned int rng = 0x1234567u;

static const unsigned short C_BLACK = RGB5(1,1,2);
static const unsigned short C_WHITE = RGB5(31,31,31);
static const unsigned short C_GRASS = RGB5(8,20,9);
static const unsigned short C_LIGHT = RGB5(12,27,13);
static const unsigned short C_PATH = RGB5(25,20,11);
static const unsigned short C_WATER = RGB5(5,14,27);
static const unsigned short C_RED = RGB5(30,5,5);
static const unsigned short C_YELLOW = RGB5(31,27,8);

static unsigned int rand_next(void) {
    rng ^= rng << 13; rng ^= rng >> 17; rng ^= rng << 5;
    return rng;
}

static void rect(int x, int y, int w, int h, unsigned short c) {
    int xx, yy;
    for (yy=y; yy<y+h; yy++) for (xx=x; xx<x+w; xx++)
        if (xx>=0 && xx<SCREEN_W && yy>=0 && yy<SCREEN_H)
            ((unsigned short*)VRAM)[yy*SCREEN_W+xx] = c;
}

static void text(int x, int y, const char *s, unsigned short c) {
    // Compact 5x7 font for uppercase letters and digits.
    // Each glyph is rendered from 5-bit rows.
    static const unsigned char glyphs[36][7] = {
      {14,17,19,21,25,17,14},{4,12,4,4,4,4,14},{14,17,1,2,4,8,31},
      {30,1,1,14,1,1,30},{2,6,10,18,31,2,2},{31,16,16,30,1,1,30},
      {14,16,16,30,17,17,14},{31,1,2,4,8,8,8},{14,17,17,14,17,17,14},
      {14,17,17,15,1,1,14},{14,17,17,31,17,17,17},{30,17,17,30,17,17,30},
      {14,17,16,16,16,17,14},{30,17,17,17,17,17,30},{31,16,16,30,16,16,31},
      {31,16,16,30,16,16,16},{14,17,16,23,17,17,15},{17,17,17,31,17,17,17},
      {14,4,4,4,4,4,14},{7,2,2,2,18,18,12},{17,18,20,24,20,18,17},
      {16,16,16,16,16,16,31},{17,27,21,21,17,17,17},{17,25,21,19,17,17,17},
      {14,17,17,17,17,17,14},{30,17,17,30,16,16,16},{14,17,17,17,21,18,13},
      {30,17,17,30,20,18,17},{15,16,16,14,1,1,30},{31,4,4,4,4,4,4},
      {17,17,17,17,17,17,14},{17,17,17,17,17,10,4},{17,17,17,21,21,21,10},
      {17,17,10,4,10,17,17},{17,17,10,4,4,4,4},{31,1,2,4,8,16,31}
    };
    int ox=x;
    while (*s) {
        char ch=*s++;
        int idx=-1, row, col;
        if (ch>='a' && ch<='z') ch-=32;
        if (ch>='A' && ch<='Z') idx=ch-'A'+10;
        else if (ch>='0' && ch<='9') idx=ch-'0';
        else if (ch=='!') idx=0;
        if (idx>=0) {
            for(row=0;row<7;row++) for(col=0;col<5;col++)
                if(glyphs[idx][row] & (1<<(4-col))) rect(ox+col,y+row,1,1,c);
        }
        ox += 6;
        if (ox > SCREEN_W-6) break;
    }
}

static void draw_world(void) {
    int x,y;
    REG_DISPCNT = MODE_3 | BG2_ENABLE;
    rect(0,0,SCREEN_W,SCREEN_H,C_GRASS);
    for(y=0;y<MAP_H;y++) for(x=0;x<MAP_W;x++) {
        int sx=x*TILE, sy=y*TILE;
        if ((x==0||y==0||x==MAP_W-1||y==MAP_H-1) ||
            (x>=3&&x<=5&&y>=2&&y<=3)) rect(sx,sy,TILE,TILE,C_LIGHT);
        if ((y==7 && x>=1&&x<=13) || (x==7&&y>=1&&y<=8))
            rect(sx,sy,TILE,TILE,C_PATH);
        if (x>=10&&x<=12&&y>=2&&y<=4) rect(sx,sy,TILE,TILE,C_WATER);
        if ((x==4&&y==5)||(x==5&&y==5)||(x==10&&y==6)||(x==11&&y==6))
            rect(sx+2,sy+2,12,12,C_LIGHT);
    }
    // Player sprite: simple original pixel-art adventurer.
    rect(px*TILE+5,py*TILE+2,6,5,C_RED);
    rect(px*TILE+4,py*TILE+7,8,6,C_YELLOW);
    rect(px*TILE+5,py*TILE+13,3,3,C_BLACK);
    rect(px*TILE+9,py*TILE+13,3,3,C_BLACK);
    rect(0,144,240,16,C_BLACK);
    text(5,149,"BORGO ALBA",C_WHITE);
    text(144,149,"A: INFO",C_WHITE);
}

static void draw_battle(void) {
    REG_DISPCNT = MODE_3 | BG2_ENABLE;
    rect(0,0,240,160,RGB5(23,27,31));
    rect(0,84,240,60,RGB5(12,22,12));
    // Original placeholder creatures, not official Pokémon sprites.
    rect(145,38,35,25,C_LIGHT); rect(153,29,18,12,C_LIGHT);
    rect(152,39,5,5,C_BLACK); rect(166,39,5,5,C_BLACK);
    rect(28,86,33,23,C_RED); rect(35,78,18,12,C_RED);
    rect(37,86,5,5,C_WHITE); rect(49,86,5,5,C_WHITE);
    rect(5,5,128,30,C_WHITE); rect(8,8,122,24,C_BLACK);
    text(12,12,"SELVA LV 3",C_WHITE);
    text(12,23,"HP",C_WHITE); rect(32,24,80,4,C_LIGHT);
    rect(32,24,(wildHP*80)/12,4,C_YELLOW);
    rect(5,115,230,40,C_WHITE); rect(8,118,224,34,C_BLACK);
    text(13,122,"WILD CREATURE!",C_WHITE);
    text(13,133,"A: ATTACK B: RUN",C_WHITE);
    if (messageTimer>0) { /* message shown in lower panel */ }
}

static void draw_info(void) {
    REG_DISPCNT = MODE_3 | BG2_ENABLE;
    rect(0,0,240,160,C_BLACK);
    text(12,12,"GENERAZIONI GBA",C_WHITE);
    text(12,32,"DEMO 0.1",C_YELLOW);
    text(12,52,"D-PAD: MOVE",C_WHITE);
    text(12,64,"A: INFO / CONFIRM",C_WHITE);
    text(12,76,"B: BACK / RUN",C_WHITE);
    text(12,98,"REGION: BORGO ALBA",C_LIGHT);
    text(12,110,"NEXT: STARTER + PARTY",C_LIGHT);
    text(12,132,"PRESS B TO RETURN",C_WHITE);
}

int main(void) {
    irqInit();
    irqEnable(IRQ_VBLANK);
    SetMode(MODE_3 | BG2_ENABLE);
    while (1) {
        VBlankIntrWait();
        scanKeys();
        u16 keys = keysDown();
        if (mode==MODE_WORLD) {
            int moved=0;
            if (keys & KEY_LEFT) { if(px>1) px--; moved=1; }
            else if (keys & KEY_RIGHT) { if(px<MAP_W-2) px++; moved=1; }
            else if (keys & KEY_UP) { if(py>1) py--; moved=1; }
            else if (keys & KEY_DOWN) { if(py<MAP_H-2) py++; moved=1; }
            if (moved && ++encounterSteps%7==0 && (rand_next()%3==0)) {
                wildHP=12; mode=MODE_BATTLE;
            }
            if (keys & KEY_A) mode=MODE_INFO;
            draw_world();
        } else if (mode==MODE_INFO) {
            draw_info();
            if (keys & KEY_B) mode=MODE_WORLD;
        } else {
            draw_battle();
            if (keys & KEY_A) {
                wildHP -= 4 + (rand_next()%4);
                if (wildHP<=0) { mode=MODE_WORLD; encounterSteps=0; }
            }
            if (keys & KEY_B) mode=MODE_WORLD;
        }
    }
    return 0;
}
