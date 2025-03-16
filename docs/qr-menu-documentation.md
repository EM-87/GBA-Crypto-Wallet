# GBA QR Code Menu Integration

This documentation explains how to use the QR code functionality integrated into the GBA menu system.

## Overview

The QR code menu system allows users to:
- Generate cryptocurrency wallet addresses
- Display addresses as QR codes on the GBA screen
- Apply anti-photography protection to QR codes
- Navigate between different menu screens

## Implementation Details

### Files Added
- `qr_menu_integration.h` - Header for QR menu integration
- `qr_menu_integration.c` - Implementation of QR menu integration
- `qr_rendering_gba.c` - GBA-specific QR rendering functions
- `qr_protection.h` - QR code anti-photography protection
- `qr_protection.c` - Implementation of protection system
- `qr_protection_menu.h` - Menu interface for protection settings
- `qr_protection_menu.c` - Implementation of protection menu

### Files Modified
- `main.c` - Added QR menu option to main menu
- `wallet_menu.c` - Updated to include QR protection
- `gba-compile-script.sh` - Updated to include new QR-related files

## Menu Structure

The QR code functionality is integrated as a submenu in the main menu:

```
MENU PRINCIPAL
├── Iniciar Juego
├── Opciones
│   ├── Sonido
│   ├── Pantalla Completa
│   └── Volver
├── QR Code
│   ├── Generate New Address
│   └── Return to Menu
└── Salir
```

## User Guide

### Accessing the QR Menu

1. Navigate to the "QR Code" option in the main menu using the D-pad (Up/Down)
2. Press A to select the QR menu

### Generating a QR Code

1. In the QR Code menu, select "Generate New Address" and press A
2. The system will generate a new cryptocurrency address
3. The address will be displayed as a QR code on the screen

### QR Protection Settings

1. From the main wallet menu, navigate to the QR Protection option
2. Press A to access protection settings
3. Choose from several options:
   - Protection Level: Set to OFF, LOW, MEDIUM, or HIGH
   - Custom Settings: Fine-tune protection parameters
   - Help: View information about the protection system

### QR Protection Levels

- **OFF**: Standard QR codes with no protection
- **LOW**: Basic protection (5 FPS refresh rate)
- **MEDIUM**: Enhanced protection (7 FPS refresh rate)
- **HIGH**: Maximum protection (10 FPS refresh rate)

### Custom Protection Settings

For advanced users, custom parameters can be adjusted:
- Refresh Rate: How many FPS to use for QR variation (0-10)
- Mask Variations: Number of different mask patterns (1-8)
- Randomize Pattern: Alter finder pattern coordinates slightly
- Reduce ECC: Reduce error correction capability
- Alt. Encoding: Use alternate encoding modes
- ECC Level: Set error correction level (L, M, Q, H)
- Invert Modules: Randomly invert some modules
- Invert Percentage: Percentage of modules to invert (0-20%)

## Technical Implementation

### QR Code Generation

The QR code generation process follows these steps:
1. Receive cryptocurrency address input
2. Encode data with appropriate error correction
3. Generate QR matrix with finder patterns
4. Apply mask pattern for optimal readability
5. Render QR code to GBA screen

### QR Rendering

Two rendering methods are provided:
- `render_qr_to_screen`: Direct bitmap rendering (Mode 3)
- `render_qr_optimized`: Tile-based rendering for better performance

### Anti-Photography Protection

The anti-photography protection works by:
1. Generating multiple valid QR codes from the same data
2. Rapidly alternating between these variations (5-10 FPS)
3. Making the QR code appear to flicker when photographed
4. While remaining readable by a QR scanner in real-time

## Memory Considerations

The QR code generation and display requires:
- Approximately 5KB of ROM for code
- 1-4KB of RAM depending on QR complexity
- Additional VRAM usage for rendering

The implementation optimizes memory usage by:
- Reusing buffers for different operations
- Using tile-based rendering when possible
- Restricting QR code sizes to appropriate dimensions for GBA

## Developer Notes

### QR Module Size

The default QR module size is 2×2 pixels, which provides a good balance between size and readability. This can be adjusted in `qr_system.h`:

```c
// In qr_system.h
#define QR_PIXEL_SIZE 2  // Each QR module is 2x2 pixels
```

### Adding New QR Features

To add new QR features:
1. Create new menu options in `qr_menu_integration.c`
2. Implement handler functions for the new options
3. Update the rendering code if needed

### Error Correction

The QR system supports four error correction levels:
- L (Low): 7% recovery capacity
- M (Medium): 15% recovery capacity
- Q (Quartile): 25% recovery capacity
- H (High): 30% recovery capacity

Higher error correction allows the QR code to be read even if partially obscured or damaged, but requires a larger QR code for the same data.

## Future Enhancements

Possible enhancements for future versions:
- Support for more cryptocurrency types
- Animated QR codes with visual effects
- Multiple visual themes for QR display
- QR code scanning via GBA link cable
- Address history functionality
- Battery-backed SRAM storage for saved addresses

## Troubleshooting

Common issues and solutions:

1. **QR code too small to scan**: Increase QR_PIXEL_SIZE in qr_system.h
2. **QR protection causes scanning issues**: Lower the protection level or reduce refresh rate
3. **Memory allocation failures**: Reduce QR complexity or size
4. **Screen flicker**: Ensure VBlank synchronization is properly implemented

## API Reference

### Key Functions

- `qr_menu_init()`: Initialize the QR menu system
- `qr_generate_new_address()`: Generate a new cryptocurrency address
- `render_qr_to_screen(QrState*, int, int, int)`: Render QR code to screen
- `qr_protection_set_level(QrProtectionLevel)`: Set protection level
- `qr_protection_update()`: Update protection system (call every frame)
- `qr_apply_module_inversion(QrState*, u8)`: Apply module inversion effect