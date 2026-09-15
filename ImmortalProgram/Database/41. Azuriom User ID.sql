USE [ACCOUNT_DBF]
ALTER TABLE ACCOUNT_TBL
ADD 
Azuriom_user_id bigint NOT NULL default 0,
Azuriom_user_access_token varchar(MAX)