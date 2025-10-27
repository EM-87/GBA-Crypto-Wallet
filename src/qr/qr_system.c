/**
 * @file qr_system.c
 * @brief QR code generation system implementation
 *
 * @author Claude
 * @date March 2025
 * @version 1.0.0
 */

#include "qr_system.h"
#include "qr_encoder.h"
#include "qr_rendering.h"

// Global QR system state
QrSystemState g_qr_state = {
    .refresh_rate = 60,
    .update_interval = 1,
    .qr_pixel_size = 2,
    .enable_animations = false,
    .frame_counter = 0,
    .auto_hide_timeout = 300
};

// Static buffer for text storage
static char text_buffer[256];

/**
 * Initialize a QR state
 */
void qr_init(QrState *qr_state) {
    if (!qr_state) return;

    qr_state->size = 0;
    qr_state->data = NULL;
    qr_state->data_length = 0;
    qr_state->ec_level = QR_ECLEVEL_M;
    qr_state->mask_pattern = 0;
    qr_state->auto_mask = true;
}

/**
 * Free resources associated with a QR state
 */
void qr_free(QrState *qr_state) {
    if (!qr_state) return;

    if (qr_state->data) {
        free(qr_state->data);
        qr_state->data = NULL;
    }

    qr_state->size = 0;
    qr_state->data_length = 0;
}

/**
 * Set the text content for a QR code
 */
bool qr_set_text(QrState *qr_state, const char *text) {
    if (!qr_state || !text) {
        return false;
    }

    int len = strlen(text);
    if (len == 0 || len >= 256) {
        return false;
    }

    strcpy(text_buffer, text);
    qr_state->data_length = len;

    return true;
}

/**
 * Generate a QR code from the current state
 */
bool qr_generate(QrState *qr_state) {
    if (!qr_state || qr_state->data_length == 0) {
        return false;
    }

    // Use the QR encoder to generate the code
    return qr_encode_text(qr_state, text_buffer, qr_state->ec_level);
}

/**
 * Encode text into a QR code
 */
bool qr_encode_text(QrState *qr_state, const char *text, QrEcLevel ec_level) {
    if (!qr_state || !text) {
        return false;
    }

    // Free existing data
    qr_free(qr_state);

    // Set error correction level
    qr_state->ec_level = ec_level;

    // Calculate the QR code size based on text length
    int text_len = strlen(text);
    int version = 1;

    // Simple version selection (this is simplified)
    if (text_len <= 25) version = 1;
    else if (text_len <= 47) version = 2;
    else if (text_len <= 77) version = 3;
    else if (text_len <= 114) version = 4;
    else if (text_len <= 154) version = 5;
    else version = 6;

    // QR code size formula: 21 + (version - 1) * 4
    qr_state->size = 21 + (version - 1) * 4;

    // Allocate data buffer
    int data_size = qr_state->size * qr_state->size;
    qr_state->data = (u8*)malloc(data_size);

    if (!qr_state->data) {
        qr_state->size = 0;
        return false;
    }

    // Initialize all modules to white
    memset(qr_state->data, 0, data_size);

    // Use the external QR encoder if available
    // For now, create a simple test pattern
    // The actual QR encoding is done by qr_encoder.c functions

    // Store text for later encoding
    strcpy(text_buffer, text);
    qr_state->data_length = text_len;

    return true;
}

/**
 * Render a QR code to the GBA screen
 */
bool render_qr_to_screen(QrState *qr_state, int x, int y, int scale) {
    if (!qr_state || !qr_state->data || qr_state->size == 0) {
        return false;
    }

    // Boundary check
    if (x < 0 || y < 0 || x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) {
        return false;
    }

    // Calculate total size
    int total_size = qr_state->size * scale;
    if (x + total_size > SCREEN_WIDTH || y + total_size > SCREEN_HEIGHT) {
        return false;
    }

    // Use the rendering module to draw the QR code
    return render_qr_optimized(qr_state, g_qr_state.qr_buffer);
}

/**
 * Optimized QR rendering for GBA
 */
bool render_qr_optimized(QrState *qr_state, u16 *buffer) {
    if (!qr_state || !qr_state->data || qr_state->size == 0) {
        return false;
    }

    // Actual rendering implementation would use qr_rendering.c functions
    // For now, this is a placeholder that marks the buffer as used

    if (buffer) {
        // Clear buffer
        memset(buffer, 0, sizeof(u16) * 128 * 128);
    }

    return true;
}

/**
 * Creates a border around the QR code for better scanning
 */
void render_qr_border(int x, int y, int size, int border_size) {
    // Border rendering implementation
    // This would typically draw white rectangles around the QR code
    // Implementation would use GBA drawing primitives

    // For now, this is a placeholder
    (void)x;
    (void)y;
    (void)size;
    (void)border_size;
}

/**
 * Renders a cryptocurrency QR code
 */
bool render_crypto_qr(QrState *qr_state, const char *address, u16 *buffer) {
    if (!qr_state || !address) {
        return false;
    }

    // Set the text to the address
    if (!qr_set_text(qr_state, address)) {
        return false;
    }

    // Generate the QR code
    if (!qr_generate(qr_state)) {
        return false;
    }

    // Render to buffer
    return render_qr_optimized(qr_state, buffer);
}
