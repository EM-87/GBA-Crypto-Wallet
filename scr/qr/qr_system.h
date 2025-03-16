/**
 * @file qr_system.h
 * @brief QR code generation system for GBA
 * 
 * This header defines the core structures and functions for QR code
 * generation and rendering on the GBA hardware. It includes definitions
 * for QR state management, error correction levels, and rendering options.
 * 
 * @author Claude
 * @date March 2025
 * @version 1.0.0
 */

 #ifndef QR_SYSTEM_H
 #define QR_SYSTEM_H
 
 #include <tonc.h>
 #include <stdlib.h>
 #include <string.h>
 
 /**
  * Maximum QR code size in modules (Version 40)
  * This is the theoretical maximum, though the GBA screen can't display
  * the largest QR codes at a readable size.
  */
 #define QR_MAX_SIZE 177
 
 /**
  * Screen dimensions (used for boundary checks)
  */
 #define SCREEN_WIDTH 240
 #define SCREEN_HEIGHT 160
 
 /**
  * QR Error correction levels
  */
 typedef enum {
     QR_ECLEVEL_L = 0, // Low (7% recovery)
     QR_ECLEVEL_M = 1, // Medium (15% recovery)
     QR_ECLEVEL_Q = 2, // Quartile (25% recovery)
     QR_ECLEVEL_H = 3, // High (30% recovery)
     QR_ECLEVEL_COUNT = 4
 } QrEcLevel;
 
 /**
  * QR code state
  * Contains all data necessary to represent and render a QR code
  */
 typedef struct {
     int size;               // Size of QR code in modules
     u8 *data;               // QR code data (0=white, 1=black)
     int data_length;        // Length of text data
     QrEcLevel ec_level;     // Error correction level
     int mask_pattern;       // Mask pattern (0-7)
     bool auto_mask;         // Whether to automatically select mask
 } QrState;
 
 /**
  * Global QR system state
  * Maintains the application-wide QR state and settings
  */
 typedef struct {
     QrState qr_state;         // The main QR state
     u16 qr_buffer[128*128];   // Buffer for QR rendering
     int refresh_rate;         // Screen refresh rate in FPS
     int update_interval;      // Update interval in frames
     int qr_pixel_size;        // Size of each QR module in pixels
     bool enable_animations;   // Whether animations are enabled
     u32 frame_counter;        // Global frame counter
     int auto_hide_timeout;    // Auto-hide timeout in frames
 } QrSystemState;
 
 /**
  * QR code generation and management functions
  */
 
 /**
  * Initialize a QR state
  * @param qr_state QR code state to initialize
  */
 void qr_init(QrState *qr_state);
 
 /**
  * Free resources associated with a QR state
  * @param qr_state QR code state to free
  */
 void qr_free(QrState *qr_state);
 
 /**
  * Set the text content for a QR code
  * @param qr_state QR code state
  * @param text Text to encode
  * @return Success status
  */
 bool qr_set_text(QrState *qr_state, const char *text);
 
 /**
  * Generate a QR code from the current state
  * @param qr_state QR code state
  * @return Success status
  */
 bool qr_generate(QrState *qr_state);
 
 /**
  * Encode text into a QR code
  * @param qr_state QR code state
  * @param text Text to encode
  * @param ec_level Error correction level
  * @return Success status
  */
 bool qr_encode_text(QrState *qr_state, const char *text, QrEcLevel ec_level);
 
 /**
  * QR rendering functions
  */
 
 /**
  * Render a QR code to the GBA screen
  * @param qr_state QR code state with pattern data
  * @param x Top-left x position for rendering
  * @param y Top-left y position for rendering
  * @param scale Scaling factor (1 = 1 pixel per module)
  * @return Success status
  */
 bool render_qr_to_screen(QrState *qr_state, int x, int y, int scale);
 
 /**
  * Optimized QR rendering for GBA
  * Uses sprite or tile-based rendering for better performance
  * @param qr_state QR code state
  * @param buffer Target buffer (can be NULL if rendering directly)
  * @return Success status
  */
 bool render_qr_optimized(QrState *qr_state, u16 *buffer);
 
 /**
  * Creates a border around the QR code for better scanning
  * @param x Top-left x position of QR code
  * @param y Top-left y position of QR code
  * @param size Size of QR code in pixels
  * @param border_size Size of border in pixels
  */
 void render_qr_border(int x, int y, int size, int border_size);
 
 /**
  * Renders a cryptocurrency QR code
  * @param qr_state QR code state
  * @param address Cryptocurrency address
  * @param buffer Video buffer
  * @return Success status
  */
 bool render_crypto_qr(QrState *qr_state, const char *address, u16 *buffer);
 
 /**
  * Global QR system state
  */
 extern QrSystemState g_qr_state;
 
 #endif // QR_SYSTEM_H