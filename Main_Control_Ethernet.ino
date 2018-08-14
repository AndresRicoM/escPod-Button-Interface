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
Program for main operation board (Arduino Leonardo) for the Button Interface. 
This board is in charge of communication with OCA, with the LED controller and with the Mother Board inside the escPod.
IMPORTANT: OCA system must be working for the controller to be able to operate!
MIT Media Lab - City Science Group
ANDRES RICO - 2018
 
 */

#include <SPI.h>   
#include <Wire.h> //Include libraries for communication
#include <Ethernet2.h> //Include libraries for Ethernet connectivity

//Ethernet communication variables
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x12, 0x33 };
IPAddress ip(10,0,4,244); //Assigned IP by OCA

unsigned int PORT = 8888; //Communication port with Mother Board
unsigned int OCAPort = 5023; //Communication port with OCA

char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //Receiving buffer
char  ReplyBuffer[] = "RECIEVED";       // a string to send back

EthernetUDP Udp; //Declare UDP objects. One for escPod and one for OCA
EthernetUDP UdpOCA;

#define SS     10U    //D10 ---- SS 
#define RST    11U    //D11 -----Reset

String input;

////////////////

int buttonpin = 2; //Button Control Variables
bool buttonstate;

int buttonlight = 4;//Visual Feedback Variables
int greenpulse = 13;
int white = 5;
int loading = 8;
int readytogo = 9;
int emptypin = 12;

bool up;
bool tempup;

void setup() {
  
  Serial.begin(115200);
  while (!Serial) { //Wait for serial to be available
    ;
  }

  //Init Ethernet ------- Do not change this or else the ethernet module will not come online.
  pinMode(SS, OUTPUT);
  pinMode(RST, OUTPUT);
  digitalWrite(SS, LOW);
  digitalWrite(RST, HIGH); //Reset
  delay(300);
  digitalWrite(RST, LOW);
  delay(300);
  digitalWrite(RST, HIGH);
  delay(300);              //Wait

  //Start UDP and ethernet connections
  Ethernet.begin(mac, ip);
  Udp.begin(PORT);
  delay(100);
  UdpOCA.begin(OCAPort);
  delay(100);
  
  pinMode(buttonpin, INPUT);//Button Setup

  pinMode(buttonlight, OUTPUT);//Light Control Pins Setup
  digitalWrite(buttonlight, LOW);
  pinMode(white, OUTPUT);
  digitalWrite(white, LOW);
  pinMode(loading, OUTPUT);
  digitalWrite(loading, LOW);
  pinMode(readytogo, OUTPUT);
  digitalWrite(readytogo, LOW);
  pinMode(emptypin, OUTPUT);
  digitalWrite(emptypin, LOW);


  up = true;
  tempup = true;

  delay(100);
  
  Serial.println("Ready to Operate");

  
}

void loop() {

  readUDPpacket(); //Read OCA's UDP packet to verify that communication is working
  
  /*while (input != "empty") { //Uncomment this to add red LED control when communication with OCA is not working
    digitalWrite(emptypin, HIGH);
    digitalWrite(buttonlight, LOW);
    readUDPpacket();
    Serial.println("Button Not Available");
  }*/
  digitalWrite(emptypin, LOW);
  digitalWrite(buttonlight, HIGH);

  get_button_state();

  Serial.println(buttonstate);
  up = tempup;//Control variables for knowing if bed is up or down. The program should be atrted with the bed UP. 
  
  if (buttonstate == true) { //Enter when button has been pressed.
    digitalWrite(white, HIGH); //Activate pin that tells LEDS to set to white. 
    //Serial.println("Button has been pressed");
    readUDPpacket(); //Read UDP communication with OCA to see if the escPod is empty
    if (input == "empty") { 
      if (up == true) {
        Serial.println("Sending signal to go down...");
        senddown(); //Send signal to mother board to go down.
        digitalWrite(white, LOW);
        digitalWrite(loading, HIGH); //Activates loading sequence on LEDs. 
        delay(42000);
        digitalWrite(readytogo, HIGH);
        digitalWrite(loading, LOW);
        tempup = false;
      }
      if (up == false) {
        Serial.println("Sending signal to go up...");
        sendup(); //Send signal to mother board to go up. 
        digitalWrite(white, LOW);
        digitalWrite(loading, HIGH); //Activates loading sequence on LEDs. 
        delay(42000);
        digitalWrite(readytogo, HIGH);
        digitalWrite(loading, LOW);
        tempup = true;
      }
      
    }
    delay(1000);
    digitalWrite(white, LOW);
    digitalWrite(readytogo, LOW);

  }
}

void readUDPpacket() { //Function for reading UDP packets from OCA. OCA will send the escPods current state derived from the thermal cameras. 
  Serial.println("Started reading");
  int packetSize = UdpOCA.parsePacket();
  
  if (packetSize) {
    IPAddress remote = UdpOCA.remoteIP();

    UdpOCA.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    //Serial.println("Receiving");
    String inputin = String(packetBuffer);
    Serial.println(inputin);
    input = inputin.substring(0,5);
  }
  delay(50);
}


void senddown() { //Function for sending a UDP packet to the Motherboard for it to go down.
  IPAddress ipUDP(10, 0, 4, 242); //Motherboards address.
    Udp.beginPacket(ipUDP, 8888);
    Udp.write("Insert Message to go Down Here"); //Contact motherboard supervisor to obtain required message. 
    Udp.endPacket();
    delay(1000);
    Serial.println("Message has been sent to Mainboard");
}

void sendup() {
  IPAddress ipUDP(10, 0, 4, 242);
    Udp.beginPacket(ipUDP, 8888);
    Udp.write("Insert Message to go Up Here"); //Contact motherboard supervisor to obtain required message.
    Udp.endPacket();
    delay(1000);
}

void get_button_state() { //Gets the state of the button. 
  buttonstate = digitalRead(buttonpin);
}


