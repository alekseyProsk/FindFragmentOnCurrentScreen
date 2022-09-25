#pragma once
#include <opencv2\opencv.hpp>
#include <windows.h>

class Screen 
{
public:
	Screen()
	{
		// Параметры экрана
		HWND hwnd = GetDesktopWindow();
		hWDC = GetWindowDC(hwnd);
		width = GetSystemMetrics(SM_CXSCREEN);
		height = GetSystemMetrics(SM_CYSCREEN);

		// Совместимый контекст в памяти
		hScreen = CreateCompatibleDC(hWDC); // - DeleteDC( hScreen );
		hBM = CreateCompatibleBitmap(hWDC, width, height); // - DeleteObject( hBM );
		hBM_Temp = SelectObject(hScreen, hBM); // - SelectObject( hScreen, hBM_Temp );

		// Описание рабочего массива бит совместимого контекста в памяти
		BMI.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		BMI.bmiHeader.biWidth = width;
		BMI.bmiHeader.biHeight = -height;
		BMI.bmiHeader.biPlanes = 1;
		BMI.bmiHeader.biBitCount = 24;
		BMI.bmiHeader.biCompression = BI_RGB;

		// Массив данных скриншота
		const int colorSize = 3; // Размер пикселя
		const int alignment = 4; // Выравнивание строки		 
		int step = (int)ceil(width * colorSize / (double)alignment) * alignment; // Шаг строки
		//int step = ( width * colorSize / alignment + ( width * colorSize % alignment? 1: 0 ) ) * alignment; // Шаг строки
		data = new char[step * height]; // - delete[] data;

		// Скриншот
		screen = new cv::Mat(height, width, CV_8UC3, data, step); // - delete screen;
	}

	~Screen() {
		SelectObject(hScreen, hBM_Temp);
		DeleteObject(hBM);
		DeleteDC(hScreen);
		delete screen; screen = NULL;
		delete[] data; data = NULL;
	};

	cv::Mat& get() {
		BitBlt(hScreen, 0, 0, width, height, hWDC, 0, 0, SRCCOPY);
		GetDIBits(hScreen, hBM, 0, height, data, &BMI, DIB_RGB_COLORS);
		return *screen;
	};

private:
	HDC hWDC; // Контекст экрана
	HDC hScreen; // Совместимый с экраном контекст в памяти
	HBITMAP hBM; // Рабочий массив бит совместимого контекста в памяти
	BITMAPINFO BMI; // Описание рабочего массива бит совместимого контекста в памяти

	int width; // Ширина экрана в пикселях
	int height; // Высота экрана в пикселях

	char* data; // Массив данных скриншота
	cv::Mat* screen; // Скриншот

	HGDIOBJ hBM_Temp; // Массив бит совместимого контекста в памяти созданный по умолчанию	
};

