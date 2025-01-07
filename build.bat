@echo off
setlocal enabledelayedexpansion

set FILES=
for %%f in (*.c) do (
    set FILES=!FILES! %%f
)

gcc -g %FILES% -o program
