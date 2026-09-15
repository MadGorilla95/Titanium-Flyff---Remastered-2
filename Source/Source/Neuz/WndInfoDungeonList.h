#ifndef __WNDINFODUNGEONLIST__H__
#define __WNDINFODUNGEONLIST__H__
#ifdef __INFO_DUNGEONS

struct INFO_DUNGEONLIST
{
	DWORD monsterID;
	bool  bIsKill;
	int	  nPhase;
};

class CWndInfoDungeonList : public CWndNeuz
{
public:
	CWndInfoDungeonList();
	virtual ~CWndInfoDungeonList();

	void OnInfoDungeonList(CAr& ar);
	void RenderDungeonList(C2DRender* p2DRender);
	BOOL Process();

private:
	vector<INFO_DUNGEONLIST> vInfoDungeonList;
	int nPlayer;
	bool bDelete;
	int nStats;
	DWORD worldID;
};
#endif // __INFO_DUNGEONS
#endif // __WNDINFODUNGEONLIST__H__