#include <windows.h>
#include <stdio.h>

//全局变量声明
HINSTANCE hInst;
HBITMAP bg;
HBITMAP girl[7];     //声明位图数组用来储存各张人物位图
HDC		mdc;     //声明"hdc"为全局变量，用来存储窗口DC，这样后面的程序进行动画绘图会比较方便
int		num;         //"num"变量用来记录目前显示的图号

//全局函数的声明
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
void MyPaintBG(HDC hdc);
void				MyPaint(HDC hdc);
HWND hw;
//**WinMain函数，程序入口点函数*********************************
int   APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG msg;

	MyRegisterClass(hInstance);

	//判断
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}
	DWORD dwlast = 0;
	//消息循环
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0,PM_REMOVE)) 
		{
			if (msg.message == WM_QUIT)
			{
				return 1;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (GetTickCount() - dwlast >= 1000 / 30)
			{
				HDC hdc = GetDC(hw);
				MyPaint(hdc);
				ReleaseDC(hw,hdc);
				dwlast = GetTickCount();
			}
			else
				Sleep(1);
		}
	}
	
	
	return msg.wParam;
}

//****设计一个窗口类，类似填空题，使用窗口结构体*************************
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "canvas";
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}

//****初始化函数************************************
// 1.从文件加载位图
// 2.建立定时器
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	char filename[20] = "";
	int i;

	hInst = hInstance;

	hWnd = CreateWindow("canvas", "动画窗口" , WS_OVERLAPPED   |   WS_SYSMENU   |WS_MINIMIZEBOX,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	MoveWindow(hWnd,10,10,432,648,true);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	hw = hWnd;
	HDC hdc = GetDC(hWnd);
	mdc = CreateCompatibleDC(hdc);

	bg = (HBITMAP)LoadImage(NULL,"b/bg.bmp",IMAGE_BITMAP,432,648,LR_LOADFROMFILE);
	//载入各个人物位图
	for(i=0;i<7;i++)      
	{
		sprintf(filename,"b/1girl%d.bmp",i);
		girl[i] = (HBITMAP)LoadImage(NULL,filename,IMAGE_BITMAP,640,240,LR_LOADFROMFILE);
	}
	
	num = 0;                     //设置初始的显示图号为"0"
//	SetTimer(hWnd,1,40,NULL);   //建立定时器，间隔0.5秒发出消息
	MyPaintBG(hdc);
	MyPaint(hdc);
	ReleaseDC(hWnd,hdc);
	return TRUE;
}

//****自定义绘图函数*********************************
// 按照图号顺序进行窗口贴图
int ix =10,iy = 400;
void MyPaint(HDC hdc)
{
	if(num == 7)               //判断是否超过最大图号，若超过最大图号“6”，则将显示图号重设为"0"。
	{
		srand(GetTickCount());
		ix = rand() % 432;
		iy = rand() % 648;
		num = 0;               
	}
	SelectObject(mdc,bg);
	BitBlt(hdc,0,0,432,648,mdc,0,0,SRCCOPY);         //以目前图号进行窗口贴图
	SelectObject(mdc,girl[num]);
	BitBlt(hdc,ix,iy,320,240,mdc,320,0,SRCAND);         //以目前图号进行窗口贴图
	BitBlt(hdc,ix,iy,320,240,mdc,0,0,SRCPAINT);         //以目前图号进行窗口贴图

	num++;                    //将“num”值加1，为下一次要显示的图号
}
void MyPaintBG(HDC hdc)
{
	SelectObject(mdc,bg);
	BitBlt(hdc,0,0,432,648,mdc,0,0,SRCCOPY);         //以目前图号进行窗口贴图
}

//****消息处理函数***********************************
// 1.加入处理WM_TIMER消息
// 2.在消息结束时删除定时器
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hpdc;
	HDC htdc = GetDC(hWnd);
	switch (message)
	{
		case WM_PAINT:
			hpdc = BeginPaint(hWnd,&ps);
			MyPaintBG(hpdc);
			EndPaint(hWnd,&ps);
			break;
// 		case WM_TIMER:						//时间消息
// 			MyPaint(htdc);                   //在消息循环中加入处理WM_TIMER消息，当接收到此消息时便调用MyPaint（）函数进行窗口绘图
// 			break;
		case WM_DESTROY:					//窗口结束消息
			KillTimer(hWnd,1);             //窗口结束时，删除所建立的定时器     
			DeleteDC(mdc);

//			ReleaseDC(hWnd,hdc);
			for(int i=0;i<7;i++)
				DeleteObject(girl[i]);
			DeleteObject(bg);
			PostQuitMessage(0);
			break;
		default:							//其他消息
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
	ReleaseDC(hWnd,htdc);
   return 0;
}