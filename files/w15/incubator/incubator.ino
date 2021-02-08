// DIY Incubator for HTGAA'15
// by FRANCISCO SANCHEZ ARROYO
// THE BEACH LAB SITGES
// January 2016
// MIT LICENSE

#include <SoftwareSerial.h>     // for LCD serial adapter
#include <Adafruit_NeoPixel.h>  // for the neopixel strip
#ifdef __AVR__
#include <avr/power.h>
#endif

#define txPin 1  // for the LCD serial adapter
#define targetTemp 37  // This is the target temperature

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN  2


// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      12

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int SHT_clockPin = 9;  // pin used for clock
int SHT_dataPin  = 8;  // pin used for data


SoftwareSerial LCD = SoftwareSerial(0, txPin);
// since the LCD does not send data back to the Arduino, we should only define the txPin
const int LCDdelay = 10; // conservative, 2 actually works

// wbp: goto with row & column
void lcdPosition(int row, int col) {
  LCD.write(0xFE);   //command flag
  LCD.write((col + row * 64 + 128));  //position
  delay(LCDdelay);
}
void clearLCD() {
  LCD.write(0xFE);   //command flag
  LCD.write(0x01);   //clear command.
  delay(LCDdelay);
}
void backlightOn() {  //turns on the backlight
  LCD.write(0x7C);   //command flag for backlight stuff
  LCD.write(157);    //light level.
  delay(LCDdelay);
}
void backlightOff() { //turns off the backlight
  LCD.write(0x7C);   //command flag for backlight stuff
  LCD.write(128);     //light level for off.
  delay(LCDdelay);
}
void serCommand() {  //a general function to call the command flag for issuing all other commands
  LCD.write(0xFE);
}

// SETUP
void setup() {
  LCD.begin(9600);
  pinMode(txPin, OUTPUT); // LCD TX
  pinMode(11, OUTPUT); // FAN RELAY
  pinMode(12, OUTPUT); // HEATER RELAY
  pinMode(7, INPUT);  // LID SWITCH
  digitalWrite(7, HIGH); // ACTIVATE PULL UP RESISTOR
  digitalWrite(11, HIGH); // TURN FAN OFF
  digitalWrite(12, HIGH); // TURN HEATER OFF
  pinMode(13, OUTPUT); // We will use this pin as a read-indicator
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

// LOOP
void loop() {

  // read the temperature and humidity
  float temperature = getTemperature();  // gets the temperature
  float humidity = getHumidity();        // gets the humidity

  // print the current temperature
  lcdPosition(0, 0);
  LCD.print("Target:");
  lcdPosition(0, 7);
  LCD.print(37);
  lcdPosition(1, 0);
  LCD.print("T:");
  lcdPosition(1, 2);
  LCD.print(temperature);
  lcdPosition(1, 8);
  LCD.print("H:");
  lcdPosition(1, 10);
  LCD.print(humidity);

  // check if the lid is openend
  if (digitalRead(7) == HIGH)
  {
    // write OPEN and turn LED RED
    lcdPosition(0, 10);
    LCD.print("OPEN  ");
    colorWipe(strip.Color(255, 0, 0), 10); // Red
    // turn off fan and heater
    digitalWrite(11, LOW); // TURN FAN OFF
    digitalWrite(12, LOW); // TURN HEATER OFF
  }

  else  // if the lid is closed try to reach target temperature
  {
    // write closed and turn LED GREEN
    lcdPosition(0, 10);
    LCD.print("CLOSED");
    colorWipe(strip.Color(0, 255, 0), 10); // Green

    if (temperature >= targetTemp)  // If we reach target temperature
    {
      digitalWrite(12, LOW); // TURN HEATER OFF
    }
    else  // temperature is below Target Temperature
    {
      digitalWrite(12, HIGH); // TURN HEATER ON
    }
  }
}

float getTemperature() {
  //Return Temperature in Celsius
  SHT_sendCommand(B00000011, SHT_dataPin, SHT_clockPin);
  SHT_waitForResult(SHT_dataPin);

  int val = SHT_getData(SHT_dataPin, SHT_clockPin);
  SHT_skipCrc(SHT_dataPin, SHT_clockPin);
  return (float)val * 0.01 - 40; //convert to celsius
}

float getHumidity() {
  //Return  Relative Humidity
  SHT_sendCommand(B00000101, SHT_dataPin, SHT_clockPin);
  SHT_waitForResult(SHT_dataPin);
  int val = SHT_getData(SHT_dataPin, SHT_clockPin);
  SHT_skipCrc(SHT_dataPin, SHT_clockPin);
  return -4.0 + 0.0405 * val + -0.0000028 * val * val;
}


void SHT_sendCommand(int command, int dataPin, int clockPin) {
  // send a command to the SHTx sensor
  // transmission start
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  digitalWrite(dataPin, HIGH);
  digitalWrite(clockPin, HIGH);
  digitalWrite(dataPin, LOW);
  digitalWrite(clockPin, LOW);
  digitalWrite(clockPin, HIGH);
  digitalWrite(dataPin, HIGH);
  digitalWrite(clockPin, LOW);

  // shift out the command (the 3 MSB are address and must be 000, the last 5 bits are the command)
  shiftOut(dataPin, clockPin, MSBFIRST, command);

  // verify we get the right ACK
  digitalWrite(clockPin, HIGH);
  pinMode(dataPin, INPUT);

  if (digitalRead(dataPin)) Serial.println("ACK error 0");
  digitalWrite(clockPin, LOW);
  if (!digitalRead(dataPin)) Serial.println("ACK error 1");
}


void SHT_waitForResult(int dataPin) {
  // wait for the SHTx answer
  pinMode(dataPin, INPUT);

  int ack; //acknowledgement

  //need to wait up to 2 seconds for the value
  for (int i = 0; i < 1000; ++i) {
    delay(2);
    ack = digitalRead(dataPin);
    if (ack == LOW) break;
  }

  if (ack == HIGH) Serial.println("ACK error 2");
}

int SHT_getData(int dataPin, int clockPin) {
  // get data from the SHTx sensor

  // get the MSB (most significant bits)
  pinMode(dataPin, INPUT);
  pinMode(clockPin, OUTPUT);
  byte MSB = shiftIn(dataPin, clockPin, MSBFIRST);

  // send the required ACK
  pinMode(dataPin, OUTPUT);
  digitalWrite(dataPin, HIGH);
  digitalWrite(dataPin, LOW);
  digitalWrite(clockPin, HIGH);
  digitalWrite(clockPin, LOW);

  // get the LSB (less significant bits)
  pinMode(dataPin, INPUT);
  byte LSB = shiftIn(dataPin, clockPin, MSBFIRST);
  return ((MSB << 8) | LSB); //combine bits
}

void SHT_skipCrc(int dataPin, int clockPin) {
  // skip CRC data from the SHTx sensor
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  digitalWrite(dataPin, HIGH);
  digitalWrite(clockPin, HIGH);
  digitalWrite(clockPin, LOW);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}
