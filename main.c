#include <windows.h>
#include <stdio.h>
#include <string.h>

void setDNS(const char* primaryDNS, const char* secondaryDNS);
void resetDNS();
void updateLog(HWND hwnd, const char *message);
int readDNSFromFile(char* primaryDNS, char* secondaryDNS, size_t bufferSize);

#define MAX_DNS_LENGTH 16
#define CONFIG_FILE "dns_config.txt"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND logBox;

    switch (uMsg) {
        case WM_CREATE:
            CreateWindow("BUTTON", "Set DNS", WS_VISIBLE | WS_CHILD, 50, 50, 80, 30, hwnd, (HMENU)1, NULL, NULL);
            CreateWindow("BUTTON", "Reset DNS", WS_VISIBLE | WS_CHILD, 150, 50, 80, 30, hwnd, (HMENU)2, NULL, NULL);
            logBox = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_READONLY | WS_VSCROLL,
                                  20, 140, 260, 80, hwnd, NULL, NULL, NULL);
            break;
        case WM_COMMAND:
            if (LOWORD(wParam) == 1) {
                char primaryDNS[MAX_DNS_LENGTH] = {0};
                char secondaryDNS[MAX_DNS_LENGTH] = {0};
                int useDefault = 0;
                
                if (!readDNSFromFile(primaryDNS, secondaryDNS, MAX_DNS_LENGTH)) {
                    useDefault = 1;
                    strncpy(primaryDNS, "178.22.122.100", MAX_DNS_LENGTH);
                    strncpy(secondaryDNS, "185.51.200.2", MAX_DNS_LENGTH);
                }
                
                setDNS(primaryDNS, secondaryDNS);
                
                char logMessage[256];
                if (secondaryDNS[0] != '\0') {
                    snprintf(logMessage, sizeof(logMessage), 
                             "DNS set to %s (Primary) and %s (Secondary)%s", 
                             primaryDNS, secondaryDNS,
                             useDefault ? " (using default values)" : "");
                } else {
                    snprintf(logMessage, sizeof(logMessage), 
                             "DNS set to %s (Primary)%s", 
                             primaryDNS,
                             useDefault ? " (using default values)" : "");
                }
                updateLog(logBox, logMessage);
            } else if (LOWORD(wParam) == 2) {
                resetDNS();
                updateLog(logBox, "DNS reset to default (DHCP)");
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "DNSChanger";

    RegisterClass(&wc);

    HWND hwnd = CreateWindow("DNSChanger", "DNS Manager", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                             100, 100, 300, 280, NULL, NULL, hInstance, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

void setDNS(const char* primaryDNS, const char* secondaryDNS) {
    char command[256];
    
    snprintf(command, sizeof(command), "netsh interface ip set dns name=\"Wi-Fi\" static %s", primaryDNS);
    system(command);
    
    if (secondaryDNS[0] != '\0') {
        snprintf(command, sizeof(command), "netsh interface ip add dns name=\"Wi-Fi\" %s index=2", secondaryDNS);
        system(command);
    }
}

void resetDNS() {
    system("netsh interface ip set dns name=\"Wi-Fi\" dhcp");
}

void updateLog(HWND hwnd, const char *message) {
    char buffer[512];
    GetWindowText(hwnd, buffer, sizeof(buffer));
    strcat(buffer, message);
    strcat(buffer, "\r\n");
    SetWindowText(hwnd, buffer);
}

int readDNSFromFile(char* primaryDNS, char* secondaryDNS, size_t bufferSize) {
    FILE* file = fopen(CONFIG_FILE, "r");
    
    if (!file) {
        return 0;
    }
    
    if (!fgets(primaryDNS, bufferSize, file)) {
        fclose(file);
        return 0;
    }
    
    primaryDNS[strcspn(primaryDNS, "\r\n")] = '\0';
    
    if (fgets(secondaryDNS, bufferSize, file)) {
        secondaryDNS[strcspn(secondaryDNS, "\r\n")] = '\0';
    } else {
        secondaryDNS[0] = '\0';
    }
    
    fclose(file);
    return 1;
}