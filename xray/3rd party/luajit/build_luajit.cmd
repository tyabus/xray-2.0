@luac5.1.exe -s opt.lua
@bin2c5.1.exe luac.out >opt.lua.h
@luac5.1.exe -s opt_inline.lua
@bin2c5.1.exe luac.out >opt_inline.lua.h