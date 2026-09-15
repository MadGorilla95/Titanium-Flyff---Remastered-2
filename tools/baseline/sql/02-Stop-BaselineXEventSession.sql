/*
Stop the Titanium Phase 0 Extended Events session after the controlled workload.
Run in SSMS with SQLCMD Mode enabled, or with sqlcmd.
*/

:setvar SessionName "Titanium_Phase0_Baseline"

USE [master];
GO

IF EXISTS (
    SELECT 1
    FROM sys.dm_xe_sessions
    WHERE name = N'$(SessionName)'
)
BEGIN
    ALTER EVENT SESSION [$(SessionName)] ON SERVER STATE = STOP;
END
GO

SELECT
    s.name,
    CASE WHEN xes.name IS NULL THEN 0 ELSE 1 END AS is_running
FROM sys.server_event_sessions AS s
LEFT JOIN sys.dm_xe_sessions AS xes
    ON xes.name = s.name
WHERE s.name = N'$(SessionName)';
GO
