/**
 * @file menu_definitions.c
 * @brief Menu definitions for GBA Cryptocurrency Wallet
 *
 * This file contains the definitions for main_menu and qr_menu
 * that are referenced in main.c
 *
 * @author Claude
 * @date March 2025
 * @version 1.0.0
 */

#include "menu_system.h"
#include "wallet_menu.h"

// Forward declarations
extern MenuItem wallet_menu;

// =====================================================================
// MAIN MENU
// =====================================================================

// Main menu options
MenuOption main_menu_options[] = {
    {
        .text = "Crypto Wallet",
        .type = MENU_ITEM_SUBMENU,
        .x = 60,
        .y = 50,
        .width = 200,
        .height = 16,
        .submenu = {
            .submenu = &wallet_menu
        }
    },
    {
        .text = "Settings",
        .type = MENU_ITEM_ACTION,
        .x = 60,
        .y = 70,
        .width = 200,
        .height = 16,
        .action = {
            .callback = menu_action_start_game,  // Placeholder
            .user_data = NULL
        }
    },
    {
        .text = "About",
        .type = MENU_ITEM_ACTION,
        .x = 60,
        .y = 90,
        .width = 200,
        .height = 16,
        .action = {
            .callback = menu_action_start_game,  // Placeholder
            .user_data = NULL
        }
    },
    {
        .text = "Exit",
        .type = MENU_ITEM_ACTION,
        .x = 60,
        .y = 110,
        .width = 200,
        .height = 16,
        .action = {
            .callback = menu_action_exit,
            .user_data = NULL
        }
    }
};

// Main menu definition
MenuItem main_menu = {
    .title = "CRYPTO WALLET - MAIN MENU",
    .options = main_menu_options,
    .num_options = 4,
    .help_text = "A: Select   B: Back",
    .parent = NULL
};

// =====================================================================
// QR MENU
// =====================================================================

// QR menu state
static bool qr_menu_initialized = false;

// QR menu options (minimal, as QR menu is mostly handled separately)
MenuOption qr_menu_options[] = {
    {
        .text = "Return to Wallet",
        .type = MENU_ITEM_ACTION,
        .x = 60,
        .y = 130,
        .width = 200,
        .height = 16,
        .action = {
            .callback = NULL,  // Handled by qr_menu_update
            .user_data = NULL
        }
    }
};

// QR menu definition
MenuItem qr_menu = {
    .title = "QR CODE DISPLAY",
    .options = qr_menu_options,
    .num_options = 1,
    .help_text = "A/B: Return",
    .parent = &wallet_menu
};

// =====================================================================
// QR MENU UPDATE AND RENDER FUNCTIONS
// =====================================================================

/**
 * @brief Update QR menu state
 */
void qr_menu_update(void) {
    // Handle QR menu-specific input
    key_poll();

    // Return to wallet menu on A or B button
    if (key_hit(KEY_A) || key_hit(KEY_B)) {
        MenuSystem* menu = menu_system_get_instance();
        menu_system_set_active_menu(menu, &wallet_menu);
    }
}

/**
 * @brief Render QR menu
 */
void qr_menu_render(void) {
    // QR rendering is handled by wallet_menu rendering
    // This function is called but most rendering is done elsewhere

    // Just ensure the screen is prepared
    if (!qr_menu_initialized) {
        qr_menu_initialized = true;
    }
}
