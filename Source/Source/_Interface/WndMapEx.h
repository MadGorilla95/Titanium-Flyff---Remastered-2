#ifndef __WND_MAP_EX_H__
#define __WND_MAP_EX_H__

#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
#include "TeleportationIconInfo.h"
#include "WndUserMarkNameChanger.h"

class CWndMapEx : public CWndNeuz
{
public:
#ifdef __CHECKLOAD
	enum ConstructionMode { NORMAL, TELEPORTATION, DESTINATION, CHECKLOAD };
#else //__CHECKLOAD
	enum ConstructionMode { NORMAL, TELEPORTATION, DESTINATION };
#endif //__CHECKLOAD

public:
	CWndMapEx( void );
	~CWndMapEx( void );

public:
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK );
	virtual	void OnInitialUpdate( void );
	virtual	BOOL Process( void );
	virtual void OnDraw( C2DRender* p2DRender );
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );
	virtual	void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );
	virtual void OnLButtonDown( UINT nFlags, CPoint point );
	virtual void OnRButtonDown( UINT nFlags, CPoint point );

public:
	void SetConstructionMode( ConstructionMode eConstructionMode );
	void InitializeTeleportationInformation( CMover* const pFocusMover );
	void UpdateDestinationPosition( void );
	void Create( DWORD dwComboBoxStyle, const RECT& rect, CWndBase* pParentWnd, UINT nID );
#ifdef __CHECKLOAD
	void addCheckLoad(std::vector<D3DXVECTOR3>&& val, const unsigned long worldId);
#endif //__CHECKLOAD

private:
	void InitializeMapComboBoxSelecting( void );
	void ResetMapInformation( void );
	void ResetNPCPosition( void );
	void RearrangeComboBoxData( DWORD dwParentSelectedListItemData, DWORD dwComboBoxWIDC , MapComboBoxDataVector* pMapComboBoxDataVector );
	void ProcessMapSizeInformation( void );
	void ProcessMonsterInformationToolTip( void );
	void ProcessUserMarkToolTip( void );
	void ProcessIconTextureAlpha( void );
#ifdef __AEGON_FIXES
	void RenderPlayerPosition(C2DRender* p2DRender, CTexture* pArrowTexture, const D3DXVECTOR3& vPlayerPosition, const D3DXVECTOR3& vCameraPosition, const CString& strName, DWORD dwNameColor, DWORD dwWorldID = WI_WORLD_NONE);
#else
	void RenderPlayerPosition( C2DRender* p2DRender, CTexture* pArrowTexture, const D3DXVECTOR3& vPlayerPosition, const D3DXVECTOR3& vCameraPosition, const CString& strName, DWORD dwNameColor );
#endif
	void RenderMapMonsterInformation( C2DRender* p2DRender );
	void RenderRainbowNPCInformation( C2DRender* p2DRender );
	void RenderTeleportationPosition( C2DRender* p2DRender );
	void RenderDestinationPosition( C2DRender* p2DRender );
	void RenderNPCPosition( C2DRender* p2DRender );
	void RenderUserMarkPosition( C2DRender* p2DRender );
	FLOAT CalculateMapIconRectFromPoint( CRect& rectDestinationIcon, FLOAT fIconPositionX, FLOAT fIconPositionY, const CTexture* const pIconTexture, FLOAT fIconSizeRatio );
	FLOAT CalculateMapIconStartPosition( CPoint& pointDestinationPosition, FLOAT fIconPositionX, FLOAT fIconPositionY, const CTexture* const pIconTexture, FLOAT fIconSizeRatio );
	const D3DXVECTOR3& ConvertPosition( D3DXVECTOR3& vDestination, const D3DXVECTOR3& vSource, BYTE byLocationID );
	const D3DXVECTOR3& ReconvertPosition( D3DXVECTOR3& vDestination, const D3DXVECTOR3& vSource, BYTE byLocationID );
	const CRect& ReviseScriptRectInformation( CRect& rectDestination, const CRect& rectSource );
#ifdef __AEGON_FIXES
	BYTE CWndMapEx::GetMapArea(const D3DXVECTOR3& vPlayerPosition, DWORD dwWorldID = WI_WORLD_NONE);
#else
	BYTE CWndMapEx::GetMapArea( const D3DXVECTOR3& vPlayerPosition );
#endif
	void CalculateMaximumWindowTileLength( void );
#ifdef __CHECKLOAD
	void renderCheckLoad(C2DRender* p2DRender);
	std::vector<D3DXVECTOR3> checkLoad;
#endif //__CHECKLOAD

private:
	enum { MINIMUM_ICON_TEXTURE_ALPHA = 0, MAXIMUM_ICON_TEXTURE_ALPHA = 255 };
	enum { NORMAL_STATE_ALPHA = 255, TRANSPARENT_STATE_ALPHA = 125 };
	enum
	{
		ID_USER_MARK_MENU_DELETE = 0, 
		ID_USER_MARK_MENU_DELETE_ALL = 1, 
		ID_USER_MARK_MENU_INSERT_CHATTING_WINDOW = 2, 
		ID_USER_MARK_MENU_CHANGE_NAME = 3, 
	};

private:
	static const FLOAT SOURCE_MAP_SIZE_X;
	static const FLOAT SOURCE_MAP_SIZE_Y;
	static const FLOAT ANIMATION_SPEED;
	static const FLOAT TELEPORTATION_POSITION_TEXTURE_SIZE_RATIO;
	static const FLOAT EXTEND_RATIO;
	static const FLOAT EXTEND_TELEPORTATION_POSITION_TEXTURE_SIZE_RATIO;
	static const FLOAT DESTINATION_POSITION_TEXTURE_SIZE_RATIO;
	static const FLOAT NPC_POSITION_TEXTURE_SIZE_RATIO;
	static const FLOAT USER_MARK_POSITION_TEXTURE_SIZE_RATIO;
	static const int WINDOW_TILE_TEXTURE_SIZE_XY;
	static const int MINIMUM_WINDOW_TILE_NUMBER_X;
	static const int MINIMUM_WINDOW_TILE_NUMBER_Y;
	bool MonsterInfo;

private:
	ConstructionMode m_eConstructionMode;
	CTexture* m_pPCArrowTexture;
	CTexture* m_pPartyPCArrowTexture;
	CTexture* m_pMapTexture;
	CTexture* m_pTeleportationPositionTexture;
	CTexture* m_pDestinationPositionTexture;
	CTexture* m_pNPCPositionTexture;
	CTexture* m_pUserMarkPositionTexture;
	FLOAT m_fRevisedMapSizeRatio;
	CRect m_rectRevisedMapPosition;
	DWORD m_dwSelectedMapID;
	BYTE m_bySelectedMapLocationID;
	BOOL m_bMonsterInformationToolTip;
	int m_nSelectedMonsterIconIndex;
	DWORD m_dwSelectedUserMarkID;
	int m_nIconTextureAlpha;
	DWORD m_tmOld;
	BOOL m_bAlphaSwitch;
	OBJID m_idTeleporter;
	vector< TeleportationIconInfo* > m_vecTeleportationPositionRect;
	FLOAT m_fDestinationPositionX;
	FLOAT m_fDestinationPositionY;
	FLOAT m_fNPCPositionX;
	FLOAT m_fNPCPositionY;
	BOOL m_bMapComboBoxInitialization;
	BYTE m_byTransparentStateAlpha;
	CWndMenu m_WndMenuUserMark;
	CWndUserMarkNameChanger* m_pWndUserMarkNameChanger;
	int m_nMinimumWindowTileWidth;
	int m_nMinimumWindowTileHeight;
	int m_nMaximumWindowTileWidth;
	int m_nMaximumWindowTileHeight;
};
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM

#endif // __WND_MAP_EX_H__