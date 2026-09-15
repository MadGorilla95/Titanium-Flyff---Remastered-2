USE [CHARACTER_01_DBF]
GO

/****** Object:  StoredProcedure [dbo].[LoadSiegeRanking]    Script Date: 22/07/2021 21:45:15 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO


CREATE proc [dbo].[LoadSiegeRanking]
 @serverindex char(2)
as
set nocount on
set xact_abort on

select szName, idPlayer, nJob, nKills, nDeaths, nPoints
 from PlayerWar_TBL
where serverindex = @serverindex
return
GO


USE [CHARACTER_01_DBF]
GO

/****** Object:  Table [dbo].[PlayerWar_TBL]    Script Date: 22/07/2021 21:43:36 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

CREATE TABLE [dbo].[PlayerWar_TBL](
	[serverindex] [char](2) NULL,
	[szName] [varchar](32) NULL,
	[idPlayer] [char](7) NULL,
	[nJob] [int] NULL,
	[nKills] [int] NULL,
	[nDeaths] [int] NULL,
	[nPoints] [int] NULL
) ON [PRIMARY]
GO


