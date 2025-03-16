/**
 * @file qr_debug.h
 * @brief Debugging and logging system
 * 
 * This header defines a logging system for debugging purposes,
 * allowing messages of different severity levels to be recorded
 * and displayed on screen.
 * 
 * @author Claude
 * @date March 2025
 * @version 1.0.0
 */

 #ifndef QR_DEBUG_H
 #define QR_DEBUG_H
 
 #include <tonc.h>
 
 /**
  * Log severity levels
  */
 typedef enum { 
     LOG_NONE,     // No logging
     LOG_ERROR,    // Error messages
     LOG_WARNING,  // Warning messages
     LOG_INFO,     // Informational messages
     LOG_DEBUG     // Detailed debug messages
 } LogLevel;
 
 /**
  * Current log level - only messages at this level or lower will be recorded
  */
 #define CURRENT_LOG_LEVEL LOG_WARNING
 
 /**
  * Maximum number of log entries to store
  */
 #define MAX_LOG_ENTRIES 10
 
 /**
  * Module identifiers for log source tracking
  */
 #define MODULE_SYSTEM    0  // Core system
 #define MODULE_RENDER    1  // Rendering
 #define MODULE_WALLET    2  // Wallet functions
 #define MODULE_PROTECT   3  // QR protection
 #define MODULE_OPTIMIZE  4  // Optimizations
 #define MODULE_TEST      5  // Tests
 #define MODULE_POWER     6  // Power management
 #define MODULE_MENU      7  // Menu system
 
 /**
  * Enable on-screen log display
  * Comment out to disable debug overlay
  */
 #define DEBUG_ENABLE_LOG_DISPLAY
 
 /**
  * Log entry structure
  */
 typedef struct {
     LogLevel level;       // Log severity level
     const char *message;  // Log message text
     int data;             // Additional numeric data
     u32 timestamp;        // System timestamp
     u8 module_id;         // Source module identifier
 } LogEntry;
 
 /**
  * Initialize debug system
  */
 void debug_init(void);
 
 /**
  * Update debug frame counter
  * Called every frame
  */
 void debug_update_tick(void);
 
 /**
  * Log a message
  * @param level Severity level
  * @param module_id Source module ID
  * @param message Log message text
  * @param data Additional numeric data
  */
 void debug_log(LogLevel level, u8 module_id, const char *message, int data);
 
 /**
  * Display log on screen
  * @param start_y Starting Y position
  * @param module_filter Module to filter by (0 for all)
  * @param level_filter Minimum level to show
  */
 void debug_show_log(int start_y, int module_filter, LogLevel level_filter);
 
 /**
  * Helper macros for logging
  */
 #define LOG_ERROR(module, msg, data) debug_log(LOG_ERROR, module, msg, data)
 #define LOG_WARNING(module, msg, data) debug_log(LOG_WARNING, module, msg, data)
 #define LOG_INFO(module, msg, data) debug_log(LOG_INFO, module, msg, data)
 #define LOG_DEBUG(module, msg, data) debug_log(LOG_DEBUG, module, msg, data)
 
 /**
  * Global log buffer
  */
 extern LogEntry g_log_buffer[MAX_LOG_ENTRIES];
 extern int g_log_index;
 extern int g_log_count;
 extern u32 g_log_frame_counter;
 
 #endif // QR_DEBUG_H