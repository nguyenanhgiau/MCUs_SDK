# SDK_MCUs
Summary:
========
This directory contains the source code for sdk_mcus, a platform for many
micro controller

Directory Hierarchy:
====================

    /chip                       chip configuration
        /portable               unique port for MCU
    /components                 
        /dbg                    debugger
        /libraries              libraries peripheral and other
        /common                 queue and timer
        /freertos               folder contain source code for FreeRTOS
        /utilities
    /external                   folder contain modules external
        /button                 module button
        /led                    module led
        ...
    /boards                     configure board selection
    /toolchain                  set of tool to build projects (intention to use GNU toolchain)
    /workspace                  directory contain projects with specify application
    /scripts                    various build scripts
    /test                       various unit test file
    /doc                        document and guide line

Software Configuration:
=======================
Configuration is usually done using C preprocessor defines;
Each project contain a file prj_config.h. This file will configure:
* [MCU Controller](#Micro-Controller)
* [Application Configuration](#App-Configurating)

Micro Controller:
------------------------------
Currently, MCU was selected in preprocessor of IAR IDE.<br>
In toolbar: Project >> Options (Alt+F7)<br>
Click General, in tab Target, select your MCU.<br>
Click C/C++ Compiler, In tab Preprocessor, select MCU family.<br>
Click [OK] for confirm.

App Configuration:
----------------------

Configuration depends on the components that was used in your project.<br>
Ex: Configuration for Button<br>
```c
#define BUTTON_TOTAL_NUMBER     1
```

Ex: Configuration for LED<br>
```c
#define LED_TOTAL_NUMBER     1
#define LED_SUPPORT_COLOR   /* if you use rgb LED */
#define LED_SUPPORT_EFFECT  /* if you use effect: blink, flash, etc. */
```
