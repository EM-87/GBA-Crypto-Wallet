/**
 * @file qr_system.c
 * @brief QR code generation system implementation
 *
 * @author Claude
 * @date March 2025
 * @version 1.0.0
 */

#include "qr_system.h"

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

// Forward declarations for functions implemented in other files
extern bool qr_encode_text(QrState *qr_state, const char *text, QrEcLevel ec_level);
extern bool render_qr_to_screen(QrState *qr_state, int x, int y, int scale);

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
