/**
 * @file wallet_system.h
 * @brief Cryptocurrency wallet management system for GBA
 * 
 * This header defines the core structures and functions for cryptocurrency
 * wallet management, including wallet entries, cryptocurrency types,
 * and address validation.
 * 
 * @author Claude
 * @date March 2025
 * @version 1.0.0
 */

 #ifndef WALLET_SYSTEM_H
 #define WALLET_SYSTEM_H
 
 #include <tonc.h>
 #include "qr_system.h"
 #include "qr_debug.h"
 #include "crypto_types.h"
 
 /**
  * Maximum number of wallet entries
  */
 #define MAX_WALLET_ENTRIES 20
 
 /**
  * Maximum length for wallet entry fields
  */
 #define MAX_NAME_LENGTH 24
 #define MAX_ADDRESS_LENGTH 64
 #define MAX_NOTES_LENGTH 128
 #define MAX_TAGS_LENGTH 32
 
 /**
  * Compatibility with old crypto type definitions
  */
 #define CRYPTO_BITCOIN CRYPTO_TYPE_BITCOIN
 #define CRYPTO_ETHEREUM CRYPTO_TYPE_ETHEREUM
 #define CRYPTO_LITECOIN CRYPTO_TYPE_LITECOIN
 #define CRYPTO_DOGECOIN CRYPTO_TYPE_DOGECOIN
 #define CRYPTO_OTHER 4
 #define CRYPTO_TYPE_COUNT 5
 
 /**
  * Wallet entry structure
  * Contains all data for a single wallet entry
  */
 typedef struct {
     char name[MAX_NAME_LENGTH];       // Name identifier
     char address[MAX_ADDRESS_LENGTH]; // Cryptocurrency address
     u8 type_index;                    // Cryptocurrency type index
     char notes[MAX_NOTES_LENGTH];     // Notes or comments
     u32 balance;                      // Stored balance (in minimum units)
     char tags[MAX_TAGS_LENGTH];       // Comma-separated tags
     u32 last_used;                    // Timestamp of last use
     bool favorite;                    // Marked as favorite
 } WalletEntry;
 
 /**
  * Global wallet system state
  * Maintains all wallet entries and system settings
  */
 typedef struct {
     WalletEntry entries[MAX_WALLET_ENTRIES]; // Wallet entries
     int count;                      // Current entry count
     int selected_index;             // Currently selected index
     int view_offset;                // Scroll offset for viewing
     bool is_encrypted;              // Whether data is encrypted
     u16 password_hash;              // Simple password hash
     u8 active_crypto_filter;        // Active crypto type filter
     bool show_favorites_only;       // Show only favorites filter
     QrState qr_state;               // QR state for address display
     u16 qr_buffer[128*128];         // Buffer for QR rendering
 } WalletSystem;
 
 /**
  * Initialization and data management
  */
 
 /**
  * Initialize the wallet system
  */
 void wallet_system_init(void);
 
 /**
  * Load wallet data from SRAM
  * @return Success status
  */
 bool wallet_system_load(void);
 
 /**
  * Save wallet data to SRAM
  * @return Success status
  */
 bool wallet_system_save(void);
 
 /**
  * Wallet entry management
  */
 
 /**
  * Add a new wallet entry
  * @param entry Pointer to entry data to add
  * @return Index of new entry or -1 on failure
  */
 int wallet_add_entry(const WalletEntry* entry);
 
 /**
  * Update an existing wallet entry
  * @param index Index of entry to update
  * @param entry New entry data
  * @return Success status
  */
 bool wallet_update_entry(int index, const WalletEntry* entry);
 
 /**
  * Delete a wallet entry
  * @param index Index of entry to delete
  * @return Success status
  */
 bool wallet_delete_entry(int index);
 
 /**
  * Get a wallet entry by index
  * @param index Index of entry to retrieve
  * @return Pointer to entry or NULL if invalid
  */
 WalletEntry* wallet_get_entry(int index);
 
 /**
  * Get the currently selected wallet entry
  * @return Pointer to selected entry or NULL if none
  */
 WalletEntry* wallet_get_selected_entry(void);
 
 /**
  * Navigation and filtering
  */
 
 /**
  * Select a wallet entry by index
  * @param index Index to select
  */
 void wallet_select_entry(int index);
 
 /**
  * Select the next wallet entry
  */
 void wallet_next_entry(void);
 
 /**
  * Select the previous wallet entry
  */
 void wallet_prev_entry(void);
 
 /**
  * Get count of entries after applying filters
  * @return Number of filtered entries
  */
 int wallet_get_filtered_count(void);
 
 /**
  * Convert a filtered index to actual index
  * @param filtered_index Index in filtered list
  * @return Actual index in entries array
  */
 int wallet_get_actual_index(int filtered_index);
 
 /**
  * Convert an actual index to filtered index
  * @param actual_index Index in entries array
  * @return Index in filtered list or -1 if filtered out
  */
 int wallet_get_filtered_index(int actual_index);
 
 /**
  * Set crypto type filter
  * @param type Crypto type to filter by
  */
 void wallet_set_crypto_filter(CryptoType type);
 
 /**
  * Toggle favorites-only filter
  */
 void wallet_toggle_favorites_filter(void);
 
 /**
  * QR code display
  */
 
 /**
  * Generate QR code for wallet entry
  * @param index Index of entry to generate QR for
  * @return Success status
  */
 bool wallet_generate_qr(int index);
 
 /**
  * Render current QR code to screen
  * @param x X position
  * @param y Y position
  * @param scale Scale factor
  * @return Success status
  */
 bool wallet_render_current_qr(int x, int y, int scale);
 
 /**
  * Get cryptocurrency name by type
  * @param type Cryptocurrency type
  * @return Name string
  */
 const char* wallet_get_crypto_name(CryptoType type);
 
 /**
  * Get cryptocurrency symbol by type
  * @param type Cryptocurrency type
  * @return Symbol string
  */
 const char* wallet_get_crypto_symbol(CryptoType type);
 
 /**
  * Security functions
  */
 
 /**
  * Set wallet password
  * @param password Password string
  * @return Success status
  */
 bool wallet_set_password(const char* password);
 
 /**
  * Verify wallet password
  * @param password Password to verify
  * @return Whether password is correct
  */
 bool wallet_verify_password(const char* password);
 
 /**
  * Encrypt wallet data
  * @return Success status
  */
 bool wallet_encrypt_data(void);
 
 /**
  * Decrypt wallet data
  * @return Success status
  */
 bool wallet_decrypt_data(void);
 
 /**
  * Get global wallet system instance
  * @return Pointer to wallet system
  */
 WalletSystem* wallet_system_get_instance(void);
 
 #endif // WALLET_SYSTEM_H