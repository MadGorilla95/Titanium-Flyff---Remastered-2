USE [CHARACTER_01_DBF]
GO
/****** Object:  StoredProcedure [dbo].[GUILD_STR]    Script Date: 20/11/2020 19:57:23 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO
ALTER  Procedure [dbo].[GUILD_STR]
	@iGu        		  				CHAR(2) 			=	'S1', 
	@im_idPlayer					CHAR(7) 			= '0000001',
	@iserverindex  				CHAR(2) 			= '01',
	@im_idGuild					CHAR(6)				= '000001',
	@im_szGuild					varCHAR(48)			='',
	@iLv_1								INT 						=1,
	@iLv_2								INT 						=1,
	@iLv_3								INT 						=1,
	@iLv_4								INT 						=1,
	@im_nLevel					INT 						=1,
	@im_nGuildGold			INT 						=0,
	@im_nGuildPxp				INT 						=0,
	@im_nWin						INT 						=0,
	@im_nLose					INT 						=0,
	@im_nSurrender			INT 						=0,
	@im_dwLogo					INT 						=0,
	@im_szNotice				VARCHAR(127)	='',
	@im_nClass					INT 						=0,
	@im_dwGC_1					INT = 255,
	@im_dwGC_2					INT = 255,
	@im_dwGC_3					INT = 255,
	@im_dwGF					INT = 1
AS
set nocount on
IF @iGu = 'S1'
	BEGIN
		SELECT  m_idGuild,serverindex,Lv_1,Lv_2,Lv_3,Lv_4,Pay_0,Pay_1,Pay_2,Pay_3,Pay_4,m_szGuild,m_nLevel,
						m_nGuildGold,m_nGuildPxp,m_nWin,m_nLose,m_nSurrender,m_nWinPoint,
						m_dwLogo,m_szNotice, m_dwGuildColor_1, m_dwGuildColor_2, m_dwGuildColor_3, m_bGuildFinder
		   FROM GUILD_TBL 
		 WHERE serverindex = @iserverindex
					AND isuse='T'
		 ORDER BY m_idGuild
		RETURN
	END
/*

	

	SELECT * FROM GUILD_TBL
	SELECT * FROM GUILD_MEMBER_TBL
	SELECT * FROM GUILD_BANK_TBL WHERE serverindex ='07'
	
	DELETE GUILD_TBL WHERE serverindex ='07'
	DELETE GUILD_MEMBER_TBL WHERE serverindex ='07'
	DELETE GUILD_BANK_TBL WHERE serverindex ='07'

	 GUILD ?? ???? - ???
	 ex ) 
	 GUILD_STR 'S1',@im_idPlayer,@iserverindex
	 GUILD_STR 'S1','','01'

*/
ELSE
IF @iGu = 'S2'
	BEGIN
		SELECT 	A.m_idPlayer,A.serverindex,A.m_idGuild,A.m_szAlias,A.m_nWin,A.m_nLose,A.m_nSurrender,
						A.m_nMemberLv,A.m_nClass,A.m_nGiveGold,A.m_nGivePxp,B.m_nJob,B.m_nLevel,B.m_dwSex,
						m_idWar=ISNULL(A.m_idWar,0),m_idVote=ISNULL(A.m_idVote,0)
		   FROM GUILD_MEMBER_TBL A, CHARACTER_TBL B
		 WHERE A.m_idPlayer = B.m_idPlayer
		      AND A.serverindex = B.serverindex
				AND B.serverindex = @iserverindex
--		      AND B.isblock='F'
			   AND A.isuse = 'T'
		 ORDER BY A.m_idPlayer
		RETURN
	END

/*
	

	 GUILD ?? ?? ????- ???
	 ex ) 
	 GUILD_STR 'S2',@im_idPlayer,@iserverindex
	 GUILD_STR 'S2','','01'

*/
ELSE
IF @iGu = 'A1'
	BEGIN
-- 		IF NOT EXISTS (SELECT * FROM GUILD_TBL WHERE m_szGuild = @im_szGuild AND serverindex = @iserverindex)
-- 			BEGIN
				IF (NOT EXISTS (SELECT * FROM GUILD_TBL WHERE m_idGuild = @im_idGuild AND serverindex = @iserverindex) and  NOT EXISTS (SELECT * FROM GUILD_BANK_TBL WHERE m_idGuild = @im_szGuild AND serverindex = @iserverindex))
					BEGIN
						INSERT  GUILD_TBL
							(
								m_idGuild,serverindex,Lv_1,Lv_2,Lv_3,Lv_4,Pay_0,Pay_1,Pay_2,Pay_3,Pay_4,
							 	m_szGuild,m_nLevel,m_nGuildGold,m_nGuildPxp,
								m_nWin,m_nLose,m_nSurrender,m_nWinPoint,m_dwLogo,
								m_szNotice,isuse,CreateTime,m_dwGuildColor_1,m_dwGuildColor_2,m_dwGuildColor_3, m_bGuildFinder
							)
						VALUES
							(
								@im_idGuild,@iserverindex,@iLv_1,@iLv_2,@iLv_3,@iLv_4,0,0,0,0,0,
							 	@im_szGuild,1,0,0,
								0,0,0,0,@im_dwLogo,
								@im_szNotice,'T',GETDATE(),255,255,255, 1
							)
						INSERT GUILD_BANK_TBL
							(
								m_idGuild,serverindex,m_apIndex,
								m_dwObjIndex,m_GuildBank
							)
						VALUES
							(
								@im_idGuild,@iserverindex,'$',
								'$','$'
							)

						INSERT GUILD_BANK_EXT_TBL
							(
								m_idGuild,serverindex,m_extGuildBank,m_GuildBankPiercing
							)
						VALUES
							(
								@im_idGuild,@iserverindex,'$','$'
							)
-- 						IF @@SERVERNAME='CHAR-DB1'
-- 						BEGIN
-- 						INSERT [LOG].LOG_DBF.dbo.GUILD_TBL  --?? ??? ?? ?? ??? ?? ????
-- 							(
-- 								m_idGuild,serverindex,m_szGuild,isuse
-- 							)
-- 						VALUES
-- 							(
-- 								@im_idGuild,@iserverindex,@im_szGuild,'T'
-- 							)
-- 						END
						SELECT  nError = '1', fText = '?? ?? OK'
					END
				ELSE
					BEGIN
						SELECT  nError = '2', fText = '?? ??? ??'
					END
-- 			END
-- 		ELSE
-- 			BEGIN
-- 				SELECT  nError = '3', fText = '?? ?? ??'
-- 			END	
-- 		RETURN
	END
/*
	
	 GUILD ??- ???
	 ex ) 
	 GUILD_STR 'A1',@im_idPlayer,@iserverindex,@im_idGuild,@im_szGuild,@iLv_1,@iLv_2,@iLv_3,@iLv_4,@im_nLevel,@im_nGuildGold,
								  @im_nGuildPxp,@im_nWin,@im_nLose,@im_nSurrender,@im_dwLogo,@im_szNotice

	 GUILD_STR 'A1','000000','01','000004','?????',1,1,1,1,1,0,
								  0,0,0,0,0,'??????? ???? ??? ?????.'

*/
ELSE
IF @iGu = 'A2'
	BEGIN		
		IF NOT EXISTS( SELECT * FROM GUILD_MEMBER_TBL  WHERE  m_idPlayer = @im_idPlayer AND serverindex = @iserverindex AND m_idGuild = @im_idGuild)
		INSERT  GUILD_MEMBER_TBL 
			(
				m_idPlayer,serverindex,m_idGuild,m_szAlias,
				m_nWin,m_nLose,m_nSurrender,m_nMemberLv,m_nClass,
				m_nGiveGold,m_nGivePxp,m_idWar,m_idVote,isuse,CreateTime
			)
		VALUES
			(
				@im_idPlayer,@iserverindex,@im_idGuild,@im_szGuild, 	--(@im_szGuild = m_szAlias)
				0,0,0,@im_nLevel,	0,														--(@iLv_1 = m_nPay , @im_nLevel = m_nMemberLv)
				@im_nGuildGold,@im_nGuildPxp,0,'','T',GETDATE()				--(@im_nGuildGold = m_nGiveGold,@im_nGuildPxp= m_nGivePxp)
			)
		RETURN
	END
/*
	
	 GUILD ??
	 ex ) 
	 GUILD_STR 'A2',@im_idPlayer,@iserverindex,@im_idGuild(@im_szGuild = m_szAlias),@im_szGuild,@iLv_1(@iLv_1 = m_nPay),
								 @iLv_2,@iLv_3,@iLv_4,@im_nLevel(@im_nLevel = m_nMemberLv),@im_nGuildGold(@im_nGuildGold = m_nGiveGold),
								  @im_nGuildPxp(@im_nGuildPxp= m_nGivePxp)

	 GUILD_STR 'A2','000023','01','000001','',1,
									0,0,0,0,1,0,
								  	0
*/
ELSE
IF @iGu = 'U1'
	BEGIN
		UPDATE GUILD_TBL
			  SET	Lv_1 = @iLv_1,
						Lv_2 = @iLv_2,
						Lv_3 = @iLv_3,
						Lv_4 = @iLv_4			 			
		 WHERE m_idGuild = @im_idGuild
			  AND serverindex = @iserverindex
			  AND isuse = 'T'
		RETURN
	END
/*
	
	 GUILD ?? ?? - ???
	 ex ) 
	 GUILD_STR 'U1',@im_idPlayer,@iserverindex,@im_idGuild,@im_szGuild,@iLv_1,@iLv_2,@iLv_3,@iLv_4

	 GUILD_STR 'U1','000000','01','000001','',0,0,0,0,0
*/

ELSE
IF @iGu = 'U2'
	BEGIN
		UPDATE GUILD_MEMBER_TBL
			  SET	m_nMemberLv = @im_nLevel,
						m_nClass = 0		 			-- ??? ????
		 WHERE m_idPlayer = @im_idPlayer
			  AND serverindex = @iserverindex
			  AND isuse = 'T'
		RETURN
	END
/*
	
	 GUILD ?? ?? ??- ???
	 ex ) 
	 GUILD_STR 'U2',@im_idPlayer,@iserverindex,@im_idGuild,@im_szGuild,@iLv_1,@iLv_2,@iLv_3,@iLv_4,@im_nLevel

	 GUILD_STR 'U2','000000','01','000001','',0,0,0,0,0,12
*/
ELSE
IF @iGu = 'U3'
	BEGIN
		UPDATE GUILD_TBL
			SET m_dwLogo = @im_dwLogo
		 WHERE m_idGuild = @im_idGuild
		      AND serverindex = @iserverindex
		RETURN
	END
/*
	
	 GUILD ?? ????- ???
	 ex ) 
	 GUILD_STR 'U3',@im_idPlayer,@iserverindex,@im_idGuild,@im_szGuild,@iLv_1,@iLv_2,@iLv_3,@iLv_4,@im_nLevel,@im_nGuildGold,
								  @im_nGuildPxp,@im_nWin,@im_nLose,@im_nSurrender,@im_dwLogo,@im_szNotice

	 GUILD_STR 'U3','000000','01','000001','',0,0,0,0,0,0,
								  0,0,0,0,0,123,@im_szNotice

*/


ELSE
IF @iGu = 'U4'
	BEGIN
		UPDATE GUILD_TBL
			SET m_nLevel = @im_nLevel,
                  m_nGuildGold = @im_nGuildGold,
                  m_nGuildPxp = @im_nGuildPxp
	   WHERE m_idGuild = @im_idGuild
		      AND serverindex = @iserverindex

		UPDATE GUILD_MEMBER_TBL
			SET m_nGiveGold = m_nGiveGold +  @iLv_1,
                  m_nGivePxp = m_nGivePxp +  @iLv_2
		WHERE  m_idGuild = @im_idGuild
		      AND serverindex = @iserverindex
			  AND m_idPlayer = @im_idPlayer
		RETURN
	END
/*
	
	 GUILD ?? - ???
	 ex ) 
	GUILD_STR 'U4',@im_idPlayer,@iserverindex,@im_idGuild,@im_szGuild,@iLv_1,@iLv_2,@iLv_3,@iLv_4,@im_nLevel,@im_nGuildGold,
								  @im_nGuildPxp

	GUILD_STR 'U4','000000','01','000001','',0,0,0,0,16,1000,
								  14

*/


ELSE
IF @iGu = 'U5'
	BEGIN
		UPDATE GUILD_TBL
			SET m_szNotice = @im_szNotice
		 WHERE m_idGuild = @im_idGuild
		      AND serverindex = @iserverindex
		RETURN
	END

/*
	
	 GUILD ???? - ???
	 ex ) 
	 GUILD_STR 'U5',@im_idPlayer,@iserverindex,@im_idGuild,@im_szGuild,@iLv_1,@iLv_2,@iLv_3,@iLv_4,@im_nLevel,@im_nGuildGold,
								  @im_nGuildPxp,@im_nWin,@im_nLose,@im_nSurrender,@im_dwLogo,@im_szNotice


	 GUILD_STR 'U5','000000','01','000001','',0,0,0,0,0,0,
								  0,0,0,0,0,0,'??????? ???? ??? ?????.'

*/


ELSE
IF @iGu = 'U6'
	BEGIN
		UPDATE GUILD_TBL
			SET Pay_0 = @im_dwLogo,
					Pay_1 = @iLv_1,
					Pay_2 = @iLv_2,
					Pay_3 = @iLv_3,
					Pay_4 = @iLv_4
		 WHERE m_idGuild = @im_idGuild
		      AND serverindex = @iserverindex
		RETURN
	END

/*
	
	 GUILD ???? - ???
	 ex ) 
	 GUILD_STR 'U6',@im_idPlayer,@iserverindex,@im_idGuild,@im_szGuild,@iLv_1,@iLv_2,@iLv_3,@iLv_4,@im_nLevel,@im_nGuildGold,
								  @im_nGuildPxp,@im_nWin,@im_nLose,@im_nSurrender,@im_dwLogo

	 GUILD_STR 'U6','000000','01','000001','',100,80,60,40,0,0,
								  0,0,0,0,0,20

*/


ELSE
IF @iGu = 'U7'
	BEGIN
		INSERT ITEM_SEND_TBL
			(m_idPlayer,serverindex,Item_Name,Item_count,m_nAbilityOption,End_Time)

		SELECT 	A.m_idPlayer,
						A.serverindex,
						'penya',
						CASE A.m_nMemberLv 	WHEN 0 THEN  B.Pay_0 
																	WHEN 1 THEN  B.Pay_1 
																	WHEN 2 THEN  B.Pay_2
																	WHEN 3 THEN  B.Pay_3
																	WHEN 4 THEN  B.Pay_4 
																 	ELSE 0 END ,
						0,
						NULL
			FROM GUILD_MEMBER_TBL A,GUILD_TBL B
        WHERE A.m_idGuild =B.m_idGuild
				AND B.m_idGuild = @im_idGuild
		      AND A.serverindex = B.serverindex
			  AND B.serverindex = @iserverindex
			  AND A.isuse = 'T'
			  AND (
							(A.m_nMemberLv = 0 AND B.Pay_0  > 0)  OR
						  	(A.m_nMemberLv = 1 AND B.Pay_1  > 0)  OR 
						  	(A.m_nMemberLv = 2 AND B.Pay_2  > 0)  OR 
						  	(A.m_nMemberLv = 3 AND B.Pay_3  > 0)  OR 
						  	(A.m_nMemberLv = 4 AND B.Pay_4  > 0) 
						)

		UPDATE GUILD_TBL 
			   SET m_nGuildGold = @iLv_1	
			 WHERE m_idGuild = @im_idGuild  AND serverindex = @iserverindex
		SELECT nError = '1',fText ='OK'
	RETURN
	END

/*
	
	 GUILD ???? - ???
	 ex ) 
	 GUILD_STR 'U7',@im_idPlayer,@iserverindex,@im_idGuild,'',@iLv_1

	 GUILD_STR 'U7','000000','02','000029','',10000


*/

ELSE
IF @iGu = 'U8'
	BEGIN
		UPDATE GUILD_TBL
			SET m_szGuild = @im_szGuild
		 WHERE m_idGuild = @im_idGuild
		      AND serverindex = @iserverindex

-- 		IF @@SERVERNAME='CHAR-DB1'
-- 		BEGIN
-- 		UPDATE [LOG].LOG_DBF.dbo.GUILD_TBL  --?? ??? ?? ?? ??? ?? ????
-- 			SET  m_szGuild = @im_szGuild
-- 		 WHERE m_idGuild = @im_idGuild
-- 		      AND serverindex = @iserverindex
-- 		END
		RETURN
	END


/*
	
	 GUILD ?? ?? - ???
	 ex ) 
	 GUILD_STR 'U8',@im_idPlayer,@iserverindex,@im_idGuild,@im_szGuild

	 GUILD_STR 'U8', '000000', '01', '000001', 'asasas'    


*/
ELSE
IF @iGu = 'U9'
	BEGIN
		UPDATE GUILD_MEMBER_TBL 
			SET m_nClass = @im_nClass
		 WHERE m_idPlayer = @im_idPlayer
		      AND serverindex = @iserverindex
		RETURN
	END


/*
	
	 GUILD? ?? ?? - ???
	 ex ) 
	 GUILD_STR 'U9',,@im_idPlayer,@iserverindex,@im_idGuild,@im_szGuild,@iLv_1,@iLv_2,@iLv_3,@iLv_4,@im_nLevel,@im_nGuildGold,
								  @im_nGuildPxp,@im_nWin,@im_nLose,@im_nSurrender,@im_dwLogo,@im_nClass

	 GUILD_STR 'U9','000000','01','000001','',100,80,60,40,0,0,
								  0,0,0,0,0,20,2


*/

ELSE
IF @iGu = 'UA'
	BEGIN
		UPDATE GUILD_MEMBER_TBL 
			SET m_szAlias = @im_szGuild --m_szAlias
		 WHERE m_idPlayer = @im_idPlayer
		      AND serverindex = @iserverindex
		RETURN
	END


/*
	
	 GUILD? ?? ?? - ???
	 ex ) 
	 GUILD_STR 'UA',,@im_idPlayer,@iserverindex,@im_idGuild,@im_szGuild(m_szAlias)

	 GUILD_STR 'UA','000000','01','000001','????'


*/
ELSE
IF @iGu = 'UC'
	BEGIN
		UPDATE GUILD_TBL SET m_dwGuildColor_1 = @im_dwGC_1 WHERE m_idGuild = @im_idGuild AND serverindex = @iserverindex
		UPDATE GUILD_TBL SET m_dwGuildColor_2 = @im_dwGC_2 WHERE m_idGuild = @im_idGuild AND serverindex = @iserverindex
		UPDATE GUILD_TBL SET m_dwGuildColor_3 = @im_dwGC_3 WHERE m_idGuild = @im_idGuild AND serverindex = @iserverindex
	RETURN
	END
ELSE
IF @iGu = 'U0'
	BEGIN
		UPDATE GUILD_TBL SET m_bGuildFinder = @im_dwGF WHERE m_idGuild = @im_idGuild AND serverindex = @iserverindex
	RETURN
	END
ELSE
IF @iGu = 'D1'
	BEGIN
		
			UPDATE CHARACTER_TBL
	         	SET m_tGuildMember = CONVERT(CHAR(8),DATEADD(d,2,GETDATE()),112) 
										+ RIGHT('00' + CONVERT(VARCHAR(2),DATEPART(hh,DATEADD(d,2,GETDATE()))),2) 
										+ RIGHT('00' + CONVERT(VARCHAR(2),DATEPART(mi,DATEADD(d,2,GETDATE()))),2) 
										+ RIGHT('00' + CONVERT(VARCHAR(2),DATEPART(ss,DATEADD(d,2,GETDATE()))),2)
				FROM CHARACTER_TBL A,GUILD_MEMBER_TBL B
			 WHERE A.m_idPlayer = B.m_idPlayer
					AND B.m_idGuild = @im_idGuild
					AND A.serverindex = B.serverindex
			      AND B.serverindex = @iserverindex

			DELETE GUILD_TBL
			 WHERE m_idGuild = @im_idGuild
			      AND serverindex = @iserverindex
	
			DELETE GUILD_MEMBER_TBL
			 WHERE m_idGuild = @im_idGuild
			      AND serverindex = @iserverindex
	
			DELETE GUILD_BANK_TBL
			 WHERE m_idGuild = @im_idGuild
		      AND serverindex = @iserverindex

			DELETE GUILD_BANK_EXT_TBL
			 WHERE m_idGuild = @im_idGuild
		      AND serverindex = @iserverindex

			DELETE GUILD_QUEST_TBL
			WHERE	m_idGuild=@im_idGuild
			AND	serverindex=@iserverindex
				
-- 			IF @@SERVERNAME='CHAR-DB1'
-- 			BEGIN
-- 			UPDATE [LOG].LOG_DBF.dbo.GUILD_TBL  --?? ??? ?? ?? ??? ?? ????
-- 				SET  isuse = 'D'
-- 			 WHERE m_idGuild = @im_idGuild
-- 			      AND serverindex = @iserverindex
-- 
-- 			END





--		??? ??? (??? ????)
-- 		UPDATE GUILD_TBL
-- 			SET isuse = 'F'
-- 		 WHERE m_idGuild = @im_idGuild
-- 		      AND serverindex = @iserverindex
-- 
-- 		UPDATE GUILD_MEMBER_TBL
-- 			SET isuse = 'F'
-- 		 WHERE m_idGuild = @im_idGuild
-- 		      AND serverindex = @iserverindex
		RETURN
	END

/*
	
	 GUILD ??- ???
	 ex ) 
	 GUILD_STR 'D1',@im_idPlayer,@iserverindex,@im_idGuild

	 GUILD_STR 'D1','000000','01','000001'

*/

ELSE
IF @iGu = 'D2'
	BEGIN

		UPDATE CHARACTER_TBL
         	SET m_tGuildMember = CONVERT(CHAR(8),DATEADD(d,2,GETDATE()),112) 
									+ RIGHT('00' + CONVERT(VARCHAR(2),DATEPART(hh,DATEADD(d,2,GETDATE()))),2) 
									+ RIGHT('00' + CONVERT(VARCHAR(2),DATEPART(mi,DATEADD(d,2,GETDATE()))),2) 
									+ RIGHT('00' + CONVERT(VARCHAR(2),DATEPART(ss,DATEADD(d,2,GETDATE()))),2)
		 WHERE m_idPlayer = @im_idPlayer
		      AND serverindex = @iserverindex


		DELETE  GUILD_MEMBER_TBL
		 WHERE m_idPlayer = @im_idPlayer
		      AND serverindex = @iserverindex


--		??? ??? (??? ????)
-- 		UPDATE GUILD_MEMBER_TBL
-- 			SET isuse = 'F'
-- 		 WHERE m_idGuild = @im_idGuild
-- 		      AND serverindex = @iserverindex
 		RETURN
	END

/*
	
	 GUILD ??/?? - ???
	 ex ) 
	 GUILD_STR 'D2',@im_idPlayer,@iserverindex,@im_idGuild

	 GUILD_STR 'D2','000000','01','000001'

*/
set nocount off
