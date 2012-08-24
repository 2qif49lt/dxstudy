
#include <stdio.h>
#include <Windows.h>

//定义一个结构体
struct dragon        //定义dragon结构，代表画面上的恐龙，其结构成员x和y为贴图坐标，dir为目前恐龙的移动方向
{
	int x,y;
	int dir;
};

//定义常数
const int draNum = 12;  //定义常数draNum，代表程序在画面上要出现的恐龙数目，在此设定为12个
//全局变量定义
HINSTANCE hInst;
HBITMAP draPic[4],bg;   //draPic[4]储存恐龙上下左右移动的连续图案，bg为存储背景图
HDC		hdc,mdc,bufdc;
HWND	hWnd;
DWORD	tPre,tNow;
int		picNum;
dragon  dra[draNum];   //按照draNum的值建立数组dra[]，产生画面上出现的恐龙。

int uframe = 0;
DWORD dwlst = 0;
//全局函数声明
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
void				MyPaint(HDC hdc);

//****WinMain函数，程序入口点函数**************************************
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
	GetMessage(&msg,NULL,NULL,NULL);//初始化msg
	//消息循环
    while( msg.message!=WM_QUIT )
    {
        if( PeekMessage( &msg, NULL, 0,0 ,PM_REMOVE) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
		else
		{
			if (GetTickCount() - dwlst >= 1000)
			{
				uframe = 0;
				dwlst = GetTickCount();
			}
			tNow = GetTickCount();
			if(tNow-tPre >= 150)
				MyPaint(hdc);
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

//****初始化函数*************************************
// 加载位图并设定各初始值
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HBITMAP bmp;
	hInst = hInstance;
	int i;

	hWnd = CreateWindow("canvas", "绘图窗口" , WS_OVERLAPPED   |   WS_SYSMENU   |WS_MINIMIZEBOX,
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

	bmp = CreateCompatibleBitmap(hdc,640,480);  //建立一个空位图并放入mdc中
	SelectObject(mdc,bmp);


	//加载各张恐龙跑动图及背景图，这里以0,1,2,3来代表恐龙的上，下，左，右移动
	draPic[0] = (HBITMAP)LoadImage(NULL,"f/dra0.bmp",IMAGE_BITMAP,528,188,LR_LOADFROMFILE);
	draPic[1] = (HBITMAP)LoadImage(NULL,"f/dra1.bmp",IMAGE_BITMAP,544,164,LR_LOADFROMFILE);
	draPic[2] = (HBITMAP)LoadImage(NULL,"f/dra2.bmp",IMAGE_BITMAP,760,198,LR_LOADFROMFILE);
	draPic[3] = (HBITMAP)LoadImage(NULL,"f/dra3.bmp",IMAGE_BITMAP,760,198,LR_LOADFROMFILE);
	bg = (HBITMAP)LoadImage(NULL,"f/bg.bmp",IMAGE_BITMAP,640,480,LR_LOADFROMFILE);

	//设定所有恐龙初始的贴图坐标都为（200,200），初始的移动方向都为向左。
	for(i=0;i<draNum;i++)
	{
		dra[i].dir = 3;    //起始方向
		dra[i].x = 200;	   //贴图的起始X坐标
		dra[i].y = 200;    //贴图的起始Y坐标
	}

	MyPaint(hdc);

	return TRUE;
}

//气泡排序
void BubSort(int n)
{
	int i,j;
	bool f;
	dragon tmp;

	for(i=0;i<n-1;i++)
	{
		f = false;
		for(j=0;j<n-i-1;j++)
		{
			if(dra[j+1].y < dra[j].y)
			{
				tmp = dra[j+1];
				dra[j+1] = dra[j];
				dra[j] = tmp;
				f = true;
			}
		}
		if(!f)
			break;
	}
}

//****自定义绘图函数*********************************
// 1.对窗口中跑动的恐龙进行排序贴图
// 2.恐龙贴图坐标修正
void MyPaint(HDC hdc)
{
	++uframe;
	int w,h,i;

	char sztxt[222];
	sprintf(sztxt,"fps: %u",uframe);

	if(picNum == 8)
		picNum = 0;

	//在mdc中先贴上背景图
	SelectObject(bufdc,bg);
	BitBlt(mdc,0,0,640,480,bufdc,0,0,SRCCOPY);
	SetBkMode(mdc,TRANSPARENT);
	TextOut(mdc,0,0,sztxt,strlen(sztxt));
	BubSort(draNum);    //贴上恐龙图之前调用BubSort（）函数进行排序


	//下面这个for循环，按照目前恐龙的移动方向dra[i].dir，选取对应的位图到bufdc中，并设定截切的大小。每一张要在窗口上出现的恐龙图案依次先在mdc上进行透明贴图的操作。
	for(i=0;i<draNum;i++)
	{
		SelectObject(bufdc,draPic[dra[i].dir]);
		switch(dra[i].dir)
		{
			case 0:
				w = 66;
				h = 94;
				break;
			case 1:
				w = 68;
				h = 82;
				break;
			case 2:
				w = 95;
				h = 99;
				break;
			case 3:
				w = 95;
				h = 99;
				break;
		}
		BitBlt(mdc,dra[i].x,dra[i].y,w,h,bufdc,picNum*w,h,SRCAND);
		BitBlt(mdc,dra[i].x,dra[i].y,w,h,bufdc,picNum*w,0,SRCPAINT);
	}

	//将最后画面显示在窗口中
	BitBlt(hdc,0,0,640,480,mdc,0,0,SRCCOPY);

	tPre = GetTickCount();         //记录此次绘图时间
	picNum++;


	//下面这个for循环，决定每一只恐龙下一次的移动方向及贴图坐标
	for(i=0;i<draNum;i++)
	{
		switch(rand()%4)          //随机数除以4的余数来决定下次移动方向，余数0,1,2,3分别代表上，下，左，右
		{
			//case 0里面的代码，按照目前的移动方向来修正因为各个方向图案尺寸不一致而产生的贴图坐标误差，加入恐龙每次移动的单位量（上，下，左，右每次20个单位）而得到下次新的贴图坐标
			case 0:					     //上
				switch(dra[i].dir)
				{
					case 0:	
						dra[i].y -= 20;
						break;
					case 1:
						dra[i].x += 12;
						dra[i].y -= 31;
						break;
					case 2:	
						dra[i].x += 14;
						dra[i].y -= 20;
						break;
					case 3:
						dra[i].x += 14;
						dra[i].y -= 20;
						break;
				}
				//在计算出新的贴图坐标之后，还需判断此新的坐标会不会使得恐龙贴图超出窗口边界，若超出，则将该方向上的坐标设定为刚好等于临界值
				if(dra[i].y < 0)
					dra[i].y = 0;
				dra[i].dir = 0;
				break;
			//其他方向按照和上面相同的方法计算
			case 1:				     	//下
				switch(dra[i].dir)
				{
					case 0:
						dra[i].x -= 12;
						dra[i].y += 31;
						break;
					case 1:
						dra[i].y += 20;
						break;
					case 2:
						dra[i].x += 15;
						dra[i].y += 29;
						break;
					case 3:
						dra[i].x += 15;
						dra[i].y += 29;
						break;
				}

				if(dra[i].y > 370)
					dra[i].y = 370;
				dra[i].dir = 1;
				break;
			case 2:				    	//左
				switch(dra[i].dir)
				{
					case 0:
						dra[i].x -= 34;
						break;
					case 1:
						dra[i].x -= 34;
						dra[i].y -= 9;
						break;
					case 2:
						dra[i].x -= 20;
						break;
					case 3:
						dra[i].x -= 20;
						break;
				}
				if(dra[i].x < 0)
					dra[i].x = 0;
				dra[i].dir = 2;
				break;
			case 3:				    	//右
				switch(dra[i].dir)
				{
					case 0:
						dra[i].x += 16;
						break;
					case 1:
						dra[i].x += 16;
						dra[i].y -= 10;
						break;
					case 2:
						dra[i].x += 20;
						break;
					case 3:
						dra[i].x += 20;
						break;
				}
				if(dra[i].x > 535)
					dra[i].x = 535;
				dra[i].dir = 3;
				break;
		}
	}
}

//****消息处理函数***********************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		int i;

		case WM_DESTROY:					//窗口结束消息，撤销各种DC  
			DeleteDC(mdc);
			DeleteDC(bufdc);
			for(i=0;i<4;i++)
				DeleteObject(draPic[i]);
			DeleteObject(bg);
			ReleaseDC(hWnd,hdc);
			PostQuitMessage(0);
			break;
		default:							//其他消息
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

