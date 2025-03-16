/**
 * @file menu_sprite.h
 * @brief Sprite definitions for the menu system
 * 
 * This file contains sprite data, palette information, and
 * constants for rendering the menu cursor and other UI elements.
 * 
 * @author Claude
 * @date March 2025
 * @version 1.0.0
 */

 #ifndef MENU_SPRITE_H
 #define MENU_SPRITE_H
 
 #include <tonc.h>
 
 //=======================================================================
 // Menu cursor constants
 //=======================================================================
 
 // Cursor dimensions and positioning
 #define CURSOR_WIDTH     16
 #define CURSOR_HEIGHT    16
 #define CURSOR_OFFSET_X  20
 #define CURSOR_BLINK_RATE 30
 
 // Menu layout constants
 #define MENU_VALUE_MARGIN 20
 #define MENU_TITLE_Y      10
 #define MENU_HELP_MARGIN  20
 
 // Menu navigation constants
 #define MAX_MENU_STACK_SIZE 8
 
 //=======================================================================
 // BG Block allocations
 //=======================================================================
 
 // Blocks for the menu background
 #define MENU_BG_CHAR_BLOCK  1
 #define MENU_BG_SCREEN_BLOCK 28
 
 //=======================================================================
 // Cursor sprite data (16x16 pixels, 4bpp)
 //=======================================================================
 
 /**
  * Menu cursor sprite data
  * 
  * A simple arrow-shaped cursor for menu navigation.
  * Size: 16x16 pixels in 4bpp format
  */
 const unsigned int cursor_sprite_data[] = {
     0x00000000, 0x00000000, 0x00000000, 0x00000000,
     0x11100000, 0x00000000, 0x11110000, 0x00000000,
     0x11111000, 0x00000000, 0x11111100, 0x00000000,
     0x11111110, 0x00000000, 0x11111111, 0x00000000,
     0x11111110, 0x00000000, 0x11111000, 0x00000000,
     0x11110000, 0x11000000, 0x11100000, 0x11100000,
     0x00000000, 0x11110000, 0x00000000, 0x11100000,
     0x00000000, 0x11000000, 0x00000000, 0x10000000
 };
 
 /**
  * Menu cursor palette 
  * 
  * Palette 0: Transparent
  * Palette 1: White (cursor color)
  * Remaining palette entries are unused
  */
 const unsigned short cursor_sprite_pal[] = {
     0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
     0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
 };
 
 //=======================================================================
 // Menu frame sprite data
 //=======================================================================
 
 /**
  * Menu frame corner sprites
  * 
  * Used for drawing menu frames with rounded corners
  * Size: 8x8 pixels per corner in 4bpp format
  */
 const unsigned int menu_frame_corners[] = {
     // Top-left corner
     0x00011111,
     0x00111111,
     0x01111100,
     0x11111000,
     0x11110000,
     0x11100000,
     0x11100000,
     0x11100000,
     
     // Top-right corner
     0x11111000,
     0x11111100,
     0x00111110,
     0x00011111,
     0x00001111,
     0x00000111,
     0x00000111,
     0x00000111,
     
     // Bottom-left corner
     0x11100000,
     0x11100000,
     0x11100000,
     0x11110000,
     0x11111000,
     0x01111100,
     0x00111111,
     0x00011111,
     
     // Bottom-right corner
     0x00000111,
     0x00000111,
     0x00000111,
     0x00001111,
     0x00011111,
     0x00111110,
     0x11111100,
     0x11111000
 };
 
 /**
  * Menu frame edge sprites
  * 
  * Used for drawing horizontal and vertical edges of menu frames
  * Size: 8x8 pixels per edge segment in 4bpp format
  */
 const unsigned int menu_frame_edges[] = {
     // Horizontal edge (top and bottom)
     0x11111111,
     0x11111111,
     0x00000000,
     0x00000000,
     0x00000000,
     0x00000000,
     0x11111111,
     0x11111111,
     
     // Vertical edge (left and right)
     0x11000011,
     0x11000011,
     0x11000011,
     0x11000011,
     0x11000011,
     0x11000011,
     0x11000011,
     0x11000011
 };
 
 #endif // MENU_SPRITE_H