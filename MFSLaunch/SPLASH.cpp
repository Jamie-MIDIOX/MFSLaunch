// SPLASH.cpp: implementation of the SPLASH class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SPLASH.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SPLASH::SPLASH() :
   m_bIsShowing(false),
   m_TimerID(0),
   m_hParentWnd(NULL),
   m_hSplashWnd(NULL)
{}


SPLASH::~SPLASH()
{
   DestroyWindow( m_hSplashWnd );
}

/// <summary>
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// <param name="resid"></param>
bool SPLASH::Init( HWND hWnd, HINSTANCE hInst, LPCWSTR szImageFile )
{
    m_hParentWnd = hWnd;
    m_hSplashWnd = CreateWindow( L"STATIC", L"", WS_POPUP|SS_BITMAP, 300, 300, 300, 300, hWnd, NULL, hInst, NULL);
    LPARAM lParm = (LPARAM)LoadImage(hInst, szImageFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION );
    if (!lParm)
       return false;

    SendMessage( m_hSplashWnd, STM_SETIMAGE, IMAGE_BITMAP, lParm );
    SetShowing(false);

    // m_hSplashWnd = CreateWindowEx(WS_EX_CLIENTEDGE, L"STATIC", L"", WS_POPUP | SS_BITMAP, 300, 300, 300, 300, hWnd, NULL, hInst, NULL);
    return true;
}

/// <summary>
/// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// </summary>
void SPLASH::Show()
{
   //get size of hSplashWnd (width and height)
   int x, y;
   int pwidth, pheight;
   int tx, ty;
   HDWP windefer;
   RECT rect, prect;
   
   GetClientRect( m_hSplashWnd, &rect );
   x = rect.right;
   y = rect.bottom;
   
   //get parent screen coordinates
   GetWindowRect( m_hParentWnd, &prect);
   
   //center splash window on parent window
   pwidth  = prect.right - prect.left;
   pheight = prect.bottom - prect.top;
   
   tx = (pwidth/2) - (x/2);
   ty = (pheight/2) - (y/2);
   
   tx += prect.left;
   ty += prect.top;
    
   windefer = BeginDeferWindowPos(1);
   DeferWindowPos( windefer, m_hSplashWnd, HWND_NOTOPMOST, tx, ty, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW|SWP_NOZORDER );
   EndDeferWindowPos( windefer );

   ShowWindow( m_hSplashWnd, SW_SHOWNORMAL);
   UpdateWindow( m_hSplashWnd );
   SetShowing(true);
}

/// <summary>
/// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// </summary>
void SPLASH::Hide()
{
   ShowWindow(m_hSplashWnd, SW_HIDE);
   SetShowing(FALSE );
}



