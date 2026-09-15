USE [CHARACTER_01_DBF]
GO

/****** Object:  StoredProcedure [dbo].[uspVendorNpcInsert]    Script Date: 10/6/2562 0:00:31 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO



CREATE PROCEDURE [dbo].[uspVendorNpcInsert]
		@iidPlayer		char(7),
		@iidWorld		int,
		@iidChannel		int,
		@iPosX			float,
		@iPosY			float,
		@iPosZ			float,
		@iAngle			float,
		@iRemaining		int,
		@iScale			float,
		@iShopName		varchar(64),
		@iszItem		varchar(MAX),
		@iszPet			varchar(MAX),
		@iszExt			varchar(MAX),
		@iszPiercing	varchar(MAX),
		@iszIndex		varchar(MAX),
		@iszObjIndex	varchar(MAX),
		@idwItemID		int,
		@idwShopDesign	int
AS
SET NOCOUNT ON

	INSERT INTO [dbo].[tblVendorNPC]
		([idPlayer]
           ,[idChannel]
           ,[idWorld]
           ,[fPosX]
           ,[fPosY]
           ,[fPosZ]
           ,[fAngle]
           ,[nRemaining]
           ,[fScale]
           ,[szShopName]
           ,[szItem]
           ,[szPet]
           ,[szExt]
           ,[szPiercing]
           ,[szIndex]
           ,[szObjIndex]
           ,[dwItemID]
		   ,[dwShopDesign])
	VALUES
		(@iidPlayer
		   ,@iidChannel
		   ,@iidWorld
		   ,@iPosX
		   ,@iPosY
		   ,@iPosZ
		   ,@iAngle
		   ,@iRemaining
		   ,@iScale
		   ,@iShopName
		   ,@iszItem
		   ,@iszPet
		   ,@iszExt
		   ,@iszPiercing
		   ,@iszIndex
		   ,@iszObjIndex
		   ,@idwItemID
		   ,@idwShopDesign)

	SELECT idVendor 
		FROM [dbo].[tblVendorNPC]
		WHERE idPlayer = @iidPlayer
		ORDER BY idVendor DESC

SET NOCOUNT OFF




GO

USE [CHARACTER_01_DBF]
GO

/****** Object:  StoredProcedure [dbo].[uspVendorNpcDelete]    Script Date: 10/6/2562 0:00:45 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO



CREATE PROCEDURE [dbo].[uspVendorNpcDelete]
		@iidVendor		int
AS
SET NOCOUNT ON

	DELETE [dbo].[tblVendorNPC]
		WHERE idVendor = @iidVendor


SET NOCOUNT OFF




GO

USE [CHARACTER_01_DBF]
GO

/****** Object:  StoredProcedure [dbo].[uspVendorNpcUpdate]    Script Date: 10/6/2562 0:00:58 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO



CREATE PROCEDURE [dbo].[uspVendorNpcUpdate]
		@iidVendor		int,
		@iRemaining		int,
		@iszItem		varchar(MAX),
		@iszPet			varchar(MAX),
		@iszExt			varchar(MAX),
		@iszPiercing	varchar(MAX),
		@iszIndex		varchar(MAX),
		@iszObjIndex	varchar(MAX),
		@idwShopDesign	int
AS
SET NOCOUNT ON

	UPDATE [dbo].[tblVendorNPC]
		SET nRemaining = @iRemaining,
			szItem = @iszItem,
			szPet = @iszPet,
			szExt = @iszExt,
			szPiercing = @iszPiercing,
			szIndex = @iszIndex,
			szObjIndex = @iszObjIndex,
			dwShopDesign = @idwShopDesign
		WHERE idVendor = @iidVendor


SET NOCOUNT OFF




GO

USE [CHARACTER_01_DBF]
GO

/****** Object:  StoredProcedure [dbo].[uspVendorNpcGet]    Script Date: 10/6/2562 0:01:10 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO



CREATE PROCEDURE [dbo].[uspVendorNpcGet]
		@iidChannel		int
AS
SET NOCOUNT ON

	SELECT a.[idVendor]
		  ,a.[idPlayer]
		  ,a.[idChannel]
		  ,a.[idWorld]
		  ,a.[fPosX]
		  ,a.[fPosY]
		  ,a.[fPosZ]
		  ,a.[fAngle]
		  ,a.[nRemaining]
		  ,a.[fScale]
		  ,a.[szShopName]
		  ,a.[szItem]
		  ,a.[szPet]
		  ,a.[szExt]
		  ,a.[szPiercing]
		  ,a.[szIndex]
		  ,a.[szObjIndex]
		  ,a.[dwItemID]
		  ,a.[dwShopDesign]
		  ,b.[m_szName] 
		FROM tblVendorNPC a, CHARACTER_TBL b
		WHERE a.idChannel = @iidChannel AND a.idPlayer = b.m_idPlayer


SET NOCOUNT OFF




GO

USE [LOGGING_01_DBF]
GO

/****** Object:  StoredProcedure [dbo].[uspVendorNpcLogInsert]    Script Date: 10/6/2562 0:02:13 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO



CREATE PROCEDURE [dbo].[uspVendorNpcLogInsert]
		@iidPlayer		char(7),
		@iidWorld		int,
		@iidChannel		int,
		@iPosX			float,
		@iPosY			float,
		@iPosZ			float,
		@iAngle			float,
		@iRemaining		int,
		@iScale			float,
		@iShopName		varchar(MAX),
		@iszItem		varchar(MAX),
		@iszPet			varchar(MAX),
		@iszExt			varchar(MAX),
		@iszPiercing	varchar(MAX),
		@iszIndex		varchar(MAX),
		@iszObjIndex	varchar(MAX),
		@idwItemID		int
AS
SET NOCOUNT ON

	INSERT INTO [dbo].[tblVendorNPCLog]
		([idPlayer]
           ,[idWorld]
           ,[idChannel]
           ,[fPosX]
           ,[fPosY]
           ,[fPosZ]
           ,[fAngle]
           ,[nRemaining]
           ,[fScale]
           ,[szShopName]
           ,[szItem]
           ,[szPet]
           ,[szExt]
           ,[szPiercing]
           ,[szIndex]
           ,[szObjIndex]
           ,[dwItemID])
	VALUES
		(@iidPlayer
		   ,@iidWorld
		   ,@iidChannel
		   ,@iPosX
		   ,@iPosY
		   ,@iPosZ
		   ,@iAngle
		   ,@iRemaining
		   ,@iScale
		   ,@iShopName
		   ,@iszItem
		   ,@iszPet
		   ,@iszExt
		   ,@iszPiercing
		   ,@iszIndex
		   ,@iszObjIndex
		   ,@idwItemID)

	SELECT idVendor 
		FROM [dbo].[tblVendorNPCLog]
		WHERE idPlayer = @iidPlayer
		ORDER BY idVendor DESC

SET NOCOUNT OFF



GO

USE [LOGGING_01_DBF]
GO

/****** Object:  StoredProcedure [dbo].[uspShopCloseLog]    Script Date: 10/6/2562 0:02:44 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO



CREATE PROCEDURE [dbo].[uspShopCloseLog]
		@itmTime		int,
		@iidPlayer		char(7),
		@iszItem		varchar(MAX),
		@iszPet			varchar(MAX),
		@iszExt			varchar(MAX),
		@iszPiercing	varchar(MAX)
AS
SET NOCOUNT ON

	INSERT INTO [dbo].[tblShopCloseLog]
		([tmTime]
			,[idPlayer]
           ,[szItem]
           ,[szPet]
           ,[szExt]
           ,[szPiercing])
	VALUES
		(@itmTime
			,@iidPlayer
		   ,@iszItem
		   ,@iszPet
		   ,@iszExt
		   ,@iszPiercing)
SET NOCOUNT OFF



GO

USE [LOGGING_01_DBF]
GO

/****** Object:  Table [dbo].[tblVendorNPCLog]    Script Date: 10/6/2562 0:07:35 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

SET ANSI_PADDING ON
GO

CREATE TABLE [dbo].[tblVendorNPCLog](
	[idVendor] [int] IDENTITY(1,1) NOT NULL,
	[idPlayer] [char](7) NULL,
	[idWorld] [int] NULL,
	[idChannel] [int] NULL,
	[fPosX] [float] NULL,
	[fPosY] [float] NULL,
	[fPosZ] [float] NULL,
	[fAngle] [float] NULL,
	[nRemaining] [int] NULL,
	[fScale] [float] NULL,
	[szShopName] [varchar](MAX) NULL,
	[szItem] [varchar](MAX) NULL,
	[szPet] [varchar](MAX) NULL,
	[szExt] [varchar](MAX) NULL,
	[szPiercing] [varchar](MAX) NULL,
	[szIndex] [varchar](MAX) NULL,
	[szObjIndex] [varchar](MAX) NULL,
	[dwItemID] [int] NULL,
	[serverindex] [char](2) NULL
) ON [PRIMARY]

GO

SET ANSI_PADDING OFF
GO

USE [LOGGING_01_DBF]
GO

/****** Object:  Table [dbo].[tblShopCloseLog]    Script Date: 10/6/2562 0:08:09 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

SET ANSI_PADDING ON
GO

CREATE TABLE [dbo].[tblShopCloseLog](
	[tmTime] [int] NULL,
	[idPlayer] [char](7) NULL,
	[szItem] [varchar](MAX) NULL,
	[szPet] [varchar](MAX) NULL,
	[szExt] [varchar](MAX) NULL,
	[szPiercing] [varchar](MAX) NULL
) ON [PRIMARY]

GO

SET ANSI_PADDING OFF
GO

USE [CHARACTER_01_DBF]
GO

/****** Object:  Table [dbo].[tblVendorNPC]    Script Date: 10/6/2562 0:09:04 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

SET ANSI_PADDING ON
GO

CREATE TABLE [dbo].[tblVendorNPC](
	[idVendor] [int] IDENTITY(1,1) NOT NULL,
	[idPlayer] [char](7) NULL,
	[idWorld] [int] NULL,
	[idChannel] [int] NULL,
	[fPosX] [float] NULL,
	[fPosY] [float] NULL,
	[fPosZ] [float] NULL,
	[fAngle] [float] NULL,
	[nRemaining] [int] NULL,
	[fScale] [float] NULL,
	[szShopName] [varchar](MAX) NULL,
	[szItem] [varchar](MAX) NULL,
	[szPet] [varchar](MAX) NULL,
	[szExt] [varchar](MAX) NULL,
	[szPiercing] [varchar](MAX) NULL,
	[szIndex] [varchar](MAX) NULL,
	[szObjIndex] [varchar](MAX) NULL,
	[dwItemID] [int] NULL,
	[dwShopDesign] [int] NULL,
	[serverindex] [char](2) NULL
) ON [PRIMARY]

GO

SET ANSI_PADDING OFF
GO

