@echo off
COPY "Source\Output\AccountServer\Release64\AccountServer.exe" "Program\1. Account-x64.exe"
COPY "Source\Output\CacheServer\Release64\CacheServer.exe" "Program\6. Cache-x64.exe"
COPY "Source\Output\Certifier\Release64\Certifier.exe" "Program\2. Certifier-x64.exe"
COPY "Source\Output\CoreServer\Release64\CoreServer.exe" "Program\4. Core-x64.exe"
COPY "Source\Output\DatabaseServer\Release64\DatabaseServer.exe" "Program\3. Database-x64.exe"
COPY "Source\Output\LoginServer\Release64\LoginServer.exe" "Program\5. Login-x64.exe"
COPY "Source\Output\WorldServer\Release64\WorldServer.exe" "Program\7. World-x64.exe"
COPY "Source\Output\Neuz\Release64\Neuz.exe" "Client\Neuz-x64.exe"
