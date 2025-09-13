#include <windows.h>
#include <stdio.h>
#include <string.h>

#define MAX_DNS_LENGTH 16
#define MAX_DNS_NAME_LENGTH 50
#define MAX_DNS_ENTRIES 20
#define DATA_FILE "dns_data.txt"

#define IDC_SET_DNS_BUTTON 1
#define IDC_RESET_DNS_BUTTON 2
#define IDC_ADD_DNS_BUTTON 3
#define IDC_REMOVE_DNS_BUTTON 4
#define IDC_DNS_COMBOBOX 5
#define IDC_NAME_EDIT 6
#define IDC_PRIMARY_EDIT 7
#define IDC_SECONDARY_EDIT 8
#define IDC_LOG_BOX 9

typedef struct {
    char name[MAX_DNS_NAME_LENGTH];
    char primary[MAX_DNS_LENGTH];
    char secondary[MAX_DNS_LENGTH];
} DNSEntry;

static DNSEntry dnsList[MAX_DNS_ENTRIES];
static int dnsCount = 0;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void setDNS(const char* primaryDNS, const char* secondaryDNS);
void resetDNS();
void updateLog(HWND hwnd, const char *message);
void initializeDNSList();
void removeDNS(int index, HWND hCombo);
void loadDNSFromFile(HWND hCombo); 
void saveDNSToFile();   

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "DNSChanger";
    
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, "DNS Manager", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 420, 480,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND hCombo, hLogBox, hNameEdit, hPrimaryEdit, hSecondaryEdit;

    switch (uMsg) {
        case WM_CREATE: {
            CreateWindow("STATIC", "Select DNS:", WS_VISIBLE | WS_CHILD, 20, 20, 100, 20, hwnd, NULL, NULL, NULL);
            hCombo = CreateWindow("COMBOBOX", "", CBS_DROPDOWNLIST | WS_VISIBLE | WS_CHILD | WS_VSCROLL, 20, 45, 250, 200, hwnd, (HMENU)IDC_DNS_COMBOBOX, NULL, NULL);
            CreateWindow("BUTTON", "Set Selected DNS", WS_VISIBLE | WS_CHILD, 280, 45, 120, 25, hwnd, (HMENU)IDC_SET_DNS_BUTTON, NULL, NULL);
            CreateWindow("BUTTON", "Reset to DHCP", WS_VISIBLE | WS_CHILD, 280, 80, 120, 25, hwnd, (HMENU)IDC_RESET_DNS_BUTTON, NULL, NULL);
            CreateWindow("STATIC", "Add/Remove DNS", WS_VISIBLE | WS_CHILD | SS_LEFT, 20, 120, 150, 20, hwnd, NULL, NULL, NULL);
            CreateWindow("STATIC", "Name:", WS_VISIBLE | WS_CHILD, 20, 150, 80, 20, hwnd, NULL, NULL, NULL);
            hNameEdit = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 110, 150, 280, 20, hwnd, (HMENU)IDC_NAME_EDIT, NULL, NULL);
            CreateWindow("STATIC", "Primary DNS:", WS_VISIBLE | WS_CHILD, 20, 180, 80, 20, hwnd, NULL, NULL, NULL);
            hPrimaryEdit = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 110, 180, 280, 20, hwnd, (HMENU)IDC_PRIMARY_EDIT, NULL, NULL);
            CreateWindow("STATIC", "Secondary DNS:", WS_VISIBLE | WS_CHILD, 20, 210, 90, 20, hwnd, NULL, NULL, NULL);
            hSecondaryEdit = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 110, 210, 280, 20, hwnd, (HMENU)IDC_SECONDARY_EDIT, NULL, NULL);
            CreateWindow("BUTTON", "Add DNS", WS_VISIBLE | WS_CHILD, 110, 240, 135, 30, hwnd, (HMENU)IDC_ADD_DNS_BUTTON, NULL, NULL);
            CreateWindow("BUTTON", "Remove Selected", WS_VISIBLE | WS_CHILD, 255, 240, 135, 30, hwnd, (HMENU)IDC_REMOVE_DNS_BUTTON, NULL, NULL);
            CreateWindow("STATIC", "Log:", WS_VISIBLE | WS_CHILD, 20, 280, 100, 20, hwnd, NULL, NULL, NULL);
            hLogBox = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_READONLY | WS_VSCROLL, 20, 300, 370, 120, hwnd, (HMENU)IDC_LOG_BOX, NULL, NULL);
            
            loadDNSFromFile(hCombo); 
            updateLog(hLogBox, "Important: This app only works properly if run as administrator.");
            break;
        }

        case WM_COMMAND: {
            int wmId = LOWORD(wParam);
            switch (wmId) {
                case IDC_SET_DNS_BUTTON: {
                    int selectedIndex = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
                    if (selectedIndex != CB_ERR) {
                        setDNS(dnsList[selectedIndex].primary, dnsList[selectedIndex].secondary);
                        char logMessage[256];
                        snprintf(logMessage, sizeof(logMessage), "DNS set to: %s (%s, %s)",
                                 dnsList[selectedIndex].name, dnsList[selectedIndex].primary, dnsList[selectedIndex].secondary);
                        updateLog(hLogBox, logMessage);
                    } else {
                        updateLog(hLogBox, "Error: No DNS selected.");
                    }
                    break;
                }
                case IDC_RESET_DNS_BUTTON: {
                    resetDNS();
                    updateLog(hLogBox, "DNS reset to default (DHCP).");
                    break;
                }
                case IDC_ADD_DNS_BUTTON: {
                    if (dnsCount >= MAX_DNS_ENTRIES) {
                        updateLog(hLogBox, "Error: DNS list is full.");
                        break;
                    }
                    
                    char name[MAX_DNS_NAME_LENGTH], primary[MAX_DNS_LENGTH], secondary[MAX_DNS_LENGTH];
                    GetWindowText(hNameEdit, name, sizeof(name));
                    GetWindowText(hPrimaryEdit, primary, sizeof(primary));
                    GetWindowText(hSecondaryEdit, secondary, sizeof(secondary));

                    if (strlen(name) == 0 || strlen(primary) == 0) {
                        updateLog(hLogBox, "Error: Name and Primary DNS fields are required.");
                        break;
                    }

                    strcpy(dnsList[dnsCount].name, name);
                    strcpy(dnsList[dnsCount].primary, primary);
                    strcpy(dnsList[dnsCount].secondary, secondary);
                    
                    SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)name);
                    dnsCount++;
                    
                    saveDNSToFile();

                    char logMessage[256];
                    snprintf(logMessage, sizeof(logMessage), "Added new DNS: %s. List saved.", name);
                    updateLog(hLogBox, logMessage);

                    SetWindowText(hNameEdit, "");
                    SetWindowText(hPrimaryEdit, "");
                    SetWindowText(hSecondaryEdit, "");
                    break;
                }
                case IDC_REMOVE_DNS_BUTTON: {
                    int selectedIndex = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
                    if (selectedIndex != CB_ERR) {
                        char removedName[MAX_DNS_NAME_LENGTH];
                        strcpy(removedName, dnsList[selectedIndex].name);
                        
                        removeDNS(selectedIndex, hCombo);
                        
                        saveDNSToFile();

                        char logMessage[256];
                        snprintf(logMessage, sizeof(logMessage), "Removed DNS: %s. List saved.", removedName);
                        updateLog(hLogBox, logMessage);
                    } else {
                        updateLog(hLogBox, "Error: No DNS selected to remove.");
                    }
                    break;
                }
            }
            break;
        }

        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


void saveDNSToFile() {
    FILE* file = fopen(DATA_FILE, "w");
    if (!file) {
        return;
    }
    for (int i = 0; i < dnsCount; i++) {
        fprintf(file, "%s,%s,%s\n", dnsList[i].name, dnsList[i].primary, dnsList[i].secondary);
    }
    fclose(file);
}

void loadDNSFromFile(HWND hCombo) {
    FILE* file = fopen(DATA_FILE, "r");
    if (!file) {
        initializeDNSList();
        saveDNSToFile();
    } else {
        char line[256];
        dnsCount = 0;
        while (fgets(line, sizeof(line), file) && dnsCount < MAX_DNS_ENTRIES) {
            char* name = strtok(line, ",");
            char* primary = strtok(NULL, ",");
            char* secondary = strtok(NULL, "\n");

            if (name && primary) {
                strcpy(dnsList[dnsCount].name, name);
                strcpy(dnsList[dnsCount].primary, primary);
                if (secondary) {
                    strcpy(dnsList[dnsCount].secondary, secondary);
                } else {
                    dnsList[dnsCount].secondary[0] = '\0';
                }
                dnsCount++;
            }
        }
        fclose(file);
    }

    for (int i = 0; i < dnsCount; i++) {
        SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)dnsList[i].name);
    }
    if (dnsCount > 0) {
        SendMessage(hCombo, CB_SETCURSEL, 0, 0);
    }
}


void initializeDNSList() {
    dnsCount = 0;
    strcpy(dnsList[dnsCount].name, "Shecan");
    strcpy(dnsList[dnsCount].primary, "178.22.122.100");
    strcpy(dnsList[dnsCount].secondary, "185.51.200.2");
    dnsCount++;

    strcpy(dnsList[dnsCount].name, "Google");
    strcpy(dnsList[dnsCount].primary, "8.8.8.8");
    strcpy(dnsList[dnsCount].secondary, "8.8.4.4");
    dnsCount++;

    strcpy(dnsList[dnsCount].name, "Cloudflare");
    strcpy(dnsList[dnsCount].primary, "1.1.1.1");
    strcpy(dnsList[dnsCount].secondary, "1.0.0.1");
    dnsCount++;
}


void removeDNS(int index, HWND hCombo) {
    if (index < 0 || index >= dnsCount) return;

    for (int i = index; i < dnsCount - 1; i++) {
        dnsList[i] = dnsList[i + 1];
    }
    dnsCount--;
    SendMessage(hCombo, CB_DELETESTRING, index, 0);

    if (dnsCount > 0) {
        SendMessage(hCombo, CB_SETCURSEL, (index > 0 ? index - 1 : 0), 0);
    } else {
        SendMessage(hCombo, CB_SETCURSEL, (WPARAM)-1, 0);
    }
}

void setDNS(const char* primaryDNS, const char* secondaryDNS) {
    char command[256];
    
    snprintf(command, sizeof(command), "netsh interface ip set dns name=\"Wi-Fi\" static %s", primaryDNS);
    system(command);
    
    if (secondaryDNS != NULL && secondaryDNS[0] != '\0') {
        snprintf(command, sizeof(command), "netsh interface ip add dns name=\"Wi-Fi\" %s index=2", secondaryDNS);
        system(command);
    }
}

void resetDNS() {
    system("netsh interface ip set dns name=\"Wi-Fi\" dhcp");
}

void updateLog(HWND hwnd, const char *message) {
    int len = GetWindowTextLength(hwnd);
    SendMessage(hwnd, EM_SETSEL, len, len);
    SendMessage(hwnd, EM_REPLACESEL, 0, (LPARAM)message);
    SendMessage(hwnd, EM_REPLACESEL, 0, (LPARAM)"\r\n");
}
