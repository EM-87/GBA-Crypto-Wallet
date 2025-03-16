/**
 * @file qr_protection_menu.h
 * @brief Menu system for QR protection settings
 *
 * This file implements the menu interface for adjusting QR protection
 * settings to prevent photography while allowing real-time scanning.
 *
 * @author Claude
 * @date March 2025
 * @version 1.0.0
 */

 #ifndef QR_PROTECTION_MENU_H
 #define QR_PROTECTION_MENU_H
 
 #include <tonc.h>
 #include "menu_system.h"
 #include "qr_protection.h"
 
 /**
  * QR protection menu screen states
  */
 typedef enum {
     QR_PROT_MENU_MAIN,          // Main protection menu
     QR_PROT_MENU_PRESET,        // Preset selection
     QR_PROT_MENU_CUSTOM,        // Custom settings
     QR_PROT_MENU_HELP           // Help/explanation screen
 } QrProtectionMenuState;
 
 /**
  * @brief Initialize the protection menu
  *
  * Sets up the menu system, initializes default values, and links to parent menu
  */
 void qr_protection_menu_init(void);
 
 /**
  * @brief Update the protection menu
  *
  * Processes user input and updates menu state. Called every frame.
  */
 void qr_protection_menu_update(void);
 
 /**
  * @brief Render the protection menu
  *
  * Draws the current menu screen based on state. Called every frame.
  */
 void qr_protection_menu_render(void);
 
 /**
  * @brief Callback to open protection menu
  *
  * Called when user selects the QR protection option from parent menu
  *
  * @param user_data Optional user data (unused)
  */
 void menu_action_open_protection_menu(void* user_data);
 
 /**
  * @brief Callback to set protection preset
  *
  * Called when user selects the "Protection Level" option
  *
  * @param user_data Optional user data (unused)
  */
 void menu_action_set_protection_preset(void* user_data);
 
 /**
  * @brief Callback to customize protection settings
  *
  * Called when user selects the "Custom Settings" option
  *
  * @param user_data Optional user data (unused)
  */
 void menu_action_customize_protection(void* user_data);
 
 /**
  * @brief Callback to show protection help screen
  *
  * Called when user selects the "Help" option
  *
  * @param user_data Optional user data (unused)
  */
 void menu_action_show_protection_help(void* user_data);
 
 /**
  * @brief Callback to return from protection menu
  *
  * Called when user selects the "Return to Menu" option
  *
  * @param user_data Optional user data (unused)
  */
 void menu_action_return_from_protection(void* user_data);
 
 /**
  * External menu item reference for integration with main menu
  */
 extern MenuItem qr_protection_menu;
 
 #endif // QR_PROTECTION_MENU_H