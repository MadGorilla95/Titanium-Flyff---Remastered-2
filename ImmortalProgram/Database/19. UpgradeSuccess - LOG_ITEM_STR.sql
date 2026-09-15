USE [LOGGING_01_DBF]
GO
/****** Object:  StoredProcedure [dbo].[LOG_ITEM_STR]    Script Date: 18/12/2020 20:22:52 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO
ALTER proc [dbo].[LOG_ITEM_STR]
	@im_szName 				VARCHAR(32),
	@iserverindex				CHAR(2),
	@im_GetszName			VARCHAR(32) 		= '',
	@idwWorldID					INT 							= 0,
	@im_dwGold					INT 							= 0,
	@im_nRemainGold		INT 							= 0,
	@iItem_UniqueNo			INT 							= 0,
	@iItem_Name					VARCHAR(32) 		= '',
	@iItem_durability			INT 							= 0,
	@im_nAbilityOption 		INT 							= 0,	
	@Im_GetdwGold			INT 							= 0,	
	@iItem_count					INT 							= 0,
	@iState							CHAR(1)					= '',
	@im_nSlot0  					INT = 0,--: ?? ??? ???? ?????
	@im_nSlot1  					INT = 0,-- : ?? ??? ??? ?????
	@im_bItemResist   		INT = 0,-- : ?? (?> ?, ?, ??, ? )
	@im_nResistAbilityOption   INT = 0,--: m_bItemResist ???
	@im_bCharged   INT = 0,
	@im_dwKeepTime  INT = 0,
	@im_nRandomOptItemId  BIGINT = 0,
	@inPiercedSize  INT = 0,
	@iadwItemId0  INT = 0,
	@iadwItemId1  INT = 0,
	@iadwItemId2  INT = 0,
	@iadwItemId3  INT = 0,
	@iadwItemId4  INT = 0,
	@iMaxDurability INT = 0
,@inPetKind int = 0, @inPetLevel int = 0, @idwPetExp int = 0, @iwPetEnergy int = 0, @iwPetLife int = 0
,@inPetAL_D int = 0, @inPetAL_C int = 0, @inPetAL_B int = 0, @inPetAL_A int = 0, @inPetAL_S int = 0
------------ ver.12
, @iadwItemId5 int = 0, @iadwItemId6 int = 0,@iadwItemId7 int = 0, @iadwItemId8 int = 0, @iadwItemId9 int = 0
, @inUMPiercedSize int = 0, @iadwUMItemId0 int = 0, @iadwUMItemId1 int = 0, @iadwUMItemId2 int = 0, @iadwUMItemId3 int = 0, @iadwUMItemId4 int = 0
	-- upgrade success
	,@inUpgradeTries int = 0
AS
set nocount on
/***********************************************************************************
***********************************************************************************
***********************************************************************************
***********************************************************************************



	LOG ?? ????
	??? : ???
	??? : 2003.10.17 



--20040908 ????
-- ALTER TABLE  LOG_ITEM_EVENT_TBL  ADD m_nSlot0  INT NULL, m_nSlot1 INT NULL, m_bItemResist  INT NULL, m_nResistAbilityOption  INT NULL
-- ALTER TABLE  LOG_BILLING_ITEM_TBL  ADD m_nSlot0  INT NULL, m_nSlot1 INT NULL, m_bItemResist  INT NULL, m_nResistAbilityOption  INT NULL
-- ALTER TABLE  LOG_ITEM_TBL  ADD m_nSlot0  INT NULL, m_nSlot1 INT NULL, m_bItemResist  INT NULL, m_nResistAbilityOption  INT NULL


***********************************************************************************
***********************************************************************************
***********************************************************************************
***********************************************************************************/

DECLARE @os_date CHAR(14)
  SELECT @os_date = CONVERT(CHAR(8),GETDATE(),112) 
									+ RIGHT('00' + CONVERT(VARCHAR(2),DATEPART(hh,GETDATE())),2) 
									+ RIGHT('00' + CONVERT(VARCHAR(2),DATEPART(mi,GETDATE())),2) 
									+ RIGHT('00' + CONVERT(VARCHAR(2),DATEPART(ss,GETDATE())),2)

	BEGIN
			/*************************************************************
			
				?? ???
				@iGu = 'L3'
			
			*************************************************************/
		IF @iState = 'E'
			BEGIN
					INSERT  LOG_ITEM_EVENT_TBL 
						(
							m_szName,
							serverindex,
							m_GetszName,
							dwWorldID,
							m_dwGold,
							m_nRemainGold,
							Item_UniqueNo,
							Item_Name,
							Item_durability,
							m_nAbilityOption,
							m_GetdwGold,
							Item_count,
							State,
							s_date,
							m_nSlot0,
							m_nSlot1,
							m_bItemResist ,
							m_nResistAbilityOption,
							m_bCharged,
							m_dwKeepTime,
							m_nRandomOptItemId,
							nPiercedSize,
							adwItemId0,
							adwItemId1,
							adwItemId2,
							adwItemId3,
							MaxDurability,
							adwItemId4,
							nPetKind, nPetLevel, dwPetExp, wPetEnergy, wPetLife, nPetAL_D, nPetAL_C, nPetAL_B, nPetAL_A, nPetAL_S
							, adwItemId5, adwItemId6, adwItemId7, adwItemId8, adwItemId9
							, nUMPiercedSize, adwUMItemId0, adwUMItemId1, adwUMItemId2, adwUMItemId3, adwUMItemId4
						)
					VALUES 
						(
							@im_szName,
							@iserverindex,
							@im_GetszName,
							@idwWorldID,
							@im_dwGold,
							@im_nRemainGold,
							@iItem_UniqueNo,
							@iItem_Name,
							@iItem_durability,
							@im_nAbilityOption ,
							@Im_GetdwGold,
							@iItem_count,
							@iState,
							@os_date,
							@im_nSlot0,
							@im_nSlot1,
							@im_bItemResist ,
							@im_nResistAbilityOption,
							@im_bCharged,
							@im_dwKeepTime,
							@im_nRandomOptItemId,
							@inPiercedSize,
							@iadwItemId0,
							@iadwItemId1,
							@iadwItemId2,
							@iadwItemId3,
							@iMaxDurability,
							@iadwItemId4
							,@inPetKind, @inPetLevel, @idwPetExp, @iwPetEnergy, @iwPetLife
							,@inPetAL_D, @inPetAL_C, @inPetAL_B, @inPetAL_A, @inPetAL_S
							, @iadwItemId5, @iadwItemId6, @iadwItemId7, @iadwItemId8, @iadwItemId9
							, @inUMPiercedSize, @iadwUMItemId0, @iadwUMItemId1, @iadwUMItemId2, @iadwUMItemId3, @iadwUMItemId4
						)
			END
		ELSE
		IF @iState in ('1','2','3','4','5','6','7','8','9','0')
			BEGIN
					INSERT  LOG_BILLING_ITEM_TBL 
						(
							m_szName,
							serverindex,
							m_GetszName,
							dwWorldID,
							m_dwGold,
							m_nRemainGold,
							Item_UniqueNo,
							Item_Name,
							Item_durability,
							m_nAbilityOption,
							m_GetdwGold,
							Item_count,
							State,
							s_date,
							m_nSlot0,
							m_nSlot1,
							m_bItemResist ,
							m_nResistAbilityOption,
							m_bCharged,
							m_dwKeepTime,
							m_nRandomOptItemId,
							nPiercedSize,
							adwItemId0,
							adwItemId1,
							adwItemId2,
							adwItemId3,
							MaxDurability,
							adwItemId4,
							nPetKind, nPetLevel, dwPetExp, wPetEnergy, wPetLife, nPetAL_D, nPetAL_C, nPetAL_B, nPetAL_A, nPetAL_S
							, adwItemId5, adwItemId6, adwItemId7, adwItemId8, adwItemId9
							, nUMPiercedSize, adwUMItemId0, adwUMItemId1, adwUMItemId2, adwUMItemId3, adwUMItemId4
						)
					VALUES 
						(
							@im_szName,
							@iserverindex,
							@im_GetszName,
							@idwWorldID,
							@im_dwGold,
							@im_nRemainGold,
							@iItem_UniqueNo,
							@iItem_Name,
							@iItem_durability,
							@im_nAbilityOption ,
							@Im_GetdwGold,
							@iItem_count,
							@iState,
							@os_date,
							@im_nSlot0,
							@im_nSlot1,
							@im_bItemResist ,
							@im_nResistAbilityOption,
							@im_bCharged,
							@im_dwKeepTime,
							@im_nRandomOptItemId,
							@inPiercedSize,
							@iadwItemId0,
							@iadwItemId1,
							@iadwItemId2,
							@iadwItemId3,
							@iMaxDurability,
							@iadwItemId4
							,@inPetKind, @inPetLevel, @idwPetExp, @iwPetEnergy, @iwPetLife
							,@inPetAL_D, @inPetAL_C, @inPetAL_B, @inPetAL_A, @inPetAL_S
							, @iadwItemId5, @iadwItemId6, @iadwItemId7, @iadwItemId8, @iadwItemId9
							, @inUMPiercedSize, @iadwUMItemId0, @iadwUMItemId1, @iadwUMItemId2, @iadwUMItemId3, @iadwUMItemId4
						)
			END
		ELSE
			BEGIN
					INSERT  LOG_ITEM_TBL 
						(
							m_szName,
							serverindex,
							m_GetszName,
							dwWorldID,
							m_dwGold,
							m_nRemainGold,
							Item_UniqueNo,
							Item_Name,
							Item_durability,
							m_nAbilityOption,
							m_GetdwGold,
							Item_count,
							State,
							s_date,
							m_nSlot0,
							m_nSlot1,
							m_bItemResist ,
							m_nResistAbilityOption,
							m_bCharged,
							m_dwKeepTime,
							m_nRandomOptItemId,
							nPiercedSize,
							adwItemId0,
							adwItemId1,
							adwItemId2,
							adwItemId3,
							MaxDurability,
							adwItemId4,
							nPetKind, nPetLevel, dwPetExp, wPetEnergy, wPetLife, nPetAL_D, nPetAL_C, nPetAL_B, nPetAL_A, nPetAL_S
							, adwItemId5, adwItemId6, adwItemId7, adwItemId8, adwItemId9
							, nUMPiercedSize, adwUMItemId0, adwUMItemId1, adwUMItemId2, adwUMItemId3, adwUMItemId4
							-- upgrade success
							, nUpgradeTries
						)
					VALUES 
						(
							@im_szName,
							@iserverindex,
							@im_GetszName,
							@idwWorldID,
							@im_dwGold,
							@im_nRemainGold,
							@iItem_UniqueNo,
							@iItem_Name,
							@iItem_durability,
							@im_nAbilityOption ,
							@Im_GetdwGold,
							@iItem_count,
							@iState,
							@os_date,
							@im_nSlot0,
							@im_nSlot1,
							@im_bItemResist ,
							@im_nResistAbilityOption,
							@im_bCharged,
							@im_dwKeepTime,
							@im_nRandomOptItemId,
							@inPiercedSize,
							@iadwItemId0,
							@iadwItemId1,
							@iadwItemId2,
							@iadwItemId3,
							@iMaxDurability,
							@iadwItemId4
							,@inPetKind, @inPetLevel, @idwPetExp, @iwPetEnergy, @iwPetLife
							,@inPetAL_D, @inPetAL_C, @inPetAL_B, @inPetAL_A, @inPetAL_S
							, @iadwItemId5, @iadwItemId6, @iadwItemId7, @iadwItemId8, @iadwItemId9
							, @inUMPiercedSize, @iadwUMItemId0, @iadwUMItemId1, @iadwUMItemId2, @iadwUMItemId3, @iadwUMItemId4
							-- upgrade success
							, @inUpgradeTries
						)					
			END

			RETURN
	END

set nocount off
