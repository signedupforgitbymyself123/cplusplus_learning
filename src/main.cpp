#include <Arduino.h>
// #include <esp32-hal-rgb-led.h>
int sigmadelay = 1000;
int sigmaredvalue = 10;
// put function declarations here:
int RGB_PIN = 48;
int STRIP_RGB_PIN = 15;
uint8_t green_v = 20;
int LED_COUNT = 12;
#define NEO_RMT_CHANNEL 2 

#include <Adafruit_NeoPixel.h>


Adafruit_NeoPixel pixels(LED_COUNT, STRIP_RGB_PIN, NEO_GRB );

int currentPixel = 0;

void setup()
{
  //rmt_driver_uninstall(RMT_CHANNEL_0);
  pixels.begin();
  pixels.clear();
  pixels.show();
}

void singlePixelCircle(uint32_t color, int delayTime)
{
 

  
  pixels.setPixelColor(currentPixel, color); // Light up the current pixel
  pixels.setPixelColor(currentPixel -1, color); // Light up the current pixel
  pixels.show();                             // Display the changes

  currentPixel++; // Move to the next pixel
  if (currentPixel >= LED_COUNT)
  {
    currentPixel = 0; // Loop back to the start
  }

}

int pixelPrevious = 0;
int patternInterval = 100;
int pixelInterval = 50;
int patternPrevious = 0;
uint32_t black_px = Adafruit_NeoPixel::Color(0, 0, 0);

void loop()
{
  unsigned long currentMillis = millis(); //  Update current time
  if ((currentMillis - patternPrevious) >= patternInterval)
  { //  Check for expired time

    patternPrevious = currentMillis;
    //  Advance to next pattern
    
  }

  if (currentMillis - pixelPrevious >= pixelInterval)
  {                                //  Check for expired time
    pixelPrevious = currentMillis; //  Run current frame

    // theaterChase(pixels.Color(127, 0, 0), 50); // Red

    // colorWipe(pixels.Color(25, 0, 0), 50); // Red
    // rainbow(10);
    //  colorWipe(pixels.Color(25, 0, 0), 50); // Red

    singlePixelCircle(pixels.Color(100, 10, 0), 100); 
     singlePixelCircle(black_px, 100); // Red light moving in a circle
  }
}
