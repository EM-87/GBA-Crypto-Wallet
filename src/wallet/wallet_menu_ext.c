/**
 * @file wallet_menu_ext.c
 * @brief Implementation for extended wallet menu functionality
 * 
 * This file implements the extensions to the wallet menu system,
 * including cryptocurrency type management and QR protection integration.
 * 
 * @author Claude
 * @date March 2025
 * @version 1.0.0
 */

 #include <string.h>
 #include "wallet_menu_ext.h"
 #include "qr_debug.h"
 #include "crypto_types.h"
 #include "qr_protection.h"
 
 // State variables for crypto type screen
 CryptoTypeScreenState g_crypto_type_screen_state = CRYPTO_TYPE_VIEW_LIST;
 int g_selected_crypto_type = 0;
 CryptoTypeInfo g_edit_crypto_type;
 
 // External variables
 extern char g_text_input_buffer[256];
 extern int g_text_input_cursor;
 extern int g_text_input_field;
 extern bool g_text_input_active;
 extern void process_text_input(void);
 extern void draw_simple_frame(int x, int y, int width, int height, u16 color);
 
 /**
  * @brief Initialize the menu for cryptocurrency types
  */
 void wallet_crypto_types_menu_init(void) {
     g_crypto_type_screen_state = CRYPTO_TYPE_VIEW_LIST;
     g_selected_crypto_type = 0;
     
     // Initialize the crypto types system
     crypto_types_init();
     
     LOG_INFO(MODULE_WALLET, "Crypto types menu initialized", 0);
 }
 
 /**
  * @brief Update the cryptocurrency types menu
  */
 void wallet_crypto_types_menu_update(void) {
     // Process input based on current state
     switch (g_crypto_type_screen_state) {
         case CRYPTO_TYPE_VIEW_LIST:
             wallet_process_crypto_types_list_input();
             break;
             
         case CRYPTO_TYPE_EDIT:
         case CRYPTO_TYPE_NEW:
             wallet_process_crypto_type_edit_input();
             break;
             
         case CRYPTO_TYPE_DELETE_CONFIRM:
             wallet_process_crypto_type_delete_input();
             break;
     }
 }
 
 /**
  * @brief Render the cryptocurrency types menu
  */
 void wallet_crypto_types_menu_render(void) {
     // Clear screen
     tte_erase_screen();
     
     switch (g_crypto_type_screen_state) {
         case CRYPTO_TYPE_VIEW_LIST:
             // Title
             tte_write_ex(10, 5, "CRYPTOCURRENCY TYPES", RGB15(31,31,0));
             
             // Draw separator
             for (int i = 0; i < SCREEN_WIDTH; i++) {
                 tte_plot(i, 15, RGB15(15,15,15));
             }
             
             // Show available types
             int y = 25;
             int count = 0;
             
             for (int i = 0; i < MAX_CRYPTO_TYPES && count < 8; i++) {
                 const CryptoTypeInfo* type = crypto_get_type_info(i);
                 if (!type) continue;
                 
                 // Select color based on selection
                 u16 color = (i == g_selected_crypto_type) ? RGB15(31,31,0) : RGB15(31,31,31);
                 
                 // Prepare type text
                 char type_text[64];
                 sprintf(type_text, "%s (%s) - %d decimals", 
                         type->name, type->symbol, type->decimal_places);
                 
                 // Show selection indicator
                 if (i == g_selected_crypto_type) {
                     tte_write_ex(10, y, ">", RGB15(0,31,0));
                 }
                 
                 // Show main text
                 tte_write_ex(20, y, type_text, color);
                 
                 // Show special indicator for predefined types
                 if (i < CRYPTO_TYPE_CUSTOM_1) {
                     tte_write_ex(220, y, "[Default]", RGB15(20,20,31));
                 }
                 
                 y += 15;
                 count++;
             }
             
             // Instructions
             tte_write_ex(5, 150, "A:Edit  START:New  X:Delete  B:Back", RGB15(31,31,31));
             break;
             
         case CRYPTO_TYPE_EDIT:
         case CRYPTO_TYPE_NEW:
             // Title
             if (g_crypto_type_screen_state == CRYPTO_TYPE_NEW) {
                 tte_write_ex(10, 5, "NEW CRYPTOCURRENCY", RGB15(31,31,0));
             } else {
                 tte_write_ex(10, 5, "EDIT CRYPTOCURRENCY", RGB15(31,31,0));
             }
             
             // Draw separator
             for (int i = 0; i < SCREEN_WIDTH; i++) {
                 tte_plot(i, 15, RGB15(15,15,15));
             }
             
             // Draw fields
             int y = 30;
             static int edit_field = 0;
             
             // Name
             tte_write_ex(10, y, "Name:", RGB15(31,31,31));
             if (edit_field == 0) {
                 draw_simple_frame(80, y-2, 160, 12, RGB15(0,31,0));
             }
             tte_write_ex(85, y, g_edit_crypto_type.name, RGB15(31,31,31));
             y += 20;
             
             // Symbol
             tte_write_ex(10, y, "Symbol:", RGB15(31,31,31));
             if (edit_field == 1) {
                 draw_simple_frame(80, y-2, 160, 12, RGB15(0,31,0));
             }
             tte_write_ex(85, y, g_edit_crypto_type.symbol, RGB15(31,31,31));
             y += 20;
             
             // Decimals
             tte_write_ex(10, y, "Decimals:", RGB15(31,31,31));
             if (edit_field == 2) {
                 draw_simple_frame(80, y-2, 160, 12, RGB15(0,31,0));
             }
             char decimals_text[16];
             sprintf(decimals_text, "%d", g_edit_crypto_type.decimal_places);
             tte_write_ex(85, y, decimals_text, RGB15(31,31,31));
             y += 20;
             
             // Min Length
             tte_write_ex(10, y, "Min Length:", RGB15(31,31,31));
             if (edit_field == 3) {
                 draw_simple_frame(100, y-2, 60, 12, RGB15(0,31,0));
             }
             char min_len_text[16];
             sprintf(min_len_text, "%d", g_edit_crypto_type.pattern.min_length);
             tte_write_ex(105, y, min_len_text, RGB15(31,31,31));
             y += 20;
             
             // Max Length
             tte_write_ex(10, y, "Max Length:", RGB15(31,31,31));
             if (edit_field == 4) {
                 draw_simple_frame(100, y-2, 60, 12, RGB15(0,31,0));
             }
             char max_len_text[16];
             sprintf(max_len_text, "%d", g_edit_crypto_type.pattern.max_length);
             tte_write_ex(105, y, max_len_text, RGB15(31,31,31));
             
             // If in text input mode, show cursor
             if (g_text_input_active) {
                 // Draw frame for text area
                 draw_simple_frame(10, 110, 220, 30, RGB15(0,0,31));
                 
                 // Show current text
                 tte_write_ex(15, 115, g_text_input_buffer, RGB15(31,31,31));
                 
                 // Draw cursor
                 static int blink = 0;
                 blink = (blink + 1) % 30;
                 if (blink < 15) {
                     int cursor_x = 15 + g_text_input_cursor * 6;
                     for (int i = 0; i < 8; i++) {
                         tte_plot(cursor_x, 115 + i, RGB15(31,31,31));
                     }
                 }
                 
                 // Instructions
                 tte_write_ex(15, 130, "A/B:Letter  SELECT:Space  START:Confirm", RGB15(20,20,31));
             } else {
                 // Normal instructions
                 tte_write_ex(5, 150, "A/B:Change value  START:Save  X:Cancel", RGB15(31,31,31));
             }
             break;
             
         case CRYPTO_TYPE_DELETE_CONFIRM:
             // Title
             tte_write_ex(10, 5, "DELETE CRYPTOCURRENCY", RGB15(31,0,0));
             
             // Draw separator
             for (int i = 0; i < SCREEN_WIDTH; i++) {
                 tte_plot(i, 15, RGB15(15,15,15));
             }
             
             // Confirmation message
             const CryptoTypeInfo* type = crypto_get_type_info(g_selected_crypto_type);
             if (type) {
                 char confirm_text[64];
                 sprintf(confirm_text, "Delete %s (%s)?", type->name, type->symbol);
                 tte_write_ex(10, 50, confirm_text, RGB15(31,31,31));
             } else {
                 tte_write_ex(10, 50, "Selected type no longer exists", RGB15(31,0,0));
             }
             
             // Warning
             tte_write_ex(10, 70, "This action cannot be undone.", RGB15(31,15,0));
             tte_write_ex(10, 85, "Wallets with this type will still exist", RGB15(31,31,31));
             tte_write_ex(10, 100, "but may display incorrect information.", RGB15(31,31,31));
             
             // Instructions
             draw_simple_frame(20, 130, 200, 25, RGB15(31,0,0));
             tte_write_ex(30, 135, "DELETE? A:Yes  B:No", RGB15(31,0,0));
             break;
     }
 }
 
 /**
  * @brief Process input for cryptocurrency types list screen
  */
 void wallet_process_crypto_types_list_input(void) {
     // Navigate through list
     if (key_hit(KEY_UP)) {
         // Find previous active type
         int prev = g_selected_crypto_type;
         do {
             prev = (prev - 1 + MAX_CRYPTO_TYPES) % MAX_CRYPTO_TYPES;
             if (prev == g_selected_crypto_type) break; // Avoid infinite loop
         } while (!crypto_get_type_info(prev));
         
         g_selected_crypto_type = prev;
     } else if (key_hit(KEY_DOWN)) {
         // Find next active type
         int next = g_selected_crypto_type;
         do {
             next = (next + 1) % MAX_CRYPTO_TYPES;
             if (next == g_selected_crypto_type) break; // Avoid infinite loop
         } while (!crypto_get_type_info(next));
         
         g_selected_crypto_type = next;
     }
     
     // Select type
     if (key_hit(KEY_A)) {
         if (crypto_get_type_info(g_selected_crypto_type)) {
             wallet_action_edit_crypto_type(NULL);
         }
     }
     
     // Create new type
     if (key_hit(KEY_START)) {
         wallet_action_new_crypto_type(NULL);
     }
     
     // Delete type
     if (key_hit(KEY_X)) {
         wallet_action_delete_crypto_type(NULL);
     }
     
     // Return to filters
     if (key_hit(KEY_B)) {
         extern WalletScreenState g_wallet_screen_state;
         g_wallet_screen_state = WALLET_SCREEN_FILTER;
     }
 }
 
 /**
  * @brief Process input for cryptocurrency type edit screen
  */
 void wallet_process_crypto_type_edit_input(void) {
     // If in text input mode
     if (g_text_input_active) {
         process_text_input();
         return;
     }
     
     // Navigate between fields
     static int edit_field = 0;
     if (key_hit(KEY_UP)) {
         edit_field = (edit_field - 1 + 5) % 5;
     } else if (key_hit(KEY_DOWN)) {
         edit_field = (edit_field + 1) % 5;
     }
     
     // Modify selected field
     if (key_hit(KEY_A)) {
         switch (edit_field) {
             case 0: // Name
                 strcpy(g_text_input_buffer, g_edit_crypto_type.name);
                 g_text_input_cursor = strlen(g_text_input_buffer);
                 g_text_input_field = 0;
                 g_text_input_active = true;
                 break;
                 
             case 1: // Symbol
                 strcpy(g_text_input_buffer, g_edit_crypto_type.symbol);
                 g_text_input_cursor = strlen(g_text_input_buffer);
                 g_text_input_field = 1;
                 g_text_input_active = true;
                 break;
                 
             case 2: // Decimals
                 g_edit_crypto_type.decimal_places = (g_edit_crypto_type.decimal_places + 1) % 19;
                 break;
                 
             case 3: // Min Length
                 g_edit_crypto_type.pattern.min_length++;
                 if (g_edit_crypto_type.pattern.min_length > g_edit_crypto_type.pattern.max_length) {
                     g_edit_crypto_type.pattern.max_length = g_edit_crypto_type.pattern.min_length;
                 }
                 break;
                 
             case 4: // Max Length
                 g_edit_crypto_type.pattern.max_length++;
                 if (g_edit_crypto_type.pattern.max_length > 100) {
                     g_edit_crypto_type.pattern.max_length = 100;
                 }
                 break;
         }
     } else if (key_hit(KEY_B)) {
         switch (edit_field) {
             case 2: // Decimals
                 if (g_edit_crypto_type.decimal_places > 0) {
                     g_edit_crypto_type.decimal_places--;
                 }
                 break;
                 
             case 3: // Min Length
                 if (g_edit_crypto_type.pattern.min_length > 1) {
                     g_edit_crypto_type.pattern.min_length--;
                 }
                 break;
                 
             case 4: // Max Length
                 if (g_edit_crypto_type.pattern.max_length > g_edit_crypto_type.pattern.min_length) {
                     g_edit_crypto_type.pattern.max_length--;
                 }
                 break;
         }
     }
     
     // Save changes
     if (key_hit(KEY_START)) {
         // Validate data
         if (strlen(g_edit_crypto_type.name) == 0 || strlen(g_edit_crypto_type.symbol) == 0) {
             // Error: name or symbol empty
             LOG_ERROR(MODULE_WALLET, "Name or symbol empty", 0);
             return;
         }
         
         // Save type
         if (g_crypto_type_screen_state == CRYPTO_TYPE_NEW) {
             int new_index = crypto_add_custom_type(&g_edit_crypto_type);
             if (new_index >= 0) {
                 g_selected_crypto_type = new_index;
             }
         } else {
             crypto_set_type_info(g_selected_crypto_type, &g_edit_crypto_type);
         }
         
         g_crypto_type_screen_state = CRYPTO_TYPE_VIEW_LIST;
     }
     
     // Cancel editing
     if (key_hit(KEY_X)) {
         g_crypto_type_screen_state = CRYPTO_TYPE_VIEW_LIST;
     }
 }
 
 /**
  * @brief Process input for cryptocurrency type deletion confirmation
  */
 void wallet_process_crypto_type_delete_input(void) {
     // Confirm deletion
     if (key_hit(KEY_A)) {
         // Deactivate the type
         CryptoTypeInfo empty_type;
         memset(&empty_type, 0, sizeof(CryptoTypeInfo));
         crypto_set_type_info(g_selected_crypto_type, &empty_type);
         
         // Find the next active type
         int next = 0;
         while (next < MAX_CRYPTO_TYPES) {
             if (crypto_get_type_info(next)) {
                 g_selected_crypto_type = next;
                 break;
             }
             next++;
         }
         
         g_crypto_type_screen_state = CRYPTO_TYPE_VIEW_LIST;
     }
     
     // Cancel deletion
     if (key_hit(KEY_B)) {
         g_crypto_type_screen_state = CRYPTO_TYPE_VIEW_LIST;
     }
 }
 
 /**
  * @brief Callback to view cryptocurrency types
  */
 void wallet_action_view_crypto_types(void* user_data) {
     (void)user_data;  // Unused
     
     // Change wallet screen state
     extern WalletScreenState g_wallet_screen_state;
     g_wallet_screen_state = WALLET_SCREEN_CRYPTO_TYPES;
     
     // Set crypto types state
     g_crypto_type_screen_state = CRYPTO_TYPE_VIEW_LIST;
     
     LOG_INFO(MODULE_WALLET, "Opened crypto types view", 0);
 }
 
 /**
  * @brief Callback to edit a cryptocurrency type
  */
 void wallet_action_edit_crypto_type(void* user_data) {
     (void)user_data;  // Unused
     
     // Get selected type
     const CryptoTypeInfo* type_info = crypto_get_type_info(g_selected_crypto_type);
     
     if (!type_info) {
         return;
     }
     
     // Copy for editing
     memcpy(&g_edit_crypto_type, type_info, sizeof(CryptoTypeInfo));
     
     // Initialize text input with current name
     strcpy(g_text_input_buffer, g_edit_crypto_type.name);
     g_text_input_cursor = strlen(g_text_input_buffer);
     g_text_input_field = 0;
     g_text_input_active = true;
     
     g_crypto_type_screen_state = CRYPTO_TYPE_EDIT;
     
     LOG_INFO(MODULE_WALLET, "Editing crypto type", g_selected_crypto_type);
 }
 
 /**
  * @brief Callback to create a new cryptocurrency type
  */
 void wallet_action_new_crypto_type(void* user_data) {
     (void)user_data;  // Unused
     
     // Initialize data for new type
     memset(&g_edit_crypto_type, 0, sizeof(CryptoTypeInfo));
     
     // Set defaults
     strcpy(g_edit_crypto_type.name, "New Crypto");
     strcpy(g_edit_crypto_type.symbol, "NEW");
     g_edit_crypto_type.decimal_places = 8;
     g_edit_crypto_type.pattern.min_length = 20;
     g_edit_crypto_type.pattern.max_length = 50;
     g_edit_crypto_type.active = true;
     g_edit_crypto_type.color = RGB15(15,15,31);
     
     // Initialize text input with default name
     strcpy(g_text_input_buffer, g_edit_crypto_type.name);
     g_text_input_cursor = strlen(g_text_input_buffer);
     g_text_input_field = 0;
     g_text_input_active = true;
     
     g_crypto_type_screen_state = CRYPTO_TYPE_NEW;
     
     LOG_INFO(MODULE_WALLET, "Creating new crypto type", 0);
 }
 
 /**
  * @brief Callback to delete a cryptocurrency type
  */
 void wallet_action_delete_crypto_type(void* user_data) {
     (void)user_data;  // Unused
     
     // Don't allow deleting predefined types
     if (g_selected_crypto_type < CRYPTO_TYPE_CUSTOM_1) {
         return;
     }
     
     // Request confirmation
     g_crypto_type_screen_state = CRYPTO_TYPE_DELETE_CONFIRM;
     
     LOG_INFO(MODULE_WALLET, "Confirming crypto type deletion", g_selected_crypto_type);
 }
 
 /**
  * @brief Enhanced wallet menu update that includes protection menu
  */
 void enhanced_wallet_menu_update(void) {
     // Get current wallet screen state
     extern WalletScreenState g_wallet_screen_state;
     extern void wallet_menu_update(void);
     
     // Check if we're in a custom protection menu screen
     if (g_wallet_screen_state == WALLET_SCREEN_QR_PROTECTION) {
         // Use protection menu update
         extern void qr_protection_menu_update(void);
         qr_protection_menu_update();
     } else {
         // Use original menu update
         wallet_menu_update();
     }
     
     // Always update protection system for QR animation
     qr_protection_update();
 }
 
 /**
  * @brief Enhanced wallet menu rendering that includes protection menu
  */
 void enhanced_wallet_menu_render(void) {
     // Get current wallet screen state
     extern WalletScreenState g_wallet_screen_state;
     extern void wallet_menu_render(void);
     
     // Check if we're in a custom protection menu screen
     if (g_wallet_screen_state == WALLET_SCREEN_QR_PROTECTION) {
         // Use protection menu render
         extern void qr_protection_menu_render(void);
         qr_protection_menu_render();
     } else {
         // Use original menu render
         wallet_menu_render();
     }
 }
 
 /**
  * @brief Apply QR protection to wallet QR codes
  */
 bool wallet_apply_qr_protection(void) {
     // Access wallet system and current selected entry
     extern WalletSystem* wallet_system_get_instance(void);
     WalletSystem* wallet = wallet_system_get_instance();
     
     if (!wallet || wallet->selected_index < 0 || wallet->selected_index >= wallet->count) {
         return false;
     }
     
     // Get the address from the selected wallet entry
     WalletEntry* entry = &wallet->entries[wallet->selected_index];
     if (!entry->address[0]) {
         return false;
     }
     
     // Generate protected QR variations from this address
     extern bool qr_protection_generate_variations(const char *data);
     return qr_protection_generate_variations(entry->address);
 }