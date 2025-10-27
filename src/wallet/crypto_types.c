/**
 * @file crypto_types.c
 * @brief Implementation of extensible cryptocurrency types for the GBA wallet system
 * 
 * This file implements functions for managing cryptocurrency types,
 * including predefined types (Bitcoin, Ethereum, etc.) and custom
 * user-defined types. It also handles address validation according
 * to type-specific rules.
 * 
 * @author Claude
 * @date March 2025
 * @version 1.0.0
 */

 #include <string.h>
 #include "crypto_types.h"
 #include "qr_debug.h"
 
 // Storage for cryptocurrency types
 static CryptoTypeInfo g_crypto_types[MAX_CRYPTO_TYPES];
 
 /**
  * Initialize predefined cryptocurrency types
  */
 void crypto_types_init() {
     // Clear all entries
     memset(g_crypto_types, 0, sizeof(g_crypto_types));
     
     // Bitcoin
     CryptoTypeInfo btc = {
         .name = "Bitcoin",
         .symbol = "BTC",
         .prefix = "bitcoin:",
         .decimal_places = 8,
         .pattern = {
             .requires_prefix = false,
             .prefix = NULL,
             .min_length = 26,
             .max_length = 74,
             .allow_uppercase = false,
             .allow_special_chars = false,
             .valid_chars = NULL
         },
         .active = true,
         .color = RGB15(31, 20, 0)  // Orange for Bitcoin
     };
     crypto_set_type_info(CRYPTO_TYPE_BITCOIN, &btc);
     
     // Ethereum
     CryptoTypeInfo eth = {
         .name = "Ethereum",
         .symbol = "ETH",
         .prefix = "ethereum:",
         .decimal_places = 18,
         .pattern = {
             .requires_prefix = true,
             .prefix = "0x",
             .min_length = 42,  // 0x + 40 characters
             .max_length = 42,
             .allow_uppercase = true,
             .allow_special_chars = false,
             .valid_chars = NULL
         },
         .active = true,
         .color = RGB15(10, 10, 31)  // Blue for Ethereum
     };
     crypto_set_type_info(CRYPTO_TYPE_ETHEREUM, &eth);
     
     // Litecoin
     CryptoTypeInfo ltc = {
         .name = "Litecoin",
         .symbol = "LTC",
         .prefix = "litecoin:",
         .decimal_places = 8,
         .pattern = {
             .requires_prefix = false,
             .prefix = NULL,
             .min_length = 26,
             .max_length = 63,
             .allow_uppercase = false,
             .allow_special_chars = false,
             .valid_chars = NULL
         },
         .active = true,
         .color = RGB15(20, 20, 20)  // Silver for Litecoin
     };
     crypto_set_type_info(CRYPTO_TYPE_LITECOIN, &ltc);
     
     // Dogecoin
     CryptoTypeInfo doge = {
         .name = "Dogecoin",
         .symbol = "DOGE",
         .prefix = "dogecoin:",
         .decimal_places = 8,
         .pattern = {
             .requires_prefix = false,
             .prefix = NULL,
             .min_length = 27,
             .max_length = 35,
             .allow_uppercase = false,
             .allow_special_chars = false,
             .valid_chars = NULL
         },
         .active = true,
         .color = RGB15(31, 31, 0)  // Yellow for Dogecoin
     };
     crypto_set_type_info(CRYPTO_TYPE_DOGECOIN, &doge);
     
     // Custom types are left uninitialized
     LOG_INFO(MODULE_WALLET, "Cryptocurrency type system initialized", 4);
 }
 
 /**
  * Get information about a cryptocurrency type
  * 
  * @param type_index Index of the cryptocurrency type
  * @return Pointer to type information, or NULL if invalid or inactive
  */
 const CryptoTypeInfo* crypto_get_type_info(int type_index) {
     if (type_index < 0 || type_index >= MAX_CRYPTO_TYPES) {
         return NULL;
     }
     
     if (!g_crypto_types[type_index].active) {
         return NULL;
     }
     
     return &g_crypto_types[type_index];
 }
 
 /**
  * Set information for a cryptocurrency type
  * 
  * @param type_index Index of the cryptocurrency type to set
  * @param info Pointer to the type information
  * @return true if successful, false otherwise
  */
 bool crypto_set_type_info(int type_index, const CryptoTypeInfo* info) {
     if (type_index < 0 || type_index >= MAX_CRYPTO_TYPES || !info) {
         return false;
     }
     
     memcpy(&g_crypto_types[type_index], info, sizeof(CryptoTypeInfo));
     g_crypto_types[type_index].active = true;
     
     LOG_INFO(MODULE_WALLET, "Cryptocurrency type updated", type_index);
     return true;
 }
 
 /**
  * Validate a cryptocurrency address against a pattern
  * 
  * @param address Address to validate
  * @param pattern Pattern to validate against
  * @return true if address matches pattern, false otherwise
  */
 bool crypto_validate_by_pattern(const char* address, const AddressPattern* pattern) {
     if (!address || !pattern) {
         return false;
     }
     
     // Check length
     int len = strlen(address);
     if (len < pattern->min_length || len > pattern->max_length) {
         return false;
     }
     
     // Check prefix
     if (pattern->requires_prefix && pattern->prefix) {
         if (strncmp(address, pattern->prefix, strlen(pattern->prefix)) != 0) {
             return false;
         }
     }
     
     // Check characters
     for (int i = 0; i < len; i++) {
         char c = address[i];
         
         // Basic validation: alphanumeric
         bool valid = (c >= '0' && c <= '9') || 
                      (c >= 'a' && c <= 'z');
         
         // If uppercase is allowed
         if (pattern->allow_uppercase) {
             valid = valid || (c >= 'A' && c <= 'Z');
         }
         
         // If special characters are allowed or there's a valid chars list
         if (pattern->allow_special_chars) {
             valid = true;  // Accept all
         } else if (pattern->valid_chars) {
             // Check in list of valid chars
             const char* found = strchr(pattern->valid_chars, c);
             if (found) {
                 valid = true;
             }
         }
         
         if (!valid) {
             return false;
         }
     }
     
     return true;
 }
 
 /**
  * Validate a cryptocurrency address based on its type
  * 
  * @param address Address to validate
  * @param type_index Type of cryptocurrency
  * @return true if address is valid for the given type, false otherwise
  */
 bool crypto_validate_address(const char* address, int type_index) {
     if (!address || type_index < 0 || type_index >= MAX_CRYPTO_TYPES) {
         return false;
     }
     
     // Check that the type is active
     if (!g_crypto_types[type_index].active) {
         return false;
     }
     
     // Special cases for well-known cryptocurrencies
     if (type_index == CRYPTO_TYPE_BITCOIN) {
         // Bitcoin validation: multiple valid forms
         int len = strlen(address);
         
         // Legacy: starts with 1
         if (address[0] == '1' && len >= 26 && len <= 34) {
             return true;
         }
         
         // P2SH: starts with 3
         if (address[0] == '3' && len >= 26 && len <= 34) {
             return true;
         }
         
         // Bech32: starts with bc1
         if (strncmp(address, "bc1", 3) == 0 && len >= 14 && len <= 74) {
             return true;
         }
         
         return false;
     } else if (type_index == CRYPTO_TYPE_ETHEREUM) {
         // Ethereum validation
         if (strlen(address) != 42 || strncmp(address, "0x", 2) != 0) {
             return false;
         }
         
         // Check hex characters
         for (int i = 2; i < 42; i++) {
             char c = address[i];
             if (!((c >= '0' && c <= '9') || 
                  (c >= 'a' && c <= 'f') || 
                  (c >= 'A' && c <= 'F'))) {
                 return false;
             }
         }
         
         return true;
     } else if (type_index == CRYPTO_TYPE_LITECOIN) {
         // Litecoin validation
         int len = strlen(address);
         
         // Legacy: starts with L
         if (address[0] == 'L' && len >= 26 && len <= 34) {
             return true;
         }
         
         // P2SH: starts with M
         if (address[0] == 'M' && len >= 26 && len <= 34) {
             return true;
         }
         
         // Bech32: starts with ltc1
         if (strncmp(address, "ltc1", 4) == 0 && len >= 14 && len <= 63) {
             return true;
         }
         
         return false;
     } else if (type_index == CRYPTO_TYPE_DOGECOIN) {
         // Dogecoin validation
         int len = strlen(address);
         
         // Legacy: starts with D
         if (address[0] == 'D' && len >= 27 && len <= 35) {
             return true;
         }
         
         return false;
     }
     
     // For other currencies, use the pattern defined
     return crypto_validate_by_pattern(address, &g_crypto_types[type_index].pattern);
 }
 
 /**
  * Get cryptocurrency type index by symbol
  * 
  * @param symbol Symbol to look up
  * @return Index of the type, or -1 if not found
  */
 int crypto_get_type_by_symbol(const char* symbol) {
     if (!symbol) {
         return -1;
     }
     
     for (int i = 0; i < MAX_CRYPTO_TYPES; i++) {
         if (g_crypto_types[i].active && 
             strcmp(g_crypto_types[i].symbol, symbol) == 0) {
             return i;
         }
     }
     
     return -1;
 }
 
 /**
  * Add a new custom cryptocurrency type
  * 
  * @param info Type information to add
  * @return Index of the added type, or -1 if failed
  */
 int crypto_add_custom_type(const CryptoTypeInfo* info) {
     if (!info) {
         return -1;
     }
     
     // Look for a free slot
     for (int i = CRYPTO_TYPE_CUSTOM_1; i < MAX_CRYPTO_TYPES; i++) {
         if (!g_crypto_types[i].active) {
             crypto_set_type_info(i, info);
             return i;
         }
     }
     
     LOG_ERROR(MODULE_WALLET, "No space for new cryptocurrency types", 0);
     return -1;
 }
 
 /**
  * Get the name of a cryptocurrency by type
  * 
  * @param type Type index
  * @return Name of the cryptocurrency, or "Unknown" if invalid
  */
 const char* crypto_get_name(int type_index) {
     const CryptoTypeInfo* info = crypto_get_type_info(type_index);
     if (info) {
         return info->name;
     }
     return "Unknown";
 }
 
 /**
  * Get the symbol of a cryptocurrency by type
  * 
  * @param type Type index
  * @return Symbol of the cryptocurrency, or "???" if invalid
  */
 const char* crypto_get_symbol(int type_index) {
     const CryptoTypeInfo* info = crypto_get_type_info(type_index);
     if (info) {
         return info->symbol;
     }
     return "???";
 }
 
 /**
  * Generate a sample address for a given cryptocurrency type
  * Used for testing and demo purposes
  * 
  * @param type_index Type of cryptocurrency
  * @param output Buffer to store the generated address
  * @param size Size of the output buffer
  * @return true if successful, false otherwise
  */
 bool crypto_generate_sample_address(int type_index, char* output, int size) {
     if (!output || size <= 0 || type_index < 0 || type_index >= MAX_CRYPTO_TYPES) {
         return false;
     }
     
     // Generate different sample addresses based on type
     switch (type_index) {
         case CRYPTO_TYPE_BITCOIN:
             strncpy(output, "1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa", size);
             break;
             
         case CRYPTO_TYPE_ETHEREUM:
             strncpy(output, "0x71C7656EC7ab88b098defB751B7401B5f6d8976F", size);
             break;
             
         case CRYPTO_TYPE_LITECOIN:
             strncpy(output, "LM2WMpR1Rp6j3Sa59cMXJs1gKiL8dD7a4t", size);
             break;
             
         case CRYPTO_TYPE_DOGECOIN:
             strncpy(output, "DH5yaieqoZN36fDVciNyRueRGvGLR3mr7L", size);
             break;
             
         default:
             // For custom types, make a fake address based on the type
             const CryptoTypeInfo* info = crypto_get_type_info(type_index);
             if (info) {
                 snprintf(output, size, "%s-SAMPLE-ADDRESS-%03d", 
                          info->symbol, type_index);
             } else {
                 strncpy(output, "INVALID-ADDRESS", size);
                 return false;
             }
             break;
     }
     
     // Ensure null-termination
     output[size - 1] = '\0';
     return true;
 }