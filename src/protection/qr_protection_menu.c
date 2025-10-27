/**
 * @file qr_protection_menu.c
 * @brief Menu system for QR protection settings
 *
 * This file implements the menu interface for adjusting QR protection
 * settings to prevent photography while allowing real-time scanning.
 * 
 * @author Claude
 * @date March 2025
 * @version 1.0.0
 */

 #include <string.h>
 #include "qr_protection_menu.h"
 #include "qr_debug.h"
 #include "wallet_menu.h"  // To access shared UI functions
 
 // Global menu state
 static QrProtectionMenuState g_protection_menu_state = QR_PROT_MENU_MAIN;
 static int g_selected_option = 0;
 static int g_custom_field = 0;
 static QrProtectionParams g_temp_params;
 static bool g_show_success_message = false;
 static int g_message_timer = 0;
 
 // Forward declaration for helper functions
 static void process_main_menu_input(void);
 static void process_preset_menu_input(void);
 static void process_custom_menu_input(void);
 static void process_help_menu_input(void);
 static void render_main_menu(void);
 static void render_preset_menu(void);
 static void render_custom_menu(void);
 static void render_help_menu(void);
 static void draw_menu_frame(int x, int y, int width, int height, u16 color);
 
 // ----- QR PROTECTION MENU -----
 
 // Options for the QR protection menu
 MenuOption protection_options[] = {
     {
         .text = "Protection Level",
         .type = MENU_ITEM_ACTION,
         .x = 60,
         .y = 50,
         .width = 200,
         .height = 16,
         .action = {
             .callback = menu_action_set_protection_preset,
             .user_data = NULL
         }
     },
     {
         .text = "Custom Settings",
         .type = MENU_ITEM_ACTION,
         .x = 60,
         .y = 70,
         .width = 200,
         .height = 16,
         .action = {
             .callback = menu_action_customize_protection,
             .user_data = NULL
         }
     },
     {
         .text = "Help",
         .type = MENU_ITEM_ACTION,
         .x = 60,
         .y = 90,
         .width = 200,
         .height = 16,
         .action = {
             .callback = menu_action_show_protection_help,
             .user_data = NULL
         }
     },
     {
         .text = "Return to Menu",
         .type = MENU_ITEM_ACTION,
         .x = 60,
         .y = 110,
         .width = 200,
         .height = 16,
         .action = {
             .callback = menu_action_return_from_protection,
             .user_data = NULL
         }
     }
 };
 
 // Definition of the QR protection menu
 MenuItem qr_protection_menu = {
     .title = "QR PROTECTION",
     .options = protection_options,
     .num_options = 4,
     .help_text = "A: Select   B: Back",
     .parent = NULL  // Will be set during init
 };
 
 /**
  * Initialize QR protection menu system
  */
 void qr_protection_menu_init(void) {
     // Initialize protection system
     qr_protection_init();
     
     // Set initial state
     g_protection_menu_state = QR_PROT_MENU_MAIN;
     g_selected_option = 0;
     g_custom_field = 0;
     g_show_success_message = false;
     g_message_timer = 0;
     
     // Initialize parent menu reference
     extern MenuItem wallet_menu;
     qr_protection_menu.parent = &wallet_menu;
     
     LOG_INFO(MODULE_MENU, "QR protection menu initialized", 0);
 }
 
 /**
  * Callback to open protection menu
  */
 void menu_action_open_protection_menu(void* user_data) {
     (void)user_data;  // Unused
     
     // Get menu system instance
     MenuSystem* menu = menu_system_get_instance();
     
     // Set active menu to protection menu
     menu_system_set_active_menu(menu, &qr_protection_menu);
     
     // Initialize menu state
     g_protection_menu_state = QR_PROT_MENU_MAIN;
     g_selected_option = 0;
     
     LOG_INFO(MODULE_MENU, "Opened QR protection menu", 0);
 }
 
 /**
  * Callback to select protection preset
  */
 void menu_action_set_protection_preset(void* user_data) {
     (void)user_data;  // Unused
     
     g_protection_menu_state = QR_PROT_MENU_PRESET;
     g_selected_option = g_qr_protection.level;
     
     LOG_INFO(MODULE_MENU, "Opened protection preset selection", 0);
 }
 
 /**
  * Callback to customize protection settings
  */
 void menu_action_customize_protection(void* user_data) {
     (void)user_data;  // Unused
     
     g_protection_menu_state = QR_PROT_MENU_CUSTOM;
     g_custom_field = 0;
     
     // Copy current parameters for editing
     memcpy(&g_temp_params, &g_qr_protection.params, sizeof(QrProtectionParams));
     
     LOG_INFO(MODULE_MENU, "Opened custom protection settings", 0);
 }
 
 /**
  * Callback to show protection help
  */
 void menu_action_show_protection_help(void* user_data) {
     (void)user_data;  // Unused
     
     g_protection_menu_state = QR_PROT_MENU_HELP;
     g_selected_option = 0;
     
     LOG_INFO(MODULE_MENU, "Opened protection help", 0);
 }
 
 /**
  * Callback to return from protection menu
  */
 void menu_action_return_from_protection(void* user_data) {
     (void)user_data;  // Unused
     
     // Get menu system instance
     MenuSystem* menu = menu_system_get_instance();
     
     // Return to the parent menu
     if (qr_protection_menu.parent) {
         menu_system_set_active_menu(menu, qr_protection_menu.parent);
     }
     
     LOG_INFO(MODULE_MENU, "Returned from QR protection menu", 0);
 }
 
 /**
  * Process input for the main protection menu
  */
 static void process_main_menu_input(void) {
     // Navigate between options
     if (key_hit(KEY_UP)) {
         g_selected_option = (g_selected_option - 1 + 4) % 4;
     } else if (key_hit(KEY_DOWN)) {
         g_selected_option = (g_selected_option + 1) % 4;
     }
     
     // Select option
     if (key_hit(KEY_A)) {
         switch (g_selected_option) {
             case 0: // Protection Level
                 menu_action_set_protection_preset(NULL);
                 break;
                 
             case 1: // Custom Settings
                 menu_action_customize_protection(NULL);
                 break;
                 
             case 2: // Help
                 menu_action_show_protection_help(NULL);
                 break;
                 
             case 3: // Return
                 menu_action_return_from_protection(NULL);
                 break;
         }
     }
     
     // Back button
     if (key_hit(KEY_B)) {
         menu_action_return_from_protection(NULL);
     }
 }
 
 /**
  * Process input for the preset selection menu
  */
 static void process_preset_menu_input(void) {
     // Navigate between presets
     if (key_hit(KEY_UP)) {
         g_selected_option = (g_selected_option - 1 + 4) % 4;
     } else if (key_hit(KEY_DOWN)) {
         g_selected_option = (g_selected_option + 1) % 4;
     }
     
     // Select preset
     if (key_hit(KEY_A)) {
         qr_protection_set_level((QrProtectionLevel)g_selected_option);
         g_protection_menu_state = QR_PROT_MENU_MAIN;
         g_selected_option = 0;
         
         // Show success message
         g_show_success_message = true;
         g_message_timer = 90; // 1.5 seconds at 60 FPS
     }
     
     // Back button
     if (key_hit(KEY_B)) {
         g_protection_menu_state = QR_PROT_MENU_MAIN;
         g_selected_option = 0;
     }
 }
 
 /**
  * Process input for the help screen
  */
 static void process_help_menu_input(void) {
     // Return to main menu
     if (key_hit(KEY_A) || key_hit(KEY_B)) {
         g_protection_menu_state = QR_PROT_MENU_MAIN;
         g_selected_option = 2; // Position cursor on "Help"
     }
 }
 
 /**
  * Process input for the custom settings menu
  */
 static void process_custom_menu_input(void) {
     // Navigate between fields
     if (key_hit(KEY_UP)) {
         g_custom_field = (g_custom_field - 1 + 8) % 8;
     } else if (key_hit(KEY_DOWN)) {
         g_custom_field = (g_custom_field + 1) % 8;
     }
     
     // Adjust field value
     if (key_hit(KEY_LEFT) || key_hit(KEY_RIGHT)) {
         int direction = key_hit(KEY_RIGHT) ? 1 : -1;
         
         switch (g_custom_field) {
             case 0: // Refresh Rate (5-10 FPS)
                 g_temp_params.refresh_rate += direction;
                 if (g_temp_params.refresh_rate < 0) g_temp_params.refresh_rate = 0;
                 if (g_temp_params.refresh_rate > 10) g_temp_params.refresh_rate = 10;
                 break;
                 
             case 1: // Mask Variations (1-8)
                 g_temp_params.mask_variations += direction;
                 if (g_temp_params.mask_variations < 1) g_temp_params.mask_variations = 1;
                 if (g_temp_params.mask_variations > 8) g_temp_params.mask_variations = 8;
                 break;
                 
             case 2: // Randomize Function Patterns
                 g_temp_params.randomize_function = !g_temp_params.randomize_function;
                 break;
                 
             case 3: // Reduce ECC
                 g_temp_params.reduce_ecc = !g_temp_params.reduce_ecc;
                 break;
                 
             case 4: // Alternate Encoding
                 g_temp_params.alternate_encoding = !g_temp_params.alternate_encoding;
                 break;
                 
             case 5: // Custom ECC Level (0-3 = L,M,Q,H)
                 g_temp_params.custom_ecc_level = 
                     (g_temp_params.custom_ecc_level + direction + 4) % 4;
                 break;
                 
             case 6: // Invert Modules
                 g_temp_params.invert_modules = !g_temp_params.invert_modules;
                 break;
                 
             case 7: // Invert Percentage (0-20)
                 g_temp_params.invert_percentage += direction * 5;
                 if (g_temp_params.invert_percentage < 0) g_temp_params.invert_percentage = 0;
                 if (g_temp_params.invert_percentage > 20) g_temp_params.invert_percentage = 20;
                 break;
         }
     }
     
     // Apply custom settings
     if (key_hit(KEY_START)) {
         qr_protection_set_params(&g_temp_params);
         g_protection_menu_state = QR_PROT_MENU_MAIN;
         g_selected_option = 1; // Position cursor on "Custom Settings"
         
         // Show success message
         g_show_success_message = true;
         g_message_timer = 90; // 1.5 seconds at 60 FPS
     }
     
     // Back button
     if (key_hit(KEY_B)) {
         g_protection_menu_state = QR_PROT_MENU_MAIN;
         g_selected_option = 1; // Position cursor on "Custom Settings"
     }
 }
 
 /**
  * Update protection menu logic
  * Called every frame when the menu is active
  */
 void qr_protection_menu_update(void) {
     // Process input based on current menu state
     switch (g_protection_menu_state) {
         case QR_PROT_MENU_MAIN:
             process_main_menu_input();
             break;
             
         case QR_PROT_MENU_PRESET:
             process_preset_menu_input();
             break;
             
         case QR_PROT_MENU_CUSTOM:
             process_custom_menu_input();
             break;
             
         case QR_PROT_MENU_HELP:
             process_help_menu_input();
             break;
     }
     
     // Update message timer
     if (g_show_success_message && g_message_timer > 0) {
         g_message_timer--;
         if (g_message_timer == 0) {
             g_show_success_message = false;
         }
     }
 }
 
 /**
  * Draw a basic frame for menu elements
  */
 static void draw_menu_frame(int x, int y, int width, int height, u16 color) {
     // Import the draw_simple_frame function from wallet_menu.c
     extern void draw_simple_frame(int x, int y, int width, int height, u16 color);
     draw_simple_frame(x, y, width, height, color);
 }
 
 /**
  * Render the protection menu
  * Called every frame when the menu is active
  */
 void qr_protection_menu_render(void) {
     // Clear screen
     tte_erase_screen();
     
     // Render appropriate screen based on current state
     switch (g_protection_menu_state) {
         case QR_PROT_MENU_MAIN:
             render_main_menu();
             break;
             
         case QR_PROT_MENU_PRESET:
             render_preset_menu();
             break;
             
         case QR_PROT_MENU_CUSTOM:
             render_custom_menu();
             break;
             
         case QR_PROT_MENU_HELP:
             render_help_menu();
             break;
     }
 }
 
 /**
  * Render the main protection menu
  */
 static void render_main_menu(void) {
     // Display title
     tte_write_ex(10, 10, "QR PROTECTION SETTINGS", RGB15(31,31,0));
     
     // Draw separator
     for (int i = 0; i < SCREEN_WIDTH; i++) {
         tte_plot(i, 20, RGB15(15,15,15));
     }
     
     // Display menu options
     const char* options[] = {
         "Protection Level",
         "Custom Settings",
         "Help",
         "Return to Menu"
     };
     
     int y = 40;
     for (int i = 0; i < 4; i++) {
         // Select color based on selection
         u16 color = (i == g_selected_option) ? RGB15(31,31,0) : RGB15(31,31,31);
         
         // Display selection indicator
         if (i == g_selected_option) {
             tte_write_ex(10, y, ">", RGB15(0,31,0));
         }
         
         // Display option text
         tte_write_ex(20, y, options[i], color);
         
         y += 20;
     }
     
     // Display current protection status
     y = 120;
     char status_text[32];
     const char* level_names[] = {"OFF", "LOW", "MEDIUM", "HIGH", "CUSTOM"};
     sprintf(status_text, "Current Level: %s", level_names[g_qr_protection.level]);
     tte_write_ex(10, y, status_text, RGB15(0,31,31));
     
     y += 12;
     sprintf(status_text, "Refresh Rate: %d FPS", g_qr_protection.params.refresh_rate);
     tte_write_ex(10, y, status_text, RGB15(31,31,31));
     
     // Display success message if active
     if (g_show_success_message) {
         draw_menu_frame(40, 80, 160, 30, RGB15(0,31,0));
         tte_write_ex(60, 90, "Settings Applied!", RGB15(31,31,31));
     }
     
     // Display help text
     tte_write_ex(5, 150, "A:Select  B:Back", RGB15(31,31,31));
 }
 
 /**
  * Render the preset selection menu
  */
 static void render_preset_menu(void) {
     // Display title
     tte_write_ex(10, 10, "SELECT PROTECTION LEVEL", RGB15(31,31,0));
     
     // Draw separator
     for (int i = 0; i < SCREEN_WIDTH; i++) {
         tte_plot(i, 20, RGB15(15,15,15));
     }
     
     // Display preset options
     const char* presets[] = {
         "OFF - No Protection",
         "LOW - Basic (5 FPS)",
         "MEDIUM - Standard (7 FPS)",
         "HIGH - Maximum (10 FPS)"
     };
     
     const char* descriptions[] = {
         "Standard QR codes without protection",
         "Basic masking variations, normal QR code",
         "Medium visual distortion, reduced redundancy",
         "Maximum protection, minimal redundancy"
     };
     
     int y = 40;
     for (int i = 0; i < 4; i++) {
         // Select color based on selection
         u16 color = (i == g_selected_option) ? RGB15(31,31,0) : RGB15(31,31,31);
         
         // Display selection indicator
         if (i == g_selected_option) {
             tte_write_ex(10, y, ">", RGB15(0,31,0));
         }
         
         // Display option text
         tte_write_ex(20, y, presets[i], color);
         
         // If this option is selected, display description
         if (i == g_selected_option) {
             tte_write_ex(30, y + 12, descriptions[i], RGB15(31,20,10));
         }
         
         y += 30;
     }
     
     // Draw highlighted frame around current selection
     draw_menu_frame(15, 38 + g_selected_option * 30, 210, 16, RGB15(0,20,31));
     
     // Display help text
     tte_write_ex(5, 150, "A:Select  B:Back", RGB15(31,31,31));
 }
 
 /**
  * Render the custom settings menu
  */
 static void render_custom_menu(void) {
     // Display title
     tte_write_ex(10, 10, "CUSTOM PROTECTION SETTINGS", RGB15(31,31,0));
     
     // Draw separator
     for (int i = 0; i < SCREEN_WIDTH; i++) {
         tte_plot(i, 20, RGB15(15,15,15));
     }
     
     // Display settings fields
     const char* field_names[] = {
         "Refresh Rate",
         "Mask Variations",
         "Randomize Pattern",
         "Reduce ECC",
         "Alt. Encoding",
         "ECC Level",
         "Invert Modules",
         "Invert Percentage"
     };
     
     int y = 35;
     for (int i = 0; i < 8; i++) {
         // Select color based on selection
         u16 color = (i == g_custom_field) ? RGB15(31,31,0) : RGB15(31,31,31);
         
         // Display selection indicator
         if (i == g_custom_field) {
             tte_write_ex(10, y, ">", RGB15(0,31,0));
         }
         
         // Display field name
         tte_write_ex(20, y, field_names[i], color);
         
         // Display field value
         char value_text[32] = "";
         switch (i) {
             case 0: // Refresh Rate
                 sprintf(value_text, "%d FPS", g_temp_params.refresh_rate);
                 break;
                 
             case 1: // Mask Variations
                 sprintf(value_text, "%d", g_temp_params.mask_variations);
                 break;
                 
             case 2: // Randomize Pattern
                 strcpy(value_text, g_temp_params.randomize_function ? "ON" : "OFF");
                 break;
                 
             case 3: // Reduce ECC
                 strcpy(value_text, g_temp_params.reduce_ecc ? "ON" : "OFF");
                 break;
                 
             case 4: // Alt. Encoding
                 strcpy(value_text, g_temp_params.alternate_encoding ? "ON" : "OFF");
                 break;
                 
             case 5: // ECC Level
                 {
                     const char* ecc_names[] = {"L (Low)", "M (Medium)", "Q (High)", "H (Max)"};
                     strcpy(value_text, ecc_names[g_temp_params.custom_ecc_level]);
                 }
                 break;
                 
             case 6: // Invert Modules
                 strcpy(value_text, g_temp_params.invert_modules ? "ON" : "OFF");
                 break;
                 
             case 7: // Invert Percentage
                 sprintf(value_text, "%d%%", g_temp_params.invert_percentage);
                 break;
         }
         
         // Draw value text
         tte_write_ex(140, y, value_text, color);
         
         y += 15;
     }
     
     // Draw highlighted frame around current field
     draw_menu_frame(15, 33 + g_custom_field * 15, 210, 14, RGB15(0,20,31));
     
     // Display help for current field
     const char* field_help[] = {
         "Change QR code every N frames (0-10 FPS)",
         "Number of different mask patterns to use (1-8)",
         "Slightly alter finder pattern positions",
         "Lower error correction for harder scanning",
         "Use different encoding modes for variation",
         "Error correction level (L=low, H=high)",
         "Randomly invert non-essential modules",
         "Percentage of modules to invert (0-20%)"
     };
     
     draw_menu_frame(10, 160, 220, 20, RGB15(0,0,31));
     tte_write_ex(15, 165, field_help[g_custom_field], RGB15(31,31,31));
     
     // Display help text
     tte_write_ex(5, 145, "LEFT/RIGHT:Change  START:Apply  B:Cancel", RGB15(31,31,31));
 }
 
 /**
  * Render the help menu
  */
 static void render_help_menu(void) {
     // Display title
     tte_write_ex(10, 10, "QR PROTECTION HELP", RGB15(31,31,0));
     
     // Draw separator
     for (int i = 0; i < SCREEN_WIDTH; i++) {
         tte_plot(i, 20, RGB15(15,15,15));
     }
     
     // Display help text
     int y = 30;
     const char* help_text[] = {
         "QR Protection prevents photography of QR codes",
         "while allowing real-time scanning.",
         "",
         "The system works by rapidly changing the QR pattern",
         "while maintaining the same encoded information.",
         "",
         "Higher protection levels make photography more",
         "difficult but may reduce scanning reliability.",
         "",
         "For maximum security, use HIGH level with",
         "reduced error correction and module inversion."
     };
     
     for (int i = 0; i < 11; i++) {
         tte_write_ex(10, y, help_text[i], RGB15(31,31,31));
         y += 12;
     }
     
     // Display help text
     tte_write_ex(60, 160, "A/B: Return to Menu", RGB15(31,31,0));
 }