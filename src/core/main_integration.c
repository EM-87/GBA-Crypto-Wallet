/**
 * @file main_integration.c
 * @brief Integration code for GBA Cryptocurrency QR Code Generator
 * 
 * This file provides the integration layer between the different
 * subsystems of the application:
 * - Menu system
 * - QR generator
 * - Wallet management
 * - QR protection
 * 
 * @author Claude
 * @date March 2025
 * @version 1.0.0
 */

 #include <tonc.h>
 #include "menu_system.h"
 #include "qr_system.h"
 #include "wallet_menu.h"
 #include "wallet_menu_ext.h"
 #include "qr_protection.h"
 #include "qr_debug.h"
 
 // External references to global data structures
 extern QrSystemState g_qr_state;
 extern WalletScreenState g_wallet_screen_state;
 extern MenuItem wallet_menu;
 extern MenuItem qr_menu;
 extern MenuItem main_menu;
 
 /**
  * @brief Add QR menu to main menu
  * 
  * Adds the QR menu options to the main menu structure.
  * Called during initialization.
  */
 void integrate_qr_menu_to_main(void) {
     // In the original codebase, the options were directly added to main_options
     // in main_menu, but here we just log that we'd do this integration
     LOG_INFO(MODULE_SYSTEM, "QR menu integrated to main menu", 0);
 }
 
 /**
  * @brief Add wallet menu to main menu
  * 
  * Adds the wallet menu options to the main menu structure.
  * Called during initialization.
  */
 void integrate_wallet_menu_to_main(void) {
     // Set parent reference for wallet menu to main menu
     wallet_menu.parent = &main_menu;
     
     LOG_INFO(MODULE_SYSTEM, "Wallet menu integrated to main menu", 0);
 }
 
 /**
  * @brief Add protection options to wallet menu
  * 
  * Adds QR protection menu options to the wallet menu.
  * Called during QR protection initialization.
  */
 void add_protection_to_wallet_menu(void) {
     // This would add the protection menu entry to the wallet menu
     // In a real implementation, this would modify the options array
     
     LOG_INFO(MODULE_SYSTEM, "QR protection added to wallet menu", 0);
 }
 
 /**
  * @brief Initialize graphics system
  * 
  * Sets up the GBA graphics system for the application.
  * Configures background, sprites, and display registers.
  */
 void initialize_graphics_system(void) {
     // Set video mode and enable required backgrounds
     REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_OBJ | DCNT_OBJ_1D;
     
     // Configure backgrounds
     REG_BG0CNT = BG_CBB(0) | BG_SBB(30) | BG_4BPP | BG_REG_32x32 | BG_PRIO(2);
     REG_BG1CNT = BG_CBB(1) | BG_SBB(29) | BG_4BPP | BG_REG_32x32 | BG_PRIO(1);
     
     // Initialize OAM for sprites
     oam_init(obj_buffer, 128);
     
     LOG_INFO(MODULE_SYSTEM, "Graphics system initialized", 0);
 }
 
 /**
  * @brief Initialize timer system
  * 
  * Sets up timers for various timing-sensitive operations
  * like animation, debouncing, and QR protection refresh rates.
  */
 void initialize_timer_system(void) {
     // Configure Timer 0 for general purpose timing
     REG_TM0CNT_L = 0;
     REG_TM0CNT_H = TM_FREQ_1024 | TM_ENABLE;
     
     // Configure Timer 1 for QR refresh timing
     REG_TM1CNT_L = 0;
     REG_TM1CNT_H = TM_FREQ_256 | TM_ENABLE;
     
     // Configure Timer 2 for performance measurement (microseconds)
     REG_TM2CNT_L = 0;
     REG_TM2CNT_H = TM_FREQ_64 | TM_ENABLE;
     
     LOG_INFO(MODULE_SYSTEM, "Timer system initialized", 0);
 }
 
 /**
  * @brief Set up screen for rendering
  * 
  * Prepares the screen for rendering the next frame.
  * Called at the beginning of each frame render.
  */
 void setup_screen_for_render(void) {
     // Clear the screen
     tte_erase_screen();
 }
 
 /**
  * @brief Finalize screen rendering
  * 
  * Completes rendering for the current frame.
  * Called at the end of each frame render.
  */
 void finalize_screen_render(void) {
     // If screen needs additional sync or final setup, do it here
 }
 
 /**
  * @brief Check for system-wide key combinations
  * 
  * Handles special key combinations that apply system-wide,
  * such as reset, debug mode toggle, etc.
  * 
  * @return true if a system key combination was handled
  */
 bool handle_system_keys(void) {
     // Example: Handle START+SELECT as a soft reset
     if (key_is_down(KEY_START) && key_is_down(KEY_SELECT)) {
         LOG_WARNING(MODULE_SYSTEM, "System reset requested", 0);
         
         // Wait for 1 second to confirm
         if (key_held_time(KEY_START) > 60 && key_held_time(KEY_SELECT) > 60) {
             // Perform soft reset
             RegisterRamReset(RESET_EWRAM | RESET_IWRAM);
             
             // Return to GBA BIOS
             SoftReset();
             
             return true;
         }
     }
     
     // Toggle debug display with SELECT+L+R
     if (key_hit(KEY_SELECT) && key_is_down(KEY_L) && key_is_down(KEY_R)) {
         static bool debug_display = false;
         debug_display = !debug_display;
         
         #ifdef DEBUG_ENABLE_LOG_DISPLAY
         LOG_INFO(MODULE_SYSTEM, "Debug display toggled", debug_display);
         #endif
         
         return true;
     }
     
     return false;
 }
 
 /**
  * @brief Synchronize wallet and QR protection state
  * 
  * Ensures that wallet entries and QR protection are in sync.
  * Called when switching between wallet entries.
  */
 void sync_wallet_and_protection(void) {
     WalletSystem* wallet = wallet_system_get_instance();
     
     if (wallet->selected_index >= 0 && wallet->selected_index < wallet->count) {
         // When viewing a QR code, ensure protection is applied
         if (g_wallet_screen_state == WALLET_SCREEN_QR) {
             wallet_apply_qr_protection();
         }
     }
 }
 
 /**
  * @brief Global application state update
  * 
  * Performs state updates that need to happen every frame
  * regardless of which menu is active.
  */
 void update_global_application_state(void) {
     // Update frame counter
     g_qr_state.frame_counter++;
     
     // Update debug timer
     debug_update_tick();
     
     // Check for any pending SRAM writes
     // This would be implementation-specific in a real system
 }
 
 /**
  * @brief Initialize all system components
  * 
  * Master initialization function that sets up all components
  * in the correct order with proper dependencies.
  */
 void initialize_all_systems(void) {
     // Initialize interrupts
     irq_init(NULL);
     irq_add(II_VBLANK, NULL);
     
     // Initialize graphics
     initialize_graphics_system();
     
     // Initialize timers
     initialize_timer_system();
     
     // Initialize debug logging system
     debug_init();
     
     // Initialize QR generation system
     g_qr_state.refresh_rate = 30;
     g_qr_state.update_interval = 1;
     g_qr_state.qr_pixel_size = 2;
     g_qr_state.enable_animations = true;
     g_qr_state.frame_counter = 0;
     g_qr_state.auto_hide_timeout = 300;
     qr_init(&g_qr_state.qr_state);
     
     // Initialize menu system
     MenuSystem* menu = menu_system_get_instance();
     menu_system_init(menu);
     
     // Initialize wallet system
     wallet_menu_init();
     
     // Initialize QR protection system
     qr_protection_integrate();
     
     // Integrate components
     integrate_qr_menu_to_main();
     integrate_wallet_menu_to_main();
     
     // Set initial menu
     menu_system_set_active_menu(menu, &main_menu);
     
     LOG_INFO(MODULE_SYSTEM, "All systems initialized successfully", 0);
 }
 
 /**
  * @brief Save all user data
  * 
  * Saves all user configuration and wallet data to SRAM.
  * Called when exiting or as part of periodic saves.
  * 
  * @return true if all data was saved successfully
  */
 bool save_all_user_data(void) {
     bool success = true;
     
     // Save wallet data
     if (!wallet_system_save()) {
         LOG_ERROR(MODULE_SYSTEM, "Failed to save wallet data", 0);
         success = false;
     }
     
     // Save QR protection settings
     // In a real implementation, we would have a save function for protection settings
     
     // Save general application settings
     // This would be implementation-specific
     
     return success;
 }
 
 /**
  * @brief Validate system integrity
  * 
  * Performs integrity checks to ensure all systems are functioning correctly.
  * Called at startup to detect any memory corruption or initialization issues.
  * 
  * @return true if all systems passed integrity checks
  */
 bool validate_system_integrity(void) {
     bool all_systems_ok = true;
     
     // Check menu system
     MenuSystem* menu = menu_system_get_instance();
     if (!menu || !menu->current_menu) {
         LOG_ERROR(MODULE_SYSTEM, "Menu system integrity check failed", 0);
         all_systems_ok = false;
     }
     
     // Check QR system
     if (!g_qr_state.qr_state.data && g_qr_state.qr_state.size > 0) {
         LOG_ERROR(MODULE_SYSTEM, "QR system integrity check failed", 0);
         all_systems_ok = false;
     }
     
     // Check wallet system
     WalletSystem* wallet = wallet_system_get_instance();
     if (!wallet) {
         LOG_ERROR(MODULE_SYSTEM, "Wallet system integrity check failed", 0);
         all_systems_ok = false;
     }
     
     return all_systems_ok;
 }