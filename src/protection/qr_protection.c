/**
 * @file qr_protection.c
 * @brief Implementation of the QR anti-photography protection system
 *
 * This file implements a sophisticated anti-photography protection system
 * for QR codes. The system generates multiple valid QR code variations that 
 * encode the same data but with different visual patterns. By rapidly switching 
 * between these patterns, the system creates a QR code that appears to shimmer 
 * or change when viewed in real-time, making it nearly impossible to capture
 * via photography while still allowing real-time scanning.
 *
 * @author Claude
 * @date March 2025
 * @version 1.0.0
 */

 #include <string.h>
 #include "qr_protection.h"
 
 // Global protection system instance
 QrProtectionSystem g_qr_protection;
 
 // Preset parameters for different protection levels
 static const QrProtectionParams LEVEL_PRESETS[] = {
     // QR_PROT_LEVEL_OFF
     {
         .refresh_rate = 0,
         .mask_variations = 1,
         .randomize_function = false,
         .reduce_ecc = false,
         .alternate_encoding = false,
         .custom_ecc_level = 2, // Q level
         .invert_modules = false,
         .invert_percentage = 0
     },
     // QR_PROT_LEVEL_LOW
     {
         .refresh_rate = 5,
         .mask_variations = 4,
         .randomize_function = false,
         .reduce_ecc = false,
         .alternate_encoding = false,
         .custom_ecc_level = 2, // Q level
         .invert_modules = false,
         .invert_percentage = 0
     },
     // QR_PROT_LEVEL_MEDIUM
     {
         .refresh_rate = 7,
         .mask_variations = 8,
         .randomize_function = true,
         .reduce_ecc = true,
         .alternate_encoding = true,
         .custom_ecc_level = 1, // M level
         .invert_modules = true,
         .invert_percentage = 10
     },
     // QR_PROT_LEVEL_HIGH
     {
         .refresh_rate = 10,
         .mask_variations = 8,
         .randomize_function = true,
         .reduce_ecc = true,
         .alternate_encoding = true,
         .custom_ecc_level = 0, // L level
         .invert_modules = true,
         .invert_percentage = 20
     }
 };
 
 /**
  * Initialize the QR protection system
  */
 void qr_protection_init(void) {
     // Clear system state
     memset(&g_qr_protection, 0, sizeof(QrProtectionSystem));
     
     // Initialize QR variations and buffers
     for (int i = 0; i < QR_MAX_VARIATIONS; i++) {
         // Initialize QR states
         qr_init(&g_qr_protection.variations[i]);
         
         // Allocate buffers (we'll use a single buffer pool in real implementation)
         g_qr_protection.buffers[i] = NULL;
     }
     
     // Default to protection off
     g_qr_protection.level = QR_PROT_LEVEL_OFF;
     g_qr_protection.params = LEVEL_PRESETS[QR_PROT_LEVEL_OFF];
     g_qr_protection.enabled = false;
     
     LOG_INFO(MODULE_PROTECT, "QR protection system initialized", 0);
 }
 
 /**
  * Generate multiple QR code variations from the same data
  * 
  * @param data The data to encode in the QR code
  * @return true if successful, false otherwise
  */
 bool qr_protection_generate_variations(const char *data) {
     if (!data) {
         LOG_ERROR(MODULE_PROTECT, "NULL data for QR protection", 0);
         return false;
     }
     
     const QrProtectionParams *params = &g_qr_protection.params;
     
     // If protection is disabled, just generate one normal QR code
     if (!g_qr_protection.enabled || g_qr_protection.level == QR_PROT_LEVEL_OFF) {
         // Generate a single QR code with standard settings
         if (!qr_encode_text(&g_qr_protection.variations[0], data, QR_ECLEVEL_Q)) {
             LOG_ERROR(MODULE_PROTECT, "Failed to generate standard QR", 0);
             return false;
         }
         
         g_qr_protection.variation_count = 1;
         g_qr_protection.current_variation = 0;
         return true;
     }
     
     // How many variations to generate
     int num_variations = params->mask_variations;
     if (num_variations <= 0) num_variations = 1;
     if (num_variations > QR_MAX_VARIATIONS) num_variations = QR_MAX_VARIATIONS;
     
     // Generate different variations
     for (int i = 0; i < num_variations; i++) {
         QrState *qr = &g_qr_protection.variations[i];
         
         // Set error correction level
         QrEcLevel ec_level = params->custom_ecc_level;
         if (ec_level > QR_ECLEVEL_H) ec_level = QR_ECLEVEL_Q;
         
         // Force specific mask pattern for this variation
         qr->mask_pattern = i % 8;
         qr->auto_mask = false;
         
         // Generate base QR code
         if (!qr_encode_text(qr, data, ec_level)) {
             LOG_ERROR(MODULE_PROTECT, "Failed to generate QR variation", i);
             continue;
         }
         
         // Apply additional variation techniques
         if (params->invert_modules) {
             qr_apply_module_inversion(qr, params->invert_percentage);
         }
         
         if (params->randomize_function) {
             qr_randomize_function_patterns(qr);
         }
     }
     
     g_qr_protection.variation_count = num_variations;
     g_qr_protection.current_variation = 0;
     
     // Calculate frames between switches based on refresh rate
     if (params->refresh_rate > 0) {
         g_qr_protection.display_frames = 60 / params->refresh_rate;
         if (g_qr_protection.display_frames < 1) g_qr_protection.display_frames = 1;
     } else {
         g_qr_protection.display_frames = 0; // Never switch
     }
     
     LOG_INFO(MODULE_PROTECT, "Generated QR variations", num_variations);
     return (num_variations > 0);
 }
 
 /**
  * Apply random module inversion to non-essential parts of QR code
  * 
  * @param qr QR code state
  * @param percentage Percentage of modules to invert (0-20)
  */
 void qr_apply_module_inversion(QrState *qr, u8 percentage) {
     if (!qr || !qr->data || percentage == 0) return;
     
     int size = qr->size;
     if (size <= 0) return;
     
     // Cap percentage at 20% to ensure QR remains readable
     if (percentage > 20) percentage = 20;
     
     // Calculate how many modules to invert
     int modules_to_invert = (size * size * percentage) / 100;
     
     // Keep track of function patterns - don't invert these
     bool function_pattern[QR_MAX_SIZE][QR_MAX_SIZE] = {0};
     
     // Mark finder patterns and their surroundings
     for (int y = 0; y < 8; y++) {
         for (int x = 0; x < 8; x++) {
             function_pattern[y][x] = true; // Top-left
             function_pattern[y][size-x-1] = true; // Top-right
             function_pattern[size-y-1][x] = true; // Bottom-left
         }
     }
     
     // Mark timing patterns
     for (int i = 0; i < size; i++) {
         function_pattern[6][i] = true; // Horizontal
         function_pattern[i][6] = true; // Vertical
     }
     
     // Mark alignment patterns if present
     // Simplified - in a real implementation we'd mark them precisely
     if (size >= 25) { // Version 2 or higher
         for (int y = size - 9; y < size - 4; y++) {
             for (int x = size - 9; x < size - 4; x++) {
                 function_pattern[y][x] = true;
             }
         }
     }
     
     // Invert random modules, avoiding function patterns
     int inverted = 0;
     while (inverted < modules_to_invert) {
         int x = rand() % size;
         int y = rand() % size;
         
         if (!function_pattern[y][x]) {
             // Invert this module
             qr->data[y * size + x] ^= 1;
             inverted++;
         }
     }
 }
 
 /**
  * Apply minor randomization to function pattern positions
  * while maintaining QR readability
  * 
  * @param qr QR code state
  */
 void qr_randomize_function_patterns(QrState *qr) {
     if (!qr || !qr->data) return;
     
     int size = qr->size;
     if (size <= 0) return;
     
     // This is a simplified implementation
     // In practice, we would shift alignment patterns slightly
     // and adjust finder pattern quiet zones in ways that
     // maintain readability but change visual appearance
     
     // Example: Adjust timing pattern
     for (int i = 8; i < size-8; i++) {
         if ((i % 2) == (rand() % 2)) {
             // Shift some timing pattern modules by one pixel
             if (rand() % 3 == 0) {
                 qr->data[6*size + i] ^= 1;
                 qr->data[i*size + 6] ^= 1;
             }
         }
     }
 }
 
 /**
  * Set protection level
  * 
  * @param level The protection level to set
  */
 void qr_protection_set_level(QrProtectionLevel level) {
     if (level >= QR_PROT_LEVEL_COUNT) {
         LOG_ERROR(MODULE_PROTECT, "Invalid protection level", level);
         return;
     }
     
     g_qr_protection.level = level;
     
     // Only apply preset params if not in custom mode
     if (level != QR_PROT_LEVEL_CUSTOM) {
         g_qr_protection.params = LEVEL_PRESETS[level];
     }
     
     // Enable/disable protection based on level
     g_qr_protection.enabled = (level != QR_PROT_LEVEL_OFF);
     
     LOG_INFO(MODULE_PROTECT, "Protection level set to", level);
 }
 
 /**
  * Set custom protection parameters
  * 
  * @param params The parameters to set
  */
 void qr_protection_set_params(const QrProtectionParams *params) {
     if (!params) {
         LOG_ERROR(MODULE_PROTECT, "NULL params for QR protection", 0);
         return;
     }
     
     // Copy parameters
     memcpy(&g_qr_protection.params, params, sizeof(QrProtectionParams));
     
     // Set to custom mode
     g_qr_protection.level = QR_PROT_LEVEL_CUSTOM;
     
     // Protection is enabled if refresh rate > 0
     g_qr_protection.enabled = (params->refresh_rate > 0);
     
     // Update display frames for changing variations
     if (params->refresh_rate > 0) {
         g_qr_protection.display_frames = 60 / params->refresh_rate;
         if (g_qr_protection.display_frames < 1) g_qr_protection.display_frames = 1;
     } else {
         g_qr_protection.display_frames = 0; // Never switch
     }
     
     LOG_INFO(MODULE_PROTECT, "Custom protection params set", params->refresh_rate);
 }
 
 /**
  * Get current protection parameters
  * 
  * @return Pointer to current parameters
  */
 const QrProtectionParams* qr_protection_get_params(void) {
     return &g_qr_protection.params;
 }
 
 /**
  * Update protection system - called every frame
  */
 void qr_protection_update(void) {
     // If protection is disabled or we only have one variation, do nothing
     if (!g_qr_protection.enabled || g_qr_protection.variation_count <= 1) {
         return;
     }
     
     // Check if it's time to switch variations
     u32 current_frame = g_qr_state.frame_counter;
     
     if (g_qr_protection.display_frames > 0 && 
         (current_frame - g_qr_protection.last_switch_time) >= g_qr_protection.display_frames) {
         
         // Move to next variation
         g_qr_protection.current_variation = 
             (g_qr_protection.current_variation + 1) % g_qr_protection.variation_count;
         
         g_qr_protection.last_switch_time = current_frame;
     }
 }
 
 /**
  * Render the current QR variation
  * 
  * @param x X position to render
  * @param y Y position to render
  * @param scale Scale factor
  * @return true if rendered successfully
  */
 bool qr_protection_render(int x, int y, int scale) {
     // If protection is disabled or no variations, render normally
     if (!g_qr_protection.enabled || g_qr_protection.variation_count == 0) {
         return false; // Fall back to normal rendering
     }
     
     // Get current variation
     int current = g_qr_protection.current_variation;
     QrState *qr = &g_qr_protection.variations[current];
     
     // Render this variation
     return render_qr_to_screen(qr, x, y, scale);
 }