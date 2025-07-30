#include <Arduino.h>
//#include <esp32-hal-rgb-led.h>
int sigmadelay=1000;
int sigmaredvalue=10;
// put function declarations here:
int RGB_PIN = 48;
int STRIP_RGB_PIN = 15;
uint8_t green_v = 20;
int LED_COUNT = 12;
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel strip(LED_COUNT, STRIP_RGB_PIN, NEO_RGB);
#include <LittleFS.h>

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>


#include "C:/Users/alex/Documents/PlatformIO/config2.h"


static AsyncWebServer server(80);

unsigned long pixelPrevious = 0;        // Previous Pixel Millis
unsigned long patternPrevious = 0;      // Previous Pattern Millis
int           patternCurrent = 0;       // Current Pattern Number
int           patternInterval = 5000;   // Pattern Interval (ms)
bool          patternComplete = false;

int           pixelInterval = 50;       // Pixel Interval (ms)
int           pixelQueue = 0;           // Pattern Pixel Queue
int           pixelCycle = 0;           // Pattern Pixel Cycle



uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void colorWipe(uint32_t color, int wait) {
  static uint16_t current_pixel = 0;
  pixelInterval = wait;                        //  Update delay time
  strip.setPixelColor(current_pixel++, color); //  Set pixel's color (in RAM)
  strip.show();                                //  Update strip to match
  if(current_pixel >= LED_COUNT) {           //  Loop the pattern from the first LED
    current_pixel = 0;
    patternComplete = true;
  }
} 

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.

void theaterChase(uint32_t color, int wait) {
  static uint32_t loop_count = 0;
  static uint16_t current_pixel = 0;

  pixelInterval = wait;                   //  Update delay time

  strip.clear();

  for(int c=current_pixel; c < LED_COUNT; c += 3) {
    strip.setPixelColor(c, color);
  }
  strip.show();

  current_pixel++;
  if (current_pixel >= 3) {
    current_pixel = 0;
    loop_count++;
  }

  if (loop_count >= 10) {
    current_pixel = 0;
    loop_count = 0;
    patternComplete = true;
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(uint8_t wait) {
  if(pixelInterval != wait)
    pixelInterval = wait;                   
  for(uint16_t i=0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, Wheel((i + pixelCycle) & 255)); //  Update delay time  
  }
  strip.show();                             //  Update strip to match
  pixelCycle++;                             //  Advance current cycle
  if(pixelCycle >= 256)
    pixelCycle = 0;                         //  Loop the cycle back to the begining
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  if(pixelInterval != wait)
    pixelInterval = wait;                   //  Update delay time  
  for(int i=0; i < LED_COUNT; i+=3) {
    strip.setPixelColor(i + pixelQueue, Wheel((i + pixelCycle) % 255)); //  Update delay time  
  }
  strip.show();
  for(int i=0; i < LED_COUNT; i+=3) {
    strip.setPixelColor(i + pixelQueue, strip.Color(0, 0, 0)); //  Update delay time  
  }      
  pixelQueue++;                           //  Advance current queue  
  pixelCycle++;                           //  Advance current cycle
  if(pixelQueue >= 3)
    pixelQueue = 0;                       //  Loop
  if(pixelCycle >= 256)
    pixelCycle = 0;                       //  Loop
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); // Start serial communication
  //neopixelWrite(STRIP_RGB_PIN,50,50,0);
 

  Serial.println(" I have started up");

  WiFi.mode(WIFI_STA);
  //WiFi.begin("axmGuest","alexanderjesseschoch");
  WiFi.begin(ssid,password);
  WiFi.setAutoReconnect(true);

  while (WiFi.status() != WL_CONNECTED){
    Serial.println("Connecting...");
    delay(1000);
  }
  

  //IPAddress foo = WiFi.localIP();
  //Serial.println(foo);

  Serial.println( WiFi.localIP() );
  

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {

    // request.hasParam("who)")
    // request->
    if (request->hasParam("who")) {
      Serial.printf("Who? %s\n", request->getParam("who")->value().c_str());
      String s = String("you sent me the who param, it was: " +  String(request->getParam("who")->value().c_str()));
      request->send(200,"text/html", s.c_str());
    }

    //String x = String("Hello world");
    //  "h", "e", "l"
    else{
      const char* x = "Hello World";
      request->send(200, "text/html", (uint8_t *)x, strlen(x) );
    }
  });


  server.on("/dork", HTTP_GET, [](AsyncWebServerRequest *request) {

    if (request->hasParam("who")) {
      Serial.printf("Who? %s\n", request->getParam("who")->value().c_str());
    }
    else{
      Serial.println("no who param found");
    }

    //String x = String("Hello world");
    const char* x = "Alex is a Big dork";
    request->send(200, "text/html", (uint8_t *)x, strlen(x) );
  });

  server.on("/dink", HTTP_GET, [](AsyncWebServerRequest *request) {
    strip.clear(); // Set all pixel colors to 'off'
    int my_delay = 200;

    // The first NeoPixel in a strand is #0, second is 1, all the way up
    // to the count of pixels minus one.
    for(int i=0; i<LED_COUNT; i++) { // For each pixel...

      // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
      // Here we're using a moderately bright green color:
      strip.setPixelColor(i, strip.Color(0, 150, 0));

      strip.show();   // Send the updated pixel colors to the hardware.

      delay(my_delay); // Pause before next pass through loop
    }
    const char* x = "exciting neopixel examples are fun";
    request->send(200, "text/html", (uint8_t *)x, strlen(x) );
  });

  server.on("/light", HTTP_GET, [](AsyncWebServerRequest *request) {
    //Serial.println(request->getHeaders)

    if (request->hasParam("r") && request->hasParam("g") && request->hasParam("b")) {
      int r = request->getParam("r")->value().toInt();
      int g = request->getParam("g")->value().toInt();
      int b = request->getParam("b")->value().toInt();
      neopixelWrite(RGB_PIN,r,g,b);

      const char* x = "set the light";
      request->send(200, "text/html", (uint8_t *)x, strlen(x) );


    }
    else{
      Serial.println("no param found");
      const char* x = "bad bad bad";
      Serial.println(request->url());
      request->send(200, "text/html", (uint8_t *)x, strlen(x) );
    }

    //String x = String("Hello world");
    
  });

  server.begin();

   strip.clear();
  strip.begin();
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

}
float arg2=5.5;
int arg1=10;
int test_counter = 0;
void loop2(){
  unsigned long currentMillis = millis(); 
  test_counter = test_counter + 1;                    //  Update current time
  if(  (currentMillis - patternPrevious) >= patternInterval) {  //  Check for expired tim
    
  
    //test_counter++;
    if( arg1<test_counter){
      //Serial.print("bob");
    }
    if(arg2<test_counter){
      //colorWipe(strip.Color(0, 255, 0), 500);
      //theaterChaseRainbow(50); 
      theaterChase(strip.Color(0, 0, 127), 50); // Blue
    }
    else{
      colorWipe(strip.Color(0, 0, 0), 50);
    }
  }
}
void loop3() {
  // put your main code here, to run repeatedly:
 
  /*
  neopixelWrite(RGB_PIN,sigmaredvalue,green_v,0);
  Serial.printf("waiting %i  red is: %i\n",sigmadelay,sigmaredvalue);
  delay(sigmadelay);
  for (uint8_t i = 1; i < 6; i++) { // Loop runs 5 times
        Serial.print("Iteration: "); 
        Serial.println(i); // Print the loop index
        delay(1000); // Wait for 1 second
        sigmaredvalue = (uint8_t) sigmaredvalue *i;
        neopixelWrite(RGB_PIN,sigmaredvalue,green_v,0);
        Serial.println(WiFi.status());
    }
  */


  unsigned long currentMillis = millis();                     //  Update current time
  if( patternComplete || (currentMillis - patternPrevious) >= patternInterval) {  //  Check for expired time
    patternComplete = false;
    patternPrevious = currentMillis;
    patternCurrent++;                                         //  Advance to next pattern
    if(patternCurrent >= 7)
      patternCurrent = 0;
  }

  if(currentMillis - pixelPrevious >= pixelInterval) {        //  Check for expired time
    pixelPrevious = currentMillis;                            //  Run current frame
    switch (patternCurrent) {
      case 7:
        theaterChaseRainbow(50); // Rainbow-enhanced theaterChase variant
        break;
      case 6:
        rainbow(10); // Flowing rainbow cycle along the whole strip
        break;     
      case 5:
        theaterChase(strip.Color(0, 0, 127), 50); // Blue
        break;
      case 4:
        theaterChase(strip.Color(127, 0, 0), 50); // Red
        break;
      case 3:
        theaterChase(strip.Color(127, 127, 127), 50); // White
        break;
      case 2:
        colorWipe(strip.Color(0, 0, 255), 50); // Blue
        break;
      case 1:
        colorWipe(strip.Color(0, 255, 0), 50); // Green
        break;        
      default:
        colorWipe(strip.Color(25, 0, 0), 50); // Red
        break;
    }
  }
}

void loop(){
  unsigned long currentMillis = millis();                     //  Update current time
  if( patternComplete || (currentMillis - patternPrevious) >= patternInterval) {  //  Check for expired time
    patternComplete = false;
    patternPrevious = currentMillis;
    patternCurrent++;                                         //  Advance to next pattern
    if(patternCurrent >= 7)
      patternCurrent = 0;
  }

  if(currentMillis - pixelPrevious >= pixelInterval) {        //  Check for expired time
    pixelPrevious = currentMillis;                            //  Run current frame
  
    colorWipe(strip.Color(25, 0, 0), 50); // Red
       
  }
  
  }

