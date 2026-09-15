USE [CHARACTER_01_DBF]
GO
/****** Object:  StoredProcedure [dbo].[MAIL_STR]    Script Date: 23.01.2022 15:37:06 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO
ALTER  proc [dbo].[MAIL_STR]
	@iGu		CHAR(2),
	@nMail		INT,
	@serverindex	CHAR(2),
	@idReceiver	CHAR(7)	= '0000000',
	@idSender	CHAR(7)	= '0000000',
	@nGold		INT	= 0,
	@tmCreate	INT	= 0,
	@byRead	INT	= 0,
	@szTitle		VARCHAR(128)	= '',
	@szText		VARCHAR(1024)	= '',
	@dwItemId	INT	= 0,
	@nItemNum	INT	= 0,
	@nRepairNumber	INT	= 0,
	@nHitPoint	INT	= 0,
	@nMaxHitPoint	INT	= 0,
	@nMaterial	INT	= 0,
	@byFlag		INT	= 0,
	@dwSerialNumber	INT	= 0,
	@nOption	INT	= 0,
	@bItemResist	INT	= 0,
	@nResistAbilityOption	INT	= 0,
	@idGuild		INT	= 0,
	@nResistSMItemId	INT	= 0,
	@bCharged	INT	= 0,
	@dwKeepTime	INT	= 0,
	@nRandomOptItemId	BIGINT	= 0,
	@nPiercedSize	INT	= 0,
	@dwItemId1	INT	= 0,
	@dwItemId2	INT	= 0,
	@dwItemId3	INT	= 0,
	@dwItemId4	INT	= 0
	------------------- Version9 Pet
	,@bPet    int = 0,
	@nKind  int = 0,
	@nLevel int = 0,
	@dwExp              int = 0,
	@wEnergy          int = 0,
	@wLife   int = 0,
	@anAvailLevel_D int = 0, 
	@anAvailLevel_C int = 0,
	@anAvailLevel_B int = 0,
	@anAvailLevel_A int = 0,
	@anAvailLevel_S int = 0,

	@dwItemId5 int = 0
	---------------- ver.12
	,@dwItemId6 int = 0, @dwItemId7 int = 0, @dwItemId8 int = 0, @dwItemId9 int = 0, @dwItemId10 int = 0
	,@dwItemId11 int = 0, @dwItemId12 int = 0, @dwItemId13 int = 0, @dwItemId14 int = 0, @dwItemId15 int = 0
	,@nPiercedSize2 int = 0
	----------- Ver. 13
	, @szPetName varchar(32) = ''
	-- upgrade success
	,@inUpgradeTries int = 0
	-- mail extension
	, @szItem varchar(1024) = ''
	, @szItemExt varchar(1024) = ''
	, @szItemPiercing varchar(1024) = ''
	, @szItemPet varchar(1024) = ''
AS
set nocount on
IF @iGu	= 'S1'
	BEGIN
		SELECT * FROM MAIL_TBL WHERE serverindex = @serverindex AND byRead<90 order by nMail
	RETURN
	END
ELSE
IF @iGu	= 'A1'
	BEGIN
		INSERT MAIL_TBL
			(
				nMail,
				serverindex,
				idReceiver,
				idSender,
				nGold,
				tmCreate,
				byRead,
				szTitle,
				szText,
				dwItemId,
				nItemNum,
				nRepairNumber,
				nHitPoint,
				nMaxHitPoint,
				nMaterial,
				byFlag,
				dwSerialNumber,
				nOption,
				bItemResist,
				nResistAbilityOption,
				idGuild,
				nResistSMItemId,
				bCharged,
				dwKeepTime,
				nRandomOptItemId,
				nPiercedSize,
				dwItemId1,
				dwItemId2,
				dwItemId3,
				dwItemId4,
				SendDt,
				bPet, 
				nKind, 
				nLevel, 
				dwExp, 
				wEnergy, 
				wLife, 
				anAvailLevel_D, 
				anAvailLevel_C, 
				anAvailLevel_B, 
				anAvailLevel_A, 
				anAvailLevel_S,
				dwItemId5,
				dwItemId6,
				dwItemId7,
				dwItemId8,
				dwItemId9,
				dwItemId10,
				dwItemId11,
				dwItemId12,
				dwItemId13,
				dwItemId14,
				dwItemId15,
				nPiercedSize2, 
				szPetName
				-- upgrade success
				, nUpgradeTries
				-- mail extension
				, szItem 
				, szItemExt
				, szItemPiercing
				, szItemPet
				)
			VALUES 
			(
				@nMail,
				@serverindex,
				@idReceiver,
				@idSender,
				@nGold,
				@tmCreate,
				@byRead,
				@szTitle,
				@szText,
				@dwItemId,
				@nItemNum,
				@nRepairNumber,
				@nHitPoint,
				@nMaxHitPoint,
				@nMaterial,
				@byFlag,
				@dwSerialNumber,
				@nOption,
				@bItemResist,
				@nResistAbilityOption,
				@idGuild,
				@nResistSMItemId,
				@bCharged,
				@dwKeepTime,
				@nRandomOptItemId,
				@nPiercedSize,
				@dwItemId1,
				@dwItemId2,
				@dwItemId3,
				@dwItemId4,
				getdate()
				,@bPet, @nKind, @nLevel, @dwExp, @wEnergy, @wLife, @anAvailLevel_D, @anAvailLevel_C, @anAvailLevel_B, @anAvailLevel_A, @anAvailLevel_S
				,@dwItemId5
				, @dwItemId6, @dwItemId7, @dwItemId8, @dwItemId9, @dwItemId10, @dwItemId11
				, @dwItemId12, @dwItemId13, @dwItemId14, @dwItemId15, @nPiercedSize2
				, @szPetName
				-- upgrade success
				, @inUpgradeTries
				-- mail extension
				, @szItem 
				, @szItemExt
				, @szItemPiercing
				, @szItemPet
			)
	RETURN
	END
IF @iGu	= 'D1'
	BEGIN
		UPDATE MAIL_TBL SET byRead=90, DeleteDt=getdate() WHERE nMail = @nMail AND serverindex = @serverindex
	RETURN
	END

if @iGu = 'D2'
begin
	UPDATE MAIL_TBL SET byRead=90, DeleteDt=getdate() WHERE serverindex = @serverindex AND tmCreate < @tmCreate
end

IF @iGu	= 'U1'
	BEGIN
		UPDATE MAIL_TBL SET 
			ItemFlag=90, ItemReceiveDt=getdate()
			WHERE nMail = @nMail AND serverindex = @serverindex
	RETURN
	END

IF @iGu	= 'U2'
	BEGIN
		UPDATE MAIL_TBL SET GoldFlag=90, GetGoldDt=getdate() WHERE nMail = @nMail AND serverindex = @serverindex
	RETURN
	END


IF @iGu	= 'U3'
	BEGIN
		UPDATE MAIL_TBL SET byRead = 1, ReadDt=getdate() WHERE nMail = @nMail AND serverindex = @serverindex
	RETURN
	END

set nocount off
