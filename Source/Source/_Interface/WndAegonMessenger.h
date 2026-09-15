#pragma once
#include "AegonSharedTypes.h"
#ifdef __AEGON_MESSENGER

#define __AEG_CHAT_STATUS_ONLINE	0
#define __AEG_CHAT_STATUS_OFFLINE	1
#define __AEG_CHAT_STATUS_AFK		2

#define __AEG_MESSAGE_TYPE_REG		1
#define __AEG_MESSAGE_TYPE_OFFLINE	2

#define __AEG_MAX_LIST_BUTTONS	5

#define __AEG_STATUS_UPDATE_INTERVAL	5000

#define __AEG_MAX_MESSAGE_LENGTH	255


typedef struct __AEGON_CHAT_MESSAGE
{
	CString m_strFrom;
	CString m_strTo;
	CString m_strMessage;
	int m_nMessageType;
	DWORD m_dwTimeStamp;

	__AEGON_CHAT_MESSAGE()
	{
		m_strFrom = "";
		m_strTo = "";
		m_strMessage = "";
		m_nMessageType = __AEG_MESSAGE_TYPE_REG;
		m_dwTimeStamp = 0;
	}

}__AEGON_CHAT_MESSAGE;

typedef struct __AEGON_CHAT_UNIT
{
	CString m_strName;
	CString m_strLastMessage;
	CEditString m_strEditString;
	int m_nLastStatus;
	int m_nUnreadMessages;
	BOOL m_bVisible;

	__AEGON_CHAT_UNIT()
	{
		Init();
	}

	void Init() 
	{
		m_strName = "";
		m_strLastMessage = "This is the beginning of this conversation.";
		m_strEditString.SetParsingString("This is the beginning of this conversation.\n", CAegonThemeSwitcherMng::GetInstance()->GetHighlightColor(HIGHLIGHT_COLOR_BLUE));
		m_nLastStatus = -1;
		m_nUnreadMessages = 0;
		m_bVisible = TRUE;
	}



}__AEGON_CHAT_UNIT;

class CAegonMessengerMng
{
	u_long m_idPlayer;

	vector<__AEGON_CHAT_UNIT> m_vAllChats;
	vector<__AEGON_CHAT_UNIT*> m_vOpenChats;
	vector<__AEGON_CHAT_MESSAGE> m_vMessageQueue;
public:

	

	CAegonMessengerMng();
	virtual ~CAegonMessengerMng();

	static CAegonMessengerMng* GetInstance(void);

	vector<__AEGON_CHAT_UNIT>* GetAllChats();
	vector<__AEGON_CHAT_UNIT*>* GetOpenChats();
	vector<__AEGON_CHAT_MESSAGE>* GetMessageQueue();

	__AEGON_CHAT_UNIT* GetChatUnitFromAll(char* szName);

	__AEGON_CHAT_UNIT* AddChatUnit(__AEGON_CHAT_UNIT sChatUnit);
	void RemoveChatUnit(int nIndex);

	void UpdateOpenChats();

	void ClearAll();

	void SetLoadedId(u_long idCurrent);
	BOOL IsInitialized(u_long idCurrent);
};


class CWndMessengerList : public CWndListBox
{
	int m_nSelectedItem;

	int m_nTotalUnreadMessages;

	CWndButton m_pWndButtons[__AEG_MAX_LIST_BUTTONS];

public:
	int m_nRowHeight;
	CRect m_rectPage;
	CRect m_rectKey;
	CRect m_rectInterval;
	CRect m_rectStatus;
	CRect m_rectAction;

	DWORD m_dwFlashAlpha;

	CWndMessengerList();
	virtual ~CWndMessengerList();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	// message
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnMouseWndSurface(CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);

	virtual void OnThemeSelectionChanged();

	void SetSelectedItem(CPoint point);
	void ToggleSelectedUnit();

	int GetSelectedItemIndex() { return m_nSelectedItem; };

	void AddChatUnit(CString strName, int nStatus);

	void OnChatMessage(CString strFrom, CString strTo, LPCSTR szMessage, int nMessageType, DWORD dwTimeStamp);

	void ProcessMessageQueue();

	void SwitchToChatUnit(CString strName);

	void SetButtonCount(int nCount);
	void CreateButtons();

	void RemoveChat(int nPos);

	void OnStatusUpdate(int nIndex, int nStatus);

	void OnUpdateUnreadMessagesCount();
};
class CWndAegonMessengerAddChat : public CWndNeuz
{
public:
	CWndAegonMessengerAddChat();
	~CWndAegonMessengerAddChat();


	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);

	void DoAddChat();
};
class CWndAegonMessenger : public CWndNeuz
{
	DWORD m_dwLastStatusUpdate;
	CWndAegonMessengerAddChat* m_pWndMessengerAddChat;


public:
	int m_nTotalUnreadMessages;

	CWndMessengerList* m_pWndMessengerList;

	__AEGON_CHAT_UNIT* m_sSelectedUnit;

	CWndAegonMessenger();
	~CWndAegonMessenger();


	virtual BOOL Initialize(CWndBase* pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT* pLResult);
	virtual	BOOL Process();
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL OnCommand(UINT nID, DWORD dwMessage, CWndBase* pWndBase);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);

	virtual void OnDestroyChildWnd(CWndBase* pWndChild);

	virtual	void PaintFrame(C2DRender* p2DRender);

	virtual void OnMinimizeToggle();

	void DoSendMessage();
	CString GetStatusString(int nStatus);
	DWORD GetStatusColor(int nStatus);

	void RefreshTextField();

	void AddChatUnit(CString strName, int nStatus);

	void OnChatMessage(CString strFrom, CString strTo, LPCSTR szMessage, int nMessageType = __AEG_MESSAGE_TYPE_REG, DWORD dwTimeStamp = 0);

	void SwitchToChat(CString strName);

	void OnStatusUpdate(int nIndex, int nStatus);

	void OnUpdateUnreadMessagesCount(int nCount);

	void MoveTopLeft();
};
#endif