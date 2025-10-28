#!/bin/bash
# Script for compiling GBA Cryptocurrency QR Generator
#
# This script uses a self-contained toolchain and does not require
# devkitPro installation. It includes:
# - Custom crt0.s startup code
# - Custom linker script (gba_cart.ld)
# - Pre-built libtonc.a library
# - gbafix wrapper (auto-downloads on first use)
#
# Usage: ./compile_script.sh [clean]
# Adding "clean" parameter will remove all build files before compiling

# Exit on error
set -e

# Change to project root directory
cd "$(dirname "$0")/.."

# Detect OS and configure gbafix
case "$(uname -s)" in
    Linux*)     GBAFIX=./build/toolchain/bin/gbafix ;;
    Darwin*)    GBAFIX=./build/toolchain/bin/gbafix-mac ;;
    MINGW*|MSYS*|CYGWIN*) GBAFIX="powershell -ExecutionPolicy Bypass -File ./build/toolchain/bin/gbafix.ps1" ;;
    *) echo "ERROR: Unsupported OS: $(uname -s)"; exit 1 ;;
esac

# Give execute permissions (in case they were lost)
chmod +x ./build/toolchain/bin/gbafix 2>/dev/null || true
chmod +x ./build/toolchain/bin/gbafix-mac 2>/dev/null || true

# Use local toolchain
export TOOLCHAIN_DIR=./build/toolchain
export LIBTONC=$TOOLCHAIN_DIR/lib/libtonc.a
export LDSCRIPT=$TOOLCHAIN_DIR/gba_cart.ld
export CRT0=$TOOLCHAIN_DIR/crt0.s

# Project name
PROJECT=crypto_wallet_qr

# Directory structure
SRC_DIR=src
BUILD_DIR=build
INCLUDE_DIR=include

# Source directories
CORE_DIR=$SRC_DIR/core
MENU_DIR=$SRC_DIR/menu
QR_DIR=$SRC_DIR/qr
WALLET_DIR=$SRC_DIR/wallet
PROTECTION_DIR=$SRC_DIR/protection
DEBUG_DIR=$SRC_DIR/debug

# Create build directory if it doesn't exist
mkdir -p $BUILD_DIR

# Clean build files if requested
if [ "$1" == "clean" ]; then
    echo "Cleaning build files..."
    rm -rf $BUILD_DIR/*.o $BUILD_DIR/*.elf $BUILD_DIR/*.gba
fi

# Check for libtonc
if [ ! -f "$LIBTONC" ]; then
    echo "ERROR: libtonc.a not found at $LIBTONC"
    echo "Please ensure the toolchain is properly set up."
    exit 1
fi

# Compile flags
CFLAGS="-mthumb -mthumb-interwork -mcpu=arm7tdmi -O2 -Wall -I/tmp/libtonc/include -I$INCLUDE_DIR"
CFLAGS="$CFLAGS -I$SRC_DIR/core -I$SRC_DIR/menu -I$SRC_DIR/qr -I$SRC_DIR/wallet -I$SRC_DIR/protection -I$SRC_DIR/debug"

# Linker flags
LDFLAGS="-mthumb -mthumb-interwork -mcpu=arm7tdmi -nostartfiles"
LDFLAGS="$LDFLAGS -T$LDSCRIPT"

# Source files by component
CORE_FILES="$CORE_DIR/main.c $CORE_DIR/syscalls.c"
MENU_FILES="$MENU_DIR/menu_system.c $MENU_DIR/menu_definitions.c"
QR_FILES="$QR_DIR/qr_system.c $QR_DIR/qr_rendering.c $QR_DIR/qr_encoder.c $QR_DIR/reed_solomon.c"
WALLET_FILES="$WALLET_DIR/wallet_system.c $WALLET_DIR/wallet_menu.c $WALLET_DIR/wallet_menu_ext_stub.c $WALLET_DIR/crypto_types.c"
PROTECTION_FILES="$PROTECTION_DIR/qr_protection.c $PROTECTION_DIR/qr_protection_menu.c $PROTECTION_DIR/qr_protection_integration.c"
DEBUG_FILES="$DEBUG_DIR/qr_debug.c"

# All source files
ALL_FILES="$CORE_FILES $MENU_FILES $QR_FILES $WALLET_FILES $PROTECTION_FILES $DEBUG_FILES"

# List of object files
OBJ_FILES=""

echo "=== GBA Crypto Wallet Compiler ==="
echo ""

# Compile crt0.s first

# Compile helper assembly functions
echo "Compiling helper functions..."
arm-none-eabi-as -mcpu=arm7tdmi "$TOOLCHAIN_DIR/gba_helpers.s" -o "$BUILD_DIR/gba_helpers.o"
OBJ_FILES="$OBJ_FILES $BUILD_DIR/gba_helpers.o"
echo "Compiling startup code..."
arm-none-eabi-as -mcpu=arm7tdmi "$CRT0" -o "$BUILD_DIR/crt0.o"
OBJ_FILES="$BUILD_DIR/crt0.o"

# Compile each C file
echo ""
echo "Compiling source files..."
for file in $ALL_FILES; do
    filename=$(basename "$file" .c)
    obj_file="$BUILD_DIR/${filename}.o"
    OBJ_FILES="$OBJ_FILES $obj_file"

    echo "  Compiling: $filename.c"
    arm-none-eabi-gcc $CFLAGS -c "$file" -o "$obj_file" 2>&1 | grep -v "warning:" || true
done

# Link files
echo ""
echo "Linking..."
arm-none-eabi-gcc $LDFLAGS -o "$BUILD_DIR/$PROJECT.elf" $OBJ_FILES "$BUILD_DIR/gba_helpers.o" $LIBTONC -lgcc

# Create GBA ROM
echo "Creating GBA ROM..."
arm-none-eabi-objcopy -O binary "$BUILD_DIR/$PROJECT.elf" "$BUILD_DIR/$PROJECT.gba"

# Pad and fix ROM header
echo "Fixing ROM header..."
$GBAFIX "$BUILD_DIR/$PROJECT.gba" -t"WALLET" -c"EDUA" -m"01" -r"00"

echo ""
echo "==================================="
echo "✅ Build completed successfully!"
echo "==================================="
echo ""
echo "ROM file: $BUILD_DIR/$PROJECT.gba"

# Show ROM size
if [ -f "$BUILD_DIR/$PROJECT.gba" ]; then
    ROM_SIZE=$(du -h "$BUILD_DIR/$PROJECT.gba" | cut -f1)
    echo "ROM size: $ROM_SIZE"
    echo ""
    echo "You can now run this ROM in a GBA emulator!"
fi
