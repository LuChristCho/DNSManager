# DNS Manager - Quick DNS Switching Tool  

![Application Icon](assets/logo.png)  

## Description  
This is a lightweight DNS changer application written in C for Windows. It allows users to quickly switch their DNS settings without navigating through complex network configurations.  

## Features  
- One-click DNS switching  
- Supports custom DNS configurations  
- Simple and intuitive interface  
- Pre-configured default DNS servers  
- Minimal resource usage  

## Usage (Portable Version)
1. Download the latest portable version from [here](https://github.com/LuChristCho/DNSManager/releases/download/v1.3/portable.exe).
2. Run `portable.exe` **always as Administrator**.  

## Usage (Installer)
1. Download the latest setup file from [here](https://github.com/LuChristCho/DNSManager/releases/download/v1.3/setupfile.exe).
2. Run the installer as administrator and complete the installation process.
3. Run the program.
4. Make sure the program is run as **administrator**. 

## Compilation Instructions  
To compile from source (requires Mingw):  
1. Place these files in the same directory:  
   - `main.c` (root folder)  
   - `icon.ico` & `resource.rc` (from assets folder)  
2. Run these commands:  

```bash
windres resource.rc -O coff -o resource.res  
gcc main.c resource.res -o program.exe -mwindows  
```  

## Requirements  
- Windows OS  
- Administrator privileges  

## License  
Released under the [MIT License](LICENSE).  

---  
*Note: This tool modifies system network settings. Use with caution.*
