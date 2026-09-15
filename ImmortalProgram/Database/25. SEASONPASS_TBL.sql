USE [CHARACTER_01_DBF]
GO

/****** Object:  Table [dbo].[SEASONPASS_TBL]    Script Date: 20/01/2019 11:53:12 AM ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

SET ANSI_PADDING ON
GO

CREATE TABLE [dbo].[SEASONPASS_TBL](
	[serverindex] [char](2) NOT NULL,
	[m_idPlayer] [varchar](7) NOT NULL,
	[m_nPassID] [int] NOT NULL default -1,
	[m_nPassPoints] [int] NOT NULL default -1,
	[m_nBoughtPass] [int] NOT NULL default -1,
	[m_szClaimed] [varchar](MAX) NOT NULL
) ON [PRIMARY]

GO

SET ANSI_PADDING OFF
GO


USE [CHARACTER_01_DBF]
GO

/****** Object:  Index [ACHIEVEMENT_ID1]    Script Date: 20/01/2019 11:53:20 AM ******/
CREATE UNIQUE NONCLUSTERED INDEX [SEASONPASS_ID1] ON [dbo].[SEASONPASS_TBL]
(
	[m_idPlayer] ASC,
	[serverindex] ASC,
	[m_nPassID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, IGNORE_DUP_KEY = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 75) ON [PRIMARY]
GO


USE [CHARACTER_01_DBF]
GO

/****** Object:  Index [SEASONPASS_TBL_ID1]    Script Date: 20/01/2019 11:53:32 AM ******/
CREATE UNIQUE NONCLUSTERED INDEX [SEASONPASS_TBL_ID1] ON [dbo].[SEASONPASS_TBL]
(
	[m_idPlayer] DESC,
	[serverindex] ASC,
	[m_nPassID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, IGNORE_DUP_KEY = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO


USE [CHARACTER_01_DBF]
GO

/****** Object:  Index [IDX_SEASONPASS_TBLL_m_idPlayer]    Script Date: 20/01/2019 11:53:40 AM ******/
CREATE UNIQUE CLUSTERED INDEX [IDX_SEASONPASS_TBLL_m_idPlayer] ON [dbo].[SEASONPASS_TBL]
(
	[m_idPlayer] ASC,
	[m_nPassID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, IGNORE_DUP_KEY = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO


USE [CHARACTER_01_DBF]
GO

/****** Object:  StoredProcedure [dbo].[SAVE_SEASONPASS]    Script Date: 20/01/2019 11:53:59 AM ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

CREATE PROCEDURE [dbo].[SAVE_SEASONPASS]
	@im_idPlayer varchar(7),
	@iserverindex char(2),
	@im_nPassID int = -1,
	@im_nPassPoints int = -1,
	@im_nBoughtPass int = -1,
	@im_szClaimed varchar(MAX)
AS
BEGIN
	SET NOCOUNT ON;

	IF EXISTS(SELECT * FROM SEASONPASS_TBL WITH (UPDLOCK,SERIALIZABLE) WHERE m_idPlayer = @im_idPlayer AND serverindex = @iserverindex AND m_nPassID = @im_nPassID)
		UPDATE SEASONPASS_TBL SET m_nPassPoints = @im_nPassPoints,
								   m_nBoughtPass = @im_nBoughtPass,
								   m_szClaimed = @im_szClaimed
							 WHERE
								  m_idPlayer = @im_idPlayer
							 AND  serverindex = @iserverindex
							 AND  m_nPassID = @im_nPassID
	ELSE
		INSERT SEASONPASS_TBL (m_idPlayer,     serverindex,    m_nPassID,   m_nPassPoints, m_nBoughtPass, m_szClaimed)
					VALUES	  (@im_idPlayer, @iserverindex, @im_nPassID, @im_nPassPoints, @im_nBoughtPass, @im_szClaimed)
END

GO


