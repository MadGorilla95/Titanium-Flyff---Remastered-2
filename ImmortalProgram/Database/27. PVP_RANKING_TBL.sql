USE [CHARACTER_01_DBF]
GO

/****** Object:  Table [dbo].[PVP_RANKING_TBL]    Script Date: 20/01/2019 11:53:12 AM ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

SET ANSI_PADDING ON
GO

CREATE TABLE [dbo].[PVP_RANKING_TBL](
	[serverindex] [char](2) NOT NULL,
	[m_idPlayer] [varchar](7) NOT NULL,
	[m_szName] [varchar](32) NOT NULL,
	[m_szProgress] [varchar](MAX) NOT NULL
	
) ON [PRIMARY]

GO

SET ANSI_PADDING OFF
GO


USE [CHARACTER_01_DBF]
GO

/****** Object:  Index [PVP_RANKING_ID1]    Script Date: 20/01/2019 11:53:20 AM ******/
CREATE UNIQUE NONCLUSTERED INDEX [PVP_RANKING_ID1] ON [dbo].[PVP_RANKING_TBL]
(
	[m_idPlayer] ASC,
	[serverindex] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, IGNORE_DUP_KEY = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 75) ON [PRIMARY]
GO


USE [CHARACTER_01_DBF]
GO

/****** Object:  Index [PVP_RANKING_TBL_ID1]    Script Date: 20/01/2019 11:53:32 AM ******/
CREATE UNIQUE NONCLUSTERED INDEX [PVP_RANKING_TBL_ID1] ON [dbo].[PVP_RANKING_TBL]
(
	[m_idPlayer] DESC,
	[serverindex] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, IGNORE_DUP_KEY = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO


USE [CHARACTER_01_DBF]
GO

/****** Object:  Index [IDX_PVP_RANKING_TBLL_m_idPlayer]    Script Date: 20/01/2019 11:53:40 AM ******/
CREATE UNIQUE CLUSTERED INDEX [IDX_PVP_RANKING_TBLL_m_idPlayer] ON [dbo].[PVP_RANKING_TBL]
(
	[m_idPlayer] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, IGNORE_DUP_KEY = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO


USE [CHARACTER_01_DBF]
GO

/****** Object:  StoredProcedure [dbo].[SAVE_PVP_RANKING]    Script Date: 20/01/2019 11:53:59 AM ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

CREATE PROCEDURE [dbo].[SAVE_PVP_RANKING]
	@im_idPlayer varchar(7),
	@iserverindex char(2),
	@im_szName varchar(32),
	@im_szProgress varchar(MAX)
AS
BEGIN
	SET NOCOUNT ON;

	BEGIN TRANSACTION;

	IF EXISTS (SELECT 1 FROM PVP_RANKING_TBL WITH (UPDLOCK,SERIALIZABLE) WHERE m_idPlayer = @im_idPlayer AND serverindex = @iserverindex)
		UPDATE PVP_RANKING_TBL SET	m_szName = @im_szName,
									m_szProgress = @im_szProgress
							 WHERE
								  m_idPlayer = @im_idPlayer
							 AND  serverindex = @iserverindex
	ELSE
		INSERT PVP_RANKING_TBL (m_idPlayer,     serverindex,    m_szName,   m_szProgress)
					VALUES	  (@im_idPlayer, @iserverindex, @im_szName, @im_szProgress)

	COMMIT TRANSACTION;
END

GO