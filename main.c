#include <windows.h>

void setDNS();
void resetDNS();
void updateLog(HWND hwnd, const char *message);

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
                setDNS();
                updateLog(logBox, "DNS set to 178.22.122.100 and 185.51.200.2");
            } else if (LOWORD(wParam) == 2) {
                resetDNS();
                updateLog(logBox, "DNS reset to default");
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

void setDNS() {
    system("netsh interface ip set dns name=\"WiFi\" static 178.22.122.100");
    system("netsh interface ip add dns name=\"WiFi\" 185.51.200.2 index=2");
}

void resetDNS() {
    system("netsh interface ip set dns name=\"WiFi\" dhcp");
}

void updateLog(HWND hwnd, const char *message) {
    char buffer[512];
    GetWindowText(hwnd, buffer, sizeof(buffer));
    strcat(buffer, message);
    strcat(buffer, "\r\n");
    SetWindowText(hwnd, buffer);
}
