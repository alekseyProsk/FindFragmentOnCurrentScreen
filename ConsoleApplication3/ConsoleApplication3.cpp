#include <opencv2\highgui\highgui_c.h>
#include <opencv2\opencv.hpp>
#include <opencv2\core.hpp>
#include <stdio.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2\core.hpp>
#include <string>
#include <iostream>
#include <windows.h>
#include <winuser.h>
#include <iostream>
#include <opencv2\imgcodecs.hpp>
#include <time.h>
#include <gdiplus.h>
#include "Screen.h"


#pragma warning(disable : 4996)



using namespace cv;
using namespace std;

DWORD pid = 0;
DWORD g_pid;
HANDLE hTimer;
HANDLE hTimerQueue;
Mat image;
Mat templ;
Mat _res;
bool clicking;
int x;
int y;

// Для проверки видимости окна и нахождения хендла от него
VOID CALLBACK TimerRoutine(PVOID lparam, BOOL TimerOrWaitFired);
BOOL CALLBACK EnumWindowsProc(_In_ HWND   hwnd, _In_ LPARAM lParam)
{
	DWORD pid = 0;

	if (GetWindow(hwnd, GW_OWNER) == (HWND)0 && IsWindowVisible(hwnd))
	{
		GetWindowThreadProcessId(hwnd, &pid);
		if (pid == (DWORD)lParam)
		{
			g_pid = pid;
			
			hTimerQueue = CreateTimerQueue();
			SetForegroundWindow((HWND)hwnd);
			ShowWindow((HWND)hwnd, SW_SHOWMAXIMIZED);
			

			BOOL A = CreateTimerQueueTimer(&hTimer, hTimerQueue, (WAITORTIMERCALLBACK)&TimerRoutine, hwnd, 10, 0, 0);
			return FALSE;
		}
	}
	return TRUE;
}

HWND ipRam = 0;
VOID CALLBACK TimerRoutine(PVOID lparam, BOOL TimerOrWaitFired)
{
	ipRam =(HWND)lparam;
	ShowWindow((HWND)lparam, SW_SHOWMAXIMIZED);
	SetForegroundWindow((HWND)lparam);
	/*printf("PID is %d, HWND is %d\n", (int)g_pid, (int)lparam);*/
}

bool Find()
{
	
	// имя картинки задаётся первым параметром
	//// получаем картинку
	/*string filename = "C:\\Users\\omnym\\source\\repos\\AUTO_REFRESH_1.0\\ConsoleApplication3\\screen.bmp";*/
	string filename = "C:\\Program Files\\AUTO_REFRESH_1.0\\screen.bmp";
	// получаем картинку
	image = imread(filename,IMREAD_COLOR);
	/*cout << filename << endl;*/
	
	// шаблон
	/*string filename2 = "C:\\Users\\omnym\\source\\repos\\AUTO_REFRESH_1.0\\ConsoleApplication3\\2.bmp";*/
	string filename2 = "C:\\Program Files\\AUTO_REFRESH_1.0\\2.bmp";
	/*cout << filename2 << endl;*/
	templ = imread(filename2, IMREAD_COLOR);
	Mat gref, gtpl;
	cvtColor(image, gref, CV_BGR2GRAY); //Задаем один цвет
	cvtColor(templ, gtpl, CV_BGR2GRAY);

	const int low_canny = 110;
	const int low_canny1 = 110;
	Canny(gref, gref, low_canny1, low_canny * 3); // Выделяем контуры
	Canny(gtpl, gtpl, low_canny1, low_canny * 3);

	//imshow("file", gref);
	//imshow("template", gtpl);
	

	Mat res_32f(gref.rows - gtpl.rows + 1, gref.cols - gtpl.cols + 1, CV_32FC1); //Саоздаем новую картинку
	
	matchTemplate(gref, gtpl, res_32f, TM_CCORR_NORMED); //Ищем
	double minval, maxval;
	Point minloc, maxloc;
	minMaxLoc(res_32f, &minval, &maxval, &minloc, &maxloc); //поиск по минимуму/мааксимуму

	
		rectangle(image, maxloc, Point(maxloc.x + templ.cols, maxloc.y + templ.rows), Scalar(0, 255, 0), 2);
		floodFill(res_32f, maxloc, 0); //mark drawn blob // Обводка и заполнение найденного маста
		
		cout << countNonZero(gtpl) << endl;
		int count_non = countNonZero(res_32f);

		imshow("final", image);
		imshow("file", gref);
		waitKey(0);
	
		if (countNonZero(res_32f) == count_non)
		{
			cout << endl <<"Нашли картинку" << countNonZero(res_32f) << endl;
			imshow("final", image);
			imshow("file", gref);
			waitKey(0);
			return true;
		}
		
		else
		{
			cout << endl << "Не нашли картинку" << countNonZero(res_32f) << endl;
			imshow("final", image);
			waitKey(0);
			return false;
			
		}
		
	
}


#pragma warning(disable : 4996)


void getSCREEN()//создаем скрин и сохраняем в файлик
{
	Screen sh;
	Mat screen = sh.get();
	imwrite("C:\\Program Files\\AUTO_REFRESH_1.0\\screen.bmp", screen);
}

//Основная функция для кликера
static void startclicker(float A)
{
	//Вечный цикл, для  запроса кнопок
	while (true)
	{
		if (GetAsyncKeyState(VK_NUMPAD1))//Фиксируем координаты
		{
			Sleep(100);
			POINT cursorPos;
			GetCursorPos(&cursorPos);
			//x = 0;
			x = cursorPos.x;
			//y = 0;
			y = cursorPos.y;
			cout << "\nКоординаты X = " << x;
			cout << "\nКоординаты Y = " << y << endl;;

		}
		if (GetAsyncKeyState(VK_ESCAPE))// Выход
		{
			cout << "Пакеда ";
			Sleep(500);
			system("exit");
		}
		if (GetAsyncKeyState(VK_NUMPAD2))// Старт кликов, после нажатия NUM1
		{
			cout << "Активно !";
			Sleep(100);
			clicking = true;

			while (true)
			{

				if (!clicking)
				{
					break;
				}

				Sleep(A); // 15 минут в милисикундах - 90000
				ShowWindow((HWND)ipRam, SW_SHOWMAXIMIZED);
				SetForegroundWindow((HWND)ipRam); // Переходим в наше окно
				Sleep(100);
				SetCursorPos(x, y);
				mouse_event(MOUSEEVENTF_LEFTDOWN, x, y, 0, 0);
				mouse_event(MOUSEEVENTF_LEFTUP, x, y, 0, 0);
				Sleep(500);
				getSCREEN(); // Скриним
				Find();// Ищем образец на полученном скрине
				if (Find()!=false)
				{
					time_t t;
					time(&t);
					cout << "\a" << "\a" << "\a" << "\a" << "\a" << "\a" << "\a" << endl;
					cout << "НОВОЕ СООБЩЕНИЕ " << ctime(&t);
				}
					
				if (GetAsyncKeyState(VK_NUMPAD3))
				{
					clicking = false;
					cout << "Остановка";
				}

				if (GetAsyncKeyState(VK_ESCAPE))
				{
					cout << "Пакеда ";
					Sleep(500);
					system("exit");
				}
			}
		}

	}
}

static void init()// Меню
{
	float A;
	clicking = false;

	cout << "[ HOTKEYS ]\n";
	cout << "NumPad1 = сохранить позицию мышки\n";
	cout << "NumPad2 = включить автообновление на заданной позиции\n";
	cout << "NumPad3 = Остановка\n";
	cout << "Escape  = выход\n";
	cout << "Введите период обновления в минутах ";
	cin >> A;
	A *= 60000; //Милисекунды умножаем до минут
	startclicker(A);
}



int main()
{
	setlocale(LC_ALL, "ru");
	
	do {
		std::cout << " Введите PID процесса из диспетчера задач ";
		std::cin >> pid;
		if(!EnumWindows(EnumWindowsProc, pid)) // Проверка, не пустое ли окно
			break;
	} while (pid);
	
	
	init();
}