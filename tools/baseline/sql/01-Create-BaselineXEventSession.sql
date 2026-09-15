/*
Titanium Flyff Phase 0 - SQL Server Extended Events baseline

Run in SSMS with SQLCMD Mode enabled, or with sqlcmd.
The directory must already exist and the SQL Server service account must have write access.

Duration is measured in microseconds by these completion events.
Start with a low threshold in a controlled local/test environment; raise it for a busy server.
*/

:setvar SessionName "Titanium_Phase0_Baseline"
:setvar EventFileBasePath "C:\Temp\Titanium_Phase0_Baseline.xel"
:setvar MinimumDurationMicroseconds "1000"

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

IF EXISTS (
    SELECT 1
    FROM sys.server_event_sessions
    WHERE name = N'$(SessionName)'
)
BEGIN
    DROP EVENT SESSION [$(SessionName)] ON SERVER;
END
GO

CREATE EVENT SESSION [$(SessionName)]
ON SERVER

ADD EVENT sqlserver.rpc_completed
(
    ACTION
    (
        sqlserver.client_app_name,
        sqlserver.client_hostname,
        sqlserver.database_id,
        sqlserver.session_id,
        sqlserver.sql_text,
        sqlserver.username
    )
    WHERE
    (
        [duration] >= $(MinimumDurationMicroseconds)
    )
),

ADD EVENT sqlserver.sql_batch_completed
(
    ACTION
    (
        sqlserver.client_app_name,
        sqlserver.client_hostname,
        sqlserver.database_id,
        sqlserver.session_id,
        sqlserver.sql_text,
        sqlserver.username
    )
    WHERE
    (
        [duration] >= $(MinimumDurationMicroseconds)
    )
)

ADD TARGET package0.event_file
(
    SET
        filename = N'$(EventFileBasePath)',
        max_file_size = (100),
        max_rollover_files = (5)
)

WITH
(
    MAX_MEMORY = 4096 KB,
    EVENT_RETENTION_MODE = ALLOW_SINGLE_EVENT_LOSS,
    MAX_DISPATCH_LATENCY = 5 SECONDS,
    MAX_EVENT_SIZE = 0 KB,
    MEMORY_PARTITION_MODE = NONE,
    TRACK_CAUSALITY = ON,
    STARTUP_STATE = OFF
);
GO

ALTER EVENT SESSION [$(SessionName)] ON SERVER STATE = START;
GO

SELECT
    s.name,
    CASE WHEN xes.name IS NULL THEN 0 ELSE 1 END AS is_running,
    N'$(EventFileBasePath)' AS event_file_base_path,
    $(MinimumDurationMicroseconds) AS minimum_duration_microseconds
FROM sys.server_event_sessions AS s
LEFT JOIN sys.dm_xe_sessions AS xes
    ON xes.name = s.name
WHERE s.name = N'$(SessionName)';
GO
