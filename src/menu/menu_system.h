/**
 * @file menu_system.h
 * @brief Menu system for GBA Cryptocurrency QR Code Generator
 * 
 * This header defines the menu system structure and functions for
 * navigating through different screens and options in the application.
 * 
 * @author Claude
 * @date March 2025
 * @version 1.0.0
 */

 #ifndef MENU_SYSTEM_H
 #define MENU_SYSTEM_H
 
 #include <tonc.h>
 #include <string.h>
 #include <stdio.h>
 
 // =====================================================================
 // CONSTANTS AND DEFINITIONS
 // =====================================================================
 
 // Screen dimensions
 #define SCREEN_WIDTH            240
 #define SCREEN_HEIGHT           160
 
 // Text layer configuration
 #define TEXT_LAYER_BG           0
 #define TEXT_CHAR_BLOCK         0
 #define TEXT_SCREEN_BLOCK       30
 #define TEXT_PALETTE_BANK       0
 
 // Maximum text buffer size
 #define MAX_TEXT_BUFFER         256
 
 // Maximum menu stack size
 #define MAX_MENU_STACK_SIZE     8
 
 // Constants for the cursor
 #define CURSOR_WIDTH            16
 #define CURSOR_HEIGHT           16
 #define CURSOR_OFFSET_X         20
 #define CURSOR_BLINK_RATE       30
 #define MENU_VALUE_MARGIN       20
 #define MENU_TITLE_Y            10
 #define MENU_HELP_MARGIN        20
 
 // =====================================================================
 // TYPES AND STRUCTURES
 // =====================================================================
 
 // Text style flags
 typedef enum {
     TEXT_STYLE_NORMAL    = 0x00,
     TEXT_STYLE_BOLD      = 0x01,
     TEXT_STYLE_ITALIC    = 0x02,
     TEXT_STYLE_UNDERLINE = 0x04,
     TEXT_STYLE_SHADOW    = 0x08
 } TextStyleFlags;
 
 // Text alignment
 typedef enum {
     TEXT_ALIGN_LEFT,
     TEXT_ALIGN_CENTER,
     TEXT_ALIGN_RIGHT
 } TextAlignment;
 
 // Text layer system
 typedef struct TextLayerSystem TextLayerSystem;
 struct TextLayerSystem {
     // Position and dimensions
     int x, y;
     int width, height;
     int scroll_x, scroll_y;
     
     // Internal state
     bool needs_full_update;
     bool dirty_areas[SCREEN_HEIGHT/8][SCREEN_WIDTH/8];
     
     // VRAM resources
     u16 *char_base;
     u16 *map_base;
     
     // Rendering configuration
     u16 current_color;
     TextStyleFlags current_style;
     TextAlignment current_alignment;
     
     // Methods
     void (*init)(TextLayerSystem *self);
     void (*clear_area)(TextLayerSystem *self, int x, int y, int width, int height);
     void (*mark_dirty)(TextLayerSystem *self, int x, int y, int width, int height);
     void (*update_dirty_areas)(TextLayerSystem *self);
     void (*render_text)(TextLayerSystem *self, const char *text, int x, int y, u16 color, TextStyleFlags style);
     void (*render_text_aligned)(TextLayerSystem *self, const char *text, int x, int y, int width, TextAlignment align, u16 color);
     int (*get_text_width)(TextLayerSystem *self, const char *text);
     void (*set_color)(TextLayerSystem *self, u16 color);
     void (*set_style)(TextLayerSystem *self, TextStyleFlags style);
     void (*set_alignment)(TextLayerSystem *self, TextAlignment alignment);
 };
 
 // Menu item types
 typedef enum {
     MENU_ITEM_ACTION,   // Executes an action when selected
     MENU_ITEM_SUBMENU,  // Opens a submenu when selected
     MENU_ITEM_TOGGLE,   // Toggles a boolean value
     MENU_ITEM_VALUE,    // Adjusts a numeric value
     MENU_ITEM_DISABLED  // Disabled option
 } MenuItemType;
 
 // Forward declarations for circular references
 typedef struct MenuItem MenuItem;
 typedef struct MenuOption MenuOption;
 typedef struct MenuSystem MenuSystem;
 
 // Menu option
 struct MenuOption {
     char text[32];           // Display text
     MenuItemType type;       // Option type
     int x, y;                // Screen position
     int width, height;       // Dimensions
     
     union {
         // Fields for MENU_ITEM_ACTION
         struct {
             void (*callback)(void* user_data);
             void* user_data;
         } action;
         
         // Fields for MENU_ITEM_SUBMENU
         struct {
             MenuItem* submenu;
         } submenu;
         
         // Fields for MENU_ITEM_TOGGLE
         struct {
             bool* value_ptr;
             void (*on_change)(bool new_value);
         } toggle;
         
         // Fields for MENU_ITEM_VALUE
         struct {
             int* value_ptr;
             int min_value;
             int max_value;
             int step;
             void (*on_change)(int new_value);
         } value;
     };
 };
 
 // Menu item (full screen)
 struct MenuItem {
     char title[32];            // Menu title
     MenuOption* options;       // Options array
     int num_options;           // Number of options
     char help_text[64];        // Help text (bottom of screen)
     MenuItem* parent;          // Parent menu (for return)
 };
 
 // Global menu system
 struct MenuSystem {
     MenuItem* current_menu;     // Currently active menu
     int cursor_position;        // Selected option index
     float cursor_x, cursor_y;   // Current cursor position
     float cursor_target_x;      // Target cursor X position
     float cursor_target_y;      // Target cursor Y position
     bool cursor_visible;        // Cursor visibility
     int cursor_blink_counter;   // Counter for blinking animation
     
     // Navigation stack
     MenuItem* menu_stack[MAX_MENU_STACK_SIZE];
     int menu_stack_size;
 };
 
 // =====================================================================
 // FUNCTION PROTOTYPES
 // =====================================================================
 
 // Menu system functions
 MenuSystem* menu_system_get_instance();
 void menu_system_init(MenuSystem *menu);
 void menu_init_graphics();
 void menu_system_set_active_menu(MenuSystem *menu, MenuItem *item);
 void menu_system_return_to_previous(MenuSystem *menu);
 void menu_system_update(MenuSystem *menu);
 void menu_system_render(MenuSystem *menu);
 
 // Utility functions
 int menu_clamp(int value, int min, int max);
 
 // Example callback functions
 void menu_action_start_game(void* user_data);
 void menu_action_exit(void* user_data);
 void menu_option_volume_changed(int new_value);
 void menu_option_fullscreen_changed(bool enabled);

 // QR menu functions
 void qr_menu_update(void);
 void qr_menu_render(void);

 // =====================================================================
 // EXTERNAL VARIABLES
 // =====================================================================
 
 // Shared OAM buffer for sprites
 extern OBJ_ATTR obj_buffer[128];
 
 // Configuration variables
 extern bool fullscreen_enabled;
 extern int volume_level;
 
 #endif // MENU_SYSTEM_H