/**
 * @file crypto_types.h
 * @brief Cryptocurrency type definitions and validation
 * 
 * This header defines the structures and functions for managing
 * different cryptocurrency types, their address formats, and
 * validation rules.
 * 
 * @author Claude
 * @date March 2025
 * @version 1.0.0
 */

 #ifndef CRYPTO_TYPES_H
 #define CRYPTO_TYPES_H
 
 #include <tonc.h>
 
 /**
  * Maximum number of cryptocurrency types that can be defined
  */
 #define MAX_CRYPTO_TYPES 10
 
 /**
  * Standard cryptocurrency type identifiers
  */
 typedef enum {
     CRYPTO_TYPE_BITCOIN = 0,
     CRYPTO_TYPE_ETHEREUM = 1,
     CRYPTO_TYPE_LITECOIN = 2,
     CRYPTO_TYPE_DOGECOIN = 3,
     CRYPTO_TYPE_CUSTOM_1 = 4,
     CRYPTO_TYPE_CUSTOM_2 = 5,
     CRYPTO_TYPE_CUSTOM_3 = 6,
     CRYPTO_TYPE_COUNT = 7  // Useful for iterations
 } CryptoType;
 
 /**
  * Address validation pattern
  * Defines rules for valid cryptocurrency addresses
  */
 typedef struct {
     bool requires_prefix;         // Whether a specific prefix is required
     const char* prefix;           // Required prefix (if applicable)
     int min_length;               // Minimum address length
     int max_length;               // Maximum address length
     bool allow_uppercase;         // Whether uppercase letters are allowed
     bool allow_special_chars;     // Whether special characters are allowed
     const char* valid_chars;      // Additional valid characters
 } AddressPattern;
 
 /**
  * Cryptocurrency type information
  * Contains all data about a cryptocurrency type
  */
 typedef struct {
     char name[16];                // Full name (e.g., "Bitcoin")
     char symbol[8];               // Symbol (e.g., "BTC")
     char prefix[8];               // URI prefix (e.g., "bitcoin:")
     u8 decimal_places;            // Decimal places for display
     AddressPattern pattern;       // Address validation pattern
     bool active;                  // Whether this type is active
     u16 color;                    // Display color
 } CryptoTypeInfo;
 
 /**
  * Initialize cryptocurrency types
  * Sets up built-in cryptocurrency types
  */
 void crypto_types_init(void);
 
 /**
  * Get cryptocurrency type information
  * @param type_index Index of cryptocurrency type
  * @return Pointer to type info or NULL if inactive/invalid
  */
 const CryptoTypeInfo* crypto_get_type_info(int type_index);
 
 /**
  * Set cryptocurrency type information
  * @param type_index Index of cryptocurrency type to update
  * @param info New type information
  * @return Success status
  */
 bool crypto_set_type_info(int type_index, const CryptoTypeInfo* info);
 
 /**
  * Validate a cryptocurrency address
  * @param address Address to validate
  * @param type_index Type of cryptocurrency
  * @return Whether address is valid for the given type
  */
 bool crypto_validate_address(const char* address, int type_index);
 
 /**
  * Get cryptocurrency type index by symbol
  * @param symbol Symbol to search for
  * @return Type index or -1 if not found
  */
 int crypto_get_type_by_symbol(const char* symbol);
 
 /**
  * Add a custom cryptocurrency type
  * @param info Type information
  * @return New type index or -1 on failure
  */
 int crypto_add_custom_type(const CryptoTypeInfo* info);
 
 /**
  * Validate an address using pattern rules
  * @param address Address to validate
  * @param pattern Validation pattern
  * @return Whether address matches pattern
  */
 bool crypto_validate_by_pattern(const char* address, const AddressPattern* pattern);
 
 /**
  * Generate a cryptocurrency address for testing
  * @param type Cryptocurrency type
  * @param output Output buffer for address
  * @param output_size Size of output buffer
  * @return Success status
  */
 bool crypto_generate_address_by_type(CryptoType type, char* output, int output_size);
 
 #endif // CRYPTO_TYPES_H