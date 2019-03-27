// Win32Project1.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "Win32Project1.h"
#include <math.h>
#include <stdio.h>

#define MAX_LOADSTRING 100

HINSTANCE hInst;                                // текущий экземпляр
HWND hwndFrame;									// Фрейм
HWND hwndClient;								// Клиент
HWND hwndChildGraph;							// Первая Доча

// Глобальные переменные для графика:
const double PI = 3.1415926535897932384626433832795;
float rotkoeff = 0, intkoeff = 0;		// Контролируем интервал и коэффициент растяжения:

int TimerID, x2 = 10, y2 = 10, f = 0;
HPEN pen;
HBRUSH brush;

// Глобальные переменные для последовательности:
int nTimerID;
// Имена классов окон:
WCHAR szFrameClassName[MAX_LOADSTRING] = L"MDIAppClass";   // имя класса главного окна
WCHAR szChildClassName[MAX_LOADSTRING] = L"MDIchildAppClass";
WCHAR szSecondChildClassName[MAX_LOADSTRING] = L"MDISecondChildAppClass";
// Заголовок окна:
WCHAR szTitle[MAX_LOADSTRING] = L"MDI_15";                  // Текст строки заголовка

// Отправить объявления функций, включенных в этот модуль кода:
BOOL                InitApp(HINSTANCE);
LRESULT CALLBACK    FrameWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    ChildGraphWndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: разместите код здесь.

	hInst = hInstance;
	MSG msg;

	// Инициализация:
	if (!InitApp(hInstance))
	{
		return FALSE;
	}

	// Создаем фрейм:
	hwndFrame = CreateWindow(
		szFrameClassName,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0,
		CW_USEDEFAULT, 0,
		0,
		0,
		hInstance,
		NULL);

	// Не удалось - идем лесом:
	if (!hwndFrame)
		return FALSE;

	// Рисуем фрейм:
	ShowWindow(hwndFrame, nCmdShow);
	UpdateWindow(hwndFrame);

	// Цикл обработки сообщений и трансляция МДИ:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateMDISysAccel(hwndClient, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

//
//   ФУНКЦИЯ: InitApp(HINSTANCE)
//
//   НАЗНАЧЕНИЕ: регистрирует классы окон.
//

BOOL InitApp(HINSTANCE hInstance)
{
	ATOM aWndClass;
	WNDCLASS wc;

	// Регистрируем класс для фрейма:
	memset(&wc, 0, sizeof(wc));
	wc.lpszMenuName = MAKEINTRESOURCE(IDC_WIN32PROJECT1);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)FrameWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, L"APP_ICON");
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = szFrameClassName;
	aWndClass = RegisterClass(&wc);

	if (!aWndClass)
		return FALSE;

	// Регистрируем класс для дочи:
	memset(&wc, 0, sizeof(wc));
	wc.lpszMenuName = 0;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)ChildGraphWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32PROJECT1));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = szChildClassName;
	aWndClass = RegisterClass(&wc);

	if (!aWndClass)
		return FALSE;


	return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  НАЗНАЧЕНИЕ:  обрабатывает сообщения в главном окне.
//
//  WM_COMMAND — обработать меню приложения
//  WM_PAINT — отрисовать главное окно
//  WM_DESTROY — отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK FrameWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CLIENTCREATESTRUCT clcs;
	CLIENTCREATESTRUCT cls;
	MDICREATESTRUCT	   mdics;
	MDICREATESTRUCT	   mcs;
	RECT rect;
	HDC hdc;
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_CREATE:
	{
		// заполняем клиентскую структуру:
		clcs.hWindowMenu = GetSubMenu(GetMenu(hWnd), 0);
		clcs.idFirstChild = 500;
		// создаем клиента:
		hwndClient = CreateWindow(
			L"MDICLIENT",
			NULL,
			WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE |
			WS_HSCROLL | WS_VSCROLL,
			0, 0, 0, 0,
			hWnd,
			(HMENU)0,
			hInst,
			(LPSTR)&clcs);
		ShowWindow(hwndClient, SW_HIDE);
		UpdateWindow(hwndClient);
		break;
	}
	case WM_COMMAND:
	{
		switch (wParam)
		{
		case IDM_GRAPH:
		{
			ShowWindow(hwndClient, SW_SHOW);
			// заполняем мди структуру:
			mdics.szClass = szChildClassName;
			mdics.szTitle = L"Окно простых чисел";
			mdics.hOwner = hInst;
			mdics.x = CW_USEDEFAULT;
			mdics.y = CW_USEDEFAULT;
			mdics.cx = CW_USEDEFAULT;
			mdics.cy = CW_USEDEFAULT;
			mdics.style = 0;
			mdics.lParam = NULL;

			//посылаем сообщение WM_MDICREATE клиентскому окну
			// в реузльтате будет создано одно документное окно
			// в этом случае окно для графика функции 
			// y = k*ctg(x), k in R
			hwndChildGraph = (HWND)SendMessage(hwndClient,
				WM_MDICREATE, 0, (LPARAM)&mdics);
			break;
		}
		case timeron:
		{
			f = 1;
			ShowWindow(hwndClient, SW_HIDE);
			TimerID = SetTimer(hWnd, 1, 500, NULL);
			InvalidateRect(hWnd, NULL, true);
			break;
		}
		case timeroff:
		{
			f = 0;
			KillTimer(hWnd, 1);
			break;
		}
		case IDM_WINDOWCLOSEALL:
		{
			HWND hwndTemp;
			ShowWindow(hwndClient, SW_HIDE);
			for (;;)
			{
				hwndTemp = GetWindow(hwndClient, GW_CHILD);
				if (!hwndTemp)
				{
					break;
				}
				while (hwndTemp && GetWindow(hwndTemp, GW_OWNER))
					hwndTemp = GetWindow(hwndTemp, GW_HWNDNEXT);
				if (hwndTemp)
					SendMessage(hwndClient,
					WM_MDIDESTROY,
					(WPARAM)hwndTemp,
					NULL);
				else
					break;
			}
			ShowWindow(hwndClient, SW_SHOW);
			break;
		}
		case IDM_EXIT:
		{
			DestroyWindow(hWnd);
			break;
		}
		default:
			return DefFrameProc(hWnd, hwndClient, message, wParam, lParam);
		}

		return DefFrameProc(hWnd, hwndClient, message, wParam, lParam);
	}
	break;

	case WM_TIMER:
	{
		x2 = x2 + 10;
		y2 = y2 + 10;
		InvalidateRect(hWnd, NULL, true);
		break;
	}
	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);
		pen = CreatePen(PS_SOLID, 10, RGB(255, 0, 0));
		SelectObject(hdc, pen);
		if (f == 1)
			Rectangle(hdc, x2, y2, x2 + 100, y2 + 100);
		EndPaint(hWnd, &ps);
		break;
	}

	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	default:
		return DefFrameProc(hWnd, hwndClient, message, wParam, lParam);
	}
	return 0;
}
LRESULT CALLBACK ChildGraphWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Для отрисовки:
	HDC hdc;
	PAINTSTRUCT ps;
	static HPEN pen1, pen2;

	// Контролируем изменение размера дочи: 
	RECT rect;

	// Координаты границы дочернего окна:
	static int sx, sy;
	wchar_t buf[4] = {0};

	// Экранные координаты:
	int a, b, x_scr, y_scr,n;
	BOOL fl;

	// Фактические координаты:
	double x, h;

	// Для popup-меню:
	HMENU GraphMenu;
	HMENU GraphPopUpMenu;
	int result;
	HFONT fnt;

	switch (message)
	{
	case WM_CREATE:
		pen1 = CreatePen(PS_SOLID, 5, RGB(0, 0, 0));
		pen2 = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
		break;
	case WM_SIZE:
		sx = LOWORD(lParam);
		sy = HIWORD(lParam);
		GetClientRect(GetParent(hWnd), &rect);
		if (rect.bottom == sy && rect.right == sx)
			MoveWindow(hWnd, 0, 0, rect.right, rect.bottom, false);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		default:
			return	DefMDIChildProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);
	    a = 0;
		b = 0;
		fnt = CreateFont(25, 15, 0, 0, 400, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, 0, DEFAULT_QUALITY, FF_MODERN, 0);
		SetTextColor(hdc, RGB(0, 0, 0));
		n = 1;
		while (n <= 1000)
		{
			fl = 1;
			for (int i = 2; i < n; i++)
				if (n % i == 0)
				{
					fl = 0;
					break;
				}
			if (fl == 1)
			{
				wsprintf(buf, L"%d", n);
				TextOut(hdc, a, b, buf, 30);
				if (a+50 > sx)
				{
					a = 0;
					b += 25;
				}
				else
				a += 30;
			}
			n = n + 2;
		}
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		SendMessage(hWnd, WM_MDIDESTROY, (WPARAM)hwndClient, NULL);
		break;

	default:
		return	DefMDIChildProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}