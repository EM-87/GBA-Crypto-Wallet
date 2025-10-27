/**
 * @file wallet_menu.c
 * @brief Implementation of the wallet menu system for GBA cryptocurrency QR generator
 * 
 * This file contains the implementation of the wallet menu interface, which
 * allows users to manage cryptocurrency wallet entries, view their details,
 * generate QR codes, and adjust settings.
 * 
 * @author Claude
 * @date March 2025
 * @version 1.0.0
 */

 #include <string.h>
 #include <stdio.h>
 #include "wallet_menu.h"
 #include "wallet_system.h"
 #include "qr_debug.h"
 #include "qr_system.h"
 #include "crypto_types.h"
 #include "qr_protection_menu.h"

 // =====================================================================
 // GLOBAL VARIABLES
 // =====================================================================
 
 // Current wallet screen state
 WalletScreenState g_wallet_screen_state = WALLET_SCREEN_LIST;
 
 // Text input buffer and state
 char g_text_input_buffer[256];
 int g_text_input_cursor = 0;
 int g_text_input_field = 0;
 bool g_text_input_active = false;
 
 // Navigation and editing state
 int g_list_scroll_position = 0;
 int g_edit_current_field = 0;
 int g_edit_scroll_position = 0;
 bool g_temp_favorite = false;
 WalletEntry g_edit_wallet_entry;
 bool g_edit_is_new_entry = false;
 bool g_confirm_delete = false;
 
 // Function pointer for QR rendering (can be replaced by QR protection system)
 bool (*wallet_render_qr_function)(int x, int y, int scale) = wallet_render_current_qr;
 
 // =====================================================================
 // WALLET MENU DEFINITION
 // =====================================================================
 
 // Wallet menu options
 MenuOption wallet_options[] = {
     {
         .text = "View Wallets",
         .type = MENU_ITEM_ACTION,
         .x = 60,
         .y = 50,
         .width = 200,
         .height = 16,
         .action = {
             .callback = wallet_action_view_list,
             .user_data = NULL
         }
     },
     {
         .text = "New Wallet",
         .type = MENU_ITEM_ACTION,
         .x = 60,
         .y = 70,
         .width = 200,
         .height = 16,
         .action = {
             .callback = wallet_action_new_wallet,
             .user_data = NULL
         }
     },
     {
         .text = "Filters",
         .type = MENU_ITEM_ACTION,
         .x = 60,
         .y = 90,
         .width = 200,
         .height = 16,
         .action = {
             .callback = wallet_action_filter,
             .user_data = NULL
         }
     },
     {
         .text = "QR Protection",
         .type = MENU_ITEM_ACTION,
         .x = 60,
         .y = 110,
         .width = 200,
         .height = 16,
         .action = {
             .callback = menu_action_open_protection_menu,
             .user_data = NULL
         }
     },
     {
         .text = "Settings",
         .type = MENU_ITEM_ACTION,
         .x = 60,
         .y = 130,
         .width = 200,
         .height = 16,
         .action = {
             .callback = wallet_action_settings,
             .user_data = NULL
         }
     },
     {
         .text = "Return to Menu",
         .type = MENU_ITEM_ACTION,
         .x = 60,
         .y = 150,
         .width = 200,
         .height = 16,
         .action = {
             .callback = wallet_action_return_to_main,
             .user_data = NULL
         }
     }
 };
 
 // Wallet menu definition
 MenuItem wallet_menu = {
     .title = "CRYPTO WALLET",
     .options = wallet_options,
     .num_options = 6,
     .help_text = "A: Select   B: Back",
     .parent = NULL  // Will be set during initialization
 };
 
 // =====================================================================
 // FUNCTION IMPLEMENTATIONS
 // =====================================================================
 
 /**
  * @brief Draws a simple frame with the given parameters
  * 
  * @param x Left position of the frame
  * @param y Top position of the frame
  * @param width Width of the frame
  * @param height Height of the frame
  * @param color Color of the frame
  */
 void draw_simple_frame(int x, int y, int width, int height, u16 color) {
     // Horizontal lines
     for (int i = x; i < x + width; i++) {
         tte_plot(i, y, color);
         tte_plot(i, y + height - 1, color);
     }
     
     // Vertical lines
     for (int i = y; i < y + height; i++) {
         tte_plot(x, i, color);
         tte_plot(x + width - 1, i, color);
     }
 }
 
 /**
  * @brief Initialize the wallet menu
  */
 void wallet_menu_init(void) {
     // Initialize wallet system
     wallet_system_init();
     
     // Initialize crypto types
     crypto_types_init();
     
     // Initialize menu state
     g_wallet_screen_state = WALLET_SCREEN_LIST;
     g_list_scroll_position = 0;
     g_edit_current_field = 0;
     g_edit_scroll_position = 0;
     g_text_input_active = false;
     g_confirm_delete = false;
     
     // Load wallet data from SRAM
     if (!wallet_system_load()) {
         LOG_WARNING(MODULE_WALLET, "Could not load wallet data, using defaults", 0);
     }
     
     LOG_INFO(MODULE_WALLET, "Wallet menu initialized", 0);
     
     // Set parent menu (will be updated by main.c)
     extern MenuItem main_menu;
     wallet_menu.parent = &main_menu;
 }
 
 /**
  * @brief View wallet list callback
  * 
  * @param user_data User data (unused)
  */
 void wallet_action_view_list(void* user_data) {
     (void)user_data;  // Unused
     g_wallet_screen_state = WALLET_SCREEN_LIST;
 }
 
 /**
  * @brief Create new wallet callback
  * 
  * @param user_data User data (unused)
  */
 void wallet_action_new_wallet(void* user_data) {
     (void)user_data;  // Unused
     
     // Initialize data for a new wallet entry
     memset(&g_edit_wallet_entry, 0, sizeof(WalletEntry));
     g_edit_wallet_entry.type_index = CRYPTO_TYPE_BITCOIN;
     g_edit_is_new_entry = true;
     g_edit_current_field = 0;
     g_wallet_screen_state = WALLET_SCREEN_NEW;
     
     // Initialize text input buffer
     strcpy(g_text_input_buffer, "New Wallet");
     g_text_input_cursor = strlen(g_text_input_buffer);
     g_text_input_field = 0;
     g_text_input_active = true;
     
     LOG_INFO(MODULE_WALLET, "Creating new wallet", 0);
 }
 
 /**
  * @brief View wallet details callback
  * 
  * @param user_data User data (unused)
  */
 void wallet_action_view_details(void* user_data) {
     (void)user_data;  // Unused
     
     WalletSystem* wallet = wallet_system_get_instance();
     
     // Verify that there is a selected wallet
     if (wallet->selected_index >= 0 && wallet->selected_index < wallet->count) {
         g_wallet_screen_state = WALLET_SCREEN_DETAILS;
         g_confirm_delete = false;
     }
     
     LOG_INFO(MODULE_WALLET, "Viewing wallet details", wallet->selected_index);
 }
 
 /**
  * @brief Edit wallet entry callback
  * 
  * @param user_data User data (unused)
  */
 void wallet_action_edit_wallet(void* user_data) {
     (void)user_data;  // Unused
     
     WalletSystem* wallet = wallet_system_get_instance();
     
     // Verify that there is a selected wallet
     if (wallet->selected_index < 0 || wallet->selected_index >= wallet->count) {
         return;
     }
     
     // Copy data for editing
     memcpy(&g_edit_wallet_entry, &wallet->entries[wallet->selected_index], sizeof(WalletEntry));
     g_edit_is_new_entry = false;
     g_edit_current_field = 0;
     g_wallet_screen_state = WALLET_SCREEN_EDIT;
     
     // Initialize text input with current name
     strcpy(g_text_input_buffer, g_edit_wallet_entry.name);
     g_text_input_cursor = strlen(g_text_input_buffer);
     g_text_input_field = 0;
     g_text_input_active = true;
     
     LOG_INFO(MODULE_WALLET, "Editing wallet", wallet->selected_index);
 }
 
 /**
  * @brief Delete wallet entry callback
  * 
  * @param user_data User data (unused)
  */
 void wallet_action_delete_wallet(void* user_data) {
     (void)user_data;  // Unused
     
     WalletSystem* wallet = wallet_system_get_instance();
     
     // Verify that there is a selected wallet
     if (wallet->selected_index < 0 || wallet->selected_index >= wallet->count) {
         return;
     }
     
     // If not in confirmation mode, activate it
     if (!g_confirm_delete) {
         g_confirm_delete = true;
         LOG_INFO(MODULE_WALLET, "Confirming wallet deletion", wallet->selected_index);
         return;
     }
     
     // Delete wallet and save
     wallet_delete_entry(wallet->selected_index);
     wallet_system_save();
     
     g_confirm_delete = false;
     g_wallet_screen_state = WALLET_SCREEN_LIST;
     
     LOG_INFO(MODULE_WALLET, "Wallet deleted", wallet->selected_index);
 }
 
 /**
  * @brief Show QR code callback
  * 
  * @param user_data User data (unused)
  */
 void wallet_action_show_qr(void* user_data) {
     (void)user_data;  // Unused
     
     WalletSystem* wallet = wallet_system_get_instance();
     
     // Verify that there is a selected wallet
     if (wallet->selected_index < 0 || wallet->selected_index >= wallet->count) {
         return;
     }
     
     // Generate QR for the selected wallet
     if (wallet_generate_qr(wallet->selected_index)) {
         g_wallet_screen_state = WALLET_SCREEN_QR;
         LOG_INFO(MODULE_WALLET, "Displaying QR for wallet", wallet->selected_index);
     } else {
         LOG_ERROR(MODULE_WALLET, "Failed to generate QR", wallet->selected_index);
     }
 }
 
 /**
  * @brief Show filter screen callback
  * 
  * @param user_data User data (unused)
  */
 void wallet_action_filter(void* user_data) {
     (void)user_data;  // Unused
     g_wallet_screen_state = WALLET_SCREEN_FILTER;
     LOG_INFO(MODULE_WALLET, "Opening filter screen", 0);
 }
 
 /**
  * @brief Show settings screen callback
  * 
  * @param user_data User data (unused)
  */
 void wallet_action_settings(void* user_data) {
     (void)user_data;  // Unused
     g_wallet_screen_state = WALLET_SCREEN_SETTINGS;
     LOG_INFO(MODULE_WALLET, "Opening settings screen", 0);
 }
 
 /**
  * @brief Return to main menu callback
  * 
  * @param user_data User data (unused)
  */
 void wallet_action_return_to_main(void* user_data) {
     (void)user_data;  // Unused
     
     // Get menu system instance
     MenuSystem* menu = menu_system_get_instance();
     
     // Save wallet data
     wallet_system_save();
     
     // Return to the parent menu
     if (wallet_menu.parent) {
         menu_system_set_active_menu(menu, wallet_menu.parent);
     }
     
     LOG_INFO(MODULE_WALLET, "Returning to main menu", 0);
 }
 
 /**
  * @brief Process text input
  */
 void process_text_input(void) {
     static int repeat_timer = 0;
     static int last_key = 0;
     
     // Determine maximum length based on active field
     int max_length = 0;
     switch (g_text_input_field) {
         case 0: // Name
             max_length = sizeof(g_edit_wallet_entry.name) - 1;
             break;
         case 1: // Address
             max_length = sizeof(g_edit_wallet_entry.address) - 1;
             break;
         case 2: // Notes
             max_length = sizeof(g_edit_wallet_entry.notes) - 1;
             break;
         case 3: // Tags
             max_length = sizeof(g_edit_wallet_entry.tags) - 1;
             break;
     }
     
     // Key repeat handling
     if (key_is_down(last_key)) {
         repeat_timer++;
         if (repeat_timer < 20) {
             return;
         }
         repeat_timer = 15;  // Faster repeat after initial delay
     } else {
         repeat_timer = 0;
         last_key = 0;
     }
     
     // Detect pressed key
     int input_key = 0;
     
     if (key_is_down(KEY_A)) {
         input_key = 'a';
         if (key_is_down(KEY_R)) input_key = 'A';
     } else if (key_is_down(KEY_B)) {
         input_key = 'b';
         if (key_is_down(KEY_R)) input_key = 'B';
     } else if (key_is_down(KEY_SELECT)) {
         input_key = ' ';  // Space
     } else if (key_is_down(KEY_START)) {
         // Finish text input
         g_text_input_active = false;
         
         // Copy text based on active field
         switch (g_text_input_field) {
             case 0: // Name
                 strncpy(g_edit_wallet_entry.name, g_text_input_buffer, sizeof(g_edit_wallet_entry.name) - 1);
                 g_edit_wallet_entry.name[sizeof(g_edit_wallet_entry.name) - 1] = '\0';
                 break;
             case 1: // Address
                 strncpy(g_edit_wallet_entry.address, g_text_input_buffer, sizeof(g_edit_wallet_entry.address) - 1);
                 g_edit_wallet_entry.address[sizeof(g_edit_wallet_entry.address) - 1] = '\0';
                 break;
             case 2: // Notes
                 strncpy(g_edit_wallet_entry.notes, g_text_input_buffer, sizeof(g_edit_wallet_entry.notes) - 1);
                 g_edit_wallet_entry.notes[sizeof(g_edit_wallet_entry.notes) - 1] = '\0';
                 break;
             case 3: // Tags
                 strncpy(g_edit_wallet_entry.tags, g_text_input_buffer, sizeof(g_edit_wallet_entry.tags) - 1);
                 g_edit_wallet_entry.tags[sizeof(g_edit_wallet_entry.tags) - 1] = '\0';
                 break;
         }
         
         // Move to next field
         g_edit_current_field++;
         if (g_edit_current_field > 5) {
             g_edit_current_field = 0;
         }
         
         return;
     } else if (key_is_down(KEY_LEFT)) {
         // Move cursor left
         if (g_text_input_cursor > 0) {
             g_text_input_cursor--;
         }
         last_key = KEY_LEFT;
         return;
     } else if (key_is_down(KEY_RIGHT)) {
         // Move cursor right
         if (g_text_input_cursor < strlen(g_text_input_buffer)) {
             g_text_input_cursor++;
         }
         last_key = KEY_RIGHT;
         return;
     } else if (key_is_down(KEY_UP)) {
         // Cycle character (a→A→0→symbols)
         if (!last_key) {
             int pos = g_text_input_cursor;
             if (pos < strlen(g_text_input_buffer)) {
                 char c = g_text_input_buffer[pos];
                 if (c >= 'a' && c <= 'z') {
                     c = c - 'a' + 'A';
                 } else if (c >= 'A' && c <= 'Z') {
                     c = c - 'A' + '0';
                 } else if (c >= '0' && c <= '9') {
                     // Cycle through common symbols
                     char symbols[] = "!@#$%^&*()-_+=[]{}|;:,.<>?/";
                     c = symbols[(c - '0') % strlen(symbols)];
                 } else {
                     c = 'a';
                 }
                 g_text_input_buffer[pos] = c;
             }
         }
         last_key = KEY_UP;
         return;
     } else if (key_is_down(KEY_DOWN)) {
         // Delete character
         if (!last_key && g_text_input_cursor > 0) {
             memmove(g_text_input_buffer + g_text_input_cursor - 1, 
                     g_text_input_buffer + g_text_input_cursor, 
                     strlen(g_text_input_buffer) - g_text_input_cursor + 1);
             g_text_input_cursor--;
         }
         last_key = KEY_DOWN;
         return;
     }
     
     // Insert character if valid key and space available
     if (input_key && g_text_input_cursor < max_length) {
         // Insert character at cursor position
         memmove(g_text_input_buffer + g_text_input_cursor + 1, 
                 g_text_input_buffer + g_text_input_cursor, 
                 strlen(g_text_input_buffer) - g_text_input_cursor + 1);
         g_text_input_buffer[g_text_input_cursor] = input_key;
         g_text_input_cursor++;
         
         last_key = input_key;
     }
 }
 
 /**
  * @brief Process input in the wallet list screen
  */
 void wallet_process_list_input(void) {
     WalletSystem* wallet = wallet_system_get_instance();
     
     // Navigate through list
     if (key_hit(KEY_UP)) {
         wallet_prev_entry();
     } else if (key_hit(KEY_DOWN)) {
         wallet_next_entry();
     }
     
     // Select wallet
     if (key_hit(KEY_A)) {
         if (wallet->count > 0 && wallet->selected_index >= 0) {
             wallet_action_view_details(NULL);
         }
     }
     
     // Create new wallet
     if (key_hit(KEY_START)) {
         wallet_action_new_wallet(NULL);
     }
     
     // Return to menu
     if (key_hit(KEY_B)) {
         wallet_action_return_to_main(NULL);
     }
 }
 
 /**
  * @brief Process input in the wallet details screen
  */
 void wallet_process_details_input(void) {
     // Options in details screen
     if (key_hit(KEY_A)) {
         // Show QR
         wallet_action_show_qr(NULL);
     } else if (key_hit(KEY_Y)) {
         // Edit wallet
         wallet_action_edit_wallet(NULL);
     } else if (key_hit(KEY_X)) {
         // Delete wallet
         wallet_action_delete_wallet(NULL);
     }
     
     // Cancel delete confirmation or return to list
     if (key_hit(KEY_B)) {
         if (g_confirm_delete) {
             g_confirm_delete = false;
             LOG_INFO(MODULE_WALLET, "Delete cancelled", 0);
         } else {
             // Return to list
             g_wallet_screen_state = WALLET_SCREEN_LIST;
         }
     }
 }
 
 /**
  * @brief Process input in the QR display screen
  */
 void wallet_process_qr_input(void) {
     // Return to details
     if (key_hit(KEY_A) || key_hit(KEY_B)) {
         g_wallet_screen_state = WALLET_SCREEN_DETAILS;
     }
 }
 
 /**
  * @brief Process input in the wallet edit screen
  */
 void wallet_process_edit_input(void) {
     WalletSystem* wallet = wallet_system_get_instance();
     
     // If text input is active, process it
     if (g_text_input_active) {
         process_text_input();
         return;
     }
     
     // Navigate between fields
     if (key_hit(KEY_UP)) {
         g_edit_current_field = (g_edit_current_field - 1 + 6) % 6;
     } else if (key_hit(KEY_DOWN)) {
         g_edit_current_field = (g_edit_current_field + 1) % 6;
     }
     
     // Edit selected field
     if (key_hit(KEY_A)) {
         switch (g_edit_current_field) {
             case 0: // Name
                 strcpy(g_text_input_buffer, g_edit_wallet_entry.name);
                 g_text_input_cursor = strlen(g_text_input_buffer);
                 g_text_input_field = 0;
                 g_text_input_active = true;
                 break;
                 
             case 1: // Address
                 strcpy(g_text_input_buffer, g_edit_wallet_entry.address);
                 g_text_input_cursor = strlen(g_text_input_buffer);
                 g_text_input_field = 1;
                 g_text_input_active = true;
                 break;
                 
             case 2: // Type
                 // Change cryptocurrency type
                 // Find next active type
                 int current_type = g_edit_wallet_entry.type_index;
                 int next_type = (current_type + 1) % MAX_CRYPTO_TYPES;
                 
                 // Find next active type
                 while (next_type != current_type) {
                     if (crypto_get_type_info(next_type)) {
                         g_edit_wallet_entry.type_index = next_type;
                         break;
                     }
                     next_type = (next_type + 1) % MAX_CRYPTO_TYPES;
                 }
                 break;
                 
             case 3: // Notes
                 strcpy(g_text_input_buffer, g_edit_wallet_entry.notes);
                 g_text_input_cursor = strlen(g_text_input_buffer);
                 g_text_input_field = 2;
                 g_text_input_active = true;
                 break;
                 
             case 4: // Tags
                 strcpy(g_text_input_buffer, g_edit_wallet_entry.tags);
                 g_text_input_cursor = strlen(g_text_input_buffer);
                 g_text_input_field = 3;
                 g_text_input_active = true;
                 break;
                 
             case 5: // Favorite
                 g_edit_wallet_entry.favorite = !g_edit_wallet_entry.favorite;
                 break;
         }
     }
     
     // Save changes
     if (key_hit(KEY_START)) {
         // Validate address for the selected type
         if (!crypto_validate_address(g_edit_wallet_entry.address, g_edit_wallet_entry.type_index)) {
             LOG_ERROR(MODULE_WALLET, "Invalid address", g_edit_wallet_entry.type_index);
             return;
         }
         
         // Update timestamp
         extern u32 get_system_ticks();
         g_edit_wallet_entry.last_used = get_system_ticks();
         
         // Save wallet
         if (g_edit_is_new_entry) {
             wallet_add_entry(&g_edit_wallet_entry);
             LOG_INFO(MODULE_WALLET, "New wallet added", wallet->count - 1);
         } else {
             wallet_update_entry(wallet->selected_index, &g_edit_wallet_entry);
             LOG_INFO(MODULE_WALLET, "Wallet updated", wallet->selected_index);
         }
         
         wallet_system_save();
         g_wallet_screen_state = WALLET_SCREEN_LIST;
     }
     
     // Cancel editing
     if (key_hit(KEY_B)) {
         g_wallet_screen_state = g_edit_is_new_entry ? WALLET_SCREEN_LIST : WALLET_SCREEN_DETAILS;
     }
 }
 
 /**
  * @brief Process input in the settings screen
  */
 void wallet_process_settings_input(void) {
     WalletSystem* wallet = wallet_system_get_instance();
     
     // Navigation between options
     static int settings_option = 0;
     if (key_hit(KEY_UP)) {
         settings_option = (settings_option - 1 + 3) % 3;
     } else if (key_hit(KEY_DOWN)) {
         settings_option = (settings_option + 1) % 3;
     }
     
     // Change options
     if (key_hit(KEY_A)) {
         switch (settings_option) {
             case 0: // Enable/disable encryption
                 if (wallet->is_encrypted) {
                     wallet_decrypt_data();
                 } else {
                     wallet_encrypt_data();
                 }
                 wallet_system_save();
                 LOG_INFO(MODULE_WALLET, "Encryption toggled", wallet->is_encrypted);
                 break;
                 
             case 1: // Change password
                 // In a real implementation, this would open a password dialog
                 wallet_set_password("1234");
                 wallet_system_save();
                 LOG_INFO(MODULE_WALLET, "Password changed", 0);
                 break;
                 
             case 2: // Reset filters
                 wallet->show_favorites_only = false;
                 wallet->active_crypto_filter = CRYPTO_TYPE_COUNT;
                 LOG_INFO(MODULE_WALLET, "Filters reset", 0);
                 break;
         }
     }
     
     // Return to menu
     if (key_hit(KEY_B)) {
         g_wallet_screen_state = WALLET_SCREEN_LIST;
     }
 }
 
 /**
  * @brief Process input in the filter screen
  */
 void wallet_process_filter_input(void) {
     WalletSystem* wallet = wallet_system_get_instance();
     
     // Navigation between options
     static int filter_option = 0;
     if (key_hit(KEY_UP)) {
         filter_option = (filter_option - 1 + 6) % 6;
     } else if (key_hit(KEY_DOWN)) {
         filter_option = (filter_option + 1) % 6;
     }
     
     // Change filters
     if (key_hit(KEY_A)) {
         switch (filter_option) {
             case 0: // All
                 wallet->show_favorites_only = false;
                 wallet->active_crypto_filter = CRYPTO_TYPE_COUNT;
                 LOG_INFO(MODULE_WALLET, "Filter: All", 0);
                 break;
                 
             case 1: // Favorites
                 wallet->show_favorites_only = !wallet->show_favorites_only;
                 LOG_INFO(MODULE_WALLET, "Filter: Favorites", wallet->show_favorites_only);
                 break;
                 
             case 2: // Bitcoin
                 wallet->active_crypto_filter = (wallet->active_crypto_filter == CRYPTO_TYPE_BITCOIN) ? 
                                                CRYPTO_TYPE_COUNT : CRYPTO_TYPE_BITCOIN;
                 LOG_INFO(MODULE_WALLET, "Filter: Bitcoin", wallet->active_crypto_filter == CRYPTO_TYPE_BITCOIN);
                 break;
                 
             case 3: // Ethereum
                 wallet->active_crypto_filter = (wallet->active_crypto_filter == CRYPTO_TYPE_ETHEREUM) ? 
                                                CRYPTO_TYPE_COUNT : CRYPTO_TYPE_ETHEREUM;
                 LOG_INFO(MODULE_WALLET, "Filter: Ethereum", wallet->active_crypto_filter == CRYPTO_TYPE_ETHEREUM);
                 break;
                 
             case 4: // Litecoin
                 wallet->active_crypto_filter = (wallet->active_crypto_filter == CRYPTO_TYPE_LITECOIN) ? 
                                                CRYPTO_TYPE_COUNT : CRYPTO_TYPE_LITECOIN;
                 LOG_INFO(MODULE_WALLET, "Filter: Litecoin", wallet->active_crypto_filter == CRYPTO_TYPE_LITECOIN);
                 break;
                 
             case 5: // Dogecoin
                 wallet->active_crypto_filter = (wallet->active_crypto_filter == CRYPTO_TYPE_DOGECOIN) ? 
                                                CRYPTO_TYPE_COUNT : CRYPTO_TYPE_DOGECOIN;
                 LOG_INFO(MODULE_WALLET, "Filter: Dogecoin", wallet->active_crypto_filter == CRYPTO_TYPE_DOGECOIN);
                 break;
         }
     }
     
     // Access crypto types management
     if (key_hit(KEY_Y)) {
         wallet_action_view_crypto_types(NULL);
     }
     
     // Return to wallet list
     if (key_hit(KEY_B)) {
         g_wallet_screen_state = WALLET_SCREEN_LIST;
     }
 }
 
 /**
  * @brief Render the wallet list screen
  */
 void wallet_render_list_screen(void) {
     WalletSystem* wallet = wallet_system_get_instance();
     
     // Clear screen
     tte_erase_screen();
     
     // Title
     tte_write_ex(10, 10, "WALLET LIST", RGB15(31,31,0));
     
     // Draw separator
     for (int i = 0; i < SCREEN_WIDTH; i++) {
         tte_plot(i, 20, RGB15(15,15,15));
     }
     
     // Filter information
     char filter_text[64] = "";
     if (wallet->show_favorites_only) {
         strcat(filter_text, "Favorites ");
     }
     if (wallet->active_crypto_filter < CRYPTO_TYPE_COUNT) {
         const char* symbol = wallet_get_crypto_symbol(wallet->active_crypto_filter);
         if (symbol) {
             strcat(filter_text, symbol);
         }
     }
     if (filter_text[0] != '\0') {
         tte_write_ex(160, 10, filter_text, RGB15(0,31,0));
     }
     
     // Check if there are any wallets
     if (wallet->count == 0) {
         tte_write_ex(10, 30, "No wallets saved.", RGB15(31,0,0));
         tte_write_ex(10, 50, "Select 'New Wallet' to create one.", RGB15(31,31,31));
         
         // Instructions
         tte_write_ex(5, 150, "START: New wallet  B: Return", RGB15(31,31,31));
         return;
     }
     
     // Get number of filtered wallets
     int filtered_count = wallet_get_filtered_count();
     if (filtered_count == 0) {
         tte_write_ex(10, 30, "No wallets match current filters.", RGB15(31,0,0));
         tte_write_ex(10, 50, "Change filters or add new wallets.", RGB15(31,31,31));
         
         // Instructions
         tte_write_ex(5, 150, "START: New wallet  B: Return", RGB15(31,31,31));
         return;
     }
     
     // Display wallet list
     int y = 30;
     int count = 0;
     
     for (int i = 0; i < wallet->count && count < 8; i++) {
         // Apply current filters
         if (wallet->show_favorites_only && !wallet->entries[i].favorite) {
             continue;
         }
         
         if (wallet->active_crypto_filter < CRYPTO_TYPE_COUNT && 
             wallet->entries[i].type_index != wallet->active_crypto_filter) {
             continue;
         }
         
         // Color selection based on whether this entry is selected
         u16 color = (i == wallet->selected_index) ? RGB15(31,31,0) : RGB15(31,31,31);
         
         // Prepare wallet text
         char wallet_text[64];
         
         // Get crypto type info
         const CryptoTypeInfo* type_info = crypto_get_type_info(wallet->entries[i].type_index);
         
         if (type_info) {
             sprintf(wallet_text, "%s [%s]", 
                     wallet->entries[i].name, 
                     type_info->symbol);
         } else {
             sprintf(wallet_text, "%s [???]", 
                     wallet->entries[i].name);
         }
         
         // Show favorite marker
         if (wallet->entries[i].favorite) {
             tte_write_ex(5, y, "★", RGB15(31,31,0));
         }
         
         // Show selection marker
         if (i == wallet->selected_index) {
             tte_write_ex(10, y, ">", RGB15(0,31,0));
         }
         
         // Show wallet text
         tte_write_ex(20, y, wallet_text, color);
         
         y += 15;
         count++;
     }
     
     // Instructions
     tte_write_ex(5, 150, "A: View  START: New  B: Return", RGB15(31,31,31));
 }
 
 /**
  * @brief Render the wallet details screen
  */
 void wallet_render_details_screen(void) {
     WalletSystem* wallet = wallet_system_get_instance();
     
     // Ensure valid selection
     if (wallet->selected_index < 0 || wallet->selected_index >= wallet->count) {
         g_wallet_screen_state = WALLET_SCREEN_LIST;
         return;
     }
     
     WalletEntry* entry = &wallet->entries[wallet->selected_index];
     
     // Clear screen
     tte_erase_screen();
     
     // Title
     tte_write_ex(10, 10, "WALLET DETAILS", RGB15(31,31,0));
     
     // Draw separator
     for (int i = 0; i < SCREEN_WIDTH; i++) {
         tte_plot(i, 20, RGB15(15,15,15));
     }
     
     // Get crypto type info
     const CryptoTypeInfo* type_info = crypto_get_type_info(entry->type_index);
     
     // Display wallet details
     int y = 30;
     
     // Name
     tte_write_ex(10, y, "Name:", RGB15(31,31,31));
     tte_write_ex(80, y, entry->name, RGB15(31,31,0));
     y += 15;
     
     // Cryptocurrency
     tte_write_ex(10, y, "Crypto:", RGB15(31,31,31));
     if (type_info) {
         tte_write_ex(80, y, type_info->name, RGB15(0,31,31));
     } else {
         tte_write_ex(80, y, "Unknown", RGB15(31,0,0));
     }
     y += 15;
     
     // Address (truncated if necessary)
     tte_write_ex(10, y, "Address:", RGB15(31,31,31));
     
     char truncated_address[32];
     if (strlen(entry->address) > 28) {
         // Show truncated address with ellipsis
         strncpy(truncated_address, entry->address, 25);
         truncated_address[25] = '\0';
         strcat(truncated_address, "...");
         tte_write_ex(80, y, truncated_address, RGB15(31,31,31));
     } else {
         tte_write_ex(80, y, entry->address, RGB15(31,31,31));
     }
     y += 15;
     
     // Notes (if present)
     if (entry->notes[0] != '\0') {
         tte_write_ex(10, y, "Notes:", RGB15(31,31,31));
         y += 12;
         tte_write_ex(15, y, entry->notes, RGB15(20,20,31));
         y += 20;
     } else {
         y += 12;
     }
     
     // Tags (if present)
     if (entry->tags[0] != '\0') {
         tte_write_ex(10, y, "Tags:", RGB15(31,31,31));
         tte_write_ex(80, y, entry->tags, RGB15(0,31,0));
         y += 15;
     }
     
     // Favorite status
     if (entry->favorite) {
         tte_write_ex(10, y, "Favorite:", RGB15(31,31,31));
         tte_write_ex(80, y, "Yes ★", RGB15(31,31,0));
     }
     
     // Delete confirmation if active
     if (g_confirm_delete) {
         draw_simple_frame(20, 90, 200, 50, RGB15(31,0,0));
         tte_write_ex(30, 100, "Delete this wallet?", RGB15(31,31,31));
         tte_write_ex(30, 120, "A: Yes  B: No", RGB15(31,0,0));
     } else {
         // Standard instructions
         tte_write_ex(5, 150, "A: QR  Y: Edit  X: Delete  B: Back", RGB15(31,31,31));
     }
 }
 
 /**
  * @brief Render the QR code screen
  */
 void wallet_render_qr_screen(void) {
     WalletSystem* wallet = wallet_system_get_instance();
     
     // Ensure valid selection
     if (wallet->selected_index < 0 || wallet->selected_index >= wallet->count) {
         g_wallet_screen_state = WALLET_SCREEN_LIST;
         return;
     }
     
     WalletEntry* entry = &wallet->entries[wallet->selected_index];
     
     // Clear screen
     tte_erase_screen();
     
     // Title
     tte_write_ex(10, 10, "QR CODE", RGB15(31,31,0));
     
     // Draw separator
     for (int i = 0; i < SCREEN_WIDTH; i++) {
         tte_plot(i, 20, RGB15(15,15,15));
     }
     
     // Display wallet name and crypto
     char title[64];
     const CryptoTypeInfo* type_info = crypto_get_type_info(entry->type_index);
     
     if (type_info) {
         sprintf(title, "%s (%s)", entry->name, type_info->symbol);
     } else {
         sprintf(title, "%s", entry->name);
     }
     
     tte_write_ex(120 - strlen(title) * 3, 25, title, RGB15(31,31,31));
     
     // Render QR code
     int qr_size = 21 * 2; // Default QR size x scale factor
     int x = (SCREEN_WIDTH - qr_size) / 2;
     int y = 40;
     
     // Add white border around QR code
     for (int i = x - 4; i < x + qr_size + 4; i++) {
         for (int j = y - 4; j < y + qr_size + 4; j++) {
             if (i >= 0 && i < SCREEN_WIDTH && j >= 0 && j < SCREEN_HEIGHT) {
                 m3_plot(i, j, CLR_WHITE);
             }
         }
     }
     
     // Render the QR code using the (potentially protected) function
     if (!wallet_render_qr_function(x, y, 2)) {
         tte_write_ex(60, 80, "Failed to render QR code", RGB15(31,0,0));
     }
     
     // Instructions
     tte_write_ex(40, 150, "A/B: Return to Details", RGB15(31,31,31));
 }
 
 /**
  * @brief Render the wallet edit screen
  */
 void wallet_render_edit_screen(void) {
     // Clear screen
     tte_erase_screen();
     
     // Title
     if (g_edit_is_new_entry) {
         tte_write_ex(10, 10, "NEW WALLET", RGB15(31,31,0));
     } else {
         tte_write_ex(10, 10, "EDIT WALLET", RGB15(31,31,0));
     }
     
     // Draw separator
     for (int i = 0; i < SCREEN_WIDTH; i++) {
         tte_plot(i, 20, RGB15(15,15,15));
     }
     
     // Display fields
     int y = 30;
     
     // Name field
     tte_write_ex(10, y, "Name:", RGB15(31,31,31));
     if (g_edit_current_field == 0) {
         draw_simple_frame(80, y - 2, 150, 12, RGB15(0,31,0));
     }
     tte_write_ex(85, y, g_edit_wallet_entry.name, RGB15(31,31,31));
     y += 20;
     
     // Address field
     tte_write_ex(10, y, "Address:", RGB15(31,31,31));
     if (g_edit_current_field == 1) {
         draw_simple_frame(80, y - 2, 150, 12, RGB15(0,31,0));
     }
     tte_write_ex(85, y, g_edit_wallet_entry.address, RGB15(31,31,31));
     y += 20;
     
     // Crypto type field
     tte_write_ex(10, y, "Type:", RGB15(31,31,31));
     if (g_edit_current_field == 2) {
         draw_simple_frame(80, y - 2, 150, 12, RGB15(0,31,0));
     }
     
     const CryptoTypeInfo* type_info = crypto_get_type_info(g_edit_wallet_entry.type_index);
     if (type_info) {
         tte_write_ex(85, y, type_info->name, RGB15(0,31,31));
     } else {
         tte_write_ex(85, y, "Unknown", RGB15(31,0,0));
     }
     y += 20;
     
     // Notes field
     tte_write_ex(10, y, "Notes:", RGB15(31,31,31));
     if (g_edit_current_field == 3) {
         draw_simple_frame(80, y - 2, 150, 12, RGB15(0,31,0));
     }
     tte_write_ex(85, y, g_edit_wallet_entry.notes, RGB15(31,31,31));
     y += 20;
     
     // Tags field
     tte_write_ex(10, y, "Tags:", RGB15(31,31,31));
     if (g_edit_current_field == 4) {
         draw_simple_frame(80, y - 2, 150, 12, RGB15(0,31,0));
     }
     tte_write_ex(85, y, g_edit_wallet_entry.tags, RGB15(31,31,31));
     y += 20;
     
     // Favorite field
     tte_write_ex(10, y, "Favorite:", RGB15(31,31,31));
     if (g_edit_current_field == 5) {
         draw_simple_frame(80, y - 2, 150, 12, RGB15(0,31,0));
     }
     tte_write_ex(85, y, g_edit_wallet_entry.favorite ? "Yes ★" : "No", RGB15(31,31,31));
     
     // Text input overlay (if active)
     if (g_text_input_active) {
         // Draw text input area
         draw_simple_frame(10, 130, 220, 20, RGB15(0,0,31));
         
         // Show current text
         tte_write_ex(15, 135, g_text_input_buffer, RGB15(31,31,31));
         
         // Draw cursor
         static int blink = 0;
         blink = (blink + 1) % 30;
         if (blink < 15) {
             int cursor_x = 15 + g_text_input_cursor * 6;
             for (int i = 0; i < 8; i++) {
                 tte_plot(cursor_x, 135 + i, RGB15(31,31,31));
             }
         }
         
         // Text input instructions
         tte_write_ex(5, 155, "A/B:Type  R+A/B:Caps  SELECT:Space  START:Done", RGB15(20,20,31));
     } else {
         // Standard instructions
         tte_write_ex(5, 150, "A:Edit  START:Save  B:Cancel", RGB15(31,31,31));
     }
 }
 
 /**
  * @brief Render the settings screen
  */
 void wallet_render_settings_screen(void) {
     WalletSystem* wallet = wallet_system_get_instance();
     
     // Clear screen
     tte_erase_screen();
     
     // Title
     tte_write_ex(10, 10, "SETTINGS", RGB15(31,31,0));
     
     // Draw separator
     for (int i = 0; i < SCREEN_WIDTH; i++) {
         tte_plot(i, 20, RGB15(15,15,15));
     }
     
     // Options
     static int settings_option = 0;
     
     // Encryption
     int y = 40;
     u16 color = (settings_option == 0) ? RGB15(31,31,0) : RGB15(31,31,31);
     
     tte_write_ex(10, y, "Encryption:", RGB15(31,31,31));
     tte_write_ex(100, y, wallet->is_encrypted ? "Enabled" : "Disabled", color);
     
     if (settings_option == 0) {
         tte_write_ex(5, y, ">", RGB15(0,31,0));
     }
     y += 25;
     
     // Password
     color = (settings_option == 1) ? RGB15(31,31,0) : RGB15(31,31,31);
     
     tte_write_ex(10, y, "Change Password", color);
     
     if (settings_option == 1) {
         tte_write_ex(5, y, ">", RGB15(0,31,0));
     }
     y += 25;
     
     // Reset filters
     color = (settings_option == 2) ? RGB15(31,31,0) : RGB15(31,31,31);
     
     tte_write_ex(10, y, "Reset All Filters", color);
     
     if (settings_option == 2) {
         tte_write_ex(5, y, ">", RGB15(0,31,0));
     }
     
     // Instructions
     tte_write_ex(5, 150, "A:Select  B:Return", RGB15(31,31,31));
 }
 
 /**
  * @brief Render the filter screen
  */
 void wallet_render_filter_screen(void) {
     WalletSystem* wallet = wallet_system_get_instance();
     
     // Clear screen
     tte_erase_screen();
     
     // Title
     tte_write_ex(10, 10, "FILTER WALLETS", RGB15(31,31,0));
     
     // Draw separator
     for (int i = 0; i < SCREEN_WIDTH; i++) {
         tte_plot(i, 20, RGB15(15,15,15));
     }
     
     // Options
     static int filter_option = 0;
     int y = 40;
     u16 color;
     
     // All wallets
     color = (filter_option == 0) ? RGB15(31,31,0) : RGB15(31,31,31);
     tte_write_ex(20, y, "All Wallets", color);
     
     if (filter_option == 0) {
         tte_write_ex(10, y, ">", RGB15(0,31,0));
     }
     
     if (wallet->active_crypto_filter == CRYPTO_TYPE_COUNT && !wallet->show_favorites_only) {
         tte_write_ex(150, y, "[Active]", RGB15(0,31,0));
     }
     y += 20;
     
     // Favorites only
     color = (filter_option == 1) ? RGB15(31,31,0) : RGB15(31,31,31);
     tte_write_ex(20, y, "Favorites Only", color);
     
     if (filter_option == 1) {
         tte_write_ex(10, y, ">", RGB15(0,31,0));
     }
     
     if (wallet->show_favorites_only) {
         tte_write_ex(150, y, "[Active]", RGB15(0,31,0));
     }
     y += 20;
     
     // Bitcoin
     color = (filter_option == 2) ? RGB15(31,31,0) : RGB15(31,31,31);
     tte_write_ex(20, y, "Bitcoin (BTC)", color);
     
     if (filter_option == 2) {
         tte_write_ex(10, y, ">", RGB15(0,31,0));
     }
     
     if (wallet->active_crypto_filter == CRYPTO_TYPE_BITCOIN) {
         tte_write_ex(150, y, "[Active]", RGB15(0,31,0));
     }
     y += 20;
     
     // Ethereum
     color = (filter_option == 3) ? RGB15(31,31,0) : RGB15(31,31,31);
     tte_write_ex(20, y, "Ethereum (ETH)", color);
     
     if (filter_option == 3) {
         tte_write_ex(10, y, ">", RGB15(0,31,0));
     }
     
     if (wallet->active_crypto_filter == CRYPTO_TYPE_ETHEREUM) {
         tte_write_ex(150, y, "[Active]", RGB15(0,31,0));
     }
     y += 20;
     
     // Litecoin
     color = (filter_option == 4) ? RGB15(31,31,0) : RGB15(31,31,31);
     tte_write_ex(20, y, "Litecoin (LTC)", color);
     
     if (filter_option == 4) {
         tte_write_ex(10, y, ">", RGB15(0,31,0));
     }
     
     if (wallet->active_crypto_filter == CRYPTO_TYPE_LITECOIN) {
         tte_write_ex(150, y, "[Active]", RGB15(0,31,0));
     }
     y += 20;
     
     // Dogecoin
     color = (filter_option == 5) ? RGB15(31,31,0) : RGB15(31,31,31);
     tte_write_ex(20, y, "Dogecoin (DOGE)", color);
     
     if (filter_option == 5) {
         tte_write_ex(10, y, ">", RGB15(0,31,0));
     }
     
     if (wallet->active_crypto_filter == CRYPTO_TYPE_DOGECOIN) {
         tte_write_ex(150, y, "[Active]", RGB15(0,31,0));
     }
     
     // Instructions
     tte_write_ex(5, 150, "A:Toggle  Y:Manage Types  B:Return", RGB15(31,31,31));
 }
 
 /**
  * @brief Update the wallet menu
  */
 void wallet_menu_update(void) {
     // Process input based on current screen
     switch (g_wallet_screen_state) {
         case WALLET_SCREEN_LIST:
             wallet_process_list_input();
             break;
             
         case WALLET_SCREEN_DETAILS:
             wallet_process_details_input();
             break;
             
         case WALLET_SCREEN_QR:
             wallet_process_qr_input();
             break;
             
         case WALLET_SCREEN_EDIT:
         case WALLET_SCREEN_NEW:
             wallet_process_edit_input();
             break;
             
         case WALLET_SCREEN_SETTINGS:
             wallet_process_settings_input();
             break;
             
         case WALLET_SCREEN_FILTER:
             wallet_process_filter_input();
             break;
             
         case WALLET_SCREEN_CRYPTO_TYPES:
             wallet_crypto_types_menu_update();
             break;
             
         case WALLET_SCREEN_QR_PROTECTION:
             // Handled by the QR protection menu
             break;
             
         default:
             break;
     }
 }
 
 /**
  * @brief Render the wallet menu
  */
 void wallet_menu_render(void) {
     // Render based on current screen
     switch (g_wallet_screen_state) {
         case WALLET_SCREEN_LIST:
             wallet_render_list_screen();
             break;
             
         case WALLET_SCREEN_DETAILS:
             wallet_render_details_screen();
             break;
             
         case WALLET_SCREEN_QR:
             wallet_render_qr_screen();
             break;
             
         case WALLET_SCREEN_EDIT:
         case WALLET_SCREEN_NEW:
             wallet_render_edit_screen();
             break;
             
         case WALLET_SCREEN_SETTINGS:
             wallet_render_settings_screen();
             break;
             
         case WALLET_SCREEN_FILTER:
             wallet_render_filter_screen();
             break;
             
         case WALLET_SCREEN_CRYPTO_TYPES:
             wallet_crypto_types_menu_render();
             break;
             
         case WALLET_SCREEN_QR_PROTECTION:
             // Handled by the QR protection system
             break;
             
         default:
             break;
     }
 }
 
 /**
  * @brief Render current QR code for wallet
  * 
  * This function can be replaced by the QR protection system.
  * 
  * @param x X position on screen
  * @param y Y position on screen
  * @param scale Scaling factor
  * @return true if successful, false otherwise
  */
 bool wallet_render_current_qr(int x, int y, int scale) {
     WalletSystem* wallet = wallet_system_get_instance();
     if (!wallet || wallet->selected_index < 0 || wallet->selected_index >= wallet->count) {
         return false;
     }
     
     // Render the QR code
     return render_qr_to_screen(&wallet->qr_state, x, y, scale);
 }