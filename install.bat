@echo off

set outdir=C:\todos\

if not exist %outdir%todo-data.txt type nul > %outdir%todo-data.txt

cl /c filestuff\*.c /I .
cl /c todos\*.c /I .
cl /c args\*.c /I .
cl main.c /Fe:%outdir%todo *.obj

del *.obj