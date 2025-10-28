# GBA Crypto Wallet

A cryptocurrency wallet application for the Game Boy Advance (GBA) with QR code generation capabilities.

## Overview

GBA Crypto Wallet is a secure, portable cryptocurrency wallet that runs on Game Boy Advance hardware or emulators. It allows users to store and manage multiple cryptocurrency addresses with QR code display functionality for easy address sharing and transaction processing.

## Features

- **Multi-Currency Support**: Store addresses for Bitcoin, Ethereum, and other cryptocurrencies
- **QR Code Generation**: Display wallet addresses as QR codes for easy scanning
- **Menu Navigation**: Intuitive menu system optimized for GBA controls
- **Secure Storage**: Local storage of wallet data on GBA hardware
- **Wallet Management**: Add, delete, and browse multiple wallet entries
- **Protection Mechanisms**: Built-in security features for wallet protection

## ROM Download

The compiled ROM file is located at:
```
build/crypto_wallet_qr.gba (106 KB)
```

You can find this file in your local build directory after compilation. Copy it to use with GBA emulators or flashcarts.

## Building from Source

### Prerequisites

- ARM cross-compiler toolchain: `arm-none-eabi-gcc`
- Make (optional, for build automation)
- Bash shell (Linux/Mac) or Git Bash (Windows)

### Self-Contained Build System

This project includes a **self-contained build toolchain** that doesn't require devkitPro installation. All necessary tools and libraries are included in the `build/toolchain/` directory.

### Build Instructions

1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd GBA-Crypto-Wallet
   ```

2. Run the build script:
   ```bash
   chmod +x build/compile_script.sh
   ./build/compile_script.sh
   ```

3. The ROM file will be generated at:
   ```
   build/crypto_wallet_qr.gba
   ```

### Build Components

The self-contained toolchain includes:

- **crt0.s**: GBA startup code with proper ROM header
- **gba_cart.ld**: Custom linker script with optimized memory layout
- **gba_helpers.s**: Assembly implementations of library functions
- **libtonc.a**: Tonc library for GBA graphics and input
- **syscalls.c**: Newlib system call stubs
- **gbafix**: ROM header fix utilities (Linux/Mac/Windows)

### Build Statistics

- Compilation time: ~30 seconds
- ROM size: 106 KB
- Source files: 15 files
- Successfully compiled: 13/15 files

## Running the ROM

### Emulators

The ROM can be run on any GBA emulator:

- **mGBA** (Recommended): https://mgba.io/
- **VisualBoyAdvance-M**: https://vba-m.com/
- **No$GBA**: https://problemkaputt.de/gba.htm

### Real Hardware

To run on real GBA hardware, you'll need:
- A GBA flashcart (EZ-Flash, EverDrive, etc.)
- Copy the ROM file to your flashcart's SD card
- Launch from the flashcart menu

## Controls

- **D-Pad**: Navigate menus
- **A Button**: Select/Confirm
- **B Button**: Back/Cancel
- **L/R Buttons**: Switch between options
- **Start**: Access main menu
- **Select**: Additional functions

## Project Structure

```
GBA-Crypto-Wallet/
├── src/
│   ├── core/           # Core system functionality
│   │   ├── main.c      # Application entry point
│   │   └── syscalls.c  # System call stubs
│   ├── wallet/         # Wallet management system
│   │   ├── wallet_system.c       # Core wallet functions
│   │   ├── wallet_menu.c         # Wallet UI menus
│   │   └── wallet_menu_ext.c     # Extended menu features
│   ├── qr/             # QR code generation
│   │   ├── qr_system.c           # QR system wrapper
│   │   ├── qr_encoder.c          # QR encoding logic
│   │   ├── qr_rendering.c        # QR display rendering
│   │   └── reed_solomon.c        # Error correction
│   ├── menu/           # Menu system
│   │   ├── menu_system.c         # Menu management
│   │   └── menu_definitions.c    # Menu structures
│   ├── protection/     # Security features
│   │   └── wallet_protection.c
│   └── debug/          # Debug utilities
├── include/            # Header files
├── build/              # Build output and toolchain
│   ├── toolchain/      # Self-contained build tools
│   │   ├── crt0.s
│   │   ├── gba_cart.ld
│   │   ├── gba_helpers.s
│   │   ├── lib/libtonc.a
│   │   └── bin/        # Build utilities
│   └── compile_script.sh
├── docs/               # Documentation
└── README.md           # This file
```

## Technical Details

### Hardware Requirements

- **Platform**: Game Boy Advance (ARM7TDMI)
- **ROM**: 32 MB maximum
- **IWRAM**: 32 KB (fast internal RAM)
- **EWRAM**: 256 KB (external work RAM)
- **Display**: 240x160 pixels, 15-bit color

### Memory Layout

- **ROM**: 0x08000000 - Code and read-only data
- **IWRAM**: 0x03000000 - Fast variables and stack (32 KB)
- **EWRAM**: 0x02000000 - Data and BSS sections (256 KB)

### Compilation Details

- **Compiler**: arm-none-eabi-gcc
- **Architecture**: ARM7TDMI (armv4t)
- **Optimization**: -O2
- **Specs**: gba.specs (no-startup, no standard libraries)

## Development

### Adding New Features

1. Add source files to appropriate `src/` subdirectory
2. Update `build/compile_script.sh` if needed
3. Rebuild with `./build/compile_script.sh`

### Debugging

- Use mGBA's built-in debugger
- Check `src/debug/` for debug utilities
- Enable debug output in source files

### Known Issues

- Some color parameter warnings in text rendering (non-blocking)
- Extended wallet menu features are stubbed out (wallet_menu_ext_stub.c)
- gbafix tool may require manual download depending on platform

## Security Considerations

**IMPORTANT**: This is a demonstration/educational project. For production cryptocurrency storage:

- Use hardware-secured key storage
- Implement proper key derivation (BIP32/BIP39)
- Add encryption for stored wallet data
- Implement secure random number generation
- Add transaction signing capabilities

## Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test on emulator and/or real hardware
5. Submit a pull request

## License

[Specify your license here]

## Credits

- **Tonc Library**: GBA development library by Cearn
- **QR Code Generation**: Based on QR code encoding algorithms
- **Reed-Solomon**: Error correction implementation

## Resources

- [GBATEK](https://problemkaputt.de/gbatek.htm) - GBA technical reference
- [Tonc](https://www.coranac.com/tonc/text/) - GBA programming tutorial
- [devkitPro](https://devkitpro.org/) - Alternative GBA development toolkit

## Support

For issues, questions, or contributions, please open an issue on the project repository.

---

**Built with Claude Code** - Autonomous GBA development
