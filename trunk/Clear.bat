@echo off


cd /D %~dp0

rmdir Bin /s/q
rmdir Output /s/q
del *.ncb
attrib *.suo -s -h -r
del *.suo /s
del *.aps /s
del *.user /s
