usage: [executable] [output bsp name] [output lump name]


#Example
Language: Batch
```
@echo off

set out=%~d1
set out=%out%%~p1
set out=%out%%~n1

set lump=%out%_out_l_0.lmp
set out=%out%_out.bsp
entlump_remover_bsp.exe %1 "%out%" "%lump%"
pause
```