@ECHO OFF
REM %1 = $(ProjectDir)
REM %2 = $(OutDir)

ECHO Copying CorePlatform headers...

IF EXIST %2\CorePlatform\ rmdir /S /Q %2\CorePlatform\ 2>nul 1>nul
mkdir %2\CorePlatform\ 2>nul 1>nul
copy /Y %1\CorePlatform\*.h %2\CorePlatform\ 2>nul 1>nul

EXIT /B 0
