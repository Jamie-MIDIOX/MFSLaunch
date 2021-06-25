// SPLASH.h: interface for the SPLASH class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPLASH_H__41182F11_BB6F_11D6_B0F5_00B0D01AD687__INCLUDED_)
#define AFX_SPLASH_H__41182F11_BB6F_11D6_B0F5_00B0D01AD687__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class SPLASH  
{
public:
	void Hide();
	void Show();
	bool Init( HWND hWnd, HINSTANCE hInst, LPCWSTR szImageFile );
	bool IsShowing() { return m_bIsShowing; }

	SPLASH();
	virtual ~SPLASH();

private:
	void SetShowing(bool bShowing) { m_bIsShowing = bShowing; }

	bool m_bIsShowing;
	UINT m_TimerID;
	HWND m_hParentWnd;
	HWND m_hSplashWnd;
    
};

#endif // !defined(AFX_SPLASH_H__41182F11_BB6F_11D6_B0F5_00B0D01AD687__INCLUDED_)
