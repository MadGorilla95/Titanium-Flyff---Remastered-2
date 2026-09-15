USE [CHARACTER_01_DBF]
GO
/****** Object:  StoredProcedure [dbo].[CHARACTER_STR]    Script Date: 22.10.2020 09:18:53 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

ALTER proc [dbo].[CHARACTER_STR]
	@iGu        		  				CHAR(2) 			= 'S1', 
	@im_idPlayer   				CHAR(7) 			= '0000001',
	@iserverindex  				CHAR(2) 			= '01',
	/**********************************************
	 INSERT Â¢Â¯e
	**********************************************/
	-- CHARACTER_TBL
	@iaccount 						VARCHAR(32)	= '',
	@im_szName 				VARCHAR(32)	= '',
	@iplayerslot 					INT						= 0,
	@idwWorldID 				INT 						= 0,
	@im_dwIndex 				INT 						= 0,
	@im_vPos_x 					REAL 					= 0,
	@im_vPos_y 					REAL 					= 0,
	@im_vPos_z 					REAL 					= 0,
	@im_szCharacterKey 	VARCHAR(32)	= '',
	@im_dwSkinSet 			INT						= 0,
	@im_dwHairMesh 		INT						= 0,
	@im_dwHairColor 		INT						= 0,
	@im_dwHeadMesh 		INT						= 0,
	@im_dwSex 					INT						= 0,
	/**********************************************
	 UPDATE Â¢Â¯e
	**********************************************/
	-- CHARACTER_TBL
	@im_vScale_x				REAL					=	0,
	@im_dwMotion				INT						=	0,
	@im_fAngle					REAL					=	0,
	@im_nHitPoint				INT						=	0,
	@im_nManaPoint			INT						=	0,
	@im_nFatiguePoint		INT						=	0,
	@im_dwRideItemIdx		INT						=	0,
	@im_dwGold					INT						=	0,
	@im_nJob						INT						=	0,
	@im_pActMover				VARCHAR(50)	=	'',
	@im_nStr						INT						=	0,
	@im_nSta						INT						=	0,
	@im_nDex						INT						=	0,
	@im_nInt							INT						=	0,
	@im_nLevel					INT						=	0,
	@im_nExp1					BIGINT						=	0,
	@im_nExp2					BIGINT						=	0,
	@im_aJobSkill				VARCHAR(500)	='',
	@im_aLicenseSkill		VARCHAR(500)	='',
	@im_aJobLv					VARCHAR(500)	='',
	@im_dwExpertLv			INT						=	0,
	@im_idMarkingWorld	INT						=	0,
	@im_vMarkingPos_x	REAL					=	0,
	@im_vMarkingPos_y	REAL					=	0,
	@im_vMarkingPos_z	REAL					=	0,
	@im_nRemainGP			INT						=	0,
	@im_nRemainLP			INT						=	0,
	@im_nFlightLv				INT						=	0,
	@im_nFxp						INT						=	0,
	@im_nTxp						INT						=	0,
	@im_lpQuestCntArray	VARCHAR(3072)= '',
	@im_chAuthority			CHAR(1)				= '',
	@im_dwMode				INT						=	0,
	@im_idparty					INT						=	0,
	@im_idMuerderer			INT						=	0,
	@im_nFame					INT						=	0,
	@im_nDeathExp				BIGINT					=  0,
	@im_nDeathLevel				INT					=  0,
	@im_dwFlyTime					INT					=  0,
	@im_nMessengerState 	INT					=  0,
	@iTotalPlayTime			INT						= 	0
	-------------- (ADD : Version8-PK System)
	,@im_nPKValue    		int=0
	,@im_dwPKPropensity    	int=0
	,@im_dwPKExp     		int=0
	-- CARD_CUBE_TBL
	,@im_Card 						VARCHAR(1980)= '',
	@im_Index_Card 			VARCHAR(215) 	= '',
	@im_ObjIndex_Card 	VARCHAR(215) 	= '',
	@im_Cube 						VARCHAR(1980)= '',
	@im_Index_Cube 			VARCHAR(215) 	= '',
	@im_ObjIndex_Cube 	VARCHAR(215) 	= '',
	-- INVENTORY_TBL
	@im_Inventory 				VARCHAR(MAX)= '',
	@im_apIndex 				VARCHAR(2500) 	= '',
	@im_adwEquipment 	VARCHAR(135) 	= '',
	@im_dwObjIndex 			VARCHAR(2500) 	= '',
	-- TASKBAR_TBL
	@im_aSlotApplet 			VARCHAR(3100)= '',
	-- TASKBAR_ITEM_TBL
	@im_aSlotItem 				VARCHAR(6885)= '',
	-- TASKBAR_TBL
	@im_aSlotQueue 			VARCHAR(225)= '',
	@im_SkillBar					SMALLINT			= 0,
	-- BANK_TBL
	@im_Bank						VARCHAR(4290)= '',
	@im_apIndex_Bank		VARCHAR(215)= '',
	@im_dwObjIndex_Bank VARCHAR(215)= '',
	@im_dwGoldBank			INT						= 0,
	@im_nFuel						INT						= -1,
	@im_tmAccFuel				INT 						= 0,
	@im_dwSMTime			VARCHAR(2560)='',
	@iSkillInfluence				varchar(2048) ='',
	@im_dwSkillPoint			INT 						= 0,
	@im_aCompleteQuest	varchar(3072) = '',
	@im_extInventory			varchar(max) = '',
	@im_extBank					varchar(2000) = '',
	@im_InventoryPiercing varchar(max) = '',
	@im_BankPiercing		varchar(8000) = '',
	@im_dwReturnWorldID INT		 				= 1,
	@im_vReturnPos_x 		REAL					= 0,
	@im_vReturnPos_y 		REAL					= 0,
	@im_vReturnPos_z 		REAL					= 0,
	-------------- ( Version 7 : Skill Update)
	@im_SkillPoint			int=0,
	@im_SkillLv				int=0,
	@im_SkillExp			bigint=0,
	-------------- (AÂ©Â¬Â¡Ã†Â¢Â® Â¨Â¬IÂ¨Â¬Â¨Â¢ : 2006 11 13 Attendant Class)
	@idwEventFlag                   bigint=0,
	@idwEventTime          int=0,


	@idwEventElapsed                int=0
	-------------- (ADD : Version8-Angel System)
	,@im_nAngelExp		bigint=0
	,@im_nAngelLevel		int=0
	--------------- Version 9 AÂ©Â¬Â¡Ã†Â¢Â® Â¨Â¬IÂ¨Â¬Â¨Â¢ PetÂ¡Ã†uÂ¡Â¤A
,@iszInventoryPet	varchar(max)     = '$'

,@iszBankPet	varchar(4200)     = '$'
,@im_dwPetId int = -1

,@im_nExpLog int = 0
,@im_nAngelExpLog int = 0
,@im_nCoupon int = 0
--------------- ver. 13
, @im_nHonor int = -1
, @im_nLayer int = 0
---------- Ver 15
--, @im_BankPW char(4) = '0000'
, @im_aCheckedQuest varchar(100) =''
, @im_nCampusPoint int = 0
, @im_idCampus int = 0
, @im_idVendorNPC int = 0				/*Offline Vendor*/
, @im_nHideCoat int = 0				/*Hide CS*/
, @im_szOwnTitle varchar(20) = '' 	/*User Own Title*/
, @im_nLootboxItem int = 0			/*Lootbox*/
, @im_nLootboxItemNum int = 0		/*Lootbox*/
, @im_szAchFav varchar(max) = ''	/*Achievement Favorites*/
, @im_ncR int = -1					/*Name Color Change*/
, @im_ncG int = -1					/*Name Color Change*/
, @im_ncB int = -1					/*Name Color Change*/
/*******************************************************
	Gu Â¡Â¾Â¢Â¬Â¨Â¬Â¨Â¢
    S : SELECT
    I  : INSERT
    U : UPDATE
    D : DELETE


2005.04.11 updated

ALTER TABLE  CHARACTER_TBL  ADD   m_aCompleteQuest  varchar(1024) NULL
ALTER TABLE CHARACTER_TBL  ALTER COLUMN   m_lpQuestCntArray	VARCHAR(3072) NULL

*******************************************************/
AS
set nocount on
declare @last_connect tinyint
set @last_connect = 1

DECLARE @om_chLoginAuthority CHAR(1),@oaccount VARCHAR(32),@oplayerslot INT

IF @iGu = 'S2' -- Â¨Ã¶Â¨Ã¶Â¡Â¤OÂ¢Â¯Â¢Â® Â¥Ã¬uÂ¢Â¬Â¡Ã CAÂ¡Â¤Â©Ã¶AIÂ¨ÃºiÂ¢Â¬Â¢Ã§Â¨Ã¶Â¨Â¬Â¨Â¡Â¢Ã§ AIÂ¨Â¬Â¡ÃAaÂ¢Â¬Â¢Ã§AÂ¢Â´Â¨Â¬Â¢Â¬  Â¡Ã†Â¢Â®AÂ¢Ã§Â¢Â¯AÂ¡Â¾a
	BEGIN
		IF @iaccount = '' OR @im_szName  = ''
 		BEGIN
 			SELECT m_chAuthority = '',fError = '1', fText = 'Â¨ÃºIEÂ¡ÃŒÂ¨Â¡Â©Ã·Â¢Â¬Â©Ã·'
 			RETURN
 		END

		select playerslot,max(m_idPlayer) as m_idplayer
		into #temp_realPlayerslot
		from dbo.CHARACTER_TBL A
		where A.isblock = 'F' AND A.account = @iaccount AND A.serverindex = @iserverindex  
		group by playerslot

			SELECT	A.dwWorldID, 
							A.m_szName,
					 		A.playerslot,
							A.End_Time, 
							A.BlockTime,
							A.m_dwIndex, 
							A.m_idPlayer, 
							A.m_idparty,
							A.m_dwSkinSet, 
							A.m_dwHairMesh, 
							A.m_dwHeadMesh, 
							A.m_dwHairColor, 
							A.m_dwSex, 
							A.m_nJob, 
							A.m_nLevel, 
							A.m_vPos_x, 
							A.m_vPos_y, 
							A.m_vPos_z, 
							A.m_nStr,
							A.m_nSta,
							A.m_nDex,
							A.m_nInt,
							A.m_aJobLv,
							A.m_chAuthority,
							A.m_idCompany,
							A.m_nMessengerState,
							B.m_Inventory, 
							B.m_apIndex,
							B.m_adwEquipment,
							B.m_dwObjIndex,
							m_idGuild = CASE WHEN C.m_idGuild  IS NULL THEN '0' ELSE C.m_idGuild END	,
							m_idWar = CASE WHEN C.m_idWar  IS NULL THEN '0' ELSE C.m_idWar END,
							D.m_extInventory,
							D.m_InventoryPiercing,
							------------- ver. 13
							A.m_nHonor,
							last_connect = @last_connect
			FROM CHARACTER_TBL as A 
				inner join INVENTORY_TBL as B on A.m_idPlayer = B.m_idPlayer and A.serverindex = B.serverindex
				inner join INVENTORY_EXT_TBL as D on B.serverindex = D.serverindex and B.m_idPlayer = D.m_idPlayer
				inner join #temp_realPlayerslot as ttt on A.m_idPlayer = ttt.m_idPlayer AND A.playerslot= ttt.playerslot
				left outer join GUILD_MEMBER_TBL as C on D.m_idPlayer = C.m_idPlayer and D.serverindex = C.serverindex
			WHERE 	A.isblock = 'F'
				AND A.account = @iaccount  
				AND A.serverindex = @iserverindex
			ORDER BY A.playerslot

insert into CHARACTER_TBL_penya_check (account, m_szName, m_dwGold, check_sec, serverindex)
select @iaccount, m_szName, m_dwGold, 9, @iserverindex
from CHARACTER_TBL (nolock)
where account = @iaccount and serverindex = @iserverindex and TotalPlayTime < 1 and m_dwGold >= 1

				RETURN
 	END
/*
	
	 Â¨Ã¶Â¨Ã¶Â¡Â¤OÂ¢Â¯Â¢Â® Â¥Ã¬uÂ¢Â¬Â¡Ã CAÂ¡Â¤Â©Ã¶AIÂ¨ÃºiÂ¢Â¬Â¢Ã§Â¨Ã¶Â¨Â¬Â¨Â¡Â¢Ã§ AIÂ¨Â¬Â¡ÃAaÂ¢Â¬Â¢Ã§AÂ¢Â´Â¨Â¬Â¢Â¬  Â¡Ã†Â¢Â®AÂ¢Ã§Â¢Â¯AÂ¡Â¾a
	 ex ) 
	 CHARACTER_STR 'S2',@im_idPlayer (iMode),@iserverindex,@iaccount,@im_szName (iPassword)
	 CHARACTER_STR 'S2','0','02','seghope','1234'


*/

ELSE
IF @iGu = 'S3' -- Â¨Ã¹Â¡Â©Â©Ã¶oÂ¡Ã†Â¢Â® AÂ©Ã¸AÂ¨Ã¶Â¨Ã¶CCaAÂ¡Ã­ COÂ¢Â¥e AÂ©Ã¸Â¢Â¬?AIAC idPlayerÂ¢Â¬| Â¢Â¥U Â¡Ã†Â¢Â®AoÂ¡Ã†iÂ¢Â¯E
	BEGIN
		 SELECT m_szName, m_idPlayer,m_idCompany
			FROM CHARACTER_TBL 
		 WHERE serverindex = @iserverindex 
--			  AND  isblock = 'F'
		 ORDER BY m_idPlayer
		RETURN
	END
/*
	
	 Â¨Ã¹Â¡Â©Â©Ã¶oÂ¡Ã†Â¢Â® AÂ©Ã¸AÂ¨Ã¶Â¨Ã¶CCaAÂ¡Ã­ COÂ¢Â¥e AÂ©Ã¸Â¢Â¬?AIAC idPlayerÂ¢Â¬| Â¢Â¥U Â¡Ã†Â¢Â®AoÂ¡Ã†iÂ¢Â¯E
	 ex ) 
	 CHARACTER_STR 'S3','',@iserverindex
	 CHARACTER_STR 'S3','','01'



*/

ELSE
IF @iGu = 'S4' -- AÂ©Â¬Â¡Ã†Â¢Â®CO Â¨ÃºÂ¨Â¡AIAU EÂ¢Ã§AI
	BEGIN

	declare @s4_account varchar(32), @i1_e_date datetime

		declare @q1 nvarchar(4000)

		set @q1 = '
		SELECT 	Item_Name, Item_count, m_nAbilityOption, m_nNo, m_bItemResist, m_nResistAbilityOption,
			m_bCharged, nPiercedSize, adwItemId0, adwItemId1, adwItemId2, adwItemId3, adwItemId4,
			m_dwKeepTime, nRandomOptItemId,
			isnull(adwItemId5, 0) as adwItemId5, isnull(adwItemId6, 0) as adwItemId6, isnull(adwItemId7, 0) as adwItemId7, isnull(adwItemId8, 0) as adwItemId8, isnull(adwItemId9, 0) as adwItemId9, isnull(nUMPiercedSize, 0) as nUMPiercedSize,
			isnull(adwUMItemId0, 0) as adwUMItemId0, isnull(adwUMItemId1, 0) as adwUMItemId1, isnull(adwUMItemId2, 0) as adwUMItemId2, isnull(adwUMItemId3, 0) as adwUMItemId3, isnull(adwUMItemId4, 0) as adwUMItemId4
		FROM ITEM_SEND_TBL 
		WHERE m_idPlayer = @im_idPlayer 	
			AND serverindex = @iserverindex
			AND ItemFlag = 0'
		exec sp_executesql @q1, N'@im_idPlayer char(7), @iserverindex char(2)', @im_idPlayer, @iserverindex
		RETURN
	END
/*
	

	 Â¨ÃºÂ¨Â¡AIAU EÂ¢Ã§AI
	 ex ) 
	 CHARACTER_STR 'S4',@im_idPlayer,@iserverindex
	 CHARACTER_STR 'S4','000001','01'

*/

ELSE
IF @iGu = 'S5' -- Â¨ÃºÂ¨Â¡AIAU AoÂ¡Â¾Â¨Â­EA AÂ¡Â¿AIÂ¨Â¬iÂ¢Â¯Â¢Â®Â¨Ã¹Â¡Â© Â¨ÃºÂ¨Â¡AIAU Â¡Ã­eA|
	BEGIN
--		DELETE ITEM_SEND_TBL 
		UPDATE ITEM_SEND_TBL SET ProvideDt=getdate(), ItemFlag=1
 		 WHERE	m_nNo  = @iplayerslot
		IF @@ROWCOUNT = 0
		SELECT fError = '0'
		ELSE
		SELECT fError = '1'
		RETURN
	END
/*
	
	 Â¨ÃºÂ¨Â¡AIAU AoÂ¡Â¾Â¨Â­EA AÂ¡Â¿AIÂ¨Â¬iÂ¢Â¯Â¢Â®Â¨Ã¹Â¡Â© Â¨ÃºÂ¨Â¡AIAU Â¡Ã­eA|
	 ex ) 
	 CHARACTER_STR 'S5',@im_idPlayer,@iserverindex,@iaccount
	 CHARACTER_STR 'S5','000001','01','Â¨Â¬Â©ÂªAUÂ¡Â¤c',1,1
*/

ELSE
IF @iGu = 'S6' -- Â¡Ã­eA|CO Â¨ÃºÂ¨Â¡AIAU EÂ¢Ã§AI

	BEGIN
		SELECT 	Item_Name, 
						Item_count,
						m_nAbilityOption,
						m_nNo,
						State,
						m_bItemResist,
						m_nResistAbilityOption
			FROM ITEM_REMOVE_TBL 
 		 WHERE	m_idPlayer   				= @im_idPlayer 	
			  AND 	serverindex 				= @iserverindex
			  AND  	ItemFlag = 0
	RETURN
	END
/*
	Â¨ÃºÂ¨Â¡AIAU EÂ¢Ã§AI
	ex )
	 CHARACTER_STR 'S6',@im_idPlayer,@iserverindex
	 CHARACTER_STR 'S6','000001','01'
*/	

ELSE
IF @iGu = 'S7' -- Â¨ÃºÂ¨Â¡AIAU Â¡Ã­eA|EA AÂ¡Â¿AIÂ¨Â¬iÂ¢Â¯Â¢Â®Â¨Ã¹Â¡Â© Â¨ÃºÂ¨Â¡AIAU Â¡Ã­eA|
	BEGIN
--		DELETE  ITEM_REMOVE_TBL 
		UPDATE ITEM_REMOVE_TBL SET DeleteDt=getdate(), ItemFlag=1
 		 WHERE	m_nNo  = @iplayerslot

		IF @@ROWCOUNT = 0
		SELECT fError = '0'
		ELSE
		SELECT fError = '1'
		RETURN
	END
/*
	 Â¨ÃºÂ¨Â¡AIAU AoÂ¡Â¾Â¨Â­EA AÂ¡Â¿AIÂ¨Â¬iÂ¢Â¯Â¢Â®Â¨Ã¹Â¡Â© Â¨ÃºÂ¨Â¡AIAU Â¡Ã­eA|
	 ex ) 
	 CHARACTER_STR 'S7',@im_idPlayer,@iserverindex,@iaccount
	 CHARACTER_STR 'S7','000001','01','Â¨Â¬Â©ÂªAUÂ¡Â¤c',1,1
*/

IF @iGu = 'S8' -- Â¥Ã¬Â¡ÃAIAI AuAÂ¨Ã¹ Â¡Ã†Â¢Â®AÂ¢Ã§Â¢Â¯AÂ¡Â¾a
	BEGIN


				SELECT @om_chLoginAuthority = m_chLoginAuthority
				  FROM  ACCOUNT_DBF.dbo.ACCOUNT_TBL_DETAIL 
				WHERE account   = @iaccount
						
				
				SELECT	m_chLoginAuthority = @om_chLoginAuthority,
								A.account,
								A.m_idPlayer,
								A.playerslot,
								A.serverindex,
								A.dwWorldID,
								A.m_szName,
								A.m_dwIndex,
								A.m_vScale_x,
								A.m_dwMotion,
								A.m_vPos_x,
								A.m_vPos_y,
								A.m_vPos_z,
								A.m_fAngle,
								A.m_szCharacterKey,
								A.m_idPlayer,
								A.m_nHitPoint,
								A.m_nManaPoint,
								A.m_nFatiguePoint,
								A.m_nFuel,
								A.m_dwSkinSet,
								A.m_dwHairMesh,
								A.m_dwHairColor,
								A.m_dwHeadMesh,
								A.m_dwSex,
								A.m_dwRideItemIdx,
								A.m_dwGold,
								A.m_nJob,
								A.m_pActMover,
								A.m_nStr,
								A.m_nSta,
								A.m_nDex,
								A.m_nInt,
								A.m_nLevel,

								A.m_nMaximumLevel,
								A.m_nExp1,
								A.m_nExp2,
								A.m_aJobSkill,
								A.m_aLicenseSkill,
								A.m_aJobLv,
								A.m_dwExpertLv,
								A.m_idMarkingWorld,
								A.m_vMarkingPos_x,
								A.m_vMarkingPos_y,
								A.m_vMarkingPos_z,
								A.m_nRemainGP,
								A.m_nRemainLP,
								A.m_nFlightLv,
								A.m_nFxp,
								A.m_nTxp,
								A.m_lpQuestCntArray,
								m_aCompleteQuest = ISNULL(A.m_aCompleteQuest,'$'),
								A.m_chAuthority,
								A.m_dwMode,
								A.m_idparty,
								A.m_idCompany,
								A.m_idMuerderer,
								A.m_nFame,
								A.m_nDeathExp,
								A.m_nDeathLevel,
								A.m_dwFlyTime,
								A.m_nMessengerState,
								A.End_Time,
								A.BlockTime,
								A.blockby,
								A.isblock,
								A.TotalPlayTime,
								A.CreateTime,
								A.m_dwSkillPoint,
								B.m_aSlotApplet,								
								B.m_aSlotQueue,
								B.m_SkillBar,
								C.m_aSlotItem,
								D.m_Inventory,
								D.m_apIndex,
								D.m_adwEquipment,
								D.m_dwObjIndex,
								m_idGuild = ISNULL(G.m_idGuild,'0'),
								m_idWar = ISNULL(G.m_idWar,'0'),
								A.m_tmAccFuel,
								A.m_tGuildMember,		
								m_dwSMTime = ISNULL(H.m_dwSMTime,'NULL')	,
								SkillInfluence = ISNULL(E.SkillInfluence,'$'),
								F.m_extInventory,																						 		
								F.m_InventoryPiercing,
								A.m_dwReturnWorldID,
								A.m_vReturnPos_x,
								A.m_vReturnPos_y,
								A.m_vReturnPos_z,
								last_connect = @last_connect,
								A.m_SkillPoint,
								A.m_SkillLv,
						        A.m_SkillExp,
						      -------------- (2006 11 13 AÂ©Â¬Â¡Ã†Â¢Â® Â¨Â¬IÂ¨Â¬Â¨Â¢ : Attedant Event)
						        A.dwEventFlag,
						        A.dwEventTime,
						        A.dwEventElapsed
						      -------------- (Version8 : PK System)
								,A.PKValue 		as m_nPKValue
								,A.PKPropensity as m_dwPKPropensity
								,A.PKExp 		as m_dwPKExp
						      -------------- (Version8 : Angel System)
								,A.AngelExp 	as m_nAngelExp
								,A.AngelLevel 	as m_nAngelLevel
							------------------- Version9 Pet

								,F.szInventoryPet as szInventoryPet
								,A.m_dwPetId
								, A.m_nExpLog, A.m_nAngelExpLog
								,m_nCoupon
								------------ ver. 13
								, A.m_nLayer
								---------- Ver 15
								, A.m_aCheckedQuest	
								, A.m_nCampusPoint
								, A.idCampus
								, isnull(R.m_nRestPoint, 0) m_nRestPoint
								, isnull(R.m_LogOutTime, 0) m_LogOutTime
								, isnull(MONST.m_nMobID, '$') m_nMobID /*Monsterhunt system*/
								, isnull(MONST.m_nCount, '$') m_nCount /*Monsterhunt system*/
								, isnull(MONST.m_nRewardSent, '$') m_nRewardSent /*Monsterhunt system*/
								, A.m_idVendorNPC	/*Offline Vendor*/
								, A.m_nHideCoat 	/*Hide CS*/
								, A.m_szOwnTitle 	/*User Own Title*/
								, A.m_nLootboxItem 	/*Lootbox*/
								, A.m_nLootboxItemNum /*Lootbox*/
								, isnull(BAD.m_nBadge, -1) m_nBadge /*Badge system*/
								, isnull(BAD.m_Badges, '$') m_Badges /*Badge system*/
								, A.m_szAchFav /*Achievement Favorites*/
								, A.m_ncR /*Name Color Change*/
								, A.m_ncG /*Name Color Change*/
								, A.m_ncB /*Name Color Change*/
			FROM CHARACTER_TBL A 
					inner join TASKBAR_TBL B on A.m_idPlayer   = B.m_idPlayer and A.serverindex  = B.serverindex
					inner join TASKBAR_ITEM_TBL C on B.m_idPlayer   = C.m_idPlayer and B.serverindex  = C.serverindex
					inner join INVENTORY_TBL D on C.m_idPlayer   = D.m_idPlayer and C.serverindex  = D.serverindex
					inner join SKILLINFLUENCE_TBL E on D.m_idPlayer   = E.m_idPlayer and D.serverindex  = E.serverindex
					inner join INVENTORY_EXT_TBL F on E.m_idPlayer   = F.m_idPlayer and E.serverindex  = F.serverindex
					left outer join GUILD_MEMBER_TBL G on F.serverindex = G.serverindex and F.m_idPlayer = G.m_idPlayer
					left outer join BILING_ITEM_TBL H on F.serverindex = H.serverindex and F.m_idPlayer = H.m_idPlayer
					left outer join tblRestPoint R on F.serverindex = R.serverindex and F.m_idPlayer = R.m_idPlayer
					left join MONSTERHUNT_TBL MONST on R.serverindex = MONST.serverindex and R.m_idPlayer = MONST.m_idPlayer /*Monsterhunt system*/
					left join BADGES_TBL BAD on R.serverindex = BAD.serverindex and R.m_idPlayer = BAD.m_idPlayer /*Badge system*/
			WHERE A.m_idPlayer = @im_idPlayer
					AND A.serverindex = @iserverindex
					AND A.account = lower(@iaccount)
					AND A.isblock = 'F' -- fix

insert into CHARACTER_TBL_validity_check (m_idPlayer, serverindex, account, m_szName, TotalPlayTime, m_dwGold, m_nLevel, m_nJob, sum_ability, CreateTime)
select m_idPlayer, serverindex, account, m_szName, TotalPlayTime, m_dwGold, m_nLevel, m_nJob, (m_nStr + m_nSta + m_nDex + m_nInt), CreateTime
from CHARACTER_TBL (nolock)
where m_idPlayer = @im_idPlayer and TotalPlayTime <= 1
	and (m_dwGold >= 1 or m_nLevel >= 2 or m_nJob >= 1 or (m_nStr + m_nSta + m_nDex + m_nInt) > 60)


declare @m_dwGold_old bigint, @m_dwGold_now bigint
select @m_dwGold_old = m_dwGold from tblLogout_Penya (nolock) where m_idPlayer = @im_idPlayer
select @m_dwGold_now = m_dwGold from CHARACTER_TBL (nolock) where serverindex = @iserverindex and m_idPlayer = @im_idPlayer
if (@m_dwGold_old <> @m_dwGold_now)
begin
	insert into tblLogout_Penya_Diff_Log (serverindex, m_idPlayer, m_dwGold_old, regdate_old, m_dwGold_now)
	select serverindex, m_idPlayer, @m_dwGold_old, regdate, @m_dwGold_now
	from tblLogout_Penya (nolock)
	where m_idPlayer = @im_idPlayer and serverindex = @iserverindex
end

				-- Â©Ã¶Â©Â£AÂ¨Ã AÂ¢Â´Â¨Â¬Â¢Â¬ Â¡Ã†Â¢Â®AÂ¢Ã§Â¢Â¯AÂ¡Â¾a account Â¨Â¬Â¡Ã†
	
-- 				DECLARE @bank TABLE (m_idPlayer CHAR(6),serverindex CHAR(2),playerslot INT)
-- 
-- 				INSERT @bank
-- 				(m_idPlayer,serverindex,playerslot)
--               SELECT m_idPlayer,serverindex,playerslot 
--               FROM CHARACTER_TBL 
--               WHERE account = @iaccount 
--               AND isblock = 'F'
--               ORDER BY playerslot

				SELECT  a.m_idPlayer,
								c.playerslot,
 								a.m_Bank,
								a.m_apIndex_Bank,
								a.m_dwObjIndex_Bank,
								a.m_dwGoldBank,
								a.m_BankPw,				 		
								b.m_extBank,
						                            b.m_BankPiercing
								,b.szBankPet
				   FROM 	dbo.BANK_TBL a,
                            dbo.BANK_EXT_TBL b,	
                            dbo.CHARACTER_TBL  c
               WHERE 	a.m_idPlayer = b.m_idPlayer
                    AND a.serverindex = b.serverindex
                    AND b.m_idPlayer = c.m_idPlayer
                    AND b.serverindex = c.serverindex
 					  AND c.account = @iaccount 
                    AND c.isblock = 'F'
				ORDER BY c.playerslot

				-- Pocket Info
			SELECT	a.nPocket,
				a.szItem,
				a.szIndex,
				a.szObjIndex,
				a.bExpired,
				a.tExpirationDate,
				b.szExt,
				b.szPiercing,
				b.szPet
			FROM	tblPocket as a inner join tblPocketExt as b
				on a.serverindex = b.serverindex AND a.idPlayer = b.idPlayer AND a.nPocket = b.nPocket
			WHERE a.serverindex = @iserverindex AND a.idPlayer = @im_idPlayer
			ORDER BY a.nPocket

		RETURN
	END
/*
	
	 Â¥Ã¬Â¡ÃAIAI AuAÂ¨Ã¹ Â¡Ã†Â¢Â®AÂ¢Ã§Â¢Â¯AÂ¡Â¾a New
	 ex ) 
	 CHARACTER_STR 'S8',@im_idPlayer,@iserverindex,@iaccount
	 CHARACTER_STR 'S8','425120','01','ata3k'
*/
ELSE
IF @iGu = 'U1' -- AÂ©Ã¸Â¢Â¬?AI AuAa
	BEGIN
		UPDATE CHARACTER_TBL
		      SET	dwWorldID 				= @idwWorldID,
						m_dwIndex 				= @im_dwIndex,			
						m_dwSex	 				= @im_dwSex,
						m_vScale_x 				= @im_vScale_x,						
						m_dwMotion 				= @im_dwMotion,
						m_vPos_x 					= @im_vPos_x,
						m_vPos_y 					= @im_vPos_y,
						m_vPos_z 					= @im_vPos_z,
						m_dwHairMesh    	= @im_dwHairMesh,
						m_dwHairColor	    	= @im_dwHairColor,
						m_dwHeadMesh	   	= @im_dwHeadMesh,  -- 2004/11/08   AÂ©Â¬Â¡Ã†Â¢Â®  
						m_fAngle 					= 0, --@im_fAngle,
						m_szCharacterKey 	= @im_szCharacterKey,
						m_nHitPoint 				= @im_nHitPoint,
						m_nManaPoint 			= @im_nManaPoint,
						m_nFatiguePoint 		= @im_nFatiguePoint,
						m_nFuel						= @im_nFuel,
						m_dwRideItemIdx 		= @im_dwRideItemIdx,
						m_dwGold 					= @im_dwGold,
						m_nJob 						= @im_nJob,
						m_pActMover 			= @im_pActMover,
						m_nStr 						= @im_nStr,
						m_nSta 						= @im_nSta,
						m_nDex 						= @im_nDex,
						m_nInt 						= @im_nInt,
						m_nLevel 					= @im_nLevel,
						m_nMaximumLevel	= CASE WHEN m_nMaximumLevel < @im_nLevel THEN @im_nLevel ELSE m_nMaximumLevel END,
						m_nExp1	 				= @im_nExp1,
						m_nExp2 					= @im_nExp2,
						m_aJobSkill 				= @im_aJobSkill,
						m_aLicenseSkill 		= @im_aLicenseSkill,
						m_aJobLv 					= @im_aJobLv,
						m_dwExpertLv 			= @im_dwExpertLv,
						m_idMarkingWorld 	= @im_idMarkingWorld,
						m_vMarkingPos_x 	= @im_vMarkingPos_x,
						m_vMarkingPos_y 	= @im_vMarkingPos_y,
						m_vMarkingPos_z 	= @im_vMarkingPos_z,
						m_nRemainGP 			= @im_nRemainGP,
						m_nRemainLP 			= @im_nRemainLP,
						m_nFlightLv 				= @im_nFlightLv,
						m_nFxp 						= @im_nFxp,
						m_nTxp 						= @im_nTxp,
						m_lpQuestCntArray 	= @im_lpQuestCntArray,
						m_aCompleteQuest = @im_aCompleteQuest,
						m_dwMode 				= @im_dwMode,
						m_idparty 					= @im_idparty,
						m_idMuerderer 		= @im_idMuerderer,
						m_nFame 					= @im_nFame,	
						m_nDeathExp			= @im_nDeathExp,
						m_nDeathLevel			= @im_nDeathLevel,
						--m_dwFlyTime				= m_dwFlyTime + @im_dwFlyTime,
						m_dwFlyTime = @im_dwFlyTime,
						m_nMessengerState = @im_nMessengerState,
						TotalPlayTime 			= TotalPlayTime + @iTotalPlayTime,
						m_tmAccFuel 			= @im_tmAccFuel,
						m_dwSkillPoint			= @im_dwSkillPoint,
						m_dwReturnWorldID= @im_dwReturnWorldID,
						m_vReturnPos_x		= @im_vReturnPos_x,
						m_vReturnPos_y		= @im_vReturnPos_y,
						m_vReturnPos_z		= @im_vReturnPos_z,
						m_SkillPoint		=@im_SkillPoint,
						m_SkillLv			=@im_SkillLv,
				        m_SkillExp                      =@im_SkillExp
				      -------------- (AÂ©Â¬Â¡Ã†Â¢Â® Â¨Â¬IÂ¨Â¬Â¨Â¢ : 2006 11 13 Attendant Event)
				        , dwEventFlag                     =@idwEventFlag
				        , dwEventTime                     =@idwEventTime
				        , dwEventElapsed          =@idwEventElapsed
				      -------------- (ADD: Version8-PK System)

						, PKValue        	= @im_nPKValue
						, PKPropensity   	= @im_dwPKPropensity
						, PKExp         	= @im_dwPKExp
				      -------------- (ADD: Version8-Angel System)
						, AngelExp			= @im_nAngelExp
						, AngelLevel		= @im_nAngelLevel
					--------------------- Version9 Pet
						, m_dwPetId = @im_dwPetId

						, m_nExpLog = @im_nExpLog
						, m_nAngelExpLog = @im_nAngelExpLog
						, m_nCoupon = @im_nCoupon
						------------- ver. 13
						, m_nHonor = @im_nHonor
						, m_nLayer = @im_nLayer
						---------- Ver 15
						, m_aCheckedQuest = @im_aCheckedQuest
						, m_nCampusPoint = @im_nCampusPoint
						, idCampus = @im_idCampus
						, m_idVendorNPC = @im_idVendorNPC			/*Offline Vendor*/
						, m_nHideCoat = @im_nHideCoat 		/*Hide CS*/
						, m_szOwnTitle	= @im_szOwnTitle 	/*User Own Title*/
						, m_nLootboxItem = @im_nLootboxItem	/*Lootbox*/
						, m_nLootboxItemNum = @im_nLootboxItemNum	/*Lootbox*/
						, m_szAchFav = @im_szAchFav	/*Achievement Favorites*/
						, m_ncR = @im_ncR	 /*Name Color Change*/
						, m_ncG = @im_ncG	 /*Name Color Change*/
						, m_ncB = @im_ncB	 /*Name Color Change*/
				WHERE   m_idPlayer                              = @im_idPlayer  
				AND 	serverindex 				= @iserverindex

-- 		if object_id('QUEST_TBL') is not null
-- 			EXEC QUEST_STR 'A1',@im_idPlayer,@iserverindex,@im_lpQuestCntArray

update tblLogout_Penya
set m_dwGold = @im_dwGold, regdate = getdate()
where m_idPlayer = @im_idPlayer and serverindex = @iserverindex

		IF @im_nLevel>=120 BEGIN
			UPDATE 	CHARACTER_TBL 
				SET FinalLevelDt=getdate() 
			WHERE 	serverindex=@iserverindex 
				AND m_idPlayer=@im_idPlayer 
				AND FinalLevelDt='2000-01-01'
		END

		--Â¨Ã¹UCoÂ¨Ã¹Â¢Ã§ Â¢Â¯aAÂ¡Ã­Â¡Ã­cCÂ¡Â¿ Â¨Â¬oÂ¢Â¬Â¥Ã¬ Â¡Ã†uÂ¡Â¤A
		IF @im_dwSMTime > '' 
			BEGIN
				IF EXISTS(SELECT * FROM BILING_ITEM_TBL WHERE m_idPlayer= @im_idPlayer  AND serverindex 	= @iserverindex)
				UPDATE BILING_ITEM_TBL
						SET m_dwSMTime = @im_dwSMTime
				 WHERE	m_idPlayer   				= @im_idPlayer 	
					  AND 	serverindex 				= @iserverindex
				ELSE
				INSERT BILING_ITEM_TBL
					(m_idPlayer,serverindex,m_dwSMTime)
				VALUES
					(@im_idPlayer,@iserverindex,@im_dwSMTime)
			END
		ELSE
			 DELETE BILING_ITEM_TBL
			 WHERE	m_idPlayer   				= @im_idPlayer 	
			      AND 	serverindex 				= @iserverindex

			
-- 		UPDATE 	CARD_CUBE_TBL 
-- 				SET m_Card 						= @im_Card,
-- 						m_apIndex_Card 		= @im_Index_Card,
-- 						m_dwObjIndex_Card= @im_ObjIndex_Card,
-- 						m_Cube 						= @im_Cube,
-- 						m_apIndex_Cube 	= @im_Index_Cube,
-- 						m_dwObjIndex_Cube=@im_ObjIndex_Cube 
-- 		 WHERE	m_idPlayer   				= @im_idPlayer 	
-- 			  AND 	serverindex 				= @iserverindex
		
		UPDATE INVENTORY_TBL 
		      SET 	m_Inventory 				= @im_Inventory,
						m_apIndex 				= @im_apIndex,
						m_adwEquipment 	= @im_adwEquipment,
						m_dwObjIndex 			= @im_dwObjIndex
		 WHERE 	m_idPlayer   				= @im_idPlayer 	
			  AND 	serverindex 				= @iserverindex
		

		
		UPDATE TASKBAR_TBL 
			  SET 	m_aSlotApplet 			= @im_aSlotApplet,
						m_aSlotQueue 			= @im_aSlotQueue,
						m_SkillBar					= @im_SkillBar
		 WHERE 	m_idPlayer   				= @im_idPlayer 	
			  AND 	serverindex 				= @iserverindex




		UPDATE TASKBAR_ITEM_TBL 
			  SET 	m_aSlotItem 				= @im_aSlotItem						
		 WHERE 	m_idPlayer   				= @im_idPlayer 	
			  AND 	serverindex 				= @iserverindex

		UPDATE BANK_TBL 
			  SET 	m_Bank 						= @im_Bank,
						m_apIndex_Bank 		= @im_apIndex_Bank, 
						m_dwObjIndex_Bank = @im_dwObjIndex_Bank, 
						m_dwGoldBank 		= @im_dwGoldBank
		 WHERE 	m_idPlayer   				= @im_idPlayer 	
			  AND 	serverindex 				= @iserverindex

		UPDATE SKILLINFLUENCE_TBL
			 SET SkillInfluence = @iSkillInfluence
		 WHERE 	m_idPlayer   				= @im_idPlayer 	
			  AND 	serverindex 				= @iserverindex

		UPDATE INVENTORY_EXT_TBL 
		      SET 	m_extInventory 				= @im_extInventory,
						m_InventoryPiercing= @im_InventoryPiercing
			,szInventoryPet	= @iszInventoryPet
		 WHERE 	m_idPlayer   				= @im_idPlayer 	
			  AND 	serverindex 				= @iserverindex

		UPDATE BANK_EXT_TBL 
			  SET 	m_extBank 						= @im_extBank,
						m_BankPiercing			= @im_BankPiercing
			, szBankPet = @iszBankPet
		 WHERE 	m_idPlayer   				= @im_idPlayer 	
			  AND 	serverindex 				= @iserverindex

		SELECT fError = '1', fText = 'OK'
		RETURN
	END
/*
	
	AÂ¢Â´Â¨Â¬Â¢Â¬Â¨ÃºÂ¡Ã€Â¥Ã¬Â¡ÃAIÂ¨Â¡Â¢Ã§
	 ex ) 
	 CHARACTER_STR 'C1', ALL ...
	 CHARACTER_STR 'C1','000001','01' ...
*/

ELSE
IF @iGu = 'U2' --AN AIÂ¢Â¯eÂ¨Ã¶AÂ¡Ã†Â¡ÃŒ Â¨Ã¹oAÂ¢Â´
	BEGIN
		UPDATE CHARACTER_TBL
		      SET	TotalPlayTime 			= TotalPlayTime + @iplayerslot 
		 WHERE	m_szName  				= @im_szName 	
			  AND 	serverindex 				= @iserverindex
		RETURN
	END
/*

	AN AIÂ¢Â¯eÂ¨Ã¶AÂ¡Ã†Â¡ÃŒ Â¨Ã¹oAÂ¢Â´
	 ex ) 
	 CHARACTER_STR 'U2','',@iserverindex,'',@im_szName,@iplayerslot (@iTotalPlayTime)
	 CHARACTER_STR 'U2','','01','','beat',10234
*/

ELSE
IF @iGu = 'U3' --AN AIÂ¢Â¯eÂ¨Ã¶AÂ¡Ã†Â¡ÃŒ Â¨Ã¹oAÂ¢Â´ new
	BEGIN
		UPDATE CHARACTER_TBL
		      SET	TotalPlayTime 			= TotalPlayTime + @iplayerslot 
		 WHERE 	m_idPlayer   				= @im_idPlayer 	
			  AND 	serverindex 				= @iserverindex
		RETURN
	END
/*

	AN AIÂ¢Â¯eÂ¨Ã¶AÂ¡Ã†Â¡ÃŒ Â¨Ã¹oAÂ¢Â´ (new)
	 ex ) 
	 CHARACTER_STR 'U3',@im_idPlayer,@iserverindex,'','',@iplayerslot (@iTotalPlayTime)
	 CHARACTER_STR 'U3','000001','01','','',10234
*/

ELSE
IF @iGu = 'U4' --AÂ©Ã¸Â¢Â¬?AI Â¢Â¬i Â¨Â¬?Â¡Ã†Â©Â¡
	BEGIN
		IF EXISTS(SELECT m_idPlayer FROM CHARACTER_TBL WHERE m_szName  = @im_szName  AND serverindex	= @iserverindex)
			BEGIN
				SELECT fError = '0'
			END
		ELSE
			BEGIN
				UPDATE CHARACTER_TBL
				      SET	m_szName			= @im_szName
				 WHERE 	m_idPlayer   				= @im_idPlayer 	
					  AND 	serverindex 				= @iserverindex
				SELECT fError = '1'
			END
		RETURN
	END


-- Ver 15
ELSE
IF @iGu = 'U5' --Â¡Ã­cA| Â¨Â¡Â¡Ã€AIÂ¨Â¡Â¢Ã§ Â¨ÃºÂ¡Ã€Â¥Ã¬Â¡ÃAIÂ¨Â¡Â¢Ã§ AÂ©Â¬Â¡Ã†Â¢Â®
	BEGIN
		IF EXISTS(SELECT m_idPlayer FROM CHARACTER_TBL WHERE m_idPlayer = @im_idPlayer AND serverindex	= @iserverindex)
			BEGIN
				UPDATE CHARACTER_TBL
				      SET	m_nCampusPoint			= m_nCampusPoint + @iplayerslot
				 WHERE 	m_idPlayer   				= @im_idPlayer 	
					  AND 	serverindex 				= @iserverindex

				declare @u5m_nCampusPoint int

				select @u5m_nCampusPoint = m_nCampusPoint from CHARACTER_TBL (nolock) WHERE  m_idPlayer = @im_idPlayer AND serverindex	= @iserverindex
				SELECT fError = '1', @u5m_nCampusPoint  m_nCampusPoint
			END
		ELSE
		RETURN
	END
ELSE
IF @iGu = 'U6' -- Â¡Ã­cA| ID Â¨ÃºÂ¡Ã€Â¥Ã¬Â¡ÃAIÂ¨Â¡Â¢Ã§ AÂ©Â¬Â¡Ã†Â¢Â®
	BEGIN
		IF EXISTS(SELECT m_idPlayer FROM CHARACTER_TBL WHERE  m_idPlayer = @im_idPlayer AND serverindex	= @iserverindex)
			BEGIN
				UPDATE CHARACTER_TBL
				      SET	idCampus			= @iplayerslot
				 WHERE 	m_idPlayer   				= @im_idPlayer 	
					  AND 	serverindex 				= @iserverindex
				SELECT fError = '1'
			END
		ELSE
			BEGIN
				SELECT fError = '0'
			END
		RETURN
	END

/*


	AÂ©Ã¸Â¢Â¬?AI Â¢Â¬i Â¨Â¬?Â¡Ã†Â©Â¡
	 ex ) 
	 CHARACTER_STR 'U4',@im_idPlayer,@iserverindex,@iaccount,@im_szName
	 CHARACTER_STR 'U4','000001','01','','Â¡Ã­uÂ¨Â¬Â¡Ã†Â¡Ã†Â©ÂªAO'
*/
ELSE
IF @iGu = 'D1' -- AÂ©Ã¸Â¢Â¬?AI Â¡Ã­eA|
	BEGIN
		IF @im_szName = ''
		BEGIN
			SELECT fError = '1', fText = '2AÂ¡Ã€ Â¨Â¬nÂ©Ã¶Â¨Â¢Â©Ã¶Â©ÂªEÂ¡ÃŒ Â¨Â¡Â©Ã·Â¢Â¬Â©Ã·'
			RETURN
		END
			if not exists (select * from CHARACTER_TBL where m_idPlayer = @im_idPlayer and account = @iaccount and serverindex = @iserverindex)
			begin
				select fError = '1'
				return
			end

			DECLARE @Exists int
/*	
			IF EXISTS(SELECT name  from syscolumns where name='m_idPlayer' AND collation= 'Japanese_BIN')
				BEGIN
					IF EXISTS(SELECT * FROM ACCOUNT_DBF.dbo.ACCOUNT_TBL  WHERE account = @iaccount AND (password =  @im_szName OR member = 'B' ))
		              SET @Exists = 1
					ELSE
						SET @Exists = 0				
				END
			ELSE
				BEGIN
--					IF EXISTS(SELECT *  FROM ACCOUNT_DBF.dbo.ACCOUNT_TBL  WHERE account = @iaccount AND (id_no2 =  @im_szName OR member = 'B' ))
					if exists (select  *  from BANK_TBL (nolock) where m_idPlayer = @im_idPlayer AND m_BankPw =  @im_szName )
		              SET @Exists = 1
					ELSE
						SET @Exists = 0		
				END
*/
set @Exists = 1
			IF @Exists > 0
				BEGIN
					DECLARE @currDate char(12)				
					SET @currDate = CONVERT(CHAR(8),GETDATE(),112) 
											   + RIGHT('00' + CONVERT(VARCHAR(2),DATEPART(hh,GETDATE())),2) 
											   + RIGHT('00' + CONVERT(VARCHAR(2),DATEPART(mi,GETDATE())),2)



					IF EXISTS(SELECT m_idPlayer FROM GUILD_MEMBER_TBL WHERE 	m_idPlayer = @im_idPlayer AND serverindex = @iserverindex	AND m_idWar > 0)
						BEGIN
						SELECT fError = '3', fText = 'Â¡Â¾Â©Â¡Â¥Ã¬aAuAÂ©Â¬'
						RETURN
						END
					ELSE
						BEGIN
						UPDATE CHARACTER_TBL
							  SET isblock 						= 'D',
									  End_Time						= @currDate ,	
									  BlockTime					= LEFT(@currDate,8)
						WHERE 	m_idPlayer   				= @im_idPlayer 	
							  AND 	serverindex 				= @iserverindex	
	
						UPDATE MESSENGER_TBL
							   SET State = 'D'
						 WHERE 	m_idPlayer   				= @im_idPlayer 	
							  AND 	serverindex 				= @iserverindex	
	
						UPDATE MESSENGER_TBL
							   SET State = 'D'
						 WHERE 	f_idPlayer   				= @im_idPlayer 	
							  AND 	serverindex 				= @iserverindex	



						IF EXISTS(SELECT m_idPlayer FROM GUILD_MEMBER_TBL   WHERE 	m_idPlayer = @im_idPlayer AND serverindex = @iserverindex) 
							BEGIN
								SELECT fError = '4', fText = m_idGuild FROM GUILD_MEMBER_TBL   WHERE 	m_idPlayer = @im_idPlayer AND serverindex = @iserverindex
								RETURN

							END
						ELSE
							BEGIN
								SELECT fError = '0', fText = 'DELETE OK'
								RETURN
							END
							
						END
				END
			ELSE
				BEGIN
					SELECT fError = '1', fText = 'AOÂ©Ã¶IÂ©Ã¶Â©ÂªEÂ¡ÃŒÂ¨Â¡Â©Ã·Â¢Â¬Â©Ã·'
					RETURN
				END
	END
/*

	AÂ©Ã¸Â¢Â¬?AI Â¡Ã­eA|
	 ex ) 
	 CHARACTER_STR 'D1',@im_idPlayer,@iserverindex,@iaccount (isblock)
	 CHARACTER_STR 'D1','001068','01','ata3k','1019311'

*/

ELSE
IF @iGu = 'I1' -- AEÂ¡Â¾a AÂ¢Â´Â¨Â¬Â¢Â¬ AOÂ¡Â¤A
	BEGIN
	
	IF (SELECT COUNT(*) AS [Count] FROM [CHARACTER_TBL] WHERE [account] = @iaccount AND [isblock] = 'F' AND [playerslot] = @iplayerslot AND [serverindex] = @iserverindex) > 0
		BEGIN
			SELECT fError = '0', fText = 'Can not overwrite player!'
			RETURN
		END

	-- 20100218 Â¡Ã­yÂ¨Ã¹Â¨Â¬ AÂ©Ã¸Â¢Â¬?AI 3Â¡Ã†Â©Ã¸ AEÂ¡Ã†u Â¨Ã¶A
	declare @i1_cnt int
	select @i1_cnt = count(*) from CHARACTER_TBL where account = @iaccount and isblock = 'F'
	if @i1_cnt > 2
	begin
		SELECT  fError = '0', fText = 'AÂ©Ã¸Â¢Â¬?AI Â¢Â¬Â©Ã¶AÂ¨Ã¶!' 
		RETURN
	end

    IF EXISTS(SELECT m_szName FROM CHARACTER_TBL
          WHERE (( lower(m_szName) = lower(@im_szName) ) OR (playerslot = @iPlayerslot AND account = @iaccount  ) ) AND isblock = 'F' AND serverindex = @iserverindex )
        BEGIN
            SELECT  fError = '0', fText = 'Can not overwrite player!' -- fix char overwrite
            RETURN
        END
    ELSE  
		BEGIN
			DECLARE
								@om_idPlayer 					CHAR		(7)		,
								@om_vScale_x					REAL					,
								@om_dwMotion					INT						,
								@om_fAngle						REAL					,
								@om_nHitPoint					INT						,
								@om_nManaPoint				INT						,
								@om_nFatiguePoint			INT						,
								@om_dwRideItemIdx		INT						,
								@om_dwGold					INT						,
								@om_nJob							INT						,
								@om_pActMover				VARCHAR(50)	,
								@om_nStr							INT						,
								@om_nSta							INT						,
								@om_nDex							INT						,
								@om_nInt							INT						,
								@om_nLevel						INT						,
								@om_nExp1						BIGINT						,
								@om_nExp2						BIGINT						,	
								@om_aJobSkill					VARCHAR	(500),
								@om_aLicenseSkill			VARCHAR	(500),
								@om_aJobLv						VARCHAR	(500),
								@om_dwExpertLv				INT						,
								@om_idMarkingWorld		INT						,
								@om_vMarkingPos_x		REAL					,
								@om_vMarkingPos_y		REAL					,
								@om_vMarkingPos_z		REAL					,
								@om_nRemainGP				INT						,
								@om_nRemainLP				INT						,
								@om_nFlightLv					INT						,
								@om_nFxp							INT						,
								@om_nTxp							INT						,
								@om_lpQuestCntArray		VARCHAR(1024),
								@om_chAuthority				CHAR(1)				,
								@om_dwMode					INT						,
								@oblockby							VARCHAR(32)	,
								@oTotalPlayTime				INT						,
								@oisblock							CHAR(1)				,
								@oEnd_Time						CHAR(12)	,
								@om_Inventory					VARCHAR(max),
								@om_apIndex					VARCHAR(1000)	,
								@om_adwEquipment		VARCHAR(135)	,
								@om_aSlotApplet				VARCHAR(3100),
								@om_aSlotItem					VARCHAR(6885),
								@om_aSlotQueue				VARCHAR(225),
								@om_SkillBar						SMALLINT,
								@om_dwObjIndex				VARCHAR(1000)	,
								@om_Card							VARCHAR(1980),
								@om_Cube						VARCHAR(1980),
								@om_apIndex_Card			VARCHAR(215)	,
								@om_dwObjIndex_Card	VARCHAR(215)	,
								@om_apIndex_Cube		VARCHAR(215)	,
								@om_dwObjIndex_Cube	VARCHAR(215)	,
								@om_idparty						INT						,
								@om_idMuerderer			INT						,
								@om_nFame						INT						,
								@om_nDeathExp				BIGINT						,
								@om_nDeathLevel			INT						,
								@om_dwFlyTime				INT						,
								@om_nMessengerState	INT						,
								@om_Bank							VARCHAR(4290),
								@om_apIndex_Bank		 	VARCHAR(215)	,
								@om_dwObjIndex_Bank VARCHAR(215)	,
								@om_dwGoldBank			INT						
								---------- Ver 15
								, @om_aCheckedQuest varchar(100) 
								, @om_nCampusPoint int 
								, @om_idCampus int
								, @om_idVendorNPC int			/*Offline Vendor*/
								, @om_nHideCoat int /*Hide CS*/
								, @om_szOwnTitle varchar (20) /*User Own Title*/
								, @om_nLootboxItem int	/*Lootbox*/
								, @om_nLootboxItemNum int	/*Lootbox*/
								, @om_szAchFav varchar(max)	/*Achievement Favorites*/
				 IF EXISTS (SELECT * FROM CHARACTER_TBL WHERE  serverindex = @iserverindex)
				 SELECT @om_idPlayer = RIGHT('0000000' + CONVERT(VARCHAR(7),MAX(m_idPlayer)+1),7)
			       FROM CHARACTER_TBL
				  WHERE  serverindex = @iserverindex
				ELSE		
				SELECT @om_idPlayer = '0000001'	
			
				 SELECT @om_vScale_x 				= m_vScale_x,
								@om_dwMotion 				= m_dwMotion,
								@om_fAngle 						= m_fAngle,
								@om_nHitPoint 					= m_nHitPoint,
								@om_nManaPoint 			= m_nManaPoint,
								@om_nFatiguePoint 			= m_nFatiguePoint,
								@om_dwRideItemIdx 		= m_dwRideItemIdx,
								@om_dwGold 					= m_dwGold,
								@om_nJob 						= m_nJob,
								@om_pActMover 				= m_pActMover,
								@om_nStr 							= m_nStr,

								@om_nSta 							= m_nSta,
								@om_nDex 						= m_nDex,
								@om_nInt 							= m_nInt,
								@om_nLevel 						= m_nLevel,
								@om_nExp1 						= m_nExp1,
								@om_nExp2 						= m_nExp2,

								@om_aJobSkill 					= m_aJobSkill,
								@om_aLicenseSkill 			= m_aLicenseSkill,
								@om_aJobLv 					= m_aJobLv,
								@om_dwExpertLv 				= m_dwExpertLv,
								@om_idMarkingWorld 		= m_idMarkingWorld,
								@om_vMarkingPos_x 		= m_vMarkingPos_x,
								@om_vMarkingPos_y 		= m_vMarkingPos_y,
								@om_vMarkingPos_z 		= m_vMarkingPos_z,
								@om_nRemainGP 			= m_nRemainGP,
								@om_nRemainLP 			= m_nRemainLP,
								@om_nFlightLv 					= m_nFlightLv,
								@om_nFxp 						= m_nFxp,
								@om_nTxp 						= m_nTxp,
								@om_lpQuestCntArray		= m_lpQuestCntArray,
								@om_chAuthority 				= m_chAuthority,
								@om_dwMode 					= m_dwMode,
								@oblockby 						= blockby,
								@oTotalPlayTime 				= TotalPlayTime,
								@oisblock 							= isblock,
								@oEnd_Time 					= CONVERT(CHAR(8),DATEADD(yy,3,GETDATE()),112) + '0000',
								@om_Inventory 					= m_Inventory,
								@om_apIndex 					= m_apIndex,
								@om_adwEquipment 		= m_adwEquipment,
								@om_aSlotApplet 				= m_aSlotApplet,
								@om_aSlotItem 					= m_aSlotItem,
								@om_aSlotQueue 			= m_aSlotQueue,
								@om_SkillBar						= m_SkillBar,
								@om_dwObjIndex 			= m_dwObjIndex,
								@om_Card 						= m_Card,
								@om_Cube 						= m_Cube,
								@om_apIndex_Card 		= m_apIndex_Card,
								@om_dwObjIndex_Card	= m_dwObjIndex_Card,
								@om_apIndex_Cube 		= m_apIndex_Cube,
								@om_dwObjIndex_Cube = m_dwObjIndex_Cube,
								@om_idparty 						= m_idparty,			
								@om_idMuerderer 			= m_idMuerderer,
								@om_nFame 						= m_nFame,
								@om_nDeathExp				= m_nDeathExp,
								@om_nDeathLevel			= m_nDeathLevel,
								@om_dwFlyTime				= m_dwFlyTime,
								@om_nMessengerState 	= m_nMessengerState,
								@om_Bank							= m_Bank,
								@om_apIndex_Bank		 	= m_apIndex_Bank,
								@om_dwObjIndex_Bank 	= m_dwObjIndex_Bank,
								@om_dwGoldBank			= m_dwGoldBank			
	
			       FROM BASE_VALUE_TBL
				 WHERE g_nSex 								= @im_dwSex
			


				INSERT CHARACTER_TBL
							(
								m_idPlayer,
								serverindex,
								account,
								m_szName,
								playerslot,
								dwWorldID,
								m_dwIndex,
								m_vScale_x,
								m_dwMotion,
								m_vPos_x,
								m_vPos_y,
								m_vPos_z,
								m_fAngle,
								m_szCharacterKey,
								m_nHitPoint,
								m_nManaPoint,
								m_nFatiguePoint,
								m_nFuel,
								m_dwSkinSet,
								m_dwHairMesh,
								m_dwHairColor,
								m_dwHeadMesh,
								m_dwSex,
								m_dwRideItemIdx,
								m_dwGold,
								m_nJob,
								m_pActMover,
								m_nStr,
								m_nSta,
								m_nDex,
								m_nInt,
								m_nLevel,
								m_nMaximumLevel,
								m_nExp1,
								m_nExp2,
								m_aJobSkill,
								m_aLicenseSkill,
								m_aJobLv,
								m_dwExpertLv,
								m_idMarkingWorld,
								m_vMarkingPos_x,
								m_vMarkingPos_y,
								m_vMarkingPos_z,
								m_nRemainGP,
								m_nRemainLP,
								m_nFlightLv,
								m_nFxp,
								m_nTxp,
								m_lpQuestCntArray,
								m_aCompleteQuest,
								m_chAuthority,
								m_dwMode,
								m_idparty,
								m_idCompany,
								m_idMuerderer,
								m_nFame,
								m_nDeathExp,
								m_nDeathLevel,
								m_dwFlyTime,
								m_nMessengerState,
								blockby,
								TotalPlayTime,
								isblock,
								End_Time,
								BlockTime,
								CreateTime,
								m_tmAccFuel,
								m_tGuildMember,
								m_dwSkillPoint,
								m_dwReturnWorldID,
								m_vReturnPos_x,
								m_vReturnPos_y,
								m_vReturnPos_z,
								m_SkillPoint,
								m_SkillLv,
								m_SkillExp
								---------- Ver 15
								, m_aCheckedQuest
								, m_nCampusPoint
								, idCampus
								, m_idVendorNPC		/*Offline Vendor*/
								, m_nHideCoat 	/*Hide CS*/
								, m_szOwnTitle 	/*User Own Title*/
								, m_nLootboxItem	/*Lootbox*/
								, m_nLootboxItemNum	/*Lootbox*/
								, m_szAchFav		/*Achievement Favorites*/
							)
				VALUES
							(
								@om_idPlayer,
								@iserverindex,
								@iaccount,
								@im_szName,
								@iplayerslot,
								@idwWorldID,
								@im_dwIndex,
								@om_vScale_x,
								@om_dwMotion,
								@im_vPos_x,

								@im_vPos_y,
								@im_vPos_z,
								@om_fAngle,
								@im_szCharacterKey,
								@om_nHitPoint,
								@om_nManaPoint,
								@om_nFatiguePoint,
								-1, --m_nFuel
								@im_dwSkinSet,
								@im_dwHairMesh,
								@im_dwHairColor,
								@im_dwHeadMesh,
								@im_dwSex,
								@om_dwRideItemIdx,
								@om_dwGold,
								@om_nJob,
								@om_pActMover,
								@om_nStr,
								@om_nSta,
								@om_nDex,
								@om_nInt,
								@om_nLevel,
								1, --m_nMaximumLevel
								@om_nExp1,
								@om_nExp2,
								@om_aJobSkill,
								@om_aLicenseSkill,
								@om_aJobLv,
								@om_dwExpertLv,
								@om_idMarkingWorld,
								@om_vMarkingPos_x,
								@om_vMarkingPos_y,
								@om_vMarkingPos_z,
								@om_nRemainGP,
								@om_nRemainLP,
								@om_nFlightLv,
								@om_nFxp,
								@om_nTxp,
								@om_lpQuestCntArray,
								'$', -- m_aCompleteQuest
								@om_chAuthority,
								@om_dwMode,
								@om_idparty,
								'000000', -- m_idCompany
								@om_idMuerderer,
								@om_nFame,
								@om_nDeathExp,
								@om_nDeathLevel,
								@om_dwFlyTime	,
								@om_nMessengerState,
								@oblockby,
								@oTotalPlayTime,
								@oisblock,
								@oEnd_Time,
								CONVERT(CHAR(8),DATEADD(d,-1,GETDATE()),112),
								GETDATE(),
								0,
								CONVERT(CHAR(8),DATEADD(d,-1,GETDATE()),112) 
									+ RIGHT('00' + CONVERT(VARCHAR(2),DATEPART(hh,DATEADD(d,-1,GETDATE()))),2) 
									+ RIGHT('00' + CONVERT(VARCHAR(2),DATEPART(mi,DATEADD(d,-1,GETDATE()))),2) 
									+ RIGHT('00' + CONVERT(VARCHAR(2),DATEPART(ss,DATEADD(d,-1,GETDATE()))),2),
								0, --m_dwSkillPoint
						                            1,
						                            0, 
						                            0,
						                            0,
							@im_SkillPoint,
							@im_SkillLv,
							@im_SkillExp
							-- Ver 15
							, '$'
							, 0
							, 0
							, 0		/*Offline Vendor*/
							, 0 	/*Hide CS*/
							, '$' 	/*User Own Title*/
							, 0		/*Lootbox*/
							, 0		/*Lootbox*/
							, '$'	/*Achievement Favorites*/
							)


				INSERT INVENTORY_TBL
							(
								m_idPlayer,
								serverindex,
								m_Inventory,
								m_apIndex,
								m_adwEquipment,
								m_dwObjIndex
							)
				VALUES 
							(
								@om_idPlayer,
								@iserverindex,
								@om_Inventory,
								@om_apIndex,
								@om_adwEquipment,
								@om_dwObjIndex
							)

				IF @@SERVERNAME = 'WEB' OR  @@SERVERNAME = 'SERVER4'
				SET @om_aSlotApplet = '0,2,400,0,0,0,0/1,2,398,0,1,0,0/2,2,2010,0,2,0,0/3,2,1005,0,3,0,0/4,3,25,0,4,0,0/$'

				INSERT TASKBAR_TBL
							(
								m_idPlayer,
								serverindex,
								m_aSlotApplet,
								m_aSlotQueue,
								m_SkillBar

							)
				VALUES 
							(
								@om_idPlayer,
								@iserverindex,
								@om_aSlotApplet,
								@om_aSlotQueue,
								@om_SkillBar
							)


				INSERT TASKBAR_ITEM_TBL
							(
								m_idPlayer,
								serverindex,
								m_aSlotItem
							)
				VALUES 
							(
								@om_idPlayer,
								@iserverindex,
								@om_aSlotItem
							)
			INSERT BANK_TBL
							(
								m_idPlayer,
								serverindex,
								m_Bank,
								m_BankPw,
								m_apIndex_Bank, 
								m_dwObjIndex_Bank ,
								m_dwGoldBank
							)
				VALUES 
							(
								@om_idPlayer,
								@iserverindex,
								@om_Bank,
								'0000', -- m_BankPw
--								@im_BankPW,
								@om_apIndex_Bank, 
								@om_dwObjIndex_Bank,
								@om_dwGoldBank 
							)

		INSERT SKILLINFLUENCE_TBL
							( 								
								m_idPlayer,
								serverindex,
								SkillInfluence
							)
				VALUES 
							(
								@om_idPlayer,
								@iserverindex,
								'$'
							)

		INSERT INVENTORY_EXT_TBL
							( 								
								m_idPlayer,
								serverindex,
								m_extInventory,
								m_InventoryPiercing
							)
				VALUES 
							(
								@om_idPlayer,
								@iserverindex,
								'$','$'
							)

		INSERT BANK_EXT_TBL
							( 								
								m_idPlayer,
								serverindex,
								m_extBank,
								m_BankPiercing
							)
				VALUES 
							(
								@om_idPlayer,
								@iserverindex,
								'$','$'
							)
		INSERT MONSTERHUNT_TBL /*Monsterhunt System*/
							(
								m_idPlayer,
								serverindex,
								m_nMobID,
								m_nCount,
								m_nRewardSent
							)
				VALUES
							(
								@om_idPlayer,
								@iserverindex,
								'0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/$',
								'0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/$',
								'0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/$'
							)
							
							/*Monsterhunt System till here*/
							
		INSERT BADGES_TBL /*Badge System*/
							(
								m_idPlayer,
								serverindex,
								m_nBadge,
								m_Badges
							)
								VALUES
							(
								@om_idPlayer,
								@iserverindex,
								'-1',
								'-1/0/$'
							)

							/*Badge System till here*/

		

		-- Skill Information
		INSERT INTO tblSkillPoint(serverindex, PlayerID, SkillID, SkillLv, SkillPosition)
        	VALUES (@iserverindex, @om_idPlayer, 1, 0, 0)
		INSERT INTO tblSkillPoint(serverindex, PlayerID, SkillID, SkillLv, SkillPosition)
        	VALUES (@iserverindex, @om_idPlayer, 2, 0, 1)
		INSERT INTO tblSkillPoint(serverindex, PlayerID, SkillID, SkillLv, SkillPosition)
        	VALUES (@iserverindex, @om_idPlayer, 3, 0, 2)

		-- Pocket
	INSERT  tblPocket ( serverindex, idPlayer, nPocket, szItem, szIndex, szObjIndex, bExpired, tExpirationDate )
	VALUES ( @iserverindex, @om_idPlayer, 0, '$', '$', '$', 0, 0 )
	
	INSERT  tblPocketExt ( serverindex, idPlayer, nPocket, szExt, szPiercing, szPet )
	VALUES ( @iserverindex, @om_idPlayer, 0, '$', '$', '$' )
	
	INSERT  tblPocket ( serverindex, idPlayer, nPocket, szItem, szIndex, szObjIndex, bExpired, tExpirationDate )
	VALUES ( @iserverindex, @om_idPlayer, 1, '$', '$', '$', 1, 0 )
	
	INSERT  tblPocketExt ( serverindex, idPlayer, nPocket, szExt, szPiercing, szPet )
	VALUES ( @iserverindex, @om_idPlayer, 1, '$', '$', '$' )
	
	INSERT  tblPocket ( serverindex, idPlayer, nPocket, szItem, szIndex, szObjIndex, bExpired, tExpirationDate )
	VALUES ( @iserverindex, @om_idPlayer, 2, '$', '$', '$', 1, 0 )
	
	INSERT  tblPocketExt ( serverindex, idPlayer, nPocket, szExt, szPiercing, szPet )
	VALUES ( @iserverindex, @om_idPlayer, 2, '$', '$', '$' )

	------------------- ver. 13
	insert into tblMaster_all (serverindex, m_idPlayer, sec)
	select @iserverindex, @om_idPlayer, 1
	insert into tblMaster_all (serverindex, m_idPlayer, sec)
	select @iserverindex, @om_idPlayer, 2
	insert into tblMaster_all (serverindex, m_idPlayer, sec)
	select @iserverindex, @om_idPlayer, 3

	------------------- ver. 15
	insert into tblRestPoint (serverindex, m_idPlayer)
	select @iserverindex, @om_idPlayer

	------------ Penay check default setting
	insert into tblLogout_Penya (serverindex, m_idPlayer)
	select @iserverindex, @om_idPlayer


		SELECT fError = '1', fText = 'OK',m_idPlayer=@om_idPlayer
		RETURN
		END
	END
set nocount off
RETURN