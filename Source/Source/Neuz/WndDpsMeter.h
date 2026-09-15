#pragma once

#ifdef __DPS_METER
class CWndDpsMeter
{
public:
	CWndDpsMeter();
	~CWndDpsMeter();
	static		CWndDpsMeter* GetInstance()
	{
		static	CWndDpsMeter wnd;
		return	&wnd;
	}

	void			InitDeviceObjects();
	void			DeleteDeviceObjects();
	void			Render(C2DRender* p2DRender, const int& nWindowHeight, const int& left);
	void			SetDisplay(bool s) { bDisplay = s; }
	void			ToggleDisplay() {
		//if (lastToggleTick < GetTickCount() - 500) {
			bDisplay = !bDisplay;
		//	lastToggleTick = GetTickCount();
		//}
	}

private:
	map<CString, float> dpsMap;
	map<CString, DWORD> colorMap;
	bool bDisplay;
	DWORD dpsColor[MAX_JOB];
	DWORD lastToggleTick;
};
#endif //__DPS_METER