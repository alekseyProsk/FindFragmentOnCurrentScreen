#pragma once
#include <opencv2\opencv.hpp>
#include <windows.h>

class Screen 
{
public:
	Screen()
	{
		// ��������� ������
		HWND hwnd = GetDesktopWindow();
		hWDC = GetWindowDC(hwnd);
		width = GetSystemMetrics(SM_CXSCREEN);
		height = GetSystemMetrics(SM_CYSCREEN);

		// ����������� �������� � ������
		hScreen = CreateCompatibleDC(hWDC); // - DeleteDC( hScreen );
		hBM = CreateCompatibleBitmap(hWDC, width, height); // - DeleteObject( hBM );
		hBM_Temp = SelectObject(hScreen, hBM); // - SelectObject( hScreen, hBM_Temp );

		// �������� �������� ������� ��� ������������ ��������� � ������
		BMI.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		BMI.bmiHeader.biWidth = width;
		BMI.bmiHeader.biHeight = -height;
		BMI.bmiHeader.biPlanes = 1;
		BMI.bmiHeader.biBitCount = 24;
		BMI.bmiHeader.biCompression = BI_RGB;

		// ������ ������ ���������
		const int colorSize = 3; // ������ �������
		const int alignment = 4; // ������������ ������		 
		int step = (int)ceil(width * colorSize / (double)alignment) * alignment; // ��� ������
		//int step = ( width * colorSize / alignment + ( width * colorSize % alignment? 1: 0 ) ) * alignment; // ��� ������
		data = new char[step * height]; // - delete[] data;

		// ��������
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
	HDC hWDC; // �������� ������
	HDC hScreen; // ����������� � ������� �������� � ������
	HBITMAP hBM; // ������� ������ ��� ������������ ��������� � ������
	BITMAPINFO BMI; // �������� �������� ������� ��� ������������ ��������� � ������

	int width; // ������ ������ � ��������
	int height; // ������ ������ � ��������

	char* data; // ������ ������ ���������
	cv::Mat* screen; // ��������

	HGDIOBJ hBM_Temp; // ������ ��� ������������ ��������� � ������ ��������� �� ���������	
};

