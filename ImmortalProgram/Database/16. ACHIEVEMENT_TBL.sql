USE [CHARACTER_01_DBF]
GO

/****** Object:  Table [dbo].[ACHIEVEMENT_TBL]    Script Date: 20/01/2019 11:53:12 AM ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

SET ANSI_PADDING ON
GO

CREATE TABLE [dbo].[ACHIEVEMENT_TBL](
	[serverindex] [char](2) NOT NULL,
	[m_idPlayer] [varchar](7) NOT NULL,
	[m_nAchievementID] [int] NOT NULL default -1,
	[m_nAchievementProg] [int] NOT NULL default -1,
	[m_nRewardSent] [int] NOT NULL default -1
) ON [PRIMARY]

GO

SET ANSI_PADDING OFF
GO


USE [CHARACTER_01_DBF]
GO

/****** Object:  Index [ACHIEVEMENT_ID1]    Script Date: 20/01/2019 11:53:20 AM ******/
CREATE UNIQUE NONCLUSTERED INDEX [ACHIEVEMENT_ID1] ON [dbo].[ACHIEVEMENT_TBL]
(
	[m_idPlayer] ASC,
	[serverindex] ASC,
	[m_nAchievementID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, IGNORE_DUP_KEY = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 75) ON [PRIMARY]
GO


USE [CHARACTER_01_DBF]
GO

/****** Object:  Index [ACHIEVEMENT_TBL_ID1]    Script Date: 20/01/2019 11:53:32 AM ******/
CREATE UNIQUE NONCLUSTERED INDEX [ACHIEVEMENT_TBL_ID1] ON [dbo].[ACHIEVEMENT_TBL]
(
	[m_idPlayer] DESC,
	[serverindex] ASC,
	[m_nAchievementID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, IGNORE_DUP_KEY = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO


USE [CHARACTER_01_DBF]
GO

/****** Object:  Index [IDX_ACHIEVEMENT_TBLL_m_idPlayer]    Script Date: 20/01/2019 11:53:40 AM ******/
CREATE UNIQUE CLUSTERED INDEX [IDX_ACHIEVEMENT_TBLL_m_idPlayer] ON [dbo].[ACHIEVEMENT_TBL]
(
	[m_idPlayer] ASC,
	[m_nAchievementID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, IGNORE_DUP_KEY = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO


USE [CHARACTER_01_DBF]
GO

/****** Object:  StoredProcedure [dbo].[SAVE_ACHIEVEMENT]    Script Date: 20/01/2019 11:53:59 AM ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

CREATE PROCEDURE [dbo].[SAVE_ACHIEVEMENT]
	@im_idPlayer varchar(7),
	@iserverindex char(2),
	@im_nAchievementID int = -1,
	@im_nAchievementProg int = -1,
	@im_nRewardSent int = -1
AS
BEGIN
	SET NOCOUNT ON;

	IF EXISTS(SELECT * FROM ACHIEVEMENT_TBL WITH (UPDLOCK,SERIALIZABLE) WHERE m_idPlayer = @im_idPlayer AND serverindex = @iserverindex AND m_nAchievementID = @im_nAchievementID)
		UPDATE ACHIEVEMENT_TBL SET m_nAchievementProg = @im_nAchievementProg,
								   m_nRewardSent = @im_nRewardSent
							 WHERE
								  m_idPlayer = @im_idPlayer
							 AND  serverindex = @iserverindex
							 AND  m_nAchievementID = @im_nAchievementID
	ELSE
		INSERT ACHIEVEMENT_TBL (m_idPlayer,     serverindex,    m_nAchievementID,   m_nAchievementProg, m_nRewardSent)
					VALUES	  (@im_idPlayer, @iserverindex, @im_nAchievementID, @im_nAchievementProg, @im_nRewardSent)
END

GO


