#pragma once 

#ifdef __WBQT_GUILD_BUFFS
class CWndGuildTabBuffs : public CWndNeuz
{
public:
	CWndGuildTabBuffs();
	virtual ~CWndGuildTabBuffs();

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);

	void UpdateData();
	void SetData(DWORD dwPower[]);
};
#endif //__WBQT_GUILD_BUFFS
