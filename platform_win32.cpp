#include <windows.h>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include "zzxoto_platform.h"
#include <cstdarg>
#include <stdlib.h>

#define win_main WinMain

//Usual:
//Memory Order:     RR GG BB xx
//Little Endian:    xx BB GG RR
//Windows:
//Memory Order:     BB GG RR xx
//Little Endian:    xx RR GG BB

//wingdi.h

struct RenderBufferWindows
{
  BITMAPINFO bitmapInfo;
  int width;
  int height;
  void *memory;  
};

global HWND g_windowHandle;
global bool32 g_running = true;
global RenderBufferWindows g_renderBufferWindows;
global platform_RenderBuffer g_renderBuffer;
global HDC g_deviceContext;
global int g_windowWidth, g_windowHeight;
global int64 g_perfCounterFrequency;
global platform_GameInput g_gameInput;
global const int RAND_SEED = 971;

platform_Bitmap *platform_loadBitmap(char *filename)
{
  platform_Bitmap *result = NULL;

  FILE *file = fopen(filename, "r");
  if (file)
  {
    fseek(file , 0 , SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    void *buffer = platform_malloc(fileSize);
    int readCount = fread(buffer, 1, fileSize, file);

    if (readCount == fileSize)
    {
      result = (platform_Bitmap *) platform_malloc(sizeof(platform_Bitmap));
      BITMAPFILEHEADER *bitmapFileHeader = (BITMAPFILEHEADER *) buffer;
      BITMAPINFOHEADER *bitmapInfoHeader = (BITMAPINFOHEADER *)((BITMAPFILEHEADER *)bitmapFileHeader + 1);      
      result->bpp = bitmapInfoHeader->biBitCount / 8;
      //only supporting 1,2,3, or 4 bytes per pixel at the moment
      platform_assert(result->bpp >= 1 && result->bpp <= 4);
      platform_assert(bitmapInfoHeader->biBitCount % 8 == 0);
      result->imageWidth = bitmapInfoHeader->biWidth;
      //negative image height have top left origin
      //which is preferred
      result->imageHeight = bitmapInfoHeader->biHeight * -1; 
      bool32 shouldInvert = result->imageHeight < 0;
      if (shouldInvert)
      {
        result->imageHeight *= -1;
      }
      result->data = (char *) platform_malloc(result->imageWidth * result->bpp * result->imageHeight);

      int destRowSize = result->imageWidth * result->bpp;
      int srcRowPadding = (4 - (destRowSize % 4)) % 4;
      int srcRowSize = destRowSize + srcRowPadding;

      char *src = (char *) buffer + bitmapFileHeader->bfOffBits;

      for (int y = 0; y < result->imageHeight ; y ++)
      {
        int yDestOffset = y * destRowSize;
        int ySrc = shouldInvert? (result->imageHeight - 1 - y): y;
        int ySrcOffset  = ySrc * srcRowSize;

        for (int x = 0; x < destRowSize; x++)
        {
          result->data[x + yDestOffset] = src[x + ySrcOffset];
        }
      }
    }
  }

  return result;  
}

void platform_freeBitmap(platform_Bitmap *bitmap)
{
  platform_free(bitmap->data);
  platform_free(bitmap);
}

int platform_Bitmap::getPixel32(int x, int y)
{
  int pitch = this->imageWidth * this->bpp;
  int pos = (pitch * y)  + (x * this->bpp);

  char r = this->data[pos];
  int color = 0;

  if (this->bpp == 4)
  {
    char g = this->data[pos + 1];
    char b = this->data[pos + 2];
    char a = this->data[pos + 3];
    color = r << 24 | g << 16 | b << 8 | a;
  }
  else if (this->bpp == 3)
  {
    char g = this->data[pos + 1];
    char b = this->data[pos + 2];
    color = r << 16 | g << 8 | b;
  }
  else if (this->bpp == 2)
  {
    char g = this->data[pos + 1];
    color = r << 8 | g;
  }
  else
  {
    color = r;
  }

  return color;
}

void platform_debugString(char *buffer)
{
  OutputDebugStringA(buffer);
}

void *platform_malloc(int size)
{
  void *result = malloc(size);
  return result;
}

void platform_free(void *ptr)
{
  free(ptr);
}

void platform_assert(bool32 ok)
{
  if (!ok)
  {
    int *a = 0;
    *a = 0;
  }
}

float platform_rand()
{
  float r = rand() * 1.0f;
  float result = r / RAND_MAX;
  return result;
}

// template <class T>
// T platform_rand(T low, T high)
// {
//   if (high < low)
//   {
//     T temp = high;
//     high = low;
//     low = temp;
//   }

//   T diff = high - low;
  
//   float result = platform_rand() * diff + low;
//   return result;
// }

LRESULT win_windowProcedure_callback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
  LRESULT result = 0;

  switch(Message)
  {
    case WM_SIZE:
    {
      RECT clientRect;
      GetClientRect(Window, &clientRect);
      g_windowWidth = clientRect.right - clientRect.left;
      g_windowHeight = clientRect.bottom - clientRect.top;
    } 
    break;
    case WM_CHAR:
    {
      platform_debugString("WM_CHAR\n");
    }
    break;
    case WM_QUIT:
    {
      g_running = false;
    }
    break;
    case WM_CLOSE:
    {
      g_running = false;
    } 
    break;
    case WM_PAINT:
    {
      //window would keep spamming WM_PAINT message 
      //had we not called BeginPaint and EndPaint
      PAINTSTRUCT Paint;
      BeginPaint(Window, &Paint);
      EndPaint(Window, &Paint);
    } break;

    default:
    {
      result = DefWindowProc(Window, Message, WParam, LParam);
    } break;
  }

  return result;
}

internal void win_processMessage(platform_GameInput *gameInput)
{
  MSG message;
  while(PeekMessage(&message, 0, 0, 0, PM_REMOVE))
  {
    if (message.message == WM_KEYDOWN ||
      message.message == WM_KEYUP)
    {
      bool32 isDown = (message.lParam & (1 << 31)) == 0;

      static int keyboardEventCount = 0;

      switch(message.wParam)
      {
        case VK_SPACE:
        {
          gameInput->spaceIsDown = isDown;
        }
        break;
        case VK_LEFT:
        {
          gameInput->leftIsDown = isDown;
        }
        break;
        case VK_UP:
        {
          gameInput->upIsDown = isDown;
        }
        break;
        case VK_RIGHT:
        {
          gameInput->rightIsDown = isDown;
        }
        break;
        case 0x52:
        {
          gameInput->rIsDown = isDown;
        }
        break;
        case 0x53:
        {
          gameInput->sIsDown = isDown;
        }
        break;
      }
    }
    else
    {
      TranslateMessage(&message);
      DispatchMessageA(&message);
    }
  }
}

internal void win_blitBuffer()
{
  StretchDIBits(g_deviceContext,
    0, 0, g_windowWidth, g_windowHeight,                                //destination
    0, 0, g_renderBufferWindows.width, g_renderBufferWindows.height,    //source
    g_renderBufferWindows.memory,
    &g_renderBufferWindows.bitmapInfo,
    DIB_RGB_COLORS, SRCCOPY);
}

internal int64 win_wallClock()
{
  LARGE_INTEGER i;
  QueryPerformanceCounter(&i);
  return i.QuadPart;
}

internal float win_timeElapsed_seconds(int64 start, int64 end)
{
  return ((float)(end - start)) / ((float)g_perfCounterFrequency);
}

int win_main(HINSTANCE hInstance, HINSTANCE foo1, LPSTR foo2, int showCode)
{
  srand(RAND_SEED);
  //-1. initialize perf counter frequency
  {
    LARGE_INTEGER perfCounterFrequencyResult;
    QueryPerformanceFrequency(&perfCounterFrequencyResult);
    g_perfCounterFrequency = perfCounterFrequencyResult.QuadPart;
  }

  //0. set sleep granularity
  bool32 sleepIsGranular;
  {
    UINT desiredSchedularMS = 1;
    sleepIsGranular = (timeBeginPeriod(desiredSchedularMS) == TIMERR_NOERROR);
  }

  //1. initialize back buffer
  {
    g_renderBufferWindows.width = 1280;
    g_renderBufferWindows.height = 720;
    g_renderBufferWindows.memory = platform_malloc(g_renderBufferWindows.width * g_renderBufferWindows.height * 4);
    g_renderBufferWindows.bitmapInfo.bmiHeader.biSize = sizeof(g_renderBufferWindows.bitmapInfo.bmiHeader);
    g_renderBufferWindows.bitmapInfo.bmiHeader.biWidth = g_renderBufferWindows.width;
    g_renderBufferWindows.bitmapInfo.bmiHeader.biHeight = -g_renderBufferWindows.height;
    g_renderBufferWindows.bitmapInfo.bmiHeader.biPlanes = 1;
    g_renderBufferWindows.bitmapInfo.bmiHeader.biBitCount = 32;
    g_renderBufferWindows.bitmapInfo.bmiHeader.biCompression = BI_RGB;

    g_renderBuffer.width = g_renderBufferWindows.width;
    g_renderBuffer.height = g_renderBufferWindows.height;
    g_renderBuffer.memory = g_renderBufferWindows.memory;
  }

  //2. register WindowClass
  WNDCLASS windowClass = {};
  windowClass.style = CS_OWNDC;           //creates private device context, and allows caching
  windowClass.lpfnWndProc = win_windowProcedure_callback;
  windowClass.hInstance = hInstance;
  windowClass.lpszClassName = "zzxotoWindowClass";
  BOOL registerWindowClassResult = RegisterClass(&windowClass);

  if(registerWindowClassResult)
  {
    //3. create Window
    g_windowHandle = CreateWindowEx(
        0,                         // no extended styles
        windowClass.lpszClassName, // class name                   
        "My Game",                 // window name                  
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,             // default horizontal position  
        CW_USEDEFAULT,             // default vertical position    
        CW_USEDEFAULT,             // default width                
        CW_USEDEFAULT,             // default height               
        0,                         // no parent or owner window    
        0,                         // class menu used              
        hInstance,                 // instance handle              
        0);                        // no window creation data

    if (g_windowHandle)
    {
      ShowWindow(g_windowHandle, SW_MAXIMIZE);
      //4. initialize window height and width
      {
        RECT clientRect;
        GetClientRect(g_windowHandle, &clientRect);
        g_windowWidth = clientRect.right - clientRect.left;
        g_windowHeight = clientRect.bottom - clientRect.top;
      }

      //5. cache DC
      g_deviceContext = GetDC(g_windowHandle);

      int gameUpdateHz = 30;
      float targetSecondsPerFrame = 1.0f / gameUpdateHz;
      int oversleptCount = 0;

      g_gameInput.secondsElapsed = 0.f;

      initializeGame();

      //6. game loop
      while(g_running)
      {
        g_gameInput.dtForFrame = targetSecondsPerFrame;
        g_gameInput.secondsElapsed += targetSecondsPerFrame;
        int64 lastFrameTimestamp = win_wallClock();

        //7. process message
        win_processMessage(&g_gameInput);

        //8. update game
        updateGame(&g_gameInput);
        renderGame(&g_renderBuffer);

        //9. stabelize frame rate
        float elapsedSecondsThisFrame = win_timeElapsed_seconds(lastFrameTimestamp, win_wallClock());
        if (elapsedSecondsThisFrame < targetSecondsPerFrame)
        {
          if (sleepIsGranular)
          {
            DWORD sleepMS = (DWORD)(1000.0f * (targetSecondsPerFrame - elapsedSecondsThisFrame)) - 1;
            if(sleepMS > 0)
            {
              Sleep(sleepMS);
            }
          }

          //TODO: we seem to be oversleeping, occasionally
          elapsedSecondsThisFrame = win_timeElapsed_seconds(lastFrameTimestamp, win_wallClock());

          while (elapsedSecondsThisFrame < targetSecondsPerFrame)
          {
            elapsedSecondsThisFrame = win_timeElapsed_seconds(lastFrameTimestamp, win_wallClock());
          }
        }
        else
        {
          //TODO: log that our game update need be faster
        }

        //8. bit to screen
        win_blitBuffer();
        
        elapsedSecondsThisFrame = win_timeElapsed_seconds(lastFrameTimestamp, win_wallClock());
        float FPS = 1.0f / elapsedSecondsThisFrame;
        float elapsedMSThisFrame = 1000.0f * elapsedSecondsThisFrame;

#if 0
        char fpsBuffer[50];
        sprintf(fpsBuffer, "%.2f f/s; %.2f ms/frame; %d overslept\n", FPS, elapsedMSThisFrame, oversleptCount);
        platform_debugString(fpsBuffer);
#endif
      }
    }
    else
    {
      //TODO: handle error
    }
  }
  else
  {
    //TODO: handle error
  }

  return 0;
}