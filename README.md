# SDK_MCUs
Summary:
========
This directory contains the source code for sdk_mcus, a platform for
micro controller

Directory Hierarchy:
====================

    /chip                       chip configuration
    /components                 
        /dbg                    debugger
        /libraries              libraries peripheral and other
        /common                 queue and timer
        /freertos               folder contain source code for FreeRTOS
        /utilities
    /external                   folder contain modules external
    /boards                     configure board selection
    /toolchain                  set of tool to build projects
    /workspace                  directory containt projects with specify application
    /scripts                    various build scripts
    /test                       various unit test file
    /doc                        document and guide line

Software Configuration:
=======================
Configuration is usually done using C preprocessor defines;
Each project contain 1 file prj_config.h. This file will configure:
* [MCU Controller](#MCU-Controller)
* [Board](#Board)
* [Application Configurating](#App-Configurating)

Micro Controller:
------------------------------
Choise one of MCUs that was supported;

Board:
----------------------------
Choise boards was supported or define new board.

App Configurating:
----------------------

Configuration depends on the combination of board and CPU type; all such information is kept in a configuration file
"include/configs/<board_name>.h".
