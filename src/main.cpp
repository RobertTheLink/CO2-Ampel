/*
  Reading CO2, humidity and temperature from the SCD30
*/

#include <Arduino.h>
#include <Wire.h>
#include <SparkFun_SCD30_Arduino_Library.h>
//#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>

// Neopixel
#define PIXEL_PIN    6  // Digital IO pin connected to the NeoPixels.
#define PIXEL_COUNT  8  // Number of NeoPixels
// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

void colorWipe(uint32_t color, int wait) {
  for(uint8_t i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

// CO2 Sensor
SCD30 airSensor;

// Display
//Adafruit_SSD1306 display= Adafruit_SSD1306();

/*****************************************************************************/

void setup()
{
  //Save Power by writing all Digital IO LOW - 
  //note that pins just need to be tied one way or another, do not damage devices!
  for (int i = 0; i < 20; i++) {
    if((i != 18) && (i !=19) )                  // skip I2C Pins 
    pinMode(i, OUTPUT);
  } 

  // Neopixel
  strip.begin(); // Initialize NeoPixel strip object (REQUIRED)
  strip.setBrightness(50);
  strip.show();  // Initialize all pixels to 'off'


  Serial.begin(9600);
  Serial.println("Simple CO2 Monitor");

  Wire.begin();

/*
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  //display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)

  display.display();
  delay(100);
  display.clearDisplay();
  display.display();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
*/

  // CO2 Sensor 
  if (airSensor.begin() == false)
  {
    Serial.println("Air sensor not detected. Please check wiring. Freezing...");
    while (1)
      ;

  }
  airSensor.setMeasurementInterval(2); //Change number of seconds between measurements: 2 to 1800 (30 minutes)
  airSensor.setAltitudeCompensation(75); //Set altitude of the sensor in m, Ratingen => 75m
  airSensor.setAmbientPressure(950); //Current ambient pressure in mBar: 700 to 1200

  float offset = airSensor.getTemperatureOffset();
  Serial.print("Current temp offset: ");
  Serial.print(offset, 2);
  Serial.println("C");
}

/*****************************************************************************/

void loop()
{
  uint16_t co2       = 0;
  
  float  temperature = 0;
  float  humidity    = 0;

/*
  display.setCursor(0,0);
  display.clearDisplay();
*/
  if (airSensor.dataAvailable())
  {
    co2         = airSensor.getCO2();
    temperature = airSensor.getTemperature();
    humidity    = airSensor.getHumidity();
/*
    Serial.print("co2(ppm):");
    Serial.print(co2);

    Serial.print(" temp(C):");
    Serial.print(temperature, 1);

    Serial.print(" humidity(%):");
    Serial.print(humidity, 1);

    Serial.println();
*/
/*
   Target values for CO2 in rooms, see EN 13779
      400-800ppm    : good
      800-1000ppm   : medium 
      1000-1400ppm  : limited 
      >1400ppm      : unhealthy
*/  

    if(co2  < 800){
      //display.println("Luft gut!");
      colorWipe(strip.Color(  0, 255, 0), 10);    // Dark Green
    }
    else if(co2 < 1000){
      //display.println("mittel");
      colorWipe(strip.Color(  106, 255,  30), 10);  //  Light Green
    }
    else if(co2 < 1400){
      //display.println("noch ok");
      colorWipe(strip.Color(  255, 255,   0), 10);  // Yellow
    }
    else {
      //display.println("l\201ften!");
      colorWipe(strip.Color(255,   0,   0), 10); // Red
    }

    
//    display.print(F("Temperatur : ")); display.print(temperature,1); display.println(" *C");
//    display.print(F("Humidity   : ")); display.print(humidity,0);   display.println("   %rH");
    //display.print(F("CO2 ")); display.print(co2 ); display.println("ppm");
    //display.display();

  }

  delay(5000);
}