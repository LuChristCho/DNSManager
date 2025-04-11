# DNS Manager - Quick DNS Switching Tool  

![Application Icon](logo.png)  

## Description  
This is a lightweight DNS changer application written in C for Windows. It allows users to quickly switch their DNS settings without navigating through complex network configurations.  

## Features  
- One-click DNS switching  
- Supports custom DNS configurations  
- Simple and intuitive interface  
- Pre-configured default DNS servers  
- Minimal resource usage  

## Usage  
1. Run `program.exe` **as Administrator**.  
2. The application will automatically apply the configured DNS settings.  
3. Restart your system to revert to previous DNS settings.  

## Custom DNS Configuration  
To use your preferred DNS servers:  
1. Create a text file named `dns_config.txt` in the same directory as the executable.  
2. Add your desired DNS addresses, one per line (max two servers).  

Example `dns_config.txt`:  
```
8.8.8.8  
8.8.4.4  
```  

## Default DNS Servers  
If the configuration file is missing, empty, or unreadable, the application will use these fallback DNS servers:  
- `185.51.200.2`  
- `178.22.122.100`  

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