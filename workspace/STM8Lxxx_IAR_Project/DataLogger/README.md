# Data Logger Project
This device will collect log from another device and write down to sd card.<br>
Data will be attach timestamp from internal RTC.<br>
In idle state, device will sleep for saving energy and wake up when get new data.<br>
When power slow down, led will flash for notification.<br>

# Operation
This device include 2 mode operation:
- Configuration mode<br>
- Normal mode<br>

## Configuration mode
In this mode, user can configure:
- Date time of RTC<br>
- Serial: baud rate, stop bit, parity, etc<br>
- Level low battery<br>

## Normal mode
Device will get data from another device through serial communicate and write to sd card.<br>