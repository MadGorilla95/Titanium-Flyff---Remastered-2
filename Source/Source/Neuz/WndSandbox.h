#pragma once

#ifdef __SYS_SANDBOX
#include "WndNeuz.h"
class CWndSandbox : public CWndNeuz
{
public:
	CWndSandbox();
	~CWndSandbox();
	CWndComboBox* m_pWndComboBox;
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual	void OnInitialUpdate();
	virtual BOOL Process();
	void FillCombo();
};
#endif //__SYS_SANDBOX