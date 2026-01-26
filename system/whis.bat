@echo off
awk -v pos=%2 -f %INSTALL_DIR%/system/whereis.awk %1
