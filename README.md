#E7 All-in-one Reader with PIN PAD (e7-uc-13-a0 ) custom firmware

[READER LINK](http://en1.fongwah.com/products/new-e7-card-reader-with-pin-pad/ "READER LINK")

What you find on keil project
* full pinout
* programming pinout
* keyboard (ch452a) driver, fm1702 driver, at24cxx driver, lcd st7920 driver, buzzer and leds

## Note
If you want to enable USB you must derive 48MHz from system clock.
Select 72MHz or 96MHz [here](https://github.com/fr3ns1s/e7_reader/blob/a40e4b9a57d59ef569e48435c4d3784186151525/Firmware/CMSIS/GD/GD32F10x/Source/system_gd32f10x.c#L61 "here")

## Programming - Debugging
See board_conf.h for the pinout
You can use any JTAG/SWD programmer 

## Thanks to
* [stm8s_fm1702](https://github.com/LiuHuiGitHub/stm8s_fm1702 "stm8s_fm1702")
* [glcd-st7920-lib](https://github.com/liyanboy74/glcd-st7920-lib "glcd-st7920-lib")


