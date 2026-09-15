#pragma once
#ifdef __WIKIPEDIA_MONSTER
class CWndWikipediaMonsterCreate;
class CWndWikipediaMonster : public CWndNeuz
{
private:
	vector<MoverProp*>	m_vecMover;
	CWndWikipediaMonsterCreate* m_pCreate;

	CWndListBox* m_pListbox;
	CWndEdit* m_pEdit;
public:

#ifdef __WIKIPEDIA_MONSTER_DROPLIST
	CWndDropInfo* m_pWndDropInfo;
#endif //__WIKIPEDIA_MONSTER_DROPLIST
	CWndWikipediaMonster(void);
	~CWndWikipediaMonster(void);

	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);

	void UpdateList();
	vector<MoverProp*>* GetMoverList() { return &m_vecMover; }

#ifdef __AEGON_FULLSCREEN_AND_RESIZE
	virtual void OnDestroyChildWnd(CWndBase* pWndChild);
#endif
};

class CWndWikipediaMonsterCreate : public CWndNeuz
{
private:
	MoverProp* m_pMoverPropCreate;
	CWndEdit* m_pEdit;
	CWndButton* m_pButtOk;
	CWndButton* m_pCheck1;
	CWndButton* m_pCheck2;
	CWndStatic* m_pStatic;
public:
	CWndWikipediaMonsterCreate();
	~CWndWikipediaMonsterCreate(void);
	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD dwStyle = 0);
	virtual void OnInitialUpdate();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	void SetItem(MoverProp* pMoverProp);
};
#endif // __WIKIPEDIA_MONSTER
