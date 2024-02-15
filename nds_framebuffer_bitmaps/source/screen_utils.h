// Screen stuff -------------------------------------------------
// typedef volatile unsigned int vu32;
typedef unsigned short u16;
typedef volatile unsigned short vu16;
typedef volatile unsigned char vu8;

#define REG_DISPCNT_MAIN  (*(vu32*)0x04000000)
#define REG_DISPCNT_SUB   (*(vu32*)0x04001000)
#define MODE_FB0          (0x00020000)
#define VRAM_A            ((u16*)0x6800000)
#define VRAM_A_CR         (*(vu8*)0x04000240)
#define VRAM_ENABLE       (1<<7)
#define SCANLINECOUNTER   *(vu16 *)0x4000006

// COLOR uses numbers from (0-31) for each of the RGB components
#define COLOR(r,g,b)  ((r) | (g)<<5 | (b)<<10)
#define HSL(h,s,l)    (COLOR(0,0,0) | (h)<<10 | (s)<<5 | (l))
#define OFFSET(r,c,w) ((r)*(w)+(c))

#define SCREENWIDTH  (256)
#define SCREENHEIGHT (192)


void waitForVblank() {
    while (SCANLINECOUNTER > SCREENHEIGHT);
    while (SCANLINECOUNTER < SCREENHEIGHT);
}
