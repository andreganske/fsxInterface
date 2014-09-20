fsxInterface
============


<br><br>
Getting started
------------

This code make the interface between Link2FS and Arduino MEGA board.
Before use this, you need to know some things, like:
- how Arduino works and how to use it;
- how to upload files to Arduino board;
- how to use Link2FS from Jim - very thank for this work Jim!;
	
If you dont know some of that stuff please read the links below before continue:
* [Getting Started with Arduino](http://arduino.cc/en/Guide/HomePage)
* [Arduino Environment basics](http://arduino.cc/en/Guide/Environment)
* [Arduino examples and tutorials](http://arduino.cc/en/Tutorial/HomePage)
* [Link2FS home page](http://www.jimspage.co.nz/intro.htm)
* [Link2FS to FSX](http://www.jimspage.co.nz/Link2fs_Multi.htm)

<br><br>
Installation
------------

<h4>On arduino board</h4>
To install this file, just upload it to your board.
It was tested and works perfectly in Arduino MEGA 2560, 
more information here: [arduino MEGA] (http://arduino.cc/en/Main/arduinoBoardMega2560).<br>

<h4>On Link2FS</h4>
In order to use all functionalities of this file on your Link2FS, you will need to enable the following options:

<br><br>
Whats is supported
------------

This version of code provides support to many extraction from MSFS.
All test was if 

<br><br>
Connections to the board
------------

This code was prepared to be easy to personilize. So, all the connections
was declered by define on the beging, as follow:

```
	#define rot_radio01 8 // radio rotary encoder
	#define rot_radio02 9 // radio rotary encoder
	#define rot_radio03 10 // radio rotary push
	#define psh_radio 11 // radio push switch

	#define radio_com1 14 // radio COM1
	#define radio_com2 15 // radio COM2
	#define radio_nav1 16 // radio NAV1
	#define radio_nav2 17 // radio NAV2
	#define radio_adf 18 // radio ADF
	#define radio_dme 19 // radio DME

	#define lcd_sda 20 // LCD I2C SDA
	#define lcd_scl 21 // LCD I2C SCL

	#define lcd_clk 22 // LCD clock
	#define lcd_alt 23 // LCD altimeter
	#define lcd_hdg 24 // LCD heading
	#define lcd_spd 25 // LCD speed

	#define rot_alt01 26 // altitude rotary encoder
	#define rot_alt02 27 // altitude rotary encoder
	#define rot_alt03 28 // altitude rotary push

	#define rot_hdg01 29 // heading rotary encoder
	#define rot_hdg02 30 // heading rotary encoder
	#define rot_hdg03 31 // heading rotary push

	#define rot_spd01 32 // speed rotary encoder
	#define rot_spd02 33 // speed rotary encoder
	#define rot_spd03 34 // speed rotary push

	#define swt_gear 40 // Gear up/down
	#define swt_pbrk 41 // parking breaks on/off
	#define swt_splr 42 // auto spoiler on/off

	#define swt_nav 44 // Light switch nav
	#define swt_bcn 45 // Light switch beacon
	#define swt_ldn 46 // Light switch landing
	#define swt_tax 47 // Light switch taxi
	#define swt_stb 48 // Light switch strobo
	#define swt_pnl 49 // Light switch panel
	#define swt_rcg 50 // Light switch recognation
	#define swt_wng 51 // Light switch wing
	#define swt_lgo 52 // Light switch logo
	#define swt_cbn 53 // Light switch cabin

	#define led_gdn 54 // LED Gear down N
	#define led_gdl 55 // LED Gear down L
	#define led_gdr 56 // LED Gear down R

	#define led_gmn 57 // LED Gear moving N
	#define led_gml 58 // LED Gear moving L
	#define led_gmr 59 // LED Gear moving R

	#define led_war_flap 60 // LED warning flaps overspeed
	#define led_war_gear 61 // LED warning gear overspeed
	#define led_war_frame 62 // LED warning frame overspeed
	#define led_war_stall 63 // LED warning stall 
	#define led_war_fuel 64 // LED warning total fuel
	#define buz_warning 65 // Buzzer Master warning
```
<br><br>
TODOs
------------

 - [x] Support to LM1637 LCDs to show alt, hdg and spd autopilot;
 - [ ] Support for more than one rotary encoder, to set alt, hdg and spd autopilot;
 - [ ] Support to time in flight
