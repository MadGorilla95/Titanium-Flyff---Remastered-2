USE [LOGGING_01_DBF]
GO
/****** Object:  StoredProcedure [dbo].[usp_GuildFurniture_Log]    Script Date: 04/03/2010 12:42:39 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO
    
CREATE proc [dbo].[usp_GuildFurniture_Log]    
 @serverindex char(2),    
 @m_idGuild  char (6),  
 @SEQ int = 0  
as    
    
set nocount on    
set xact_abort on    
  
if @SEQ = 0  
begin  
-- EXEC('  insert into LOG_' + @serverindex + '_DBF.dbo.tblGuildHouse_FurnitureLog   
 EXEC('  insert into LOGGING_01_DBF.dbo.tblGuildHouse_FurnitureLog   
   (  
    serverindex, m_idGuild, SEQ, ItemIndex, bSetup, s_date, set_date  
   )  
  SELECT  serverindex, m_idGuild, SEQ, ItemIndex, bSetup, s_date, set_date   
   from tblGuildHouse_Furniture (nolock)  
   where serverindex = ' + @serverindex + ' and m_idGuild = ' + @m_idGuild +''  
  )  
end  
else if @SEQ <> 0  
begin  
-- EXEC('  insert into LOG_' + @serverindex + '_DBF.dbo.tblGuildHouse_FurnitureLog   
 EXEC('  insert into LOGGING_01_DBF.dbo.tblGuildHouse_FurnitureLog    (  
    serverindex, m_idGuild, SEQ, ItemIndex, bSetup, s_date, set_date  
   )  
  SELECT  serverindex, m_idGuild, SEQ, ItemIndex, bSetup, s_date, set_date  
   
   from tblGuildHouse_Furniture (nolock)  
   where serverindex = ' + @serverindex + ' and m_idGuild = ' + @m_idGuild + ' and SEQ = ' + @SEQ + ' '  
  )  
end
GO
/****** Object:  StoredProcedure [dbo].[usp_GuildHouse_Log]    Script Date: 04/03/2010 12:42:39 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO
  
CREATE      proc [dbo].[usp_GuildHouse_Log]  
	@serverindex char(2),  
	@m_idGuild  char (6),
	@SEQ int = 0
as  
  
set nocount on  
set xact_abort on  

--	EXEC('  insert into LOG_' + @serverindex + '_DBF.dbo.tblGuildHouse_FurnitureLog 
	EXEC('  insert into LOGGING_01_DBF.dbo.tblGuildHouseLog 			(
				serverindex, m_idGuild, dwWorldID, tKeepTime, m_szGuild
			)
		SELECT  serverindex, m_idGuild, dwWorldID, tKeepTime, m_szGuild
			from tblGuildHouse (nolock)
			where serverindex = ' + @serverindex + ' and m_idGuild = ' + @m_idGuild + ' '
		)
GO

USE [LOGGING_01_DBF]
GO

/****** Object:  Table [dbo].[tblGuildHouse_Furniture]    Script Date: 21-5-2020 19:23:57 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

CREATE TABLE [dbo].[tblGuildHouse_Furniture](
	[serverindex] [char](2) NOT NULL,
	[m_idGuild] [char](6) NOT NULL,
	[SEQ] [int] NOT NULL,
	[ItemIndex] [int] NULL,
	[bSetup] [int] NULL,
	[x_Pos] [float] NULL,
	[y_Pos] [float] NULL,
	[z_Pos] [float] NULL,
	[fAngle] [float] NULL,
	[tKeepTime] [int] NULL,
	[s_date] [datetime] NULL,
	[set_date] [datetime] NULL
) ON [PRIMARY]
GO

ALTER TABLE [dbo].[tblGuildHouse_Furniture] ADD  CONSTRAINT [DF_tblGuildHoues_Furniture_bSetup]  DEFAULT ((0)) FOR [bSetup]
GO

ALTER TABLE [dbo].[tblGuildHouse_Furniture] ADD  CONSTRAINT [DF_tblGuildHoues_Furniture_x_Pos]  DEFAULT ((0)) FOR [x_Pos]
GO

ALTER TABLE [dbo].[tblGuildHouse_Furniture] ADD  CONSTRAINT [DF_tblGuildHoues_Furniture_y_Pos]  DEFAULT ((0)) FOR [y_Pos]
GO

ALTER TABLE [dbo].[tblGuildHouse_Furniture] ADD  CONSTRAINT [DF_tblGuildHoues_Furniture_z_Pos]  DEFAULT ((0)) FOR [z_Pos]
GO

ALTER TABLE [dbo].[tblGuildHouse_Furniture] ADD  CONSTRAINT [DF_tblGuildHoues_Furniture_fAngle]  DEFAULT ((0)) FOR [fAngle]
GO

ALTER TABLE [dbo].[tblGuildHouse_Furniture] ADD  CONSTRAINT [DF_tblGuildHoues_Furniture_tKeepTime]  DEFAULT ((0)) FOR [tKeepTime]
GO

ALTER TABLE [dbo].[tblGuildHouse_Furniture] ADD  CONSTRAINT [DF_tblGuildHouse_Furniture_s_date]  DEFAULT (getdate()) FOR [s_date]
GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'???' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tblGuildHouse_Furniture', @level2type=N'COLUMN',@level2name=N'serverindex'
GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'?? ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tblGuildHouse_Furniture', @level2type=N'COLUMN',@level2name=N'm_idGuild'
GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'??? ??' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tblGuildHouse_Furniture', @level2type=N'COLUMN',@level2name=N'SEQ'
GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'?? ITEM ID' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tblGuildHouse_Furniture', @level2type=N'COLUMN',@level2name=N'ItemIndex'
GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'??? ?? ??' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tblGuildHouse_Furniture', @level2type=N'COLUMN',@level2name=N'bSetup'
GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'?? ?? ??' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tblGuildHouse_Furniture', @level2type=N'COLUMN',@level2name=N'fAngle'
GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'??? ?? ????' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tblGuildHouse_Furniture', @level2type=N'COLUMN',@level2name=N'tKeepTime'
GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'??????' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tblGuildHouse_Furniture', @level2type=N'COLUMN',@level2name=N's_date'
GO

EXEC sys.sp_addextendedproperty @name=N'MS_Description', @value=N'? ????' , @level0type=N'SCHEMA',@level0name=N'dbo', @level1type=N'TABLE',@level1name=N'tblGuildHouse_Furniture', @level2type=N'COLUMN',@level2name=N'set_date'
GO
