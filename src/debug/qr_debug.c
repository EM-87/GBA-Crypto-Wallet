/**
 * @file qr_debug.c
 * @brief Implementation of debugging and logging system for GBA QR Code Generator
 * 
 * This file implements a comprehensive debugging system that allows for:
 * - Logging at different severity levels (ERROR, WARNING, INFO, DEBUG)
 * - Module-specific logging
 * - Circular buffer storage of log messages
 * - On-screen display of log messages with filtering
 * 
 * @author Claude
 * @date March 2025
 * @version 1.0.0
 */

 #include "qr_debug.h"
 #include <string.h>
 
 // Global log buffer
 LogEntry g_log_buffer[MAX_LOG_ENTRIES];
 int g_log_index = 0;
 int g_log_count = 0;
 u32 g_log_frame_counter = 0;
 
 // Module names for display purposes
 static const char* MODULE_NAMES[] = {
     "SYSTEM",
     "RENDER",
     "WALLET",
     "PROTECT",
     "OPTIMIZE",
     "TEST",
     "POWER",
     "MENU"
 };
 
 /**
  * @brief Initialize debug logging system
  */
 void debug_init(void) {
     // Clear log buffer
     for (int i = 0; i < MAX_LOG_ENTRIES; i++) {
         g_log_buffer[i].level = LOG_NONE;
         g_log_buffer[i].message = "";
         g_log_buffer[i].data = 0;
         g_log_buffer[i].timestamp = 0;
         g_log_buffer[i].module_id = 0;
     }
     
     // Initialize counters
     g_log_index = 0;
     g_log_count = 0;
     g_log_frame_counter = 0;
     
     // Log initialization
     LOG_INFO(MODULE_SYSTEM, "Debug system initialized", 0);
 }
 
 /**
  * @brief Update frame counter for timing
  * 
  * This function should be called once per frame to update
  * the global frame counter used for timestamping log entries.
  */
 void debug_update_tick(void) {
     g_log_frame_counter++;
 }
 
 /**
  * @brief Log a message
  * 
  * @param level Log level (ERROR, WARNING, INFO, DEBUG)
  * @param module_id Source module ID
  * @param message Log message text
  * @param data Additional numeric data
  */
 void debug_log(LogLevel level, u8 module_id, const char *message, int data) {
     // Only log messages at or below the current log level
     if (CURRENT_LOG_LEVEL < level) return;
     
     // Validate parameters
     if (!message || module_id >= sizeof(MODULE_NAMES)/sizeof(MODULE_NAMES[0])) {
         return;
     }
     
     // Save log message in buffer
     LogEntry *entry = &g_log_buffer[g_log_index];
     
     entry->level = level;
     entry->message = message;
     entry->data = data;
     entry->timestamp = g_log_frame_counter;
     entry->module_id = module_id;
     
     // Advance buffer index with wrap-around (circular buffer)
     g_log_index = (g_log_index + 1) % MAX_LOG_ENTRIES;
     
     // Keep track of how many entries we've logged
     if (g_log_count < MAX_LOG_ENTRIES) {
         g_log_count++;
     }
 }
 
 /**
  * @brief Display the log buffer on screen
  * 
  * @param start_y Starting Y position for display
  * @param module_filter Module ID to filter (0 for all)
  * @param level_filter Minimum log level to show
  */
 void debug_show_log(int start_y, int module_filter, LogLevel level_filter) {
     int y = start_y;
     int shown = 0;
     
     // Display header
     tte_write_ex(5, y, "DEBUG LOG:", RGB15(31,31,0));
     y += 10;
     
     // If no entries, show message
     if (g_log_count == 0) {
         tte_write_ex(5, y, "No log entries available", RGB15(20,20,20));
         return;
     }
     
     // Iterate through log entries in order (oldest first)
     for (int i = 0; i < g_log_count && shown < 10; i++) {
         // Calculate index into circular buffer
         int index = (g_log_index - g_log_count + i + MAX_LOG_ENTRIES) % MAX_LOG_ENTRIES;
         
         LogEntry *entry = &g_log_buffer[index];
         
         // Apply filters
         if (entry->level < level_filter) continue;
         if (module_filter != 0 && entry->module_id != module_filter) continue;
         
         // Select color based on log level
         u16 color;
         const char *prefix;
         
         switch (entry->level) {
             case LOG_ERROR:
                 color = RGB15(31,0,0);    // Red
                 prefix = "ERROR";
                 break;
             case LOG_WARNING:
                 color = RGB15(31,31,0);   // Yellow
                 prefix = "WARN ";
                 break;
             case LOG_INFO:
                 color = RGB15(0,31,0);    // Green
                 prefix = "INFO ";
                 break;
             case LOG_DEBUG:
                 color = RGB15(20,20,31);  // Light blue
                 prefix = "DEBUG";
                 break;
             default:
                 color = RGB15(31,31,31);  // White
                 prefix = "     ";
                 break;
         }
         
         // Format log message
         char buffer[64];
         const char *module_name = MODULE_NAMES[entry->module_id];
         
         // Format: "[LEVEL] Module: Message (Data)"
         sprintf(buffer, "[%s] %s: %s (%d)", 
                 prefix, 
                 module_name, 
                 entry->message, 
                 entry->data);
         
         // Display log entry
         tte_write_ex(5, y, buffer, color);
         y += 8;
         shown++;
     }
     
     // If no logs were shown (all filtered out), display a message
     if (shown == 0) {
         tte_write_ex(5, y, "No log entries match filters", RGB15(20,20,20));
     }
     
     // Display filter info
     char filter_info[32];
     if (module_filter != 0) {
         sprintf(filter_info, "Filter: %s", MODULE_NAMES[module_filter]);
         tte_write_ex(SCREEN_WIDTH - 100, start_y, filter_info, RGB15(20,20,31));
     }
 }
 
 /**
  * @brief Get string representation of a log level
  * 
  * @param level Log level to convert
  * @return String representation of the log level
  */
 const char* debug_level_to_string(LogLevel level) {
     switch (level) {
         case LOG_ERROR:   return "ERROR";
         case LOG_WARNING: return "WARNING";
         case LOG_INFO:    return "INFO";
         case LOG_DEBUG:   return "DEBUG";
         default:          return "UNKNOWN";
     }
 }
 
 /**
  * @brief Get string representation of a module ID
  * 
  * @param module_id Module ID to convert
  * @return String representation of the module
  */
 const char* debug_module_to_string(u8 module_id) {
     if (module_id < sizeof(MODULE_NAMES)/sizeof(MODULE_NAMES[0])) {
         return MODULE_NAMES[module_id];
     }
     return "UNKNOWN";
 }
 
 /**
  * @brief Clear all log entries
  */
 void debug_clear_log(void) {
     g_log_count = 0;
     g_log_index = 0;
     LOG_INFO(MODULE_SYSTEM, "Log cleared", 0);
 }
 
 /**
  * @brief Get total number of log entries
  * 
  * @return Number of log entries
  */
 int debug_get_log_count(void) {
     return g_log_count;
 }
 
 /**
  * @brief Get current frame counter value
  * 
  * @return Current frame counter
  */
 u32 debug_get_frame_counter(void) {
     return g_log_frame_counter;
 }