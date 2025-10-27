/**
 * @file tonc.h
 * @brief Main header for Tonc GBA library
 *
 * This header consolidates all Tonc modules for GBA development.
 * Tonc is a library for Game Boy Advance development with clean,
 * comprehensive code and documentation.
 *
 * @author J Vijn (original author)
 * @version 1.4.3
 * @see https://www.coranac.com/tonc/
 */

 #ifndef TONC_H
 #define TONC_H
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 // === CORE GBA MEMORY MAP ===
 #define MEM_IO      0x04000000  // I/O registers
 #define MEM_PAL     0x05000000  // Palette RAM
 #define MEM_VRAM    0x06000000  // Video RAM
 #define MEM_OAM     0x07000000  // Object Attribute Memory
 
 // === VRAM MEMORY ADDRESSES ===
 #define MEM_VRAM_ADDR(n)   (MEM_VRAM + ((n) * 0x4000))
 #define tile_mem           ((TILE*)(MEM_VRAM))
 #define tile8_mem          ((TILE8*)(MEM_VRAM))
 
 // === DISPLAY DEFINITIONS ===
 #define SCREEN_WIDTH   240
 #define SCREEN_HEIGHT  160
 #define M3_WIDTH       SCREEN_WIDTH
 #define M3_HEIGHT      SCREEN_HEIGHT
 #define M4_WIDTH       SCREEN_WIDTH
 #define M4_HEIGHT      SCREEN_HEIGHT
 #define M5_WIDTH       160
 #define M5_HEIGHT      128
 
 // === DISPLAY CONTROL REGISTER (REG_DISPCNT) ===
 #define REG_DISPCNT        *(volatile u16*)(MEM_IO+0x0000)
 
 // REG_DISPCNT bits
 #define DCNT_MODE0     0x0000  // Mode 0; bg 0-3: all reg
 #define DCNT_MODE1     0x0001  // Mode 1; bg 0-1: reg; bg 2: affine
 #define DCNT_MODE2     0x0002  // Mode 2; bg 2-3: affine
 #define DCNT_MODE3     0x0003  // Mode 3; bg 2: 240x160\@16 bitmap
 #define DCNT_MODE4     0x0004  // Mode 4; bg 2: 240x160\@8 bitmap
 #define DCNT_MODE5     0x0005  // Mode 5; bg 2: 160x128\@16 bitmap
 #define DCNT_BG0       0x0100  // Enable bg 0
 #define DCNT_BG1       0x0200  // Enable bg 1
 #define DCNT_BG2       0x0400  // Enable bg 2
 #define DCNT_BG3       0x0800  // Enable bg 3
 #define DCNT_OBJ       0x1000  // Enable objects
 #define DCNT_WIN0      0x2000  // Enable window 0
 #define DCNT_WIN1      0x4000  // Enable window 1
 #define DCNT_WINOBJ    0x8000  // Enable object window
 #define DCNT_MODE_MASK 0x0007
 #define DCNT_BLANK     0x0080  // Force screen blank
 #define DCNT_OBJ_1D    0x0040  // OBJ-VRAM as array (1D)
 
 // === BACKGROUND CONTROL REGISTERS ===
 #define REG_BG0CNT     *(volatile u16*)(MEM_IO+0x0008)
 #define REG_BG1CNT     *(volatile u16*)(MEM_IO+0x000A)
 #define REG_BG2CNT     *(volatile u16*)(MEM_IO+0x000C)
 #define REG_BG3CNT     *(volatile u16*)(MEM_IO+0x000E)
 
 // BG control bits
 #define BG_PRIO_MASK   0x0003
 #define BG_PRIO_SHIFT      0
 #define BG_PRIO(n)     ((n)<<BG_PRIO_SHIFT)
 #define BG_CBB_MASK    0x000C
 #define BG_CBB_SHIFT       2
 #define BG_CBB(n)      ((n)<<BG_CBB_SHIFT)
 #define BG_SBB_MASK    0x1F00
 #define BG_SBB_SHIFT       8
 #define BG_SBB(n)      ((n)<<BG_SBB_SHIFT)
 #define BG_REG_32x32   0x0000  // bg is 32x32 (256x256 px)
 #define BG_REG_64x32   0x4000  // bg is 64x32 (512x256 px)
 #define BG_REG_32x64   0x8000  // bg is 32x64 (256x512 px)
 #define BG_REG_64x64   0xC000  // bg is 64x64 (512x512 px)
 #define BG_SIZE_MASK   0xC000
 #define BG_SIZE_SHIFT      14
 #define BG_SIZE(n)     ((n)<<BG_SIZE_SHIFT)
 #define BG_WRAP        0x2000  // Wrap around
 #define BG_MOSAIC      0x0040  // Mosaic enabled
 #define BG_4BPP        0x0000  // 4bpp (16 color) bg
 #define BG_8BPP        0x0080  // 8bpp (256 color) bg
 #define BG_PRIO(n)     ((n)<<0)
 
 // === COLOR CONSTANTS ===
 #define CLR_BLACK   0x0000
 #define CLR_RED     0x001F
 #define CLR_GREEN   0x03E0
 #define CLR_BLUE    0x7C00
 #define CLR_CYAN    0x7FE0
 #define CLR_MAGENTA 0x7C1F
 #define CLR_YELLOW  0x03FF
 #define CLR_WHITE   0x7FFF
 
 // === RGB CONVERSION ===
 #define RGB15(r,g,b)  ((r) | ((g)<<5) | ((b)<<10))
 
 // === TIMERS ===
 #define REG_TM0CNT_L  *(volatile u16*)(MEM_IO+0x0100)
 #define REG_TM0CNT_H  *(volatile u16*)(MEM_IO+0x0102)
 #define REG_TM1CNT_L  *(volatile u16*)(MEM_IO+0x0104)
 #define REG_TM1CNT_H  *(volatile u16*)(MEM_IO+0x0106)
 #define REG_TM2CNT_L  *(volatile u16*)(MEM_IO+0x0108)
 #define REG_TM2CNT_H  *(volatile u16*)(MEM_IO+0x010A)
 #define REG_TM3CNT_L  *(volatile u16*)(MEM_IO+0x010C)
 #define REG_TM3CNT_H  *(volatile u16*)(MEM_IO+0x010E)
 
 // Timer control bits
 #define TM_FREQ_1      0x00  // Timer freq: processor
 #define TM_FREQ_64     0x01  // Timer freq: 64 cycles/tick
 #define TM_FREQ_256    0x02  // Timer freq: 256 cycles/tick
 #define TM_FREQ_1024   0x03  // Timer freq: 1024 cycles/tick
 #define TM_CASCADE     0x04  // Increment when parent timer overflows
 #define TM_IRQ         0x40  // Generate IRQ on overflow
 #define TM_ENABLE      0x80  // Start timer
 
 // === IRQ HANDLING ===
 typedef enum
 {
     II_VBLANK=0,   // Vertical blank interrupt
     II_HBLANK,     // Horizontal blank interrupt
     II_VCOUNT,     // Vertical scanline count interrupt
     II_TIMER0,     // Timer 0 interrupt
     II_TIMER1,     // Timer 1 interrupt
     II_TIMER2,     // Timer 2 interrupt
     II_TIMER3,     // Timer 3 interrupt
     II_COM,        // Serial communication interrupt
     II_DMA0,       // DMA 0 interrupt
     II_DMA1,       // DMA 1 interrupt
     II_DMA2,       // DMA 2 interrupt
     II_DMA3,       // DMA 3 interrupt
     II_KEYPAD,     // Key interrupt
     II_GAMEPAK,    // Game cartridge interrupt
     II_MAX
 } eIrqIndex;
 
 // === BASIC DATA TYPES ===
 typedef unsigned char  u8;
 typedef unsigned short u16;
 typedef unsigned int   u32;
 typedef signed char    s8;
 typedef signed short   s16;
 typedef signed int     s32;
 typedef unsigned int   uint;
 typedef signed int     int32;
 
 // === OBJECT ATTRIBUTE MEMORY (OAM) ===
 typedef struct OBJ_ATTR
 {
     u16 attr0;
     u16 attr1;
     u16 attr2;
     s16 fill;
 } ALIGN4 OBJ_ATTR;
 
 // OAM attribute 0 bits
 #define ATTR0_REG          0x0000  // Regular object
 #define ATTR0_AFF          0x0100  // Affine object
 #define ATTR0_HIDE         0x0200  // Inactive object
 #define ATTR0_AFF_DBL      0x0300  // Double-size affine object
 #define ATTR0_MODE_NORMAL  0x0000  // Normal rendering
 #define ATTR0_MODE_ALPHA   0x0400  // Alpha blending
 #define ATTR0_MODE_WINDOW  0x0800  // Object window
 #define ATTR0_MOSAIC       0x1000  // Mosaic effect
 #define ATTR0_4BPP         0x0000  // 4bpp (16 color) sprite
 #define ATTR0_8BPP         0x2000  // 8bpp (256 color) sprite
 #define ATTR0_SQUARE       0x0000  // Square shape
 #define ATTR0_WIDE         0x4000  // Wide shape (horizontal)
 #define ATTR0_TALL         0x8000  // Tall shape (vertical)
 #define ATTR0_Y_MASK       0x00FF
 #define ATTR0_Y(n)         ((n) & ATTR0_Y_MASK)
 
 // OAM attribute 1 bits
 #define ATTR1_HFLIP        0x1000  // Horizontal flip
 #define ATTR1_VFLIP        0x2000  // Vertical flip
 #define ATTR1_SIZE_8       0x0000  // 8x8 pixel sprite
 #define ATTR1_SIZE_16      0x4000  // 16x16 pixel sprite
 #define ATTR1_SIZE_32      0x8000  // 32x32 pixel sprite
 #define ATTR1_SIZE_64      0xC000  // 64x64 pixel sprite
 #define ATTR1_X_MASK       0x01FF
 #define ATTR1_X(n)         ((n) & ATTR1_X_MASK)
 #define ATTR1_AFF_ID_MASK  0x3E00
 #define ATTR1_AFF_ID(n)    ((n)<<9)
 
 // OAM attribute 2 bits
 #define ATTR2_ID_MASK      0x03FF
 #define ATTR2_ID(n)        ((n) & ATTR2_ID_MASK)
 #define ATTR2_PRIO_MASK    0x0C00
 #define ATTR2_PRIO_SHIFT   10
 #define ATTR2_PRIO(n)      ((n)<<ATTR2_PRIO_SHIFT)
 #define ATTR2_PALBANK_MASK 0xF000
 #define ATTR2_PALBANK(n)   ((n)<<12)
 
 // Sprite size values for ATTR1
 #define ATTR1_SIZE_8x8     0x0000
 #define ATTR1_SIZE_16x16   0x4000
 #define ATTR1_SIZE_32x32   0x8000
 #define ATTR1_SIZE_64x64   0xC000
 
 #define ATTR1_SIZE_8x16    0x4000
 #define ATTR1_SIZE_8x32    0x8000
 #define ATTR1_SIZE_16x32   0xC000
 
 #define ATTR1_SIZE_16x8    0x4000
 #define ATTR1_SIZE_32x8    0x8000
 #define ATTR1_SIZE_32x16   0xC000
 
 // === KEY INPUT REGISTERS ===
 #define REG_KEYINPUT   *(volatile u16*)(MEM_IO+0x0130)
 #define REG_KEYCNT     *(volatile u16*)(MEM_IO+0x0132)
 
 // Key masks
 #define KEY_MASK    0x03FF
 #define KEY_A       0x0001
 #define KEY_B       0x0002
 #define KEY_SELECT  0x0004
 #define KEY_START   0x0008
 #define KEY_RIGHT   0x0010
 #define KEY_LEFT    0x0020
 #define KEY_UP      0x0040
 #define KEY_DOWN    0x0080
 #define KEY_R       0x0100
 #define KEY_L       0x0200
 #define KEY_ANY     0x03FF
 
 // === BIOS FUNCTIONS ===
 void VBlankIntrWait(void);
 void RegisterRamReset(u32 flags);
 
 // === SYSTEM RESET FLAGS ===
 #define RESET_ALL             0xFF
 #define RESET_EWRAM           0x01
 #define RESET_IWRAM           0x02
 #define RESET_PALETTE         0x04
 #define RESET_VRAM            0x08
 #define RESET_OAM             0x10
 #define RESET_REG_SIO         0x20
 #define RESET_REG_SOUND       0x40
 #define RESET_REG             0x80
 
 // === TEXT FUNCTIONS ===
 void tte_init_se(int bg, int cbb, int pb, u16 color, int filter, void *font, void *def_font);
 void tte_init_con(void);
 void tte_write(const char *text);
 void tte_write_ex(int x, int y, const char *text, u16 color);
 void tte_erase_screen(void);
 void tte_set_pos(int x, int y);
 void tte_set_ink(u16 color);
 void tte_set_margins(int left, int top, int right, int bottom);
 void tte_get_margins(int *left, int *top, int *right, int *bottom);
 void tte_plot(int x, int y, u16 color);
 
 // === ALIGNMENT ===
 #define ALIGN4 __attribute__((aligned(4)))
 
 // === INPUT HANDLING ===
 extern u16 __key_curr, __key_prev;   // Current and previous key states
 
 static inline void key_poll()
 {
     __key_prev = __key_curr;
     __key_curr = ~REG_KEYINPUT & KEY_MASK;
 }
 
 static inline u32 key_hit(u32 key)
 {
     return (__key_curr & ~__key_prev) & key;
 }
 
 static inline u32 key_is_down(u32 key)
 {
     return __key_curr & key;
 }
 
 // === MODE 3 FUNCTIONS ===
 static inline void m3_plot(int x, int y, u16 clr)
 {
     u16 *dst= (u16*)(MEM_VRAM + ((y*M3_WIDTH+x)<<1));
     *dst= clr;
 }
 
 // === IRQ FUNCTIONS ===
 void irq_init(void (*isr)(void));
 void irq_add(enum eIrqIndex ii, void (*isr)(void));
 
 // === OBJECT FUNCTIONS ===
 void oam_init(OBJ_ATTR *obj, uint count);
 void oam_copy(OBJ_ATTR *dst, const OBJ_ATTR *src, uint count);
 
 static inline void obj_set_attr(OBJ_ATTR *obj, u16 a0, u16 a1, u16 a2)
 {
     obj->attr0= a0; obj->attr1= a1; obj->attr2= a2;
 }
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif // TONC_H