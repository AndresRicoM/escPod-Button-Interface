/*
███████╗███████╗ ██████╗██████╗  ██████╗ ██████╗ 
██╔════╝██╔════╝██╔════╝██╔══██╗██╔═══██╗██╔══██╗
█████╗  ███████╗██║     ██████╔╝██║   ██║██║  ██║
██╔══╝  ╚════██║██║     ██╔═══╝ ██║   ██║██║  ██║
███████╗███████║╚██████╗██║     ╚██████╔╝██████╔╝
╚══════╝╚══════╝ ╚═════╝╚═╝      ╚═════╝ ╚═════╝ 
                                                 
██████╗ ██╗   ██╗████████╗████████╗ ██████╗ ███╗   ██╗
██╔══██╗██║   ██║╚══██╔══╝╚══██╔══╝██╔═══██╗████╗  ██║
██████╔╝██║   ██║   ██║      ██║   ██║   ██║██╔██╗ ██║
██╔══██╗██║   ██║   ██║      ██║   ██║   ██║██║╚██╗██║
██████╔╝╚██████╔╝   ██║      ██║   ╚██████╔╝██║ ╚████║
╚═════╝  ╚═════╝    ╚═╝      ╚═╝    ╚═════╝ ╚═╝  ╚═══╝
                                                   
██╗███╗   ██╗████████╗███████╗██████╗ ███████╗ █████╗  ██████╗███████╗
██║████╗  ██║╚══██╔══╝██╔════╝██╔══██╗██╔════╝██╔══██╗██╔════╝██╔════╝
██║██╔██╗ ██║   ██║   █████╗  ██████╔╝█████╗  ███████║██║     █████╗  
██║██║╚██╗██║   ██║   ██╔══╝  ██╔══██╗██╔══╝  ██╔══██║██║     ██╔══╝  
██║██║ ╚████║   ██║   ███████╗██║  ██║██║     ██║  ██║╚██████╗███████╗
╚═╝╚═╝  ╚═══╝   ╚═╝   ╚══════╝╚═╝  ╚═╝╚═╝     ╚═╝  ╚═╝ ╚═════╝╚══════╝

Button Interface V.1
Program for controlling LED interface mounted on button stand. This program is to be used on an Arduino Nano or UNO. 
Review Documentation to Understand Connections to Button Controller. 
MIT Media Lab - City Science Group
ANDRES RICO - 2018
 
 */

#include <Adafruit_NeoPixel.h> //Include Adafruit Libraries to address LED strip. 
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6 //Output pin for LED strip. Add a 330 Ohm resistor between output and fisrt LED. 

int col, i; //Numeric variables for color animation

//Declare pins that will be used to control different states of color
int greenpulsepin = 11; //Activates greenpulse() function
int buttonpin = 5; //Activates white() function
int buttonpin2 = 8; //Activates loading() function
int buttonpin3 = 9; //Exits loading() function
int emptypin = 10; //If pin reads HIGH, activates red() function


bool buttonpinstate, tempstate, greenpinstate; //Declare control booleans for knowing each pin state. 
int buttonpinstate2, buttonpinstate3;
int emptypinstate, errorpinstate;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_GRB + NEO_KHZ800); //Declare strip object with 16 LEDs

void setup() {
  Serial.begin(9600);
  pinMode(buttonpin, INPUT); //Set buttonpins as INPUTS. 
  pinMode(buttonpin2, INPUT);
  pinMode(emptypin, INPUT);
  /*pinMode(buttonpin3, INPUT);
  pinMode(emptypin, INPUT);
  pinMode(greenpulsepin, INPUT);*/
  checkpin();
  checkpin2();
  checkemptypin();
  /*checkpin3();
  checkemptypin();
  checkgreenpin();*/
  strip.begin();
  strip.show();
  delay(1000);
  Serial.println("Setup is done");
}

void loop() { 

  tempstate = false; //Temporary state for exiting greenpulse immedeately if button is pressed.
  
  /*checkemptypin(); //Uncomment this section to add red light indication when escPod is not available.m
  while (emptypinstate == true) {
    Serial.println("escPod is not available");
    red();
    checkemptypin();
    Serial.print(emptypinstate);
  }*/
  
  checkpin(); //Check fisrt pin. 
  while (buttonpinstate == true) { 
    Serial.println("Sending signal");
    white(); //Signals that button press has been received. 
    checkpin();
    Serial.print(buttonpinstate);
  }
  checkpin2();
  while (buttonpinstate2 == true) {
    loading(); //Signals that bed has been activated and that it is moving. 
    checkpin2();
    Serial.println(buttonpinstate2);
  }
  green_pulse(); //Pulses in green until button is pressed. 
}

void loading() { //Lights up white loading sequence
  
  strip.show();
  delay(50);
  
  for (int turn = 0; turn < 10; turn++) {
    for (i = 0; i < 16; i++) {
      checkpin2();
      strip.setPixelColor(i,255, 255, 255);
      strip.setPixelColor(i-1, 0, 0, 0);
      delay(100);
      strip.show();
      if (i == 15) {
        strip.setPixelColor(i,0,0,0);
        strip.show();
      }
    }
  }
}

void green_pulse() { //Green pulse sequence
    for (col = 0; col < 256; col++) {
      checkpin();
      if (buttonpinstate == true | tempstate == true) {
        tempstate = true;
        break;
        }
     for (i = 0; i < 16; i++) {
      checkpin();
      if (buttonpinstate == true | tempstate == true) {
        tempstate = true;
        break;
      }
      strip.setPixelColor(i, 0, col, 0);
      delay(1);
    }
    strip.show();
  }
    
  
  for (col = 255; col > -1; col--) {
    checkpin();
      if (buttonpinstate == true | tempstate == true) {
        tempstate = true;
        break;
      }
    for (i = 0; i < 16; i++) {
      checkpin();
      if (buttonpinstate == true | tempstate == true) {
        tempstate = true;
        break;
      }
      strip.setPixelColor(i, 0, col, 0);
      delay(1);
    }
    strip.show();
  }

}

void red() { //Solid red state to signal error or occupancy
  for (i = 0; i < 16; i++) {
    strip.setPixelColor(i, 255, 0, 0);
    delay(1);
  }
  strip.show(); 
}

void green() { //Sets all strip to solid green
  for (i = 0; i < 16; i++) {
    strip.setPixelColor(i, 0, 255, 0, 0);
    delay(1);
  }
  strip.show();
}

void white() { //Sets all strip to solid white
  for (i = 0; i < 16; i++) {
    strip.setPixelColor(i, 255, 255, 255);
    delay(1);
  }
  strip.show();
}

void checkpin() { //Assigns state to pin1
  buttonpinstate = digitalRead(buttonpin);
}

void checkpin2() { //Assigns state to pin2
  buttonpinstate2 = digitalRead(buttonpin2);
  //Serial.println(buttonpinstate2);
}

void checkpin3() { //Assigns state to pin3 
  buttonpinstate3 = digitalRead(buttonpin3);
}

void checkemptypin() { //Assigns state to emptypin
  emptypinstate = digitalRead(emptypin);
}

void checkgreenpin () { //Assigns state to green pin
  greenpinstate = digitalRead(greenpulsepin);
}

