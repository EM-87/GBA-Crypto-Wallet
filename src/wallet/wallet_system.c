/**
 * @file wallet_system.c
 * @brief Cryptocurrency wallet management system implementation
 *
 * @author Claude
 * @date March 2025
 * @version 1.0.0
 */

#include "wallet_system.h"
#include "crypto_types.h"
#include <string.h>

// Global wallet system instance
static WalletSystem g_wallet_system;

/**
 * Simple hash function for password
 */
static u16 simple_hash(const char* str) {
    u16 hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

/**
 * Check if an entry passes current filters
 */
static bool entry_passes_filter(const WalletEntry* entry) {
    // Check crypto type filter
    if (g_wallet_system.active_crypto_filter != 0xFF) {
        if (entry->type_index != g_wallet_system.active_crypto_filter) {
            return false;
        }
    }

    // Check favorites filter
    if (g_wallet_system.show_favorites_only && !entry->favorite) {
        return false;
    }

    return true;
}

/**
 * Initialize the wallet system
 */
void wallet_system_init(void) {
    memset(&g_wallet_system, 0, sizeof(WalletSystem));

    g_wallet_system.count = 0;
    g_wallet_system.selected_index = -1;
    g_wallet_system.view_offset = 0;
    g_wallet_system.is_encrypted = false;
    g_wallet_system.password_hash = 0;
    g_wallet_system.active_crypto_filter = 0xFF; // No filter
    g_wallet_system.show_favorites_only = false;

    // Initialize crypto types
    crypto_types_init();
}

/**
 * Load wallet data from SRAM
 */
bool wallet_system_load(void) {
    // TODO: Implement SRAM loading when GBA SRAM access is available
    // For now, just return success
    return true;
}

/**
 * Save wallet data to SRAM
 */
bool wallet_system_save(void) {
    // TODO: Implement SRAM saving when GBA SRAM access is available
    // For now, just return success
    return true;
}

/**
 * Add a new wallet entry
 */
int wallet_add_entry(const WalletEntry* entry) {
    if (!entry || g_wallet_system.count >= MAX_WALLET_ENTRIES) {
        return -1;
    }

    int index = g_wallet_system.count;
    memcpy(&g_wallet_system.entries[index], entry, sizeof(WalletEntry));
    g_wallet_system.count++;

    // If this is the first entry, select it
    if (g_wallet_system.selected_index == -1) {
        g_wallet_system.selected_index = 0;
    }

    return index;
}

/**
 * Update an existing wallet entry
 */
bool wallet_update_entry(int index, const WalletEntry* entry) {
    if (!entry || index < 0 || index >= g_wallet_system.count) {
        return false;
    }

    memcpy(&g_wallet_system.entries[index], entry, sizeof(WalletEntry));
    return true;
}

/**
 * Delete a wallet entry
 */
bool wallet_delete_entry(int index) {
    if (index < 0 || index >= g_wallet_system.count) {
        return false;
    }

    // Shift all entries after the deleted one
    for (int i = index; i < g_wallet_system.count - 1; i++) {
        memcpy(&g_wallet_system.entries[i], &g_wallet_system.entries[i + 1],
               sizeof(WalletEntry));
    }

    g_wallet_system.count--;

    // Adjust selected index if necessary
    if (g_wallet_system.selected_index >= g_wallet_system.count) {
        g_wallet_system.selected_index = g_wallet_system.count - 1;
    }

    if (g_wallet_system.count == 0) {
        g_wallet_system.selected_index = -1;
    }

    return true;
}

/**
 * Get a wallet entry by index
 */
WalletEntry* wallet_get_entry(int index) {
    if (index < 0 || index >= g_wallet_system.count) {
        return NULL;
    }

    return &g_wallet_system.entries[index];
}

/**
 * Get the currently selected wallet entry
 */
WalletEntry* wallet_get_selected_entry(void) {
    return wallet_get_entry(g_wallet_system.selected_index);
}

/**
 * Select a wallet entry by index
 */
void wallet_select_entry(int index) {
    if (index >= 0 && index < g_wallet_system.count) {
        g_wallet_system.selected_index = index;
    }
}

/**
 * Select the next wallet entry
 */
void wallet_next_entry(void) {
    int filtered_count = wallet_get_filtered_count();
    if (filtered_count == 0) return;

    int current_filtered = wallet_get_filtered_index(g_wallet_system.selected_index);
    if (current_filtered == -1) current_filtered = 0;

    int next_filtered = (current_filtered + 1) % filtered_count;
    g_wallet_system.selected_index = wallet_get_actual_index(next_filtered);
}

/**
 * Select the previous wallet entry
 */
void wallet_prev_entry(void) {
    int filtered_count = wallet_get_filtered_count();
    if (filtered_count == 0) return;

    int current_filtered = wallet_get_filtered_index(g_wallet_system.selected_index);
    if (current_filtered == -1) current_filtered = 0;

    int prev_filtered = (current_filtered - 1 + filtered_count) % filtered_count;
    g_wallet_system.selected_index = wallet_get_actual_index(prev_filtered);
}

/**
 * Get count of entries after applying filters
 */
int wallet_get_filtered_count(void) {
    int count = 0;

    for (int i = 0; i < g_wallet_system.count; i++) {
        if (entry_passes_filter(&g_wallet_system.entries[i])) {
            count++;
        }
    }

    return count;
}

/**
 * Convert a filtered index to actual index
 */
int wallet_get_actual_index(int filtered_index) {
    int count = 0;

    for (int i = 0; i < g_wallet_system.count; i++) {
        if (entry_passes_filter(&g_wallet_system.entries[i])) {
            if (count == filtered_index) {
                return i;
            }
            count++;
        }
    }

    return -1;
}

/**
 * Convert an actual index to filtered index
 */
int wallet_get_filtered_index(int actual_index) {
    if (actual_index < 0 || actual_index >= g_wallet_system.count) {
        return -1;
    }

    if (!entry_passes_filter(&g_wallet_system.entries[actual_index])) {
        return -1;
    }

    int filtered_index = 0;

    for (int i = 0; i < actual_index; i++) {
        if (entry_passes_filter(&g_wallet_system.entries[i])) {
            filtered_index++;
        }
    }

    return filtered_index;
}

/**
 * Set crypto type filter
 */
void wallet_set_crypto_filter(CryptoType type) {
    g_wallet_system.active_crypto_filter = (u8)type;
}

/**
 * Toggle favorites-only filter
 */
void wallet_toggle_favorites_filter(void) {
    g_wallet_system.show_favorites_only = !g_wallet_system.show_favorites_only;
}

/**
 * Generate QR code for wallet entry
 */
bool wallet_generate_qr(int index) {
    WalletEntry* entry = wallet_get_entry(index);
    if (!entry) {
        return false;
    }

    // Initialize QR state if not already done
    qr_init(&g_wallet_system.qr_state);

    // Set the address text
    if (!qr_set_text(&g_wallet_system.qr_state, entry->address)) {
        return false;
    }

    // Generate the QR code
    return qr_generate(&g_wallet_system.qr_state);
}

/**
 * Render current QR code to screen
 */
bool wallet_render_current_qr(int x, int y, int scale) {
    return render_qr_to_screen(&g_wallet_system.qr_state, x, y, scale);
}

/**
 * Get cryptocurrency name by type
 */
const char* wallet_get_crypto_name(CryptoType type) {
    const CryptoTypeInfo* info = crypto_get_type_info((int)type);
    return info ? info->name : "Unknown";
}

/**
 * Get cryptocurrency symbol by type
 */
const char* wallet_get_crypto_symbol(CryptoType type) {
    const CryptoTypeInfo* info = crypto_get_type_info((int)type);
    return info ? info->symbol : "???";
}

/**
 * Set wallet password
 */
bool wallet_set_password(const char* password) {
    if (!password) {
        return false;
    }

    g_wallet_system.password_hash = simple_hash(password);
    return true;
}

/**
 * Verify wallet password
 */
bool wallet_verify_password(const char* password) {
    if (!password) {
        return false;
    }

    return simple_hash(password) == g_wallet_system.password_hash;
}

/**
 * Encrypt wallet data
 */
bool wallet_encrypt_data(void) {
    // TODO: Implement proper encryption
    // For now, just mark as encrypted
    g_wallet_system.is_encrypted = true;
    return true;
}

/**
 * Decrypt wallet data
 */
bool wallet_decrypt_data(void) {
    // TODO: Implement proper decryption
    // For now, just mark as decrypted
    g_wallet_system.is_encrypted = false;
    return true;
}

/**
 * Get global wallet system instance
 */
WalletSystem* wallet_system_get_instance(void) {
    return &g_wallet_system;
}
