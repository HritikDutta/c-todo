@echo off

set outdir=C:\hd-tools\

if not exist %outdir%.todos type nul > %outdir%.todos

cl /c filestuff\*.c /I .
cl /c todos\*.c /I .
cl /c args\*.c /I .
cl main.c /Fe:%outdir%todo *.obj

del *.obj