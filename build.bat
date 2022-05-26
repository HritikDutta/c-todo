@echo off

cl /Zi /c filestuff\*.c /I .
cl /Zi /c todos\*.c /I .
cl /Zi /c args\*.c /I .
cl /Zi main.c /Fe:todo-test *.obj

del *.obj