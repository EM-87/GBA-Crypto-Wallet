# QR Anti-Photography Protection System

## Overview

The QR Anti-Photography Protection system provides enhanced security for cryptocurrency addresses by making QR codes difficult to photograph while maintaining real-time scanning capability. This system is specifically designed for the Game Boy Advance Cryptocurrency QR Code Generator application.

## How It Works

The system generates multiple valid QR code variations that encode the same data but with different visual patterns. By rapidly switching between these patterns (5-10 times per second), the system creates a QR code that appears to shimmer or change when viewed in real-time, making it nearly impossible to capture via photography.

Key technical aspects:
- Multiple QR variations using different mask patterns
- Optional randomization of function patterns
- Adjustable refresh rate (5-10 FPS)
- Customizable error correction levels
- Optional module inversion for additional visual distortion

## Protection Levels

The system offers four protection levels:

1. **OFF** - Standard QR code display with no protection
2. **LOW** - Basic protection with 5 FPS refresh rate and normal QR redundancy
3. **MEDIUM** - Enhanced protection with 7 FPS refresh rate and reduced redundancy
4. **HIGH** - Maximum protection with 10 FPS refresh rate and minimal redundancy

## Custom Settings

Advanced users can customize the protection system with the following settings:

| Setting | Description | Range |
|---------|-------------|-------|
| Refresh Rate | How many times per second the QR code changes | 0-10 FPS |
| Mask Variations | Number of different mask patterns to use | 1-8 |
| Randomize Pattern | Slightly alter finder pattern positions | ON/OFF |
| Reduce ECC | Lower error correction capability | ON/OFF |
| Alt. Encoding | Use alternate encoding modes | ON/OFF |
| ECC Level | Error correction level | L, M, Q, H |
| Invert Modules | Randomly invert non-essential modules | ON/OFF |
| Invert Percentage | Percentage of modules to invert | 0-20% |

## Usage

1. Navigate to the Wallet Menu
2. Select "QR Protection"
3. Choose a protection level or customize settings
4. View any QR code in the wallet with protection enabled

## Notes on Effectiveness

- **Scanning Distance**: Higher protection levels may require scanning from a closer distance
- **Scanner Compatibility**: Most modern QR scanners can read protected QR codes in real-time
- **Photography Prevention**: Higher refresh rates are more effective at preventing photography
- **Reduced Redundancy**: Using lower ECC levels makes the QR code more vulnerable to damage or dirt

## Recommended Settings

For optimal balance between security and scanability:
- MEDIUM protection level
- 7 FPS refresh rate
- M level error correction
- 10% module inversion

For maximum security:
- HIGH protection level
- 10 FPS refresh rate
- L level error correction
- 20% module inversion
- Randomized function patterns

## Technical Implementation

The protection system is implemented in the following files:
- `qr_protection.h/c` - Core protection system
- `qr_protection_menu.h/c` - User interface for protection settings
- `qr_protection_integration.c` - Integration with wallet system

## Security Considerations

This protection system is designed to prevent casual photography of sensitive QR codes containing cryptocurrency addresses. While effective against standard cameras, it may not prevent all forms of capture. For instance:

- High-speed cameras (120+ FPS) may still capture a readable frame
- Video recording with frame-by-frame analysis may recover the QR code
- Specialized hardware designed to counter this protection might exist

Always treat cryptocurrency addresses with appropriate security measures and do not rely solely on this protection system for high-value transactions.

## Performance Impact

The protection system has a minimal performance impact on the GBA hardware:
- LOW level: Negligible impact
- MEDIUM level: Slight impact (<5% CPU usage)
- HIGH level: Moderate impact (~10% CPU usage)

Battery life may be slightly reduced when using HIGH protection level due to increased processing and screen refresh rate.

## Future Enhancements

Planned improvements to the protection system:
- Additional variation techniques
- Machine learning based patterns to defeat automated capture systems
- Adaptive refresh rate based on ambient light conditions