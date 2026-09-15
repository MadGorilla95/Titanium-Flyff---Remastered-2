USE [CHARACTER_01_DBF]
GO

/****** Object:  Table [dbo].[EQUIPSWITCH_TBL]    Script Date: 20/01/2019 11:53:12 AM ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

SET ANSI_PADDING ON
GO

CREATE TABLE [dbo].[EQUIPSWITCH_TBL](
	[serverindex] [char](2) NOT NULL,
	[m_idPlayer] [varchar](7) NOT NULL,
	[m_nIndex] [int] NOT NULL default -1,
	[m_szTitle] [varchar](128) NOT NULL,
	[m_nActivate] [int] NOT NULL default -1,
	[m_szObjIndex] [varchar](128) NOT NULL
) ON [PRIMARY]

GO

SET ANSI_PADDING OFF
GO


USE [CHARACTER_01_DBF]
GO

/****** Object:  Index [EQUIPSWITCH_ID1]    Script Date: 20/01/2019 11:53:20 AM ******/
CREATE UNIQUE NONCLUSTERED INDEX [EQUIPSWITCH_ID1] ON [dbo].[EQUIPSWITCH_TBL]
(
	[m_idPlayer] ASC,
	[serverindex] ASC,
	[m_nIndex] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, IGNORE_DUP_KEY = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, FILLFACTOR = 75) ON [PRIMARY]
GO


USE [CHARACTER_01_DBF]
GO

/****** Object:  Index [EQUIPSWITCH_TBL_ID1]    Script Date: 20/01/2019 11:53:32 AM ******/
CREATE UNIQUE NONCLUSTERED INDEX [EQUIPSWITCH_TBL_ID1] ON [dbo].[EQUIPSWITCH_TBL]
(
	[m_idPlayer] DESC,
	[serverindex] ASC,
	[m_nIndex] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, IGNORE_DUP_KEY = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO


USE [CHARACTER_01_DBF]
GO

/****** Object:  Index [IDX_EQUIPSWITCH_TBLL_m_idPlayer]    Script Date: 20/01/2019 11:53:40 AM ******/
CREATE UNIQUE CLUSTERED INDEX [IDX_EQUIPSWITCH_TBLL_m_idPlayer] ON [dbo].[EQUIPSWITCH_TBL]
(
	[m_idPlayer] ASC,
	[m_nIndex] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, IGNORE_DUP_KEY = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
GO


USE [CHARACTER_01_DBF]
GO

/****** Object:  StoredProcedure [dbo].[SAVE_EQUIPSWITCH]    Script Date: 20/01/2019 11:53:59 AM ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

CREATE PROCEDURE [dbo].[SAVE_EQUIPSWITCH]
	@im_idPlayer varchar(7),
	@iserverindex char(2),
	@im_nIndex int,
	@im_szTitle varchar(128),
	@im_nActivate int,
	@im_szObjIndex varchar(128)
AS
BEGIN
	SET NOCOUNT ON;

	BEGIN TRANSACTION;

	IF EXISTS (SELECT 1 FROM EQUIPSWITCH_TBL WITH (UPDLOCK,SERIALIZABLE) WHERE m_idPlayer = @im_idPlayer AND serverindex = @iserverindex AND m_nIndex = @im_nIndex)
		UPDATE EQUIPSWITCH_TBL SET m_szTitle = @im_szTitle,
								   m_nActivate = @im_nActivate,
								   m_szObjIndex = @im_szObjIndex
							 WHERE
								  m_idPlayer = @im_idPlayer
							 AND  serverindex = @iserverindex
							 AND  m_nIndex = @im_nIndex
	ELSE
		INSERT EQUIPSWITCH_TBL (m_idPlayer,     serverindex,    m_nIndex,   m_szTitle, m_nActivate, m_szObjIndex)
					VALUES	  (@im_idPlayer, @iserverindex, @im_nIndex, @im_szTitle, @im_nActivate, @im_szObjIndex)

	COMMIT TRANSACTION;
END

GO


/****** Object:  StoredProcedure [dbo].[DELETE_EQUIPSWITCH]    Script Date: 20/01/2019 11:53:59 AM ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

CREATE PROCEDURE [dbo].[DELETE_EQUIPSWITCH]
	@im_idPlayer varchar(7),
	@iserverindex char(2),
	@im_nIndex int
AS
BEGIN
	SET NOCOUNT ON;

	BEGIN TRANSACTION;

	DELETE FROM EQUIPSWITCH_TBL WITH (UPDLOCK,SERIALIZABLE) WHERE  m_idPlayer = @im_idPlayer AND serverindex = @iserverindex AND m_nIndex >= @im_nIndex

	COMMIT TRANSACTION;
END

GO
