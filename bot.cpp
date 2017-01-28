// Lumber bot.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include <WinBase.h>
#include <cstdlib>
#include <vector>
#include <queue>
#include <string>
#include <cmath>
#include <string>

#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)
#define HRCHECK(__expr) {hr=(__expr);if(FAILED(hr)){wprintf(L"FAILURE 0x%08X (%i)\n\tline: %u file: '%s'\n\texpr: '" WIDEN(#__expr) L"'\n",hr, hr, __LINE__,__WFILE__);goto cleanup;}}
#define RELEASE(__p) {if(__p!=nullptr){__p->Release();__p=nullptr;}}

#define DEVELOPMENT 0

#define SLEEP1 180
#define SLEEP2 15

#define RIGHT_BUTTON_X 844
#define LEFT_BUTTON_X 650
#define BUTTON_Y 700

#define WATCH_Y 520
#define RIGHT_WATCH 1030
#define LEFT_WATCH 890
#define WATCH_STEP 61

bool playing = false;
bool clickRight = true;
int LEVEL = 1;

int TAP_SLEEP = 135;
int LAST_TAP = 0;
int LASTNUMLOCK = 0,
	LASTR = 0;

int FreeMargin = 0;

D3DLOCKED_RECT rc;

using namespace std;

DWORD WINAPI Screenshot(LPVOID lpParam)
{

	UINT adapter = 0;
	//UINT count = 1;
	//UINT i = 0;

	HRESULT hr = S_OK;
	IDirect3D9 *d3d = nullptr;
	IDirect3DDevice9 *device = nullptr;
	IDirect3DSurface9 *surface = nullptr;
	D3DPRESENT_PARAMETERS parameters = { 0 };
	D3DDISPLAYMODE mode;

	UINT pitch;
	//SYSTEMTIME st;
	//LPBYTE *shots = nullptr;

	// init D3D and get screen size
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	HRCHECK(d3d->GetAdapterDisplayMode(adapter, &mode));

	parameters.Windowed = TRUE;
	parameters.BackBufferCount = 1;
	parameters.BackBufferHeight = mode.Height;
	parameters.BackBufferWidth = mode.Width;
	parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	parameters.hDeviceWindow = NULL;

	// create device & capture surface
	HRCHECK(d3d->CreateDevice(adapter, D3DDEVTYPE_HAL, NULL, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &parameters, &device));
	HRCHECK(device->CreateOffscreenPlainSurface(mode.Width, mode.Height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &surface, nullptr));

	// compute the required buffer size
	HRCHECK(surface->LockRect(&rc, NULL, 0));
	pitch = rc.Pitch;
	HRCHECK(surface->UnlockRect());

	// allocate screenshots buffers
	//    shots = new LPBYTE[count];
	//    for (UINT i = 0; i < count; i++)
	//    {
	//      shots[i] = new BYTE[pitch * mode.Height];
	//    }

	/*
	int WATCH_Y = 520;
	int RIGHT_WATCH = 1030;
	int LEFT_WATCH = 890;
	int WATCH_STEP = 61;
	*/

	while (true)
	{
		if (playing)
		{
			//++i;
			//GetSystemTime(&st);
			//wprintf(L"Shotting #%i    %i:%i:%i.%i\n", i, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

			// get the data
			HRCHECK(device->GetFrontBufferData(0, surface));

			// copy it into our buffers
			HRCHECK(surface->LockRect(&rc, NULL, 0));
			
			
			BYTE *bytePointer = (BYTE*)rc.pBits;
			DWORD index;

			index = (LEFT_WATCH * 4 + (WATCH_Y * (rc.Pitch)));

			if ((int)bytePointer[index + 2] == 161 && (int)bytePointer[index + 1] == 116) {
				clickRight = true;
				//cout << "on the left\n";
			}

			index = (RIGHT_WATCH * 4 + (WATCH_Y * (rc.Pitch)));
			
			if ((int)bytePointer[index + 2] == 161 && (int)bytePointer[index + 1] == 116) {
				clickRight = false;
				//cout << "on the right\n";
			}
			
			FreeMargin = 6;

			if (clickRight) {
				//	calc the FreeMargin on right
				for (int i = 0; i < 7; i++) {
					index = (RIGHT_WATCH * 4 + ((WATCH_Y -((i+1) * WATCH_STEP)) * (rc.Pitch)));

					if ((int)bytePointer[index + 2] == 161 && (int)bytePointer[index + 1] == 116) {
						FreeMargin = i;
						break;
					}
				}
			}
			else {
				// calc the FreeMargin on left
				for (int i = 0; i < 7; i++) {
					index = (LEFT_WATCH * 4 + ((WATCH_Y - ((i + 1) * WATCH_STEP)) * (rc.Pitch)));

					if ((int)bytePointer[index + 2] == 161 && (int)bytePointer[index + 1] == 116) {
						FreeMargin = i;
						break;
					}
				}
			}

			//cout << "freeMargin :: " << FreeMargin << "   right? " << clickRight << "\n";

			if (clickRight)
				SetCursorPos(RIGHT_BUTTON_X, BUTTON_Y);
			else
				SetCursorPos(LEFT_BUTTON_X, BUTTON_Y);

			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

			//clickCount++;

			//int faster = (clickCount / 20) * 5;
			
			if (FreeMargin == 0)
				Sleep(SLEEP1);
			else if (FreeMargin > 1) {
				for (; FreeMargin > 1; FreeMargin--) {
					//cout << "	freeMargin :: " << FreeMargin << "   right? " << clickRight << "\n";
					mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
					mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
					//clickCount++;
					Sleep(SLEEP2);
					//Sleep(100 - (FreeMargin*15) );
				}
			}

			if (FreeMargin == 1) {
				//cout << "	freeMargin :: " << FreeMargin << "   right? " << clickRight << "	NEW SHOT ";
				//mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
				//mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
				
				FreeMargin--;
				Sleep(SLEEP1);
			}

			/*

			BYTE b=bytePointer[index]; // Blue
			BYTE g=bytePointer[index+1]; // Green
			BYTE r=bytePointer[index+2]; // Red
			BYTE a=bytePointer[index+3]; // Alpha

			*/

			HRCHECK(surface->UnlockRect());
		}
	}

	// save all screenshots
	//for (UINT i = 0; i < count; i++)
	//{
	//  WCHAR file[100];
	//  wsprintf(file, L"cap%i.png", i);
	//  HRCHECK(SavePixelsToFile32bppPBGRA(mode.Width, mode.Height, pitch, shots[i], file, GUID_ContainerFormatPng));
	//}

cleanup:

	RELEASE(surface);
	RELEASE(device);
	RELEASE(d3d);

	return hr;

}

DWORD WINAPI click(LPVOID lpParam)
{
	// left button  : (844,  890)
	// right button : (1070, 887)

	while (true) {

		if (playing) {
		
			if (clickRight)
				SetCursorPos(RIGHT_BUTTON_X, BUTTON_Y);
			else
				SetCursorPos(LEFT_BUTTON_X, BUTTON_Y);
			
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

			if (FreeMargin == 0)
				Sleep(250);
			else if (FreeMargin == 1) {
				Sleep(250);
				FreeMargin--;
			}
			else {
				Sleep(50);
				FreeMargin--;
			}

		}
	}

	return 0;
}



int _tmain(int argc, _TCHAR* argv[])
{

	LPDWORD dwThreadID = 0;
	LPDWORD dwThreadIDscrn = 0;

	//HANDLE hThread = CreateThread(NULL, 0, click, NULL, 0, dwThreadID);
	HANDLE hThreadscrn = CreateThread(NULL, 0, Screenshot, NULL, 0, dwThreadIDscrn);

	while (true)
	{
		if (GetAsyncKeyState(VK_NUMLOCK) && LASTNUMLOCK == 0)
		{
			cout << "Let's start the game - Trial " << LEVEL << "!\n";

			playing = true;
			LASTNUMLOCK = 100;
		}

		if (GetAsyncKeyState(0x52) && LASTR == 0)
		{
			//clickCount = 0;
			playing = false;
			++LEVEL;
			LASTR = 100;
		}

		if (LASTNUMLOCK)
		{
			LASTNUMLOCK += 10;

			if (LASTNUMLOCK >= 1010)
				LASTNUMLOCK = 0;
		}


		if (LASTR)
		{
			LASTR += 10;

			if (LASTR >= 1010)
				LASTR = 0;
		}

		if (LASTNUMLOCK || LASTR)	Sleep(10);

	}

	return 0;

}

