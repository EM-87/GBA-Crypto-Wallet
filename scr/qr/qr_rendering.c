/**
 * @file qr_rendering.c
 * @brief QR code rendering implementation for GBA
 * 
 * This file provides the rendering functions for QR codes on the GBA screen.
 * It includes both direct pixel plotting and optimized tile-based rendering.
 * 
 * @author Claude
 * @date March 2025
 * @version 1.0.0
 */

 #include <tonc.h>
 #include "qr_system.h"
 #include "qr_debug.h"
 
 // Constants for QR rendering
 #define QR_MODULE_WHITE 0
 #define QR_MODULE_BLACK 1
 #define QR_PIXEL_SIZE   2  // Each QR module is 2x2 pixels by default
 
 /**
  * @brief Renders a QR code to the GBA screen
  * 
  * @param qr_state QR code state with pattern data
  * @param x Top-left x position for rendering
  * @param y Top-left y position for rendering
  * @param scale Scaling factor (1 = 1 pixel per module)
  * @return Success status
  */
 bool render_qr_to_screen(QrState *qr_state, int x, int y, int scale) {
     if (!qr_state || !qr_state->data) {
         LOG_ERROR(MODULE_RENDER, "Invalid QR state for rendering", 0);
         return false;
     }
     
     // Size checks - ensure QR code will fit on screen
     int qr_size = qr_state->size;
     int screen_size = qr_size * scale;
     
     if (x < 0 || y < 0 || 
         x + screen_size > SCREEN_WIDTH || 
         y + screen_size > SCREEN_HEIGHT) {
         LOG_ERROR(MODULE_RENDER, "QR code won't fit on screen", qr_size);
         return false;
     }
     
     // For direct mode 3 bitmap rendering
     for (int qr_y = 0; qr_y < qr_size; qr_y++) {
         for (int qr_x = 0; qr_x < qr_size; qr_x++) {
             // Get module value (0 = white, 1 = black)
             int module_value = qr_state->data[qr_y * qr_size + qr_x];
             
             // Set color based on module value
             u16 color = (module_value == QR_MODULE_BLACK) ? CLR_BLACK : CLR_WHITE;
             
             // Draw scaled module
             for (int py = 0; py < scale; py++) {
                 for (int px = 0; px < scale; px++) {
                     int screen_x = x + qr_x * scale + px;
                     int screen_y = y + qr_y * scale + py;
                     
                     // Place pixel - for Mode 3
                     m3_plot(screen_x, screen_y, color);
                 }
             }
         }
     }
     
     LOG_INFO(MODULE_RENDER, "QR rendered to screen", qr_size);
     return true;
 }
 
 /**
  * @brief Optimized QR rendering for GBA
  * 
  * Uses buffer-based rendering for better performance
  * 
  * @param qr_state QR code state
  * @param buffer Target buffer (can be NULL if rendering directly)
  * @return Success status
  */
 bool render_qr_optimized(QrState *qr_state, u16 *buffer) {
     if (!qr_state) {
         LOG_ERROR(MODULE_RENDER, "Invalid QR state", 0);
         return false;
     }
     
     // Get QR code size
     int qr_size = qr_state->size;
     
     // If buffer is provided, fill it with QR data
     if (buffer) {
         // Safety check to prevent buffer overrun
         if (qr_size > 128) {
             LOG_ERROR(MODULE_RENDER, "QR size too large for buffer", qr_size);
             return false;
         }
         
         for (int y = 0; y < qr_size; y++) {
             for (int x = 0; x < qr_size; x++) {
                 int module_value = qr_state->data[y * qr_size + x];
                 buffer[y * qr_size + x] = (module_value == QR_MODULE_BLACK) ? CLR_BLACK : CLR_WHITE;
             }
         }
         
         LOG_INFO(MODULE_RENDER, "QR rendered to buffer", qr_size);
         return true;
     }
     
     // If no buffer provided, fall back to direct rendering
     return render_qr_to_screen(qr_state, 10, 40, 2);
 }
 
 /**
  * @brief Creates a border around the QR code for better scanning
  * 
  * @param x Top-left x position of QR code
  * @param y Top-left y position of QR code
  * @param size Size of QR code in pixels
  * @param border_size Size of border in pixels
  */
 void render_qr_border(int x, int y, int size, int border_size) {
     // Draw white border around QR code
     for (int i = 0; i < border_size; i++) {
         // Top and bottom borders
         for (int px = x - border_size; px < x + size + border_size; px++) {
             // Top border
             if (px >= 0 && px < SCREEN_WIDTH && y - i - 1 >= 0) {
                 m3_plot(px, y - i - 1, CLR_WHITE);
             }
             // Bottom border
             if (px >= 0 && px < SCREEN_WIDTH && y + size + i < SCREEN_HEIGHT) {
                 m3_plot(px, y + size + i, CLR_WHITE);
             }
         }
         
         // Left and right borders
         for (int py = y - border_size; py < y + size + border_size; py++) {
             // Left border
             if (x - i - 1 >= 0 && py >= 0 && py < SCREEN_HEIGHT) {
                 m3_plot(x - i - 1, py, CLR_WHITE);
             }
             // Right border
             if (x + size + i < SCREEN_WIDTH && py >= 0 && py < SCREEN_HEIGHT) {
                 m3_plot(x + size + i, py, CLR_WHITE);
             }
         }
     }
     
     LOG_INFO(MODULE_RENDER, "QR border rendered", border_size);
 }
 
 /**
  * @brief Renders a QR code with improved tile-based rendering
  * 
  * @param qr_state QR code state
  * @param x X position on screen
  * @param y Y position on screen 
  * @param scale Scale factor
  * @return Success status
  */
 bool render_qr_tile_based(QrState *qr_state, int x, int y, int scale) {
     if (!qr_state || !qr_state->data) {
         LOG_ERROR(MODULE_RENDER, "Invalid QR state for tile rendering", 0);
         return false;
     }
     
     // This is a placeholder for an optimized tile-based rendering that would use
     // the GBA's hardware tile capabilities instead of slow pixel-by-pixel rendering
     
     // In a real implementation, we would:
     // 1. Convert QR modules to tiles
     // 2. Upload tiles to VRAM
     // 3. Set up a screen background to display those tiles
     
     // For now, we'll just use the direct rendering method
     return render_qr_to_screen(qr_state, x, y, scale);
 }
 
 /**
  * @brief Renders a cryptocurrency QR code
  * 
  * This function is the main entry point for rendering QR codes for cryptocurrency addresses.
  * It handles both generation and rendering of the QR code.
  * 
  * @param qr_state QR code state
  * @param address Cryptocurrency address
  * @param buffer Video buffer
  * @return Success status
  */
 bool render_crypto_qr(QrState *qr_state, const char *address, u16 *buffer) {
     if (!qr_state || !address || !buffer) {
         LOG_ERROR(MODULE_RENDER, "Invalid parameters for crypto QR", 0);
         return false;
     }
     
     // Set QR text content
     if (!qr_set_text(qr_state, address)) {
         LOG_ERROR(MODULE_RENDER, "Failed to set QR text", 0);
         return false;
     }
     
     // Generate QR code
     if (!qr_generate(qr_state)) {
         LOG_ERROR(MODULE_RENDER, "Failed to generate QR", 0);
         return false;
     }
     
     // Render to buffer
     if (!render_qr_optimized(qr_state, buffer)) {
         LOG_ERROR(MODULE_RENDER, "Failed to render QR", 0);
         return false;
     }
     
     LOG_INFO(MODULE_RENDER, "Crypto QR rendered successfully", qr_state->size);
     return true;
 }
 
 /**
  * @brief Specialized renderer for displaying QR codes with custom parameters
  * 
  * @param qr_state QR code state
  * @param params Rendering parameters
  * @return Success status
  */
 bool render_qr_with_params(QrState *qr_state, const QrRenderParams *params) {
     if (!qr_state || !params) {
         LOG_ERROR(MODULE_RENDER, "Invalid parameters for custom QR rendering", 0);
         return false;
     }
     
     // Apply rendering parameters
     int x = params->x;
     int y = params->y;
     int scale = params->scale;
     bool show_border = params->show_border;
     int border_size = params->border_size;
     
     // Render QR code
     bool success = render_qr_to_screen(qr_state, x, y, scale);
     
     // Add border if requested
     if (success && show_border) {
         render_qr_border(x, y, qr_state->size * scale, border_size);
     }
     
     return success;
 }
 
 /**
  * @brief Initialize rendering system
  */
 void qr_rendering_init(void) {
     // Set up video mode
     REG_DISPCNT = DCNT_MODE3 | DCNT_BG2;
     
     // Clear screen to white
     for (int y = 0; y < SCREEN_HEIGHT; y++) {
         for (int x = 0; x < SCREEN_WIDTH; x++) {
             m3_plot(x, y, CLR_WHITE);
         }
     }
     
     LOG_INFO(MODULE_RENDER, "QR rendering initialized", 0);
 }