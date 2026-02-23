#pragma once
#include <Windows.h>
class Window
{
public:
	Window();
	
	bool broadcast();
	bool isRun();

	RECT getClientWindowRect();

	RECT getScreenSize();

	

	virtual void onCreate() = 0;
	virtual void onUpdate() = 0;
	virtual void onDestroy();
	virtual void onFocus();
	virtual void onKillFocus();
	virtual void onSize();



	
	~Window();
protected:
	HWND m_hwnd;
	bool m_is_run;
	bool m_is_init=false;
};

