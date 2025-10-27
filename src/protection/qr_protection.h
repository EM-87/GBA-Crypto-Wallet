/**
 * @file qr_protection.h
 * @brief Anti-photography protection system for QR codes
 * 
 * This system generates multiple visually different QR codes from the same data,
 * alternating between them rapidly to prevent photography while allowing real-time scanning.
 * 
 * @author Claude
 * @date March 2025
 * @version 1.0.0
 */

 #ifndef QR_PROTECTION_H
 #define QR_PROTECTION_H
 
 #include <tonc.h>
 #include "qr_system.h"
 #include "qr_debug.h"
 
 /**
  * Maximum number of QR code variations to generate
  */
 #define QR_MAX_VARIATIONS 8
 
 /**
  * Protection levels
  */
 typedef enum {
     QR_PROT_LEVEL_OFF,       // Protection disabled
     QR_PROT_LEVEL_LOW,       // Basic protection - 5 FPS, normal QR
     QR_PROT_LEVEL_MEDIUM,    // Medium protection - 7 FPS, reduced redundancy
     QR_PROT_LEVEL_HIGH,      // High protection - 10 FPS, minimal redundancy
     QR_PROT_LEVEL_CUSTOM,    // Custom settings
     QR_PROT_LEVEL_COUNT      // Number of protection levels
 } QrProtectionLevel;
 
 /**
  * Custom protection parameters
  * Define how QR codes are varied for anti-photography
  */
 typedef struct {
     int refresh_rate;          // Refresh rate in frames per second (5-10)
     int mask_variations;       // Number of mask pattern variations (1-8)
     bool randomize_function;   // Randomize function pattern coordinates slightly
     bool reduce_ecc;           // Reduce error correction capability
     bool alternate_encoding;   // Use alternate encoding modes
     u8 custom_ecc_level;       // Custom ECC level (0-3 = L,M,Q,H)
     bool invert_modules;       // Randomly invert some non-essential modules
     u8 invert_percentage;      // Percentage of modules to invert (0-20)
 } QrProtectionParams;
 
 /**
  * Protection system state
  */
 typedef struct {
     bool enabled;                     // Whether protection is active
     QrProtectionLevel level;          // Current protection level
     QrProtectionParams params;        // Current parameters
     QrState variations[QR_MAX_VARIATIONS]; // QR code variations
     u16 *buffers[QR_MAX_VARIATIONS];  // Buffers for each variation
     int current_variation;            // Current displayed variation
     int variation_count;              // Number of variations generated
     u32 last_switch_time;             // Last time we switched variations
     u32 display_frames;               // Frames between switching
 } QrProtectionSystem;
 
 /**
  * Initialize the protection system
  */
 void qr_protection_init(void);
 
 /**
  * Generate variation QR codes from the same data
  * @param data Data to encode in QR variations
  * @return Success status
  */
 bool qr_protection_generate_variations(const char *data);
 
 /**
  * Set protection level
  * @param level Protection level to set
  */
 void qr_protection_set_level(QrProtectionLevel level);
 
 /**
  * Set custom protection parameters
  * @param params Custom parameters to use
  */
 void qr_protection_set_params(const QrProtectionParams *params);
 
 /**
  * Get current protection parameters
  * @return Pointer to current parameters
  */
 const QrProtectionParams* qr_protection_get_params(void);
 
 /**
  * Update protection system
  * Called every frame to update variations
  */
 void qr_protection_update(void);
 
 /**
  * Render current QR variation
  * @param x X position on screen
  * @param y Y position on screen
  * @param scale Scale factor
  * @return Success status
  */
 bool qr_protection_render(int x, int y, int scale);
 
 /**
  * Apply module inversion to QR code
  * Randomly inverts non-essential modules for visual variation
  * @param qr QR code state
  * @param percentage Percentage of modules to invert (0-20)
  */
 void qr_apply_module_inversion(QrState *qr, u8 percentage);
 
 /**
  * Randomize function patterns
  * Makes slight adjustments to finder patterns without breaking QR
  * @param qr QR code state
  */
 void qr_randomize_function_patterns(QrState *qr);
 
 /**
  * Integrate protection system with wallet
  * Set up function pointers and event handlers
  */
 void qr_protection_integrate(void);
 
 /**
  * Apply protection to wallet QR code
  * @return Success status
  */
 bool wallet_apply_qr_protection(void);
 
 /**
  * Global protection system instance
  */
 extern QrProtectionSystem g_qr_protection;
 
 #endif // QR_PROTECTION_H