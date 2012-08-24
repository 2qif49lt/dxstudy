
#include <stdio.h>
#include <windows.h>

//全局变量声明
HINSTANCE hInst;
HBITMAP girl[4],bg;
HDC		hdc,mdc,bufdc;
HWND	hWnd;
DWORD	tPre,tNow;
int		num,dir,x,y;       //x，y变量为人物贴图坐标，dir为人物移动方向，这里我们中以0，1，2，3代表人物上，下，左，右方向上的移动：num为连续贴图中的小图编号

//全局函数声明
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
void				MyPaint(HDC hdc);

//****WinMain函数，程序入口点函数***********************
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG msg;

	MyRegisterClass(hInstance);

	//初始化
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	//消息循环
    while( 1 )
    {
        if( PeekMessage( &msg, NULL, 0,0 ,PM_REMOVE) )
        {
			if (msg.message==WM_QUIT)
			{
				return 1;
			}
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
		else
		{
			tNow = GetTickCount();
			if(tNow-tPre >= 40)
				MyPaint(hdc);
			else
				Sleep(1);
		}
    }

	return msg.wParam;
}

//****设计一个窗口类，类似填空题，使用窗口结构体*******************
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

//****初始化函数*************************************
// 加载位图并设定各种初始值
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HBITMAP bmp;
	hInst = hInstance;

	hWnd = CreateWindow("canvas", "绘图窗口" , WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	MoveWindow(hWnd,10,10,640,480,true);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	hdc = GetDC(hWnd);
	mdc = CreateCompatibleDC(hdc);
	bufdc = CreateCompatibleDC(hdc);


	//建立空的位图并置入mdc中
	bmp = CreateCompatibleBitmap(hdc,640,480);
	SelectObject(mdc,bmp);


	//设定人物贴图初始位置和移动方向
	x = 300;
	y = 250;
	dir = 0;
	num = 0;

	//载入各连续移动位图及背景图
	girl[0] = (HBITMAP)LoadImage(NULL,"g/girl0.bmp",IMAGE_BITMAP,440,148,LR_LOADFROMFILE);
	girl[1] = (HBITMAP)LoadImage(NULL,"g/girl1.bmp",IMAGE_BITMAP,424,154,LR_LOADFROMFILE);
	girl[2] = (HBITMAP)LoadImage(NULL,"g/girl2.bmp",IMAGE_BITMAP,480,148,LR_LOADFROMFILE);
	girl[3] = (HBITMAP)LoadImage(NULL,"g/girl3.bmp",IMAGE_BITMAP,480,148,LR_LOADFROMFILE);
	bg = (HBITMAP)LoadImage(NULL,"g/bg.bmp",IMAGE_BITMAP,640,480,LR_LOADFROMFILE);

	MyPaint(hdc);

	return TRUE;
}

//****自定义绘图函数*********************************
// 人物贴图坐标修正及窗口贴图
void MyPaint(HDC hdc)
{
	int w,h;

	//先在mdc中贴上背景图
	SelectObject(bufdc,bg);
	BitBlt(mdc,0,0,640,480,bufdc,0,0,SRCCOPY);

	//按照目前的移动方向取出对应人物的连续走动图，并确定截取人物图的宽度与高度
	SelectObject(bufdc,girl[dir]);
	switch(dir)
	{
		case 0:
			w = 55;
			h = 74;
			break;
		case 1:
			w = 53;
			h = 77;
			break;
		case 2:
			w = 60;
			h = 74;
			break;
		case 3:
			w = 60;
			h = 74;
			break;
	}
	//按照目前的X，Y的值在mdc上进行透明贴图，然后显示在窗口画面上
	BitBlt(mdc,x,y,w,h,bufdc,num*w,h,SRCAND);
	BitBlt(mdc,x,y,w,h,bufdc,num*w,0,SRCPAINT);
	
	BitBlt(hdc,0,0,640,480,mdc,0,0,SRCCOPY);

	tPre = GetTickCount();         //记录此次绘图时间

	num++;
	if(num == 8)
		num = 0;

}

//****消息处理函数***********************************
// 1.按下【Esc】键结束程序
// 2.按下方向键重设贴图坐标
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_KEYDOWN:	     //按下键盘消息
			//判断按键的虚拟键码
			switch (wParam) 
			{
				case VK_ESCAPE:           //按下【Esc】键
					PostQuitMessage( 0 );  //结束程序
					break;
				case VK_UP:				  //按下【↑】键
					//先按照目前的移动方向来进行贴图坐标修正，并加入人物往上移动的量（每次按下一次按键移动10个单位），来决定人物贴图坐标的X与Y值，接着判断坐标是否超出窗口区域，若有则再次修正
					switch(dir)
					{
						case 0:	
							y -= 10;
							break;
						case 1:
							x -= 1;
							y -= 8;
							break;
						case 2:	
							x += 2;
							y -= 10;
							break;
						case 3:
							x += 2;
							y -= 10;
							break;
					}
					if(y < 0)
						y = 0;
					dir = 0;
					break;
				case VK_DOWN:			  //按下【↓】键
					switch(dir)
					{
						case 0:
							x += 1;
							y += 8;
							break;
						case 1:
							y += 10;
							break;
						case 2:
							x += 3;
							y += 6;
							break;
						case 3:
							x += 3;
							y += 6;
							break;
					}

					if(y > 375)
						y = 375;
					dir = 1;
					break;
				case VK_LEFT:			  //按下【←】键
					switch(dir)
					{
						case 0:
							x -= 12;
							break;
						case 1:
							x -= 13;
							y += 4;
							break;
						case 2:
							x -= 10;
							break;
						case 3:
							x -= 10;
							break;
					}
					if(x < 0)
						x = 0;
					dir = 2;
					break;
				case VK_RIGHT:			   //按下【→】键
					switch(dir)
					{
						case 0:
							x += 8;
							break;
						case 1:
							x += 7;
							y += 4;
							break;
						case 2:
							x += 10;
							break;
						case 3:
							x += 10;
							break;
					}
					if(x > 575)
						x = 575;
					dir = 3;
					break;
			}
			break;
		case WM_DESTROY:			    	//窗口结束消息
			int i;

			DeleteDC(mdc);
			DeleteDC(bufdc);
			for(i=0;i<4;i++)
				DeleteObject(girl[i]);
			DeleteObject(bg);
			ReleaseDC(hWnd,hdc);

			PostQuitMessage(0);
			break;
		default:							//其他消息
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

