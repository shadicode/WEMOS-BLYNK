#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <WidgetRTC.h>
#include <SimpleTimer.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <BH1750.h>

// Music notes 
const int c = 261;
const int d = 294;
const int e = 329;
const int f = 349;
const int g = 391;
const int gS = 415;
const int a = 440;
const int aS = 455;
const int b = 466;
const int cH = 523;
const int cSH = 554;
const int dH = 587;
const int dSH = 622;
const int eH = 659;
const int fH = 698;
const int fSH = 740;
const int gH = 784;
const int gSH = 830;
const int aH = 880;

int counter = 0;
const int ledPin1 = 2;


char auth[] = "............................";

BH1750 lightMeter;
SimpleTimer timer;
WidgetTerminal terminal(V5);
WidgetRTC rtc;


// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "xxxx";
char pass[] = "xxxxxxxxxxx";

const int sclPin = D1;
const int sdaPin = D2;

#define PIN 12
#define NUMPIXELS      4
int delayval = 500; // delay for half a second

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16


#if (SSD1306_LCDHEIGHT != 48)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


// Select your pin with physical button
#define btnPin 13
#define buzPin 14

// V3 LED Widget represents the physical button state
boolean btnState = false;


String x = {};

void setup()   {



  Serial.begin(9600);
  Wire.begin(sdaPin, sclPin);

  pinMode(btnPin, INPUT);
  pinMode(buzPin, OUTPUT);
  pinMode(ledPin1, OUTPUT);

  lightMeter.begin();
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  // init done

  Blynk.begin(auth, ssid, pass);
  //lcd.clear(); //Use it to clear the LCD Widget

  // Clear the buffer.
  display.clearDisplay();
  rtc.begin();
  //timer.setInterval(100L, buttonLedWidget);
  pixels.begin(); // This initializes the NeoPixel library.

  timer.setInterval(1000L, clockDisplay );
}


BLYNK_WRITE(V5)
{

  String c = param.asStr();

  if (c == "wake up!")
  {
    //display.println(param.asStr()) ;
    Serial.println (param.asStr());
    while (digitalRead(btnPin) != HIGH)
    {
      // terminal.write(param.getBuffer(), param.getLength());
      tone(buzPin, 1000);
      delay(100);
      noTone(buzPin);
    }
  }
  digitalWrite(buzPin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);                       // wait for a second
  digitalWrite(buzPin, LOW);    // turn the LED off by making the voltage LOW
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  display.println(param.asStr()) ;
  Serial.println (param.asStr());
  display.display();

  delay(2000);

  display.clearDisplay();
  digitalWrite(buzPin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);                       // wait for a second
  digitalWrite(buzPin, LOW);    // turn the LED off by making the voltage LOW

}

//BLYNK_WRITE(V0)
//{
//  Serial.println("WebHook data:");
//  Serial.println(param.asStr());
//}


BLYNK_WRITE(V3)
{
  String action = param.asStr();

  if (action == "play" ) {
    testdrawroundrect();


    for (int i = 0; i < NUMPIXELS; i++) {

      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      pixels.setPixelColor(i, pixels.Color(0, 100, 0)); // Moderately bright green color.

      pixels.show(); // This sends the updated pixel color to the hardware.

      delay(delayval); // Delay for a period of time (in milliseconds).

    }

    firstSection();

  }
  if (action == "stop") {
    // Do something
  }
  if (action == "next") {
    for (int i = 0; i < NUMPIXELS; i++) {

      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      pixels.setPixelColor(i, pixels.Color(0, 0, 100)); // Moderately bright green color.

      pixels.show(); // This sends the updated pixel color to the hardware.

      delay(delayval); // Delay for a period of time (in milliseconds).

    }
    secondSection();

  }
  if (action == "prev") {
    for (int i = 0; i < NUMPIXELS; i++) {

      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      pixels.setPixelColor(i, pixels.Color(100, 0, 0)); // Moderately bright green color.

      pixels.show(); // This sends the updated pixel color to the hardware.

      delay(delayval); // Delay for a period of time (in milliseconds).

    }
  }

  Blynk.setProperty(V3, "label", action);

}

void loop() {




  Blynk.run();

  timer.run(); // Initiates SimpleTimer

  if (Serial.available())
  {
    x = Serial.readString();        // Read message from the serial terminal
    terminal.println (x);           // Send message to Blynk virtual terminal
    terminal.flush();

    Serial.println (x);             //Echo the message in the serial terminal

    //This section of code for displaying the message on the OLED
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println(x);
    display.display();
    delay(2000);
    display.clearDisplay();
  }
}



// Function for display the internet time/Blynk time and the LUX value of the light sensor on the OLED and
void clockDisplay()
{
  uint16_t lux = lightMeter.readLightLevel();

  // You can call hour(), minute(), ... at any time
  // Please see Time library examples for details

  String currentTime = String(hour()) + ":" + minute() + ":" + second();
  String currentDate = String(day()) + " " + month() + " " + year();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  // display.display();

  display.println( currentTime);
  display.print( currentDate);
  display.println("");

  display.print("LUX:");
  display.print(lux);
  display.println("");

  display.println("");
  display.display();
  //delay(10);
  display.clearDisplay();

  if (digitalRead(btnPin) == HIGH)
  {

    for (int i = 0; i < NUMPIXELS; i++) {

      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      pixels.setPixelColor(i, pixels.Color(100, 100, 100)); // Moderately bright green color.

      pixels.show(); // This sends the updated pixel color to the hardware.

      delay(delayval); // Delay for a period of time (in milliseconds).

    }
    terminal.println ("Message Acknowledged!");
    //clockDisplay();
    //Blynk.syncVirtual(V5);
    terminal.flush();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("OK!");
    display.display();
    delay(1000 );

    display.clearDisplay();
  }
}



// A function for creating a beeb sound using the buzer
void beep(int note, int duration)
{
  //Play tone on buzPin
  tone(buzPin, note, duration);

  //Play different LED depending on value of 'counter'
  if (counter % 2 == 0)
  {
    digitalWrite(ledPin1, HIGH);
    delay(duration);
    digitalWrite(ledPin1, LOW);
  } else
  {
    digitalWrite(ledPin1, HIGH);
    delay(duration);
    digitalWrite(ledPin1, LOW);
  }

  //Stop tone on buzPin
  noTone(buzPin);

  delay(50);

  //Increment counter
  counter++;
}


void testdrawroundrect(void) {

  display.drawRoundRect(2 0, 20, display.width() - 2 * 20, display.height() - 2 * 20, display.height() / 4, WHITE);
  display.display();
  delay(1);

}


void firstSection()
{
  beep(a, 500);
  beep(a, 500);
  beep(a, 500);
  beep(f, 350);
  beep(cH, 150);
  beep(a, 500);
  beep(f, 350);
  beep(cH, 150);
  beep(a, 650);

  delay(500);

  beep(eH, 500);
  beep(eH, 500);
  beep(eH, 500);
  beep(fH, 350);
  beep(cH, 150);
  beep(gS, 500);
  beep(f, 350);
  beep(cH, 150);
  beep(a, 650);

  delay(500);
}

void secondSection()
{
  beep(aH, 500);
  beep(a, 300);
  beep(a, 150);
  beep(aH, 500);
  beep(gSH, 325);
  beep(gH, 175);
  beep(fSH, 125);
  beep(fH, 125);
  beep(fSH, 250);

  delay(325);

  beep(aS, 250);
  beep(dSH, 500);
  beep(dH, 325);
  beep(cSH, 175);
  beep(cH, 125);
  beep(b, 125);
  beep(cH, 250);

  delay(350);
}

