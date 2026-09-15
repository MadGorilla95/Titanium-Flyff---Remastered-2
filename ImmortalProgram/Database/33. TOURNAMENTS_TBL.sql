USE [CHARACTER_01_DBF]
GO

SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

SET ANSI_PADDING ON
GO

CREATE TABLE [dbo].[TOURNAMENTS_TBL](
	[serverindex] [char](2) NOT NULL,
	[m_nTournamentID] [int] NOT NULL default 0,
	[m_nTeamSize] [int] NOT NULL default 0,
	[m_nRounds] [int] NOT NULL default 0,
	[m_nRoundTime] [int] NOT NULL default 0,	
	[m_nStartTimeHour] [int] NOT NULL default 0,
	[m_nStartTimeMin] [int] NOT NULL default 0,
	[m_nStartTimeDay] [int] NOT NULL default 0,
	[m_nStartTimeMonth] [int] NOT NULL default 0,
	[m_nStartTimeYear] [int] NOT NULL default 0,
	[m_nStartTimeDst] [int] NOT NULL default 0,	
	[m_nFriendlyFire] [int] NOT NULL default 0,
	[m_nLevelReqMin] [int] NOT NULL default 0,
	[m_nLevelReqMax] [int] NOT NULL default 0,
	[m_nAllowedClasses] [int] NOT NULL default 0,
	[m_nTeamReqNum] [int] NOT NULL default 0,
	[m_szRewards] [varchar](MAX) NOT NULL,
	[m_szTeams] [varchar](MAX) NOT NULL,
	
) ON [PRIMARY]

GO