#!/bin/bash
# Script for compiling GBA Cryptocurrency QR Generator
# 
# This script compiles all components of the GBA Crypto QR Code Generator:
# - Menu system
# - QR generation and rendering
# - Wallet management
# - QR protection system
# - Debug logging
#
# Usage: ./compile_script.sh [clean]
# Adding "clean" parameter will remove all build files before compiling

# Exit on error
set -e

# Check if devkitPro is installed
if [ -z "$DEVKITPRO" ]; then
    echo "ERROR: devkitPro not found. Please install devkitPro first."
    echo "Visit https://devkitpro.org/wiki/Getting_Started for installation instructions."
    exit 1
fi

# Set environment variables
export DEVKITARM=$DEVKITPRO/devkitARM
export PATH=$DEVKITARM/bin:$PATH

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
    rm -rf $BUILD_DIR/*
fi

# Compile flags
CFLAGS="-mthumb -mthumb-interwork -mcpu=arm7tdmi -O2 -Wall -I$DEVKITPRO/libtonc/include -I$INCLUDE_DIR"
LDFLAGS="-mthumb -mthumb-interwork -specs=gba.specs -L$DEVKITPRO/libtonc/lib -ltonc"

# Source files by component
CORE_FILES="$CORE_DIR/main.c"
MENU_FILES="$MENU_DIR/menu_system.c $MENU_DIR/menu_definitions.c"
QR_FILES="$QR_DIR/qr_system.c $QR_DIR/qr_rendering.c $QR_DIR/qr_encoder.c $QR_DIR/reed_solomon.c"
WALLET_FILES="$WALLET_DIR/wallet_system.c $WALLET_DIR/wallet_menu.c $WALLET_DIR/wallet_menu_ext.c $WALLET_DIR/crypto_types.c"
PROTECTION_FILES="$PROTECTION_DIR/qr_protection.c $PROTECTION_DIR/qr_protection_menu.c $PROTECTION_DIR/qr_protection_integration.c"
DEBUG_FILES="$DEBUG_DIR/qr_debug.c"

# All source files
ALL_FILES="$CORE_FILES $MENU_FILES $QR_FILES $WALLET_FILES $PROTECTION_FILES $DEBUG_FILES"

# List of object files
OBJ_FILES=""

# Compile each file
echo "Compiling source files..."
for file in $ALL_FILES; do
    filename=$(basename "$file" .c)
    obj_file="$BUILD_DIR/${filename}.o"
    OBJ_FILES="$OBJ_FILES $obj_file"
    
    echo "  Compiling: $file"
    arm-none-eabi-gcc $CFLAGS -c "$file" -o "$obj_file"
done

# Link files
echo "Linking..."
arm-none-eabi-gcc $LDFLAGS -o "$BUILD_DIR/$PROJECT.elf" $OBJ_FILES

# Create GBA ROM
echo "Creating GBA ROM..."
arm-none-eabi-objcopy -O binary "$BUILD_DIR/$PROJECT.elf" "$BUILD_DIR/$PROJECT.gba"

# Pad ROM to valid size and set header using gbafix
echo "Validating ROM..."
if command -v gbafix &> /dev/null; then
    gbafix "$BUILD_DIR/$PROJECT.gba" -t"CryptoQR" -c"CRYP"
else
    echo "WARNING: gbafix not found. ROM may not have valid header."
fi

echo "Build completed successfully!"
echo "ROM file location: $BUILD_DIR/$PROJECT.gba"

# Show ROM size
ROM_SIZE=$(du -h "$BUILD_DIR/$PROJECT.gba" | cut -f1)
echo "ROM size: $ROM_SIZE"