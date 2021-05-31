# AltServer Certificate Dumper

> A proxy DLL for Windows to dump your developer account certificate from AltServer.

There is no official way to obtain your developer account's certificate in Windows. [AltServer](https://altstore.io/) does this under the hood, but for (security?) reasons, the certificate never touches your disk unprotected.

This project is a simple patch that will hook into AltServer and dump your certificate and password to your Desktop as they are processed. The patch is completely transparent and will cause no side effects.

## Usage

### Installing

1. If you haven't already, install [AltServer](https://altstore.io/)
2. Download this project's proxy `zlib1.dll` from the [releases](https://github.com/SignTools/altserver-cert-dumper/releases)
3. Go to your AltServer's installation folder. By default, this is: `C:\Program Files (x86)\AltServer`
4. Inside AltServer's folder, rename the original `zlib1.dll` to `zlib2.dll`
5. Move this project's proxy `zlib1.dll`, the one you just downloaded, inside AltServer's folder. You should end up with both `zlib1.dll` and `zlib2.dll` next to each other. Nothing should have been replaced.

### Dumping

1. Run AltServer as normal
2. You should immediately see a message box saying: `Hooks initialized`. If you don't see this, then you didn't install the proxy correctly.
3. Proceed to install AltStore on your phone as normal. If you already have it installed, you will have to reinstall it.
4. At the end of the process, you will see a message box saying: `Certificate successfully saved to Desktop`. Then, on your desktop, you will find two new files: `AltServer-Cert.p12` and `AltServer-Cert-Pass.txt`. These are your certificate and password, respectively.

### Uninstalling

1. Go to your AltServer's installation folder. By default, this is: `C:\Program Files (x86)\AltServer`
2. Delete the proxy `zlib1.dll`
3. Rename the original `zlib2.dll` back to `zlib1.dll`

## Building

Tested and working is the following setup:

- mingw-w64 7.0.0+ (32-bit)
- CMake 3.16+

## References

Based on [PerfectProxyDLL](https://github.com/ViRb3/PerfectProxyDLL).
