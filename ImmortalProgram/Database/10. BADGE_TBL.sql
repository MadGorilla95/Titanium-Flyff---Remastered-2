USE [CHARACTER_01_DBF]
GO

/****** Object:  Table [dbo].[BADGES_TBL]    Script Date: 20/01/2019 11:53:12 AM ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

SET ANSI_PADDING ON
GO

CREATE TABLE [dbo].[BADGES_TBL](
	[serverindex] [char](2) NOT NULL,
	[m_idPlayer] [varchar](7) NOT NULL,
	[m_nBadge] [int] NOT NULL,
	[m_Badges] [varchar](3096) NOT NULL
) ON [PRIMARY]

GO

SET ANSI_PADDING OFF
GO


USE [CHARACTER_01_DBF]
GO

/****** Object:  Index [BADGES_ID1]    Script Date: 20/01/2019 11:53:20 AM ******/
CREATE UNIQUE NONCLUSTERED INDEX [BADGES_ID1] ON [dbo].[BADGES_TBL]
(
	[m_idPlayer] ASC,
	[serverindex] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, IGNORE_DUP_KEY = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 75) ON [PRIMARY]
GO


USE [CHARACTER_01_DBF]
GO

/****** Object:  Index [BADGES_TBL_ID1]    Script Date: 20/01/2019 11:53:32 AM ******/
CREATE UNIQUE NONCLUSTERED INDEX [BADGES_TBL_ID1] ON [dbo].[BADGES_TBL]
(
	[m_idPlayer] DESC,
	[serverindex] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, IGNORE_DUP_KEY = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO


USE [CHARACTER_01_DBF]
GO

/****** Object:  Index [IDX_BADGES_TBLL_m_idPlayer]    Script Date: 20/01/2019 11:53:40 AM ******/
CREATE UNIQUE CLUSTERED INDEX [IDX_BADGES_TBLL_m_idPlayer] ON [dbo].[BADGES_TBL]
(
	[m_idPlayer] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, IGNORE_DUP_KEY = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO


USE [CHARACTER_01_DBF]
GO

/****** Object:  StoredProcedure [dbo].[SAVE_BADGES]    Script Date: 20/01/2019 11:53:59 AM ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

CREATE PROCEDURE [dbo].[SAVE_BADGES]
	@im_idPlayer varchar(7),
	@iserverindex char(2),
	@im_nBadge int = 0,
	@im_szBadges varchar(3096) = ''
AS
BEGIN
	SET NOCOUNT ON;

	IF EXISTS(SELECT * FROM BADGES_TBL WHERE m_idPlayer = @im_idPlayer AND serverindex = @iserverindex)
		UPDATE BADGES_TBL SET m_nBadge = @im_nBadge,
								  m_Badges = @im_szBadges
							 WHERE
								  m_idPlayer = @im_idPlayer
							 AND  serverindex = @iserverindex
	ELSE
		INSERT BADGES_TBL (m_idPlayer,     serverindex,    m_nBadge,   m_Badges)
					VALUES	  (@im_idPlayer, @iserverindex, @im_nBadge, @im_szBadges)
END

GO


