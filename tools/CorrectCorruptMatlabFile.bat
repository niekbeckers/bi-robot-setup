@echo off

IF "%~1" == "" GOTO HELP

python %~dp0CorrectCorruptMatlabFile.py %1
goto END

:HELP
echo Usage: drag-drop a .mat file onto this .batch-file.
goto END

:END
pause
