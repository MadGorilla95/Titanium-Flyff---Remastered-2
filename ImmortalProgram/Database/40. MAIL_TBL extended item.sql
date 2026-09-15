USE [CHARACTER_01_DBF]
ALTER TABLE MAIL_TBL
ADD 
szItem varchar(MAX) not NULL default '$',
szItemExt varchar(MAX) not NULL default '$',
szItemPiercing varchar(MAX) not NULL default '$',
szItemPet varchar(MAX) not NULL default '$'