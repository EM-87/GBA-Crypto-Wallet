/**
 * @file wallet_menu.h
 * @brief User interface for cryptocurrency wallet management
 * 
 * This header defines the wallet menu system that allows users to
 * view, create, edit, and manage cryptocurrency wallet entries.
 * 
 * @author Claude
 * @date March 2025
 * @version 1.0.0
 */

 #ifndef WALLET_MENU_H
 #define WALLET_MENU_H
 
 #include <tonc.h>
 #include "menu_system.h"
 #include "wallet_system.h"
 
 /**
  * Wallet screen states
  * Different screens in the wallet menu system
  */
 typedef enum {
     WALLET_SCREEN_LIST = 0,      // List of wallet entries
     WALLET_SCREEN_DETAILS,       // Details of selected entry
     WALLET_SCREEN_QR,            // QR code display
     WALLET_SCREEN_EDIT,          // Edit entry
     WALLET_SCREEN_NEW,           // Create new entry
     WALLET_SCREEN_SETTINGS,      // Wallet settings
     WALLET_SCREEN_FILTER,        // Filter entries
     WALLET_SCREEN_CRYPTO_TYPES,  // Manage crypto types
     WALLET_SCREEN_QR_PROTECTION  // QR protection settings
 } WalletScreenState;
 
 /**
  * Initialize the wallet menu system
  */
 void wallet_menu_init(void);
 
 /**
  * Update wallet menu logic
  * Called every frame
  */
 void wallet_menu_update(void);
 
 /**
  * Render wallet menu
  * Called every frame
  */
 void wallet_menu_render(void);
 
 /**
  * Enhanced wallet menu update with QR protection
  */
 void enhanced_wallet_menu_update(void);
 
 /**
  * Enhanced wallet menu render with QR protection
  */
 void enhanced_wallet_menu_render(void);
 
 /**
  * Menu callbacks
  */
 
 /**
  * View wallet list
  */
 void wallet_action_view_list(void* user_data);
 
 /**
  * Create new wallet
  */
 void wallet_action_new_wallet(void* user_data);
 
 /**
  * View wallet details
  */
 void wallet_action_view_details(void* user_data);
 
 /**
  * Edit wallet
  */
 void wallet_action_edit_wallet(void* user_data);
 
 /**
  * Delete wallet
  */
 void wallet_action_delete_wallet(void* user_data);
 
 /**
  * Show QR code
  */
 void wallet_action_show_qr(void* user_data);
 
 /**
  * Show filter screen
  */
 void wallet_action_filter(void* user_data);
 
 /**
  * Show settings screen
  */
 void wallet_action_settings(void* user_data);
 
 /**
  * Return to main menu
  */
 void wallet_action_return_to_main(void* user_data);
 
 /**
  * Input processing functions
  */
 
 /**
  * Process input on list screen
  */
 void wallet_process_list_input(void);
 
 /**
  * Process input on details screen
  */
 void wallet_process_details_input(void);
 
 /**
  * Process input on QR screen
  */
 void wallet_process_qr_input(void);
 
 /**
  * Process input on edit screen
  */
 void wallet_process_edit_input(void);
 
 /**
  * Process input on settings screen
  */
 void wallet_process_settings_input(void);
 
 /**
  * Process input on filter screen
  */
 void wallet_process_filter_input(void);
 
 /**
  * Process text input
  */
 void process_text_input(void);
 
 /**
  * Render functions
  */
 
 /**
  * Render wallet list screen
  */
 void wallet_render_list_screen(void);
 
 /**
  * Render wallet details screen
  */
 void wallet_render_details_screen(void);
 
 /**
  * Render QR code screen
  */
 void wallet_render_qr_screen(void);
 
 /**
  * Render wallet edit screen
  */
 void wallet_render_edit_screen(void);
 
 /**
  * Render wallet settings screen
  */
 void wallet_render_settings_screen(void);
 
 /**
  * Render filter screen
  */
 void wallet_render_filter_screen(void);
 
 /**
  * Draw a simple frame
  * @param x X position
  * @param y Y position
  * @param width Width
  * @param height Height
  * @param color Color
  */
 void draw_simple_frame(int x, int y, int width, int height, u16 color);
 
 /**
  * External references
  */
 extern MenuItem wallet_menu;
 extern WalletScreenState g_wallet_screen_state;
 extern char g_text_input_buffer[256];
 extern int g_text_input_cursor;
 extern int g_text_input_field;
 extern bool g_text_input_active;
 
 #endif // WALLET_MENU_H