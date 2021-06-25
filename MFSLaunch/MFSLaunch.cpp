/* Based on MSFS 2020 SDK sample: fsdevmodelauncher 
   
	This version offers some options:
	1) Fast Launch
	2) A timed splash screen

	Copyright (c) 2021 by Jamie O'Connell
	Licensed under the MIT License
*/

#include "stdafx.h"
#include "MFSLaunch.h"
#include "SPLASH.h"

#include <atlbase.h>
#include <ShObjIdl.h>
#include <string>

#define SZAPP			L"MFSLaunch"
#define SZINI			L"MFSLaunch.ini"
#define SZMUTEX		L"MFSMutex"
#define SZFAST			L"FastLaunch"
#define SZNOASK      L"NoAsk"
#define SZDUR        L"SplashDuration"
#define SZSHOWSPLASH L"ShowSplash"

#define SZIMAGEFILE L"Splash" 

struct COMInitRAII
{
	HRESULT	hr;
	COMInitRAII()
	{
		hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
		if (FAILED( hr ))
		{
			WCHAR wMsg[ 128 ] = { 0 };
			wsprintf( wMsg, L"Error: failed to init COM. hr = 0x%08lx \n", hr );
			MessageBox( NULL, wMsg, SZAPP, MB_OK | MB_ICONERROR );
		}
	}

	~COMInitRAII()
	{
		CoUninitialize();
	}
};

/////////////////////////////////////////////////////////////////////////
// MakeExePath
//
BOOL MakeExePath( LPTSTR pszPathName, int cbMax, LPCTSTR pszBase /* = NULL */ )
{
	// Call us with pszBase NULL or not.
	//
	// If NULL, then we simply return the .EXE path, sans the trailing '\\'.
	// If not NULL, then we append pszBase to the .EXE path to form a
	// complete pathname.

	// Warn about buffer too small under Win32!
	// pszBase must NOT contain a leading '\\'!

	// Set up pathname to be same path as .EXE
	GetModuleFileName( NULL, pszPathName, cbMax );

	// Guarantee that the base name and the path name together doesn't exceed cbMax!
	if (NULL != pszBase)
	{
		if ((int)_tcslen( pszPathName ) + (int)_tcslen( pszBase ) + 1 > cbMax)
			return FALSE;
	}

	TCHAR* p = _tcsrchr( pszPathName, '\\' );
	if (p)
	{
		if (pszBase != NULL)
			_tcscpy( p + 1, pszBase );
		else
			*p = '\0';	// zap trailing '\\'
	}
	else
	{
		if (pszBase != NULL)
			_tcscpy( pszPathName, pszBase );
		else
			*pszPathName = '\0';
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static WCHAR szIniPath[ _MAX_PATH ] = { 0 };

void MakeIniPath()
{
	MakeExePath( szIniPath, _MAX_PATH, SZINI );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void WritePrivateProfileInt(
	LPCTSTR lpAppName,
	LPCTSTR lpKeyName,
	int     nValue,
	LPCTSTR lpFileName
)
{
	_TCHAR szBuf[ 32 ] = { 0 };
	_stprintf( szBuf, L"%d", nValue );
	WritePrivateProfileString( lpAppName, lpKeyName, szBuf, lpFileName );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL GetChecked()
{
	return GetPrivateProfileInt( SZAPP, SZFAST, TRUE, LPCWSTR(szIniPath) );
}

void StoreChecked( BOOL bChecked )
{
	WritePrivateProfileInt( SZAPP, SZFAST, bChecked, LPCWSTR( szIniPath ) );
}

///////////////////////////
// 
BOOL GetNoAsk()
{
	int nResult = GetPrivateProfileInt(SZAPP, SZNOASK, -1, LPCWSTR(szIniPath));
	return nResult == TRUE; // Only if the flag exists and is set
}

void StoreNoAsk( BOOL bNoAsk )
{
	WritePrivateProfileInt(SZAPP, SZNOASK, bNoAsk, LPCWSTR(szIniPath));
}

///////////////////////////

LPCWSTR GetSplashFile( LPWSTR szPath )
{
	GetPrivateProfileString( SZAPP, SZIMAGEFILE, L"MFSSplash.dib", szPath, _MAX_PATH, LPCWSTR(szIniPath) );
	return szPath;
}

BOOL SetSplashFile(LPWSTR szPath)
{ 
	// To write the default
	return WritePrivateProfileString( SZAPP, SZIMAGEFILE, szPath, LPCWSTR(szIniPath) );
}

//////////////////////////

DWORD GetDurSec()
{
	int nSec = GetPrivateProfileInt(SZAPP, SZDUR, 30, LPCWSTR(szIniPath)); // 30 second default
	nSec = max(1, nSec);       // ignore any negative or 0 amount 

	return (DWORD)nSec;        // return seconds
}

DWORD GetDuration()
{
	return GetDurSec() * 1000;
}

void StoreDurSec( DWORD dwSec )
{
	WritePrivateProfileInt(SZAPP, SZDUR, dwSec, LPCWSTR(szIniPath));
}

void StoreDuration(DWORD dwMillisec)
{
	DWORD dwSec = dwMillisec / 1000;
	StoreDurSec(dwSec);
}

//////////////////////////

BOOL GetShowSplash()
{
	BOOL bResult = GetPrivateProfileInt(SZAPP, SZSHOWSPLASH, TRUE, LPCWSTR(szIniPath));
	return bResult != FALSE; 
}

void StoreShowSplash(BOOL bShow)
{
	WritePrivateProfileInt(SZAPP, SZSHOWSPLASH, bShow, LPCWSTR(szIniPath));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void EnableItem(HWND hDlg, UINT uID, bool bEnable)
{
	HWND hCtl = GetDlgItem(hDlg, uID);
	if (hCtl)
	{
		EnableWindow(hCtl, bEnable ? TRUE : FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::wstring BuildArguments(bool bFast)
{
	std::wstring wArgs = L"";
	if (bFast)
	   wArgs = L"-FastLaunch";

	return wArgs;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Message handler for about box.
			
INT_PTR CALLBACK LaunchDlg( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER( lParam );
	
	bool bShow;

	switch (message)
	{
	case WM_INITDIALOG:
		CheckDlgButton( hDlg, IDC_FAST, ::GetChecked() ? BST_CHECKED : BST_UNCHECKED );
		bShow = ::GetShowSplash() ? true : false;
		CheckDlgButton( hDlg, IDC_SPLASH, bShow ? BST_CHECKED : BST_UNCHECKED );
		SetDlgItemInt(  hDlg, IDC_SPLASHDUR, ::GetDurSec(), FALSE);
		::EnableItem( hDlg, IDC_SPLASHDUR, bShow );
		return (INT_PTR)TRUE;

	case WM_COMMAND:
	   if (LOWORD( wParam ) == IDCANCEL)
	   {
		   EndDialog( hDlg, IDCANCEL );
		   return (INT_PTR)TRUE;
	   }
		else if (LOWORD( wParam ) == IDC_SPLASH)
		{
			// Keep synced 
			bShow = IsDlgButtonChecked(hDlg, IDC_SPLASH) ? true : false;
			::EnableItem(hDlg, IDC_SPLASHDUR, bShow);
		}
		else if (LOWORD( wParam ) == IDC_LAUNCH)
		{
			BOOL bChecked = IsDlgButtonChecked( hDlg, IDC_FAST );
			::StoreChecked( bChecked );
			const int nFlavor =  bChecked ? IDC_FAST : IDC_SLOW;

			BOOL bNoAskAgain = IsDlgButtonChecked(hDlg, IDC_NOASK );
			::StoreNoAsk( bNoAskAgain );

			BOOL bTranslated = FALSE;
			DWORD dwSec = GetDlgItemInt(hDlg, IDC_SPLASHDUR, &bTranslated, FALSE);
			::StoreDurSec( dwSec );

			BOOL bShow = IsDlgButtonChecked(hDlg, IDC_SPLASH );
			::StoreShowSplash( bShow );

			EndDialog( hDlg, nFlavor );
			return (INT_PTR)TRUE;
		}

	   break;
   }
		
	return (INT_PTR)FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CMutexScope
{
public:
	CMutexScope()
	{
		m_hMutex = NULL;
	}

	~CMutexScope()
	{
		if (m_hMutex)
		{
			ReleaseMutex( m_hMutex );
			CloseHandle( m_hMutex );
			m_hMutex = NULL;
		}
	}

	bool GrabMutex()
	{
		m_hMutex = CreateMutex( NULL, TRUE, SZMUTEX );
		if (GetLastError() == ERROR_ALREADY_EXISTS)
			m_hMutex = NULL;
		
		return m_hMutex != NULL;
	}

private:
	HANDLE m_hMutex;
	
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{

	HWND hWndDesk = GetDesktopWindow();

	CMutexScope mScope; // Scoped mutex to prevent reentry
	if (!mScope.GrabMutex())
	{
		MessageBox(hWndDesk, L"Error: Already running - MFSLaunch", SZAPP, MB_OK | MB_ICONERROR);
		return EXIT_FAILURE;
	}

	MakeIniPath();

	WCHAR wMsg[128] = { 0 };

	// Open a dialog to ask about fast launch unless previously dismissed.
	// You can change behavior back by editing MFSLaunch.ini
	INT_PTR nResponse = IDCANCEL;
	if (!::GetNoAsk())
	{
		nResponse = DialogBoxParamW(hInstance, MAKEINTRESOURCE(IDD_LAUNCHER), hWndDesk, LaunchDlg, 0);
		if (nResponse == IDCANCEL)
			return EXIT_FAILURE;
	}
	else
	{
		nResponse = ::GetChecked() ? IDC_FAST : IDC_SLOW;
	}

	bool bFast = (nResponse == IDC_FAST) ? true : false;
	const std::wstring wArgs = BuildArguments(bFast);

	COMInitRAII comInit;
	if (FAILED(comInit.hr))
		return EXIT_FAILURE;

	HRESULT hr = S_OK;

	CComPtr<IApplicationActivationManager> AppActivationMgr = nullptr;
	hr = CoCreateInstance(CLSID_ApplicationActivationManager, nullptr, CLSCTX_LOCAL_SERVER, IID_PPV_ARGS(&AppActivationMgr));
	if (FAILED(hr))
	{
		wsprintf(wMsg, L"Error: failed to create Application Activation Manager. hr = 0x%08lx ", hr);
		MessageBox(hWndDesk, wMsg, SZAPP, MB_OK | MB_ICONERROR);
		return EXIT_FAILURE;
	}

	static LPCWSTR appUserModelIDList[] = {
		L"Microsoft.KHAlpha_8wekyb3d8bbwe!App",
		L"Microsoft.FlightSimDisc_8wekyb3d8bbwe!App",
		L"Microsoft.FlightSimulator_8wekyb3d8bbwe!App",
	};

	LPCWSTR appUserModelID = NULL;

	DWORD dwProcessId = 0;
	for (int curAppIdx = 0; curAppIdx < _countof(appUserModelIDList); curAppIdx++)
	{
		appUserModelID = appUserModelIDList[curAppIdx];
		hr = AppActivationMgr->ActivateApplication(appUserModelID, wArgs.c_str(), AO_NOERRORUI, &dwProcessId);
		if (SUCCEEDED(hr))
			break;
	}

	if (FAILED(hr))
	{
		wsprintf(wMsg, L"Error: Failed to Activate App %s. hr = 0x%08lx ", appUserModelID, hr);
		MessageBox(hWndDesk, wMsg, SZAPP, MB_OK | MB_ICONERROR);
		return EXIT_FAILURE;
	}

	// Sync to the Simulator and get ready to launch the splash screen (if it exists)
	HANDLE hProcess = ::OpenProcess(SYNCHRONIZE, FALSE, dwProcessId);
	if (hProcess == NULL)
	{
		wsprintf(wMsg, L"Error: Failed to open process for synchronization: pid = %d ", dwProcessId);
		MessageBox(hWndDesk, wMsg, SZAPP, MB_OK | MB_ICONERROR);
		return EXIT_FAILURE;
	}

	// Launch splash for 30 seconds
	// The file name (or name and path) can be customized in MFSLaunch.ini. Default: MFSSplash.dib
	// Image must be a BMP or DIB
	DWORD  dwDuration = GetDuration(); // From INI default = 30 sec (30000 millisec)
	SPLASH splash;
	WCHAR  szFile[_MAX_PATH] = { 0 };

	if (GetShowSplash() && (splash.Init(hWndDesk, hInstance, GetSplashFile(szFile))))
	{
		SetSplashFile(szFile);
		splash.Show();

		::WaitForSingleObject( hProcess, dwDuration ); // millisecond duration to display the splash (deault 30000)

		splash.Hide(); // hide it
	}

	::WaitForSingleObject( hProcess, INFINITE );     // Wait for simulator exit

	DWORD exitCode = 0;
	if (::GetExitCodeProcess( hProcess, &exitCode ))
		return exitCode;

	::CloseHandle( hProcess );

	return EXIT_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
