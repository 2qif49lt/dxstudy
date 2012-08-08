#include <windows.h>
#include <stdio.h>

//ȫ�ֱ�������
HINSTANCE hInst;
HBITMAP bg;
HBITMAP girl[7];     //����λͼ�������������������λͼ
HDC		mdc;     //����"hdc"Ϊȫ�ֱ����������洢����DC����������ĳ�����ж�����ͼ��ȽϷ���
int		num;         //"num"����������¼Ŀǰ��ʾ��ͼ��

//ȫ�ֺ���������
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
void MyPaintBG(HDC hdc);
void				MyPaint(HDC hdc);

//**WinMain������������ڵ㺯��*********************************
int   APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG msg;

	MyRegisterClass(hInstance);

	//�ж�
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	//��Ϣѭ��
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return msg.wParam;
}

//****���һ�������࣬��������⣬ʹ�ô��ڽṹ��*************************
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

//****��ʼ������************************************
// 1.���ļ�����λͼ
// 2.������ʱ��
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	char filename[20] = "";
	int i;

	hInst = hInstance;

	hWnd = CreateWindow("canvas", "��������" , WS_OVERLAPPED   |   WS_SYSMENU   |WS_MINIMIZEBOX,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	MoveWindow(hWnd,10,10,432,648,true);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	HDC hdc = GetDC(hWnd);
	mdc = CreateCompatibleDC(hdc);

	bg = (HBITMAP)LoadImage(NULL,"b/bg.bmp",IMAGE_BITMAP,432,648,LR_LOADFROMFILE);
	//�����������λͼ
	for(i=0;i<7;i++)      
	{
		sprintf(filename,"b/1girl%d.bmp",i);
		girl[i] = (HBITMAP)LoadImage(NULL,filename,IMAGE_BITMAP,640,240,LR_LOADFROMFILE);
	}
	
	num = 0;                     //���ó�ʼ����ʾͼ��Ϊ"0"
	SetTimer(hWnd,1,40,NULL);   //������ʱ�������0.5�뷢����Ϣ
	MyPaintBG(hdc);
	MyPaint(hdc);
	ReleaseDC(hWnd,hdc);
	return TRUE;
}

//****�Զ����ͼ����*********************************
// ����ͼ��˳����д�����ͼ
int ix =10,iy = 400;
void MyPaint(HDC hdc)
{
	if(num == 7)               //�ж��Ƿ񳬹����ͼ�ţ����������ͼ�š�6��������ʾͼ������Ϊ"0"��
	{
		srand(GetTickCount());
		ix = rand() % 432;
		iy = rand() % 648;
		num = 0;               
	}
	SelectObject(mdc,bg);
	BitBlt(hdc,0,0,432,648,mdc,0,0,SRCCOPY);         //��Ŀǰͼ�Ž��д�����ͼ
	SelectObject(mdc,girl[num]);
	BitBlt(hdc,ix,iy,320,240,mdc,320,0,SRCAND);         //��Ŀǰͼ�Ž��д�����ͼ
	BitBlt(hdc,ix,iy,320,240,mdc,0,0,SRCPAINT);         //��Ŀǰͼ�Ž��д�����ͼ

	num++;                    //����num��ֵ��1��Ϊ��һ��Ҫ��ʾ��ͼ��
}
void MyPaintBG(HDC hdc)
{
	SelectObject(mdc,bg);
	BitBlt(hdc,0,0,432,648,mdc,0,0,SRCCOPY);         //��Ŀǰͼ�Ž��д�����ͼ
}

//****��Ϣ������***********************************
// 1.���봦��WM_TIMER��Ϣ
// 2.����Ϣ����ʱɾ����ʱ��
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
		case WM_TIMER:						//ʱ����Ϣ
			MyPaint(htdc);                   //����Ϣѭ���м��봦��WM_TIMER��Ϣ�������յ�����Ϣʱ�����MyPaint�����������д��ڻ�ͼ
			break;
		case WM_DESTROY:					//���ڽ�����Ϣ
			KillTimer(hWnd,1);             //���ڽ���ʱ��ɾ���������Ķ�ʱ��     
			DeleteDC(mdc);

//			ReleaseDC(hWnd,hdc);
			for(int i=0;i<7;i++)
				DeleteObject(girl[i]);
			DeleteObject(bg);
			PostQuitMessage(0);
			break;
		default:							//������Ϣ
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
	ReleaseDC(hWnd,htdc);
   return 0;
}