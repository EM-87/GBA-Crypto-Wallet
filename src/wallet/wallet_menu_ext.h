/**
 * @file wallet_menu_ext.h
 * @brief Extension header for the wallet menu system
 * 
 * This file extends the wallet menu system with additional functionality:
 * - Cryptocurrency type management
 * - QR protection integration
 * - Extended wallet screen states
 * 
 * @author Claude
 * @date March 2025
 * @version 1.0.0
 */

 #ifndef WALLET_MENU_EXT_H
 #define WALLET_MENU_EXT_H
 
 #include "wallet_menu.h"
 #include "crypto_types.h"

 /**
  * Note: WalletScreenState is defined in wallet_menu.h
  * This header extends the wallet menu with additional functionality
  */

 /**
  * States for cryptocurrency type management screen
  */
 typedef enum {
     CRYPTO_TYPE_VIEW_LIST,          // View list of cryptocurrency types
     CRYPTO_TYPE_EDIT,               // Edit existing cryptocurrency type
     CRYPTO_TYPE_NEW,                // Create new cryptocurrency type
     CRYPTO_TYPE_DELETE_CONFIRM      // Confirm deletion of cryptocurrency type
 } CryptoTypeScreenState;
 
 /**
  * @brief Initialize cryptocurrency types menu
  */
 void wallet_crypto_types_menu_init(void);
 
 /**
  * @brief Update cryptocurrency types menu
  */
 void wallet_crypto_types_menu_update(void);
 
 /**
  * @brief Render cryptocurrency types menu
  */
 void wallet_crypto_types_menu_render(void);
 
 /**
  * Menu callback functions for cryptocurrency types
  */
 void wallet_action_view_crypto_types(void* user_data);
 void wallet_action_new_crypto_type(void* user_data);
 void wallet_action_edit_crypto_type(void* user_data);
 void wallet_action_delete_crypto_type(void* user_data);
 
 /**
  * Input processing for cryptocurrency type screens
  */
 void wallet_process_crypto_types_list_input(void);
 void wallet_process_crypto_type_edit_input(void);
 void wallet_process_crypto_type_delete_input(void);
 
 /**
  * QR protection integration functions
  */
 void enhanced_wallet_menu_update(void);
 void enhanced_wallet_menu_render(void);
 void qr_protection_integrate(void);
 bool wallet_apply_qr_protection(void);
 
 /**
  * Global state variables
  */
 extern CryptoTypeScreenState g_crypto_type_screen_state;
 extern int g_selected_crypto_type;
 extern CryptoTypeInfo g_edit_crypto_type;
 extern WalletScreenState g_wallet_screen_state;
 
 #endif // WALLET_MENU_EXT_H