USE [CHARACTER_01_DBF]
GO

ALTER TABLE tblPropose
ALTER COLUMN tPropose bigint ;
GO

USE [CHARACTER_01_DBF]
GO
/****** Object:  StoredProcedure [dbo].[usp_RestPoint_Update]    Script Date: 30.05.2021 15:04:06 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO
/****** Object:  Stored Procedure dbo.usp_RestPoint_Update    Script Date: 2009-12-01 ?? 2:41:44 ******/
/*============================================================
1. ??? : ???
2. ??? : 2009.11.30
3. ???? ? : usp_RestPoint_Update
4. ???? ?? : ??? ?? ??? ????
5. ????
	@serverindex char(2)		???
	@m_idPlyaer char(7)			??? ID
	@m_nRestPoint int			?? ???
	@m_LogOutTime int			??
6. ??? 	
7. ?? ??
8. ?? ?? ??
    EXEC usp_RestPoint_Update '01', '1234567', 100
9. ?? ? ident ? ???
	select * from tblRestPoint
	delete tblGuildHouse_Furniture
	dbcc CHECKIDENT ( 'tblGuildHouse_Furniture', reseed, 0)
============================================================*/

ALTER   proc [dbo].[usp_RestPoint_Update]
	@serverindex char(2), 
	@m_idPlayer char(7), 
	@m_nRestPoint int,
	@m_LogOutTime bigint
as

set nocount on
set xact_abort on

	if exists ( select * from tblRestPoint (nolock) where serverindex = @serverindex and m_idPlayer = @m_idPlayer )
		begin
			update tblRestPoint 
			set m_nRestPoint = @m_nRestPoint, m_LogOutTime = @m_LogOutTime 
			where serverindex = @serverindex and m_idPlayer = @m_idPlayer
		end
	else
		begin
			insert into tblRestPoint (serverindex, m_idPlayer, m_nRestPoint, m_LogOutTime)
			select @serverindex, @m_idPlayer, @m_nRestPoint, @m_LogOutTime
		end
GO

USE [CHARACTER_01_DBF]
GO

ALTER TABLE tblRestPoint
ALTER COLUMN m_LogOutTime bigint ;
GO

USE [CHARACTER_01_DBF]
GO
/****** Object:  StoredProcedure [dbo].[uspElectionAddDeposit]    Script Date: 30.05.2021 15:09:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
ALTER proc [dbo].[uspElectionAddDeposit]
@nServer int,
@idElection int,
@idPlayer int,
@iDeposit bigint,
@tCreate bigint
AS
SET NOCOUNT ON

IF EXISTS ( SELECT * FROM tblLordCandidates WHERE nServer = @nServer AND idElection = @idElection AND idPlayer = @idPlayer )
BEGIN
	UPDATE tblLordCandidates
	SET iDeposit = iDeposit + @iDeposit
	WHERE nServer = @nServer AND idElection = @idElection AND idPlayer = @idPlayer
END
ELSE
BEGIN
	INSERT INTO tblLordCandidates ( nServer, idElection, idPlayer, iDeposit, szPledge, nVote, tCreate)
	VALUES ( @nServer, @idElection, @idPlayer, @iDeposit, '', 0, @tCreate )
END
GO

USE [CHARACTER_01_DBF]
GO

ALTER TABLE tblLordCandidates
DROP CONSTRAINT DF_tblLordCandidates_tCreate;

GO

ALTER TABLE tblLordCandidates
ALTER COLUMN tCreate bigint ;
GO

USE [CHARACTER_01_DBF]
GO

ALTER TABLE [dbo].[tblLordCandidates] ADD  CONSTRAINT [DF_tblLordCandidates_tCreate]  DEFAULT ((0)) FOR [tCreate]
GO