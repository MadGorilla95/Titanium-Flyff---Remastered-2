USE [CHARACTER_01_DBF]
GO

/****** Object:  Table [dbo].[MONSTERHUNT_TBL]    Script Date: 20/01/2019 11:53:12 AM ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

SET ANSI_PADDING ON
GO

CREATE TABLE [dbo].[MONSTERHUNT_TBL](
	[serverindex] [char](2) NOT NULL,
	[m_idPlayer] [varchar](7) NOT NULL,
	[m_nQuestID] [int] NOT NULL default -1,
	[m_nMobID] [int] NOT NULL default -1,
	[m_nCount] [int] NOT NULL default -1,
	[m_nRewardSent] [int] NOT NULL default -1
) ON [PRIMARY]

GO

SET ANSI_PADDING OFF
GO


USE [CHARACTER_01_DBF]
GO

/****** Object:  Index [MONSTERHUNT_ID1]    Script Date: 20/01/2019 11:53:20 AM ******/
CREATE UNIQUE NONCLUSTERED INDEX [MONSTERHUNT_ID1] ON [dbo].[MONSTERHUNT_TBL]
(
	[m_idPlayer] ASC,
	[serverindex] ASC,
	[m_nQuestID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, IGNORE_DUP_KEY = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 75) ON [PRIMARY]
GO


USE [CHARACTER_01_DBF]
GO

/****** Object:  Index [MONSTERHUNT_TBL_ID1]    Script Date: 20/01/2019 11:53:32 AM ******/
CREATE UNIQUE NONCLUSTERED INDEX [MONSTERHUNT_TBL_ID1] ON [dbo].[MONSTERHUNT_TBL]
(
	[m_idPlayer] DESC,
	[serverindex] ASC,
	[m_nQuestID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, IGNORE_DUP_KEY = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO


USE [CHARACTER_01_DBF]
GO

/****** Object:  Index [IDX_MONSTERHUNT_TBLL_m_idPlayer]    Script Date: 20/01/2019 11:53:40 AM ******/
CREATE UNIQUE CLUSTERED INDEX [IDX_MONSTERHUNT_TBLL_m_idPlayer] ON [dbo].[MONSTERHUNT_TBL]
(
	[m_idPlayer] ASC,
	[m_nQuestID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, IGNORE_DUP_KEY = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO


USE [CHARACTER_01_DBF]
GO

/****** Object:  StoredProcedure [dbo].[SAVE_MONSTERHUNT]    Script Date: 20/01/2019 11:53:59 AM ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

CREATE PROCEDURE [dbo].[SAVE_MONSTERHUNT]
	@im_idPlayer varchar(7),
	@iserverindex char(2),
	@im_nQuestID int,
	@im_nMobID int,
	@im_nCount int,
	@im_nRewardSent int

AS
BEGIN
	SET NOCOUNT ON;

	BEGIN TRANSACTION;

	IF EXISTS (SELECT 1 FROM MONSTERHUNT_TBL WITH (UPDLOCK,SERIALIZABLE) WHERE m_idPlayer = @im_idPlayer AND serverindex = @iserverindex AND m_nQuestID = @im_nQuestID)
		UPDATE MONSTERHUNT_TBL SET m_nMobID = @im_nMobID,
								   m_nCount = @im_nCount,
								   m_nRewardSent = @im_nRewardSent
							 WHERE
								  m_idPlayer = @im_idPlayer
							 AND  serverindex = @iserverindex
							 AND  m_nQuestID = @im_nQuestID
	ELSE
		INSERT MONSTERHUNT_TBL (m_idPlayer,     serverindex,    m_nQuestID,   m_nMobID, m_nCount, m_nRewardSent)
					VALUES	  (@im_idPlayer, @iserverindex, @im_nQuestID, @im_nMobID, @im_nCount, @im_nRewardSent)

	COMMIT TRANSACTION;
END

GO
