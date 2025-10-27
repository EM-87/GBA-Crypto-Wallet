/**
 * @file main.c
 * @brief Main entry point for GBA Cryptocurrency QR Code Generator
 * 
 * This file integrates all components of the system:
 * - Menu system
 * - QR generation and rendering
 * - Wallet management
 * - QR protection system
 * - Debug logging
 * 
 * @author Claude
 * @date March 2025
 * @version 1.0.0
 */

 #include <tonc.h>
 #include "menu_system.h"
 #include "qr_system.h"
 #include "qr_debug.h"
 #include "wallet_menu.h"
 #include "wallet_menu_ext.h"
 #include "qr_protection.h"
 
 // Global QR system state
 QrSystemState g_qr_state;
 
 // Buffer for OAM (Object Attribute Memory)
 OBJ_ATTR obj_buffer[128];
 
 // Forward declarations for internal functions
 void initialize_systems(void);
 void main_loop(void);
 
 /**
  * Main entry point
  */
 int main(void) {
     // Initialize all systems
     initialize_systems();
     
     // Enter main loop
     main_loop();
     
     // Never reached in practice
     return 0;
 }
 
 /**
  * Initialize all system components
  */
 void initialize_systems(void) {
     // Initialize interrupts
     irq_init(NULL);
     irq_add(II_VBLANK, NULL);
     
     // Initialize menu system
     MenuSystem* menu = menu_system_get_instance();
     menu_system_init(menu);
     
     // Initialize debug system
     debug_init();
     
     // Initialize QR system
     g_qr_state.refresh_rate = 30;
     g_qr_state.update_interval = 1;
     g_qr_state.qr_pixel_size = 2;
     g_qr_state.enable_animations = true;
     g_qr_state.frame_counter = 0;
     g_qr_state.auto_hide_timeout = 300;
     qr_init(&g_qr_state.qr_state);
     
     // Initialize wallet menu
     wallet_menu_init();
     
     // Initialize and integrate QR protection system
     qr_protection_integrate();
     
     // Set initial menu
     extern MenuItem main_menu;
     menu_system_set_active_menu(menu, &main_menu);
     
     LOG_INFO(MODULE_SYSTEM, "All systems initialized", 0);
 }
 
 /**
  * Main application loop
  */
 void main_loop(void) {
     // Get menu system instance
     MenuSystem* menu = menu_system_get_instance();
     
     // External references
     extern MenuItem qr_menu;
     extern MenuItem wallet_menu;
     
     // Main loop
     while (1) {
         // Wait for vertical retrace (synchronize with screen refresh)
         VBlankIntrWait();
         
         // Update global frame counter
         g_qr_state.frame_counter++;
         
         // Update debug counter
         debug_update_tick();
         
         // Read input state
         key_poll();
         
         // Update menu logic
         menu_system_update(menu);
         
         // If we're in the QR menu, update it
         if (menu->current_menu == &qr_menu) {
             qr_menu_update();
         }
         
         // If we're in the wallet menu, update it with enhancement
         if (menu->current_menu == &wallet_menu) {
             // Use enhanced update which includes QR protection
             enhanced_wallet_menu_update();
         }
         
         // Always update QR protection system
         qr_protection_update();
         
         // Render menu
         menu_system_render(menu);
         
         // If we're in the QR menu, render it on top
         if (menu->current_menu == &qr_menu) {
             qr_menu_render();
         }
         
         // If we're in the wallet menu, render it with enhancement
         if (menu->current_menu == &wallet_menu) {
             // Use enhanced render which includes QR protection
             enhanced_wallet_menu_render();
         }
         
         // Show debug log if enabled
         #ifdef DEBUG_ENABLE_LOG_DISPLAY
         debug_show_log(150, 0, LOG_WARNING);
         #endif
     }
 }
 
 /**
  * Callback for "Start Game" menu option
  */
 void menu_action_start_game(void* user_data) {
     (void)user_data; // Unused parameter
     
     LOG_INFO(MODULE_SYSTEM, "Start game selected", 0);
     
     // In a real implementation, this would transition to a game mode
     // For now, just log a message
 }
 
 /**
  * Callback for "Exit" menu option
  */
 void menu_action_exit(void* user_data) {
     (void)user_data; // Unused parameter
     
     LOG_INFO(MODULE_SYSTEM, "Exit selected", 0);
     
     // On GBA we can't really exit, but we can simulate it by showing a message
     // and then resetting the system
     
     // Show exit message
     tte_erase_screen();
     tte_write_ex(10, 70, "Exiting application...", RGB15(31,31,31));
     
     // Wait a moment
     for (int i = 0; i < 60; i++) {
         VBlankIntrWait();
     }
     
     // Reset system
     RegisterRamReset(RESET_ALL);
     
     // If reset fails, just loop
     while(1) {
         VBlankIntrWait();
     }
 }
 
 /**
  * Callback for volume setting change
  */
 void menu_option_volume_changed(int new_value) {
     LOG_INFO(MODULE_SYSTEM, "Volume changed", new_value);
     
     // In a real implementation, this would adjust the sound volume
 }
 
 /**
  * Callback for fullscreen setting change
  */
 void menu_option_fullscreen_changed(bool enabled) {
     LOG_INFO(MODULE_SYSTEM, "Fullscreen changed", enabled ? 1 : 0);
     
     // In a real implementation, this would adjust the display mode
 }