USE [ACCOUNT_DBF]
GO

DECLARE	@return_value int

EXEC	@return_value = [dbo].[usp_CreateNewAccount]
		@account = N'Marvin7',				-- ID: test
		@pw = N'0cbc6611f5540bd0809a388dc95a615b'	-- PW: test

SELECT	'Return Value' = @return_value

GO

UPDATE account_tbl_detail set m_chLoginAuthority = 'Z'