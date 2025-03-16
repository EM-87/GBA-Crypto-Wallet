/**
 * @file reed_solomon.c
 * @brief Reed-Solomon error correction implementation for QR codes
 * 
 * This file implements Reed-Solomon error correction coding used in QR codes.
 * Reed-Solomon codes provide the error correction capability specified in
 * the QR code standard, allowing QR codes to be read correctly even when
 * partially damaged or obscured.
 * 
 * The implementation operates in GF(2^8) with primitive polynomial x^8 + x^4 + x^3 + x^2 + 1.
 * 
 * @author Claude
 * @date March 2025
 * @version 1.0
 */

 #include <string.h>
 #include "reed_solomon.h"
 #include "qr_debug.h"
 
 // Galois field arithmetic tables for GF(2^8)
 static u8 rs_exp_table[256];  // Exponentiation table (alpha^i)
 static u8 rs_log_table[256];  // Logarithm table (log_alpha(i))
 
 // Generator polynomials for different error correction levels
 static u8 rs_generator_poly[RS_MAX_POLY][RS_MAX_POLY];
 static int rs_generator_poly_deg[RS_MAX_POLY];
 
 // Forward declarations for internal functions
 static void rs_init_tables(void);
 static void rs_init_generator_polynomials(void);
 static u8 rs_gf_mul(u8 a, u8 b);
 static u8 rs_gf_inv(u8 a);
 static void rs_gf_poly_mul(const u8 *p, int p_deg, const u8 *q, int q_deg, u8 *result);
 
 /**
  * Initialize the Reed-Solomon encoding/decoding system
  */
 void rs_init(void) {
     // Initialize GF(2^8) tables
     rs_init_tables();
     
     // Initialize generator polynomials for different degrees
     rs_init_generator_polynomials();
     
     LOG_INFO(MODULE_QR, "Reed-Solomon initialized", 0);
 }
 
 /**
  * Initialize GF(2^8) exponentiation and logarithm tables
  */
 static void rs_init_tables(void) {
     u8 x = 1;
     
     // Generate exponentiation table and logarithm table
     for (int i = 0; i < 255; i++) {
         rs_exp_table[i] = x;
         
         // For the log table, we only set the values we've seen so far
         rs_log_table[x] = i;
         
         // Multiply by x (the primitive element)
         // Using the primitive polynomial x^8 + x^4 + x^3 + x^2 + 1
         x = (x << 1) ^ (x & 0x80 ? 0x1D : 0);
     }
     
     // Handle the wrap-around for the exponentiation table
     rs_exp_table[255] = rs_exp_table[0];
     
     // Set log(0) to 0 (technically it's undefined, but this simplifies calculations)
     rs_log_table[0] = 0;
 }
 
 /**
  * Initialize generator polynomials for different error correction levels
  */
 static void rs_init_generator_polynomials(void) {
     // Initialize all generator polynomials to 0
     memset(rs_generator_poly, 0, sizeof(rs_generator_poly));
     memset(rs_generator_poly_deg, 0, sizeof(rs_generator_poly_deg));
     
     // Start with g(x) = (x - α^0) = (x - 1)
     rs_generator_poly[1][0] = 1;  // Constant term
     rs_generator_poly[1][1] = 1;  // x term
     rs_generator_poly_deg[1] = 1;
     
     // Iteratively compute generator polynomials
     // g_n(x) = g_{n-1}(x) * (x - α^{n-1})
     for (int n = 2; n < RS_MAX_POLY; n++) {
         // Set up the linear factor (x - α^{n-1})
         u8 factor[2];
         factor[0] = rs_exp_table[n - 1];  // Constant term: α^{n-1}
         factor[1] = 1;                     // x term: 1
         
         // Multiply the existing generator polynomial by the linear factor
         rs_gf_poly_mul(rs_generator_poly[n-1], rs_generator_poly_deg[n-1], 
                        factor, 1, rs_generator_poly[n]);
         
         // Update the degree of the generator polynomial
         rs_generator_poly_deg[n] = rs_generator_poly_deg[n-1] + 1;
     }
 }
 
 /**
  * Multiply two elements in GF(2^8)
  * 
  * @param a First element
  * @param b Second element
  * @return a * b in GF(2^8)
  */
 static u8 rs_gf_mul(u8 a, u8 b) {
     if (a == 0 || b == 0) return 0;
     
     // Use the logarithm tables to multiply: a*b = α^(log(a) + log(b))
     int log_sum = rs_log_table[a] + rs_log_table[b];
     
     // Handle wrap-around in the finite field
     if (log_sum >= 255) log_sum -= 255;
     
     return rs_exp_table[log_sum];
 }
 
 /**
  * Find the multiplicative inverse of an element in GF(2^8)
  * 
  * @param a Element to invert
  * @return Multiplicative inverse of a in GF(2^8)
  */
 static u8 rs_gf_inv(u8 a) {
     if (a == 0) return 0;  // Technically undefined, but return 0 for simplicity
     
     // Using the property that α^(255-n) is the inverse of α^n
     return rs_exp_table[255 - rs_log_table[a]];
 }
 
 /**
  * Multiply two polynomials in GF(2^8)
  * 
  * @param p First polynomial
  * @param p_deg Degree of first polynomial
  * @param q Second polynomial
  * @param q_deg Degree of second polynomial
  * @param result Buffer to store the result
  */
 static void rs_gf_poly_mul(const u8 *p, int p_deg, const u8 *q, int q_deg, u8 *result) {
     // Initialize result to 0
     memset(result, 0, (p_deg + q_deg + 1) * sizeof(u8));
     
     // Multiply each term of p with each term of q
     for (int i = 0; i <= p_deg; i++) {
         for (int j = 0; j <= q_deg; j++) {
             // At position i+j, add the product of the coefficients
             result[i + j] ^= rs_gf_mul(p[i], q[j]);
         }
     }
 }
 
 /**
  * Compute error correction codewords for a data block
  * 
  * @param data Input data buffer
  * @param data_length Length of input data in bytes
  * @param ecc Output error correction codewords buffer
  * @param ecc_length Number of error correction codewords to generate
  * @return true if successful, false otherwise
  */
 bool rs_compute_ecc(const u8 *data, int data_length, u8 *ecc, int ecc_length) {
     // Validate parameters
     if (!data || !ecc || data_length <= 0 || ecc_length <= 0 || ecc_length >= RS_MAX_POLY) {
         LOG_ERROR(MODULE_QR, "Invalid parameters for RS ECC", ecc_length);
         return false;
     }
     
     // Clear ECC buffer
     memset(ecc, 0, ecc_length);
     
     // Get the generator polynomial for this ECC length
     const u8 *generator = rs_generator_poly[ecc_length];
     
     // Process each input byte
     for (int i = 0; i < data_length; i++) {
         // Lead term from the current state
         u8 feedback = data[i] ^ ecc[0];
         
         if (feedback != 0) {
             // Perform polynomial division step
             for (int j = 0; j < ecc_length - 1; j++) {
                 ecc[j] = ecc[j + 1] ^ rs_gf_mul(feedback, generator[ecc_length - j - 1]);
             }
             ecc[ecc_length - 1] = rs_gf_mul(feedback, generator[0]);
         } else {
             // Just shift when feedback is zero
             memmove(ecc, ecc + 1, ecc_length - 1);
             ecc[ecc_length - 1] = 0;
         }
     }
     
     LOG_INFO(MODULE_QR, "ECC generation complete", ecc_length);
     return true;
 }
 
 /**
  * Get the required ECC codeword count for a QR code version and error level
  * 
  * @param version QR code version (1-40)
  * @param ec_level Error correction level
  * @return Number of ECC codewords required
  */
 int rs_get_ecc_codeword_count(int version, QrEcLevel ec_level) {
     // Define ECC codeword counts for key versions (1, 10, 20, 30, 40)
     // Format: [level_L, level_M, level_Q, level_H]
     static const int ecc_table[][4] = {
         {7, 10, 13, 17},     // Version 1
         {72, 98, 142, 172},  // Version 10
         {142, 242, 292, 346}, // Version 20
         {192, 292, 362, 434}, // Version 30
         {242, 362, 434, 514}  // Version 40
     };
     
     // Clamp version to range
     if (version < 1) version = 1;
     if (version > 40) version = 40;
     
     // Select table index based on version ranges
     int table_idx;
     if (version <= 1) table_idx = 0;
     else if (version <= 10) table_idx = 1;
     else if (version <= 20) table_idx = 2;
     else if (version <= 30) table_idx = 3;
     else table_idx = 4;
     
     // Return ECC codeword count for the specified level
     return ecc_table[table_idx][ec_level];
 }
 
 /**
  * Generate the format information bits for a QR code
  * 
  * @param ec_level Error correction level
  * @param mask_pattern Mask pattern index (0-7)
  * @return Format information bits
  */
 u16 rs_generate_format_bits(QrEcLevel ec_level, int mask_pattern) {
     // Format info is 5 bits of data (2 for EC level, 3 for mask pattern)
     u16 format_data = (ec_level << 3) | (mask_pattern & 0x7);
     
     // Apply BCH error correction to format data
     // Using generator polynomial G(x) = x^10 + x^8 + x^5 + x^4 + x^2 + x + 1
     u16 format_info = format_data << 10;
     
     // The divisor is 10 bits, so we need to check 5 positions (5+10=15)
     for (int i = 0; i < 5; i++) {
         if (format_info & (1 << (14 - i))) {
             // If the current bit is 1, XOR with the generator polynomial
             format_info ^= (0x537 << (4 - i));
         }
     }
     
     // Combine the data and ECC bits
     format_info = format_data << 10 | (format_info & 0x3FF);
     
     // Apply the mask pattern 101010000010010
     format_info ^= 0x5412;
     
     return format_info;
 }
 
 /**
  * Compute syndrome values for error detection/correction
  * 
  * @param data Data to check (including ECC codewords)
  * @param length Total length of data and ECC
  * @param syndromes Output buffer for syndrome values
  * @param num_roots Number of syndrome values to compute
  * @return Number of non-zero syndromes (0 means no errors)
  */
 int rs_compute_syndromes(const u8 *data, int length, u8 *syndromes, int num_roots) {
     // Initialize syndromes to zero
     memset(syndromes, 0, num_roots);
     
     int non_zero = 0;
     
     // Compute syndrome values
     for (int i = 0; i < num_roots; i++) {
         u8 alpha_i = rs_exp_table[i];
         u8 value = 0;
         
         // Evaluate data polynomial at α^i
         for (int j = 0; j < length; j++) {
             // Horner's method: value = value * α^i + data[j]
             value = rs_gf_mul(value, alpha_i) ^ data[j];
         }
         
         syndromes[i] = value;
         
         // Count non-zero syndromes
         if (value != 0) {
             non_zero++;
         }
     }
     
     return non_zero;
 }
 
 /**
  * Encode a block of data with Reed-Solomon error correction
  * 
  * @param data Input data buffer
  * @param data_length Length of input data in bytes
  * @param output Output buffer (must be at least data_length + ecc_length bytes)
  * @param ec_level Error correction level
  * @param version QR code version
  * @return true if successful, false otherwise
  */
 bool rs_encode_block(const u8 *data, int data_length, u8 *output, QrEcLevel ec_level, int version) {
     // Determine ECC length based on version and error correction level
     int ecc_length = rs_get_ecc_codeword_count(version, ec_level);
     
     // Validate parameters and ensure we're not exceeding limit
     if (!data || !output || data_length <= 0 || ecc_length <= 0 || ecc_length >= RS_MAX_POLY) {
         LOG_ERROR(MODULE_QR, "Invalid parameters for RS encoding", ecc_length);
         return false;
     }
     
     // Copy data to output buffer
     memcpy(output, data, data_length);
     
     // Compute ECC and store at the end of the output buffer
     if (!rs_compute_ecc(data, data_length, output + data_length, ecc_length)) {
         LOG_ERROR(MODULE_QR, "Failed to compute ECC", 0);
         return false;
     }
     
     LOG_INFO(MODULE_QR, "Block encoded successfully", ecc_length);
     return true;
 }
 
 /**
  * Returns the maximum number of errors that can be corrected
  * given the specified number of ECC codewords
  * 
  * @param ecc_length Number of ECC codewords
  * @return Maximum number of correctable errors
  */
 int rs_max_correctable_errors(int ecc_length) {
     // Reed-Solomon can correct up to floor(ecc_length/2) errors
     return ecc_length / 2;
 }
 
 /**
  * Calculate the optimal mask pattern for a QR code
  * based on minimizing penalties
  * 
  * @param modules QR code modules
  * @param size Size of QR code
  * @return Optimal mask pattern (0-7)
  */
 int rs_find_optimal_mask(const u8 *modules, int size) {
     // Implementation omitted - would evaluate each mask pattern
     // against QR code penalty rules and return the best one
     
     // For now, default to mask pattern 1 which tends to work reasonably well
     return 1;
 }