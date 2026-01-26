@echo off
awk -v pos=%2 -f whereis.awk %1
