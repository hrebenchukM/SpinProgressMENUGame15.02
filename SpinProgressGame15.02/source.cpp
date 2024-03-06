#include <windows.h>
#include <windowsX.h>
#include <tchar.h>
#include <commctrl.h>
#include <vector>
#include <ctime>
#include <algorithm>
#include "resource.h"
#pragma comment(lib, "comctl32")
using namespace std;

int CALLBACK DlgProc(HWND hWnd, UINT mes, WPARAM wp, LPARAM lp);

int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrev, LPTSTR lpszCmdLine, int nCmdShow)
{
    return DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DlgProc);
}

HWND hDialog;
TCHAR text[300];
int count;
vector<int> numbers;

struct ButtonInfo {
    HWND hwnd;
    int number;
};

HMENU hMenu;

ButtonInfo buttons[16];
HWND hList;
int currentNumber = 0;
bool gameStarted = false;
int totalSeconds = 0;

int GetRandNum(int min, int max)
{
    return min + rand() % (max - min + 1);
}

void PlaceRandNum(HWND hWnd)
{
    numbers.clear(); 
    for (int i = 0; i < 16; ++i) {
        int randomNumber = GetRandNum(0, 100);
        TCHAR buffer[16];
        _stprintf_s(buffer, _T("%d"), randomNumber);
        SetDlgItemText(hWnd, IDC_BUTTON1 + i, buffer);
        buttons[i].hwnd = GetDlgItem(hWnd, IDC_BUTTON1 + i);
        buttons[i].number = randomNumber;
        numbers.push_back(randomNumber);
    }
    std::sort(numbers.begin(), numbers.end());
    SendMessage(hList, LB_RESETCONTENT, 0, 0);
}

void ResetGame(HWND hWnd) {
    gameStarted = false;
    currentNumber = 0;
    totalSeconds = 0;
    SendMessage(GetDlgItem(hWnd, IDC_PROGRESS1), PBM_SETPOS, 0, 0);
    SetDlgItemInt(hWnd, IDC_EDIT1, 0, FALSE);
}
void StartGame(HWND hWnd) {
    gameStarted = true;
    totalSeconds = SendMessage(GetDlgItem(hWnd, IDC_SPIN1), UDM_GETPOS32, 0, 0);
    if (!gameStarted) {
        SendMessage(GetDlgItem(hWnd, IDC_PROGRESS1), PBM_SETRANGE32, 0, totalSeconds);
        SendMessage(GetDlgItem(hWnd, IDC_PROGRESS1), PBM_SETPOS, 0, 0);
    }

    PlaceRandNum(hWnd);
    currentNumber = 0;
    SetDlgItemInt(hWnd, IDC_EDIT1, totalSeconds, FALSE);
    SetTimer(hWnd, 1, 1000, NULL);
    SendMessage(hList, LB_RESETCONTENT, 0, 0);
}
int CALLBACK DlgProc(HWND hWnd, UINT mes, WPARAM wp, LPARAM lp)
{
    switch (mes)
    {
    case WM_INITDIALOG:
    {

        
        HWND hProgress1 = GetDlgItem(hDialog, IDC_PROGRESS1);
        SendMessage(hProgress1, PBM_SETRANGE32, 0, 120); // Установка интервала для индикатора 
        SendMessage(hProgress1, PBM_SETSTEP, 1, 0); // Установка шага приращения  индикатора 
        SendMessage(hProgress1, PBM_SETPOS, 0, 0); // Установка текущей позиции индикатора


        SendMessage(GetDlgItem(hWnd, IDC_SPIN1), UDM_SETRANGE32, 0, 120);
        SendMessage(GetDlgItem(hWnd, IDC_SPIN1), UDM_SETBUDDY, (WPARAM)GetDlgItem(hWnd, IDC_EDIT1), 0); 
        SendDlgItemMessage(hWnd, IDC_SPIN1, UDM_SETPOS32, 0, 0); 
        SetWindowText(GetDlgItem(hWnd, IDC_EDIT1), TEXT("0")); 

        hMenu = LoadMenu(GetModuleHandle(0), MAKEINTRESOURCE(IDR_MENU1));
        SetMenu(hWnd, hMenu);
        hList = GetDlgItem(hWnd, IDC_LIST1);

        srand(static_cast<unsigned int>(time(nullptr)));
        PlaceRandNum(hWnd);





        break;
    }
    case WM_COMMAND:
    {
        switch (LOWORD(wp))
        {
        case IDC_BUTTON1:
        case IDC_BUTTON2:
        case IDC_BUTTON3:
        case IDC_BUTTON4:
        case IDC_BUTTON5:
        case IDC_BUTTON6:
        case IDC_BUTTON7:
        case IDC_BUTTON8:
        case IDC_BUTTON9:
        case IDC_BUTTON10:
        case IDC_BUTTON11:
        case IDC_BUTTON12:
        case IDC_BUTTON13:
        case IDC_BUTTON14:
        case IDC_BUTTON15:
        case IDC_BUTTON16:
        {
            if (gameStarted) {
                TCHAR buttonText[16];
                GetWindowText((HWND)lp, buttonText, sizeof(buttonText));
                int buttonNumber = _tstoi(buttonText);
                if (buttonNumber == numbers[currentNumber]) {
                    SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)buttonText);
                    currentNumber++;
                    if (currentNumber == 16) {
                        MessageBox(hWnd, L"Поздравляем! Вы выиграли!", L"Победа", MB_OK | MB_ICONINFORMATION);
                        ResetGame(hWnd);
                    }
                }
            }
            break;
        }

        case IDC_SPIN1:
        {
            if (HIWORD(wp) == UDN_DELTAPOS) {
                int newVal = (int)SendMessage(GetDlgItem(hWnd, IDC_SPIN1), UDM_GETPOS32, 0, 0);
                SetDlgItemInt(hWnd, IDC_EDIT1, newVal, FALSE);
                return TRUE;
            }
            break;
        }
        case ID_NewGame:
        {
            if (!gameStarted) {
                MessageBox(0, L"Новая игра", L"Новая игра", MB_OK);
                StartGame(hWnd);
            }

            break;
        }
        case ID_EXIT:
            MessageBox(0, L"Exit Game", L"Exit Game", MB_OK);
            EndDialog(hWnd, 0);
            break;
        default:
            break;
        }
        break;
    }
    case WM_TIMER:
    {
        if (gameStarted) {
            totalSeconds--;
            if (totalSeconds == 0) {
                MessageBox(hWnd, L"Время истекло! Вы проиграли!", L"Проигрыш", MB_OK | MB_ICONEXCLAMATION);
                ResetGame(hWnd);
                return TRUE;
            }
            SendMessage(GetDlgItem(hWnd, IDC_PROGRESS1), PBM_STEPIT, -1, 0);


            SetDlgItemInt(hWnd, IDC_EDIT1, totalSeconds, FALSE);
        }
        break;
    }



   
    case WM_CLOSE:
        EndDialog(hWnd, 0);
        return TRUE;
    }
    return FALSE;
}
