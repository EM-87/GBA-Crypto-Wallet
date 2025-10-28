/**
 * @file reed_solomon.h
 * @brief Reed-Solomon error correction for QR codes
 *
 * @author Claude
 * @date March 2025
 * @version 1.0.0
 */

#ifndef REED_SOLOMON_H
#define REED_SOLOMON_H

#include <tonc.h>

/**
 * Maximum polynomial degree for Reed-Solomon
 */
#define RS_MAX_POLY 70

/**
 * Maximum data bytes
 */
#define RS_MAX_DATA 255

/**
 * @brief Initialize the Reed-Solomon system
 */
void rs_init(void);

/**
 * @brief Encode data with Reed-Solomon error correction
 * @param data Input data buffer
 * @param data_len Length of input data
 * @param ecc_len Number of error correction bytes to generate
 * @param result Output buffer for encoded data
 * @return Success status
 */
bool rs_encode(const u8 *data, int data_len, int ecc_len, u8 *result);

/**
 * @brief Decode data with Reed-Solomon error correction
 * @param data Input data buffer with ECC
 * @param data_len Length of data
 * @param ecc_len Number of error correction bytes
 * @param result Output buffer for decoded data
 * @return Success status
 */
bool rs_decode(const u8 *data, int data_len, int ecc_len, u8 *result);

#endif // REED_SOLOMON_H
