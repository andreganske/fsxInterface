/*
 This sketch is a complete radio head for FSX
 It needs to be used with Link2fs_Multi
 It was compiled with Arduino version 1.1
 
 Full project info at http://www.jimspage.co.nz/intro.htm 
 This example code is in the public domain.
 July 2013
 Once you get it going you may be able to remove some DELAY's
 
 **************************************************************
 This sketch was modified by Andre Ganske | @andreganske
 * include support to i2c LCD module
 * include support to TM1637 lcds modules
 */
#include <Arduino.h>

#include "math.h"
#include <Wire.h>

// Rotary encoders library, you can get some great stuff about here: http://www.jimspage.co.nz/encoders2.htm
#include "Quadrature.h"

// TM1637 lcds library, you can get this library here: https://github.com/avishorp/TM1637
#include <TM1637Display.h>

// Get the LCD I2C library here: https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads
#include <LiquidCrystal_I2C.h>

/****************************************************************************************/
// Defining some stuff

#define ON 1
#define OFF 0

const uint8_t disp_ap[] = {0x00,0x00,0x00,0x00};

// define all pins 
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

// Set LCD I2C address
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

// Defining rotary encoders pins
Quadrature QUAD_radio(rot_radio01, rot_radio02);
Quadrature QUAD_alt(rot_alt01, rot_alt02);
Quadrature QUAD_spd(rot_hdg01, rot_hdg02);
Quadrature QUAD_hdg(rot_spd01, rot_spd03);

// set LCDs TM1637 addresses
TM1637Display tm1637_alt(lcd_clk, lcd_alt);
TM1637Display tm1637_hdg(lcd_clk, lcd_hdg);
TM1637Display tm1637_spd(lcd_clk, lcd_spd);

int CodeIn,  // used on all serial reads
    pulseOn = 0,
    active,
    activeold,
    mark = 10;
    
int alt_X,
    alt_Xold,
    alt_Xdif;
    
int hdg_X,
    hdg_Xold,
    hdg_Xdif;
    
int spd_X,
    spd_Xold,
    spd_Xdif;
    
int radio_X,
    radio_Xold,
    radio_Xdif;
    
String stringoldstate,
       pinStateSTR,
       pinStateSTRold,
       stringnewstate;
       
String com1,
       com1old,
       com1sb, 
       com1sbold,
       com2,
       com2old,
       com2sb,
       com2sbold;
       
String nav1,
       nav1old,
       nav1sb,
       nav1sbold,
       nav2,
       nav2old,
       nav2sb,
       nav2sbold;
       
String adf,
       adfold,
       xpdr,
       xpdrold,
       dme1,
       dme1old,
       dme2,
       dme2old;
       
String hdg,
       hdgold,
       spd,
       spdold,
       alt,
       altold;

String AnunB = "0",
       AnunC = "0",
       AnunD = "0",
       AnunE = "0",
       AnunH = "0";
       
String AnunBx = "0",
       AnunCx = "0",
       AnunDx = "0",
       AnunEx = "0",
       AnunHx = "0";
       
String gearSimple;
       
unsigned long TimeStart = 0;
unsigned long TimeNow = 0;
unsigned long TimeInterval = 500;

void lcdClear() {
  tm1637_alt.setBrightness(0x0f);
  tm1637_spd.setBrightness(0x0f);
  tm1637_hdg.setBrightness(0x0f);
  
  tm1637_alt.showNumberDec(0, true, 3, 0);
  tm1637_spd.showNumberDec(0, true, 3, 0);
  tm1637_hdg.showNumberDec(0, true, 3, 0);
    
  lcd.clear();
}

void setup() {
 
  // initialize the lcd for 16 chars 2 lines, turn on backlight
  lcd.begin(16,2);
  lcd.backlight();

  // just to show something
  lcdClear();
  
  stringoldstate = "111111111111111111111111111111111111111111111111111111111111111111111";
    
  // setup the input pins 
  for (int doutPin = 42; doutPin >= 10; doutPin--) {  
    pinMode(doutPin, INPUT);
    digitalWrite(doutPin, HIGH);  
  } 
  
  // Get all the OUTPUT pins ready.
  for (int PinNo = 69; PinNo >= 43; PinNo--) {
    pinMode(PinNo, OUTPUT);
    digitalWrite(PinNo, LOW);
  }
  
  Serial.begin(115200);
}

/******************************************************************************************************************/
void loop() {
  {INPUTS();}     //Check the "Inputs" section
  {LCDMODE();}    // Sets up the LCD for the mode it's in (From the rotary switch)
  {ROTARYS();}    // Go and check the rotary encoder
  {PULSE_LEDs();} // Check the pulsing void

  if (Serial.available()) {
    CodeIn = getChar();
    if (CodeIn == '=') {EQUALS();}   // The first identifier is "="
    if (CodeIn == '<') {LESSTHAN();} // The first identifier is "<"
    if (CodeIn == '?') {QUESTION();} // The first identifier is "?"
    if (CodeIn == '/') {SLASH();}    // The first identifier is "/"
  }
}

/******************************************************************************************************************/
 
// Get a character from the serial buffer
char getChar() {
  while(Serial.available() == 0);
  return((char)Serial.read());
}

int8_t convertString(char str) {
  switch (str) {
     case '0': return 0; break;
     case '1': return 1; break;
     case '2': return 2; break;
     case '3': return 3; break;
     case '4': return 4; break;
     case '5': return 5; break;
     case '6': return 6; break;
     case '7': return 7; break;
     case '8': return 8; break;
     case '9': return 9; break;
  }
}

/******************************************************************************************************************/
void EQUALS(){ // The first identifier is "="
  CodeIn = getChar();
  uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };
 
  switch (CodeIn) {
     case 'b':     // Autopilot ALT
       alt = "";       
       alt += getChar();
       alt += getChar();
       alt += getChar();
       alt += getChar();
       alt += getChar();
      
       data[0] = tm1637_alt.encodeDigit(alt[0]-'0');
       data[1] = tm1637_alt.encodeDigit(alt[1]-'0');
       data[2] = tm1637_alt.encodeDigit(alt[2]-'0');
       data[3] = tm1637_alt.encodeDigit(alt[3]-'0');
     
       if (alt != altold) {
         tm1637_alt.setSegments(data, 4, 0);
         altold = alt;
       }
     break;
     
     case 'f':     // Autopilot SPD
       spd = "";       
       spd += getChar();
       spd += getChar();
       spd += getChar();
       spd += getChar();
      
       data[0] = tm1637_spd.encodeDigit(0);
       data[1] = tm1637_spd.encodeDigit(spd[0]-'0');
       data[2] = tm1637_spd.encodeDigit(spd[1]-'0');
       data[3] = tm1637_spd.encodeDigit(spd[2]-'0');
     
       if (spd != spdold) {
         tm1637_spd.setSegments(data, 4, 0);
         spdold = spd;
       }
     break;
     
     case 'd':     // Autopilot HDG
       hdg = "";       
       hdg += getChar();
       hdg += getChar();
       hdg += getChar();
       hdg += getChar();
      
       data[0] = tm1637_hdg.encodeDigit(0);
       data[1] = tm1637_hdg.encodeDigit(hdg[0]-'0');
       data[2] = tm1637_hdg.encodeDigit(hdg[1]-'0');
       data[3] = tm1637_hdg.encodeDigit(hdg[2]-'0');
     
       if (hdg != hdgold) {
         tm1637_hdg.setSegments(data, 4, 0);
         hdgold = hdg;
       }
     break;
     
     case 'A':     // NAV1
       com1 = "";
       com1 += getChar();
       com1 += getChar();
       com1 += getChar();
       com1 += getChar();
       com1 += getChar();
       com1 += getChar();
       com1 += getChar();
       
       if (com1 != com1old && digitalRead(radio_com1) == LOW) {
         delay (11);
         lcd.setCursor(9, 0);
         delay (11);
         lcd.print(com1);
         com1old = com1;
       }
     break;
     
     case 'B':     // NAV1 S/B
       com1sb = "";
       com1sb += getChar();
       com1sb += getChar();
       com1sb += getChar();
       com1sb += getChar();
       com1sb += getChar();
       com1sb += getChar();
       com1sb += getChar();
       
       if (com1sb != com1sbold && digitalRead(radio_com1) == LOW ){
         delay (11);
         lcd.setCursor(9, 1);
         delay (11);
         lcd.print(com1sb);
         com1sbold = com1sb;
       }
     break;
     
     case  'C':     // COM2
       com2 = "";
       com2 += getChar();
       com2 += getChar();
       com2 += getChar();
       com2 += getChar();
       com2 += getChar();
       com2 += getChar();
       com2 += getChar();
       
       if (com2 != com2old && digitalRead(radio_com2) == LOW) {
         lcd.setCursor(9, 0);
         delay (11);
         lcd.print(com2);
         com2old = com2;
       }
     break;
     
     case  'D':     // COM2 S/B
       com2sb = "";
       com2sb += getChar();
       com2sb += getChar();
       com2sb += getChar();
       com2sb += getChar();
       com2sb += getChar();
       com2sb += getChar();
       com2sb += getChar();
       
       if (com2sb != com2sbold && digitalRead(radio_com2) == LOW) {
         lcd.setCursor(9, 1);
         delay (11);
         lcd.print(com2sb);
         com2sbold = com2sb;
       }
     break;          
     
     case  'E':     // NAV1
       delay (11);
       nav1 = "";
       nav1 += getChar();
       nav1 += getChar();
       nav1 += getChar();
       nav1 += getChar();
       nav1 += getChar();
       nav1 += getChar();
       nav1 += (" ");
       
       if (nav1 != nav1old && digitalRead(radio_nav1) == LOW) {
         lcd.setCursor(9, 0);
         delay (11);
         lcd.print(nav1);
         nav1old = nav1;
       }
     break;
     
     case  'F':     // NAV1 S/B
       nav1sb = "";
       nav1sb += getChar();
       nav1sb += getChar();
       nav1sb += getChar();
       nav1sb += getChar();
       nav1sb += getChar();
       nav1sb += getChar();
       nav1sb += (" ");
       
       if (nav1sb != nav1sbold && digitalRead(radio_nav1) == LOW) {
         lcd.setCursor(9, 1);
         delay (11);
         lcd.print(nav1sb);
         nav1sbold = nav1sb;
       }
     break;
     
     case  'G':     // NAV2
       delay (11);
       nav2 = "";
       nav2 += getChar();
       nav2 += getChar();
       nav2 += getChar();
       nav2 += getChar();
       nav2 += getChar();
       nav2 += getChar();
       nav2 += (" ");
       
       if (nav2 != nav2old && digitalRead(radio_nav2) == LOW) {
         lcd.setCursor(9, 0);
         delay (11);
         lcd.print(nav2);
         nav2old = nav2;
       }
     break;
     
     case  'H':     // NAV2 S/B
       nav2sb = "";
       nav2sb += getChar();
       nav2sb += getChar();
       nav2sb += getChar();
       nav2sb += getChar();
       nav2sb += getChar();
       nav2sb += getChar();
       nav2sb += (" ");
       
       if (nav2sb != nav2sbold && digitalRead(radio_nav2) == LOW) {
         lcd.setCursor(9, 1);
         delay (11);
         lcd.print(nav2sb);
         nav2sbold = nav2sb;
       }
     break;
     
     case  'I':     // ADF
       adf = "";
       adf += getChar();
       adf += getChar();
       adf += getChar();
       adf += getChar();
       adf += getChar();
       adf += getChar();
       adf += (" ");
       
       if (adf != adfold && digitalRead(radio_adf) == LOW) {
         lcd.setCursor(9, 0);
         delay (11);
         lcd.print(adf);
         adfold = adf;
       }
     break;   
     
     case  'K':     // DME1
       delay (11);
       dme1 = "";
       dme1 += getChar();
       dme1 += getChar();
       dme1 += getChar();
       dme1 += getChar();
       dme1 += getChar();
       dme1 += ("   ");
       
       if (dme1 != dme1old && digitalRead(radio_dme) == LOW) {
         lcd.setCursor(9, 0);
         delay (11);
         lcd.print(dme1);
         dme1old = dme1;
       }
     break;          
     
     case  'L':    // DME2
       delay (11);
       dme2 = "";
       dme2 += getChar();
       dme2 += getChar();
       dme2 += getChar();
       dme2 += getChar();
       dme2 += getChar();
       dme2 += ("   ");
       
       if (dme2 != dme2old && digitalRead(radio_dme) == LOW) {
         lcd.setCursor(9, 1);
         delay (11);
         lcd.print(dme2);
         dme2old = dme2;
       }
      break;
  }
}

/******************************************************************************************************************/
void LESSTHAN(){ // The first identifier is "<"
  CodeIn = getChar();
}

/******************************************************************************************************************/
void QUESTION(){ // The first identifier is "?"
  CodeIn = getChar();
  switch(CodeIn) {
    case 'Y': // Landing gear
      gearSimple = "";

      gearSimple += getChar(); // nose gear
      if (gearSimple == "2"){digitalWrite(led_gdn, HIGH);}else{digitalWrite(led_gdn, LOW);} // down
      if (gearSimple == "1"){digitalWrite(led_gmn, HIGH);}else{digitalWrite(led_gmn, LOW);} // moving
      
      gearSimple += getChar(); // left gear
      if (gearSimple.endsWith("2")){digitalWrite(led_gdl, HIGH);}else{digitalWrite(led_gdl, LOW);} // down
      if (gearSimple.endsWith("1")){digitalWrite(led_gml, HIGH);}else{digitalWrite(led_gml, LOW);} // moving
    
      gearSimple += getChar(); // right gear
      if (gearSimple.endsWith("2")){digitalWrite(led_gdr, HIGH);}else{digitalWrite(led_gdr, LOW);} // down
      if (gearSimple.endsWith("1")){digitalWrite(led_gmr, HIGH);}else{digitalWrite(led_gmr, LOW);} // moving
    break;
  }
}

/******************************************************************************************************************/
void SLASH(){ // The first identifier is "/"
  CodeIn = getChar();
  switch(CodeIn) {
    case 'B':
      AnunB = String(getChar()); // get state if /B  (Overspeed flaps)
      if (AnunB == "1") {
        if (AnunBx != "3") {
          digitalWrite(led_war_flap, HIGH);
          delay(11);
          digitalWrite(buz_warning, HIGH);
        }
      } else { 
        digitalWrite(led_war_flap, LOW);
        AnunBx = "0";
        digitalWrite(buz_warning, LOW);
      }
    break;

    case 'C':
      AnunC = String(getChar());// get state if /C  (Overspeed gear)
      if (AnunC == "1") {
        if (AnunCx != "3") {
          digitalWrite(led_war_gear, HIGH);
          delay(11); 
          digitalWrite(buz_warning, HIGH);
        }
      } else {
        digitalWrite(led_war_gear, LOW);
        AnunCx = "0";
        digitalWrite(buz_warning, LOW);
      }
    break;

    case 'D':
      AnunD = String(getChar());// get state if /D  (Overspeed frame)
      if (AnunD == "1") {
        if (AnunDx != "3") {
          digitalWrite(led_war_frame, HIGH);
          delay(11);
          digitalWrite(buz_warning, HIGH);
        }
      } else {
        digitalWrite(led_war_frame, LOW);
        AnunDx = "0";
        digitalWrite(buz_warning, LOW);
      }
    break;

    case 'E':
      AnunE = String(getChar());// get state if /E  (Stall warning)
      if (AnunE == "1") {
        if (AnunEx != "3") {
          digitalWrite(led_war_stall, HIGH);
          delay(11);
          digitalWrite(buz_warning, HIGH);
        }
      } else {
        digitalWrite(led_war_stall, LOW);
        AnunEx = "0";
        digitalWrite(buz_warning, LOW);
      }
    break;

    case 'H':
      AnunH = String(getChar());// get state if /H  (Low fuel)
      if (AnunH == "1") {
        if (AnunHx != "3") {
          digitalWrite(led_war_fuel, HIGH);
          delay(11);
          digitalWrite(buz_warning, HIGH);
        }
      } else {
        digitalWrite(led_war_fuel, LOW);
        AnunHx = "0";
        digitalWrite(buz_warning, LOW);
      }
    break;
  }
}

/******************************************************************************************************************/
void INPUTS(){
  stringnewstate = "";
  for (int pinNo = 10; pinNo <= 42; pinNo++){ // validade all input pins
    pinStateSTR = String(digitalRead(pinNo));
    pinStateSTRold = "";
    pinStateSTRold += String(stringoldstate.charAt(pinNo - 10));
    if(pinStateSTR != pinStateSTRold) {
      if (pinNo == psh_radio and pinStateSTR == "0") {
        if (active == radio_com1) Serial.println("A06"); //com1
        if (active == radio_com2) Serial.println("A12"); //com2
        if (active == radio_nav1) Serial.println("A18"); //nav1
        if (active == radio_nav2) Serial.println("A24"); //nav2
        
        if ( active == radio_adf){ //adf
          mark = (mark + 1);
          if (mark == 13) mark = radio_com1; // sort out for piont in ADF
          active = 1;
          if (mark > radio_com1) mark = 10;
        }
      }
      
      if (pinNo == swt_gear){if (pinStateSTR == "0"){Serial.println ("C01");}else {Serial.println ("C02");}} // Gear DOWN
      if (pinNo == swt_pbrk){if (pinStateSTR == "0"){Serial.println ("C040");}else {Serial.println ("C041");}} // Parking brake
      if (pinNo == swt_splr){if (pinStateSTR == "0"){Serial.println ("C20");}else {Serial.println ("C21");}} // Auto spoiler
       
      if (pinNo == swt_nav){if (pinStateSTR == "1" ){Serial.println ("C411");}else {Serial.println ("C410");}} //Nav lights
      if (pinNo == swt_bcn){if (pinStateSTR == "1" ){Serial.println ("C421");}else {Serial.println ("C420");}} //Beacon lights
      if (pinNo == swt_ldn){if (pinStateSTR == "1" ){Serial.println ("C431");}else {Serial.println ("C430");}} //Landing lights
      if (pinNo == swt_tax){if (pinStateSTR == "1" ){Serial.println ("C441");}else {Serial.println ("C440");}} //Taxi lights
      if (pinNo == swt_stb){if (pinStateSTR == "1" ){Serial.println ("C451");}else {Serial.println ("C450");}} //Strobe lights
      if (pinNo == swt_pnl){if (pinStateSTR == "1" ){Serial.println ("C461");}else {Serial.println ("C460");}} //Panel lights
      if (pinNo == swt_rcg){if (pinStateSTR == "1" ){Serial.println ("C471");}else {Serial.println ("C470");}} //Recognitian lights
      if (pinNo == swt_wng){if (pinStateSTR == "1" ){Serial.println ("C481");}else {Serial.println ("C480");}} //Wing lightsD
      if (pinNo == swt_lgo){if (pinStateSTR == "1" ){Serial.println ("C491");}else {Serial.println ("C490");}} //Logo lights
      if (pinNo == swt_cbn){if (pinStateSTR == "1" ){Serial.println ("C501");}else {Serial.println ("C500");}} //Cabin lights
       
     if (pinNo > swt_cbn){
        Serial.print ("D"); 
        if (pinNo < 10) Serial.print ("0");
        Serial.print (pinNo);
        Serial.println (pinStateSTR);
      }
    }
    stringnewstate += pinStateSTR;
  }
  stringoldstate = stringnewstate;
  delay(11);
}

/******************************************************************************************************************/
void LCDMODE(){
  
  if (active != activeold) {
    activeold = active;  
  }

  if (digitalRead(radio_com1)==0 and active != radio_com1) { // Sets up the LCD when switching to Com1)
    lcd.setCursor(0, 0);
    delay (11);
    lcd.print("Com.1    "); 
    lcd.setCursor(9, 0);
    delay (11);
    lcd.print(com1); 
    lcd.setCursor(0, 1);
    delay (11);
    lcd.print("  s/b    ");
    lcd.setCursor(9, 1);
    delay (11);
    lcd.print(com1sb); 
    active = radio_com1;
  }

  if (digitalRead(radio_com2)==0 and active != radio_com2) {  // Sets up the LCD when switching to Com2)
    lcd.setCursor(0, 0);
    delay (11);
    lcd.print("Com.2    ");  
    lcd.setCursor(9, 0);
    delay (11);
    lcd.print(com2); 
    lcd.setCursor(0, 1);
    lcd.print("  s/b    ");
    lcd.setCursor(9, 1);
    delay (11);
    lcd.print(com2sb);
    active = radio_com2;
  }

  if (digitalRead(radio_nav1)==0 and active != radio_nav1) {  // Sets up the LCD when switching to Nav1)
    lcd.setCursor(0, 0);
    delay (11);
    lcd.print("Nav.1    ");  
    lcd.setCursor(9, 0);
    delay (11);
    lcd.print(nav1); 
    lcd.setCursor(0, 1);
    lcd.print("  s/b    ");
    lcd.setCursor(9, 1);
    delay (11);
    lcd.print(nav1sb);
    active = radio_nav1;
  }

  if (digitalRead(radio_nav2)==0 and active != radio_nav2) {  // Sets up the LCD when switching to Nav2)
    lcd.setCursor(0, 0);
    delay (11);
    lcd.print("Nav.2    ");  
    lcd.setCursor(9, 0);
    delay (11);
    lcd.print(nav2); 
    lcd.setCursor(0, 1);
    lcd.print("  s/b    ");
    lcd.setCursor(9, 1);
    delay (11);
    lcd.print(nav2sb);
    active = radio_nav2;
  }   

  if (digitalRead(radio_adf)==0 and active != radio_adf) {  // Sets up the LCD when switching to ADF)
    lcd.setCursor(0, 0);
    delay (11);
    lcd.print("ADF      ");  
    lcd.setCursor(9, 0);
    delay (11);
    lcd.print(adf); 
    lcd.print("  ");
    lcd.setCursor(0, 1);
    lcd.print("                ");
    if (mark == 9 ){mark = 10;}
    lcd.setCursor(mark, 1);
    lcd.print("-");
    active = radio_adf;             
  }

  if (digitalRead(radio_dme)==0 and active != radio_dme) {  // Sets up the LCD when switching to DME)
    lcd.setCursor(0, 0);
    delay (11);
    lcd.print("DME1     ");  
    lcd.setCursor(9, 0);
    delay (11);
    lcd.print(dme1); 
    lcd.print("  ");
    lcd.setCursor(0, 1);
    lcd.print("DME2     ");
    lcd.setCursor(9, 1);
    delay (11);
    lcd.print(dme2);
    active = radio_dme;
  }
}

/******************************************************************************************************************/
void ROTARYS(){
  RADIO_ROTARY();
  ALT_ROTARY();
  HDG_ROTARY();
  SPD_ROTARY();
}

void ALT_ROTARY() {
  alt_X = (QUAD_alt.position()/2);
  if(alt_X != alt_Xold) {
    alt_Xdif = (alt_X - alt_Xold);
    if (alt_Xdif == 1) Serial.println("B11");
    if (alt_Xdif == -1) Serial.println("B12");
  }
  
  if (QUAD_alt.position() > 1000) QUAD_alt.position(0);
  if (QUAD_alt.position() < -1000) QUAD_alt.position(0);
  alt_Xold = alt_X;
}

void HDG_ROTARY() {
  hdg_X = (QUAD_hdg.position()/2);
  if(hdg_X != hdg_Xold) {
    hdg_Xdif = (hdg_X - hdg_Xold);
    if (hdg_Xdif == 1) Serial.println("A57");
    if (hdg_Xdif == -1) Serial.println("A58");
  }
  
  if (QUAD_hdg.position() > 1000) QUAD_hdg.position(0);
  if (QUAD_hdg.position() < -1000) QUAD_hdg.position(0);
  hdg_Xold = hdg_X;
}

void SPD_ROTARY() {
  spd_X = (QUAD_spd.position()/2);
  if(spd_X != spd_Xold) {
    spd_Xdif = (spd_X - spd_Xold);
    if (spd_Xdif == 1) Serial.println("B15");
    if (spd_Xdif == -1) Serial.println("B16");
  }
  
  if (QUAD_spd.position() > 1000) QUAD_spd.position(0);
  if (QUAD_spd.position() < -1000) QUAD_spd.position(0);
  spd_Xold = spd_X;
}
  
void RADIO_ROTARY() {
  radio_X = (QUAD_radio.position()/2);
  if (radio_X != radio_Xold) {
    (radio_Xdif = (radio_X - radio_Xold));
    switch (active) {
      case radio_com1: // COM1 rotary encoder output
        if (radio_Xdif == 1){if (digitalRead(rot_radio03)==1) Serial.println("A01"); else Serial.println("A03");}
        if (radio_Xdif == -1){if (digitalRead(rot_radio03)==1) Serial.println("A02"); else Serial.println("A04");}
      break;
      
      case radio_com2: // COM2 rotary encoder output
        if (radio_Xdif == 1){if (digitalRead(rot_radio03)==1) Serial.println("A07"); else Serial.println("A09");}
        if (radio_Xdif == -1){if (digitalRead(rot_radio03)==1) Serial.println("A08"); else Serial.println("A10");}
      break;
      
      case radio_nav1: // NAV1 rotary encoder output
        if (radio_Xdif == 1){if (digitalRead(rot_radio03)==1) Serial.println("A13"); else Serial.println("A15");}
        if (radio_Xdif == -1){if (digitalRead(rot_radio03)==1) Serial.println("A14"); else Serial.println("A16");}
      break;
      
      case radio_nav2: // NAV2 rotary encoder output
        if (radio_Xdif == 1){if (digitalRead(rot_radio03)==1) Serial.println("A19"); else Serial.println("A21");}
        if (radio_Xdif == -1){if (digitalRead(rot_radio03)==1) Serial.println("A20"); else Serial.println("A22");}
      break;
      
      case radio_adf: // ADF rotary encoder output
        if (digitalRead(rot_radio03)==1){
          if (radio_Xdif == 1) {
            mark = (mark - 1);
            active = 2;
            if (mark == 13) mark = 12;
          }
          if (radio_Xdif == -1){ 
            mark = (mark + 1);
            active = 2;
            if (mark == 13) mark = radio_com1;
          }
          if (mark > radio_com1) mark = radio_com1;
          if (mark < 10) mark = 10;
        } else {
          if (radio_Xdif == 1) {
            if (mark == 10) Serial.println("A29");
            if (mark == 11) Serial.println("A30");
            if (mark == 12) Serial.println("A31");
            if (mark == radio_com1) Serial.println("A32");
          }
          if (radio_Xdif == -1){
            if (mark == 10) Serial.println("A25");
            if (mark == 11) Serial.println("A26");
            if (mark == 12) Serial.println("A27");
            if (mark == radio_com1) Serial.println("A28");
          }
        }
        break;
      }
    if (QUAD_radio.position() > 1000) QUAD_radio.position(0);
    if (QUAD_radio.position() < -1000) QUAD_radio.position(0);
    radio_Xold = radio_X; // overwrites the old reading with the new one.
  }
}

/******************************************************************************************************************/
void PULSE_LEDs(){
  TimeNow = millis();
  if(TimeNow - TimeStart > TimeInterval) {
    TimeStart = TimeNow;
    pulseOn++;
    if (pulseOn > 1){pulseOn = 0;};
    if (AnunBx == "3"){if (pulseOn == 1){digitalWrite(led_war_flap, HIGH);}else{digitalWrite(led_war_flap, LOW);}}
    if (AnunCx == "3"){if (pulseOn == 1){digitalWrite(led_war_gear, HIGH);}else{digitalWrite(led_war_gear, LOW);}}
    if (AnunDx == "3"){if (pulseOn == 1){digitalWrite(led_war_frame, HIGH);}else{digitalWrite(led_war_frame, LOW);}}
    if (AnunEx == "3"){if (pulseOn == 1){digitalWrite(led_war_stall, HIGH);}else{digitalWrite(led_war_stall, LOW);}}
    if (AnunHx == "3"){if (pulseOn == 1){digitalWrite(led_war_fuel, HIGH);}else{digitalWrite(led_war_fuel, LOW);}}
  }
}
