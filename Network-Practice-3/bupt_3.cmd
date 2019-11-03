@echo off
call bin/script/copyright.cmd
title 控制台,请不要关闭本窗口！
echo * bupt 路由器实验                                                           *
echo *=============================================================================*
cd tmp
"../bin/dynagen/dynagen.exe" ..\net\bupt_3.net