/***************************************************
  This is a library for the Adafruit 1.8" SPI display.
  
This library works with the Adafruit 1.8" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/358
The 1.8" TFT shield
  ----> https://www.adafruit.com/product/802
The 1.44" TFT breakout
  ----> https://www.adafruit.com/product/2088
as well as Adafruit raw 1.8" TFT display
  ----> http://www.adafruit.com/products/618

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limo r Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
 
//TFT & SD LIBRARY
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>

//******************************

//included libraries
#include <OneWire.h>
#include <DallasTemperature.h>

//******************************
//temperature sensor declarations & variables
#define tempSensor 2

OneWire oneWire(tempSensor);
DallasTemperature sensors(&oneWire);

float celsius = 0;
float fahrenheit = 0;
float previousFahrenheit = 0;

//******************************

// TFT display and SD card will share the hardware SPI interface.
// Hardware SPI pins are specific to the Arduino board type and
// cannot be remapped to alternate pins.  For Arduino Uno,
// Duemilanove, etc., pin 11 = MOSI, pin 12 = MISO, pin 13 = SCK.
#define TFT_CS  10  // Chip select line for TFT display
#define TFT_RST  9  // Reset line for TFT (or see below...)
#define TFT_DC   8  // Data/command line for TFT

#define SD_CS    5  // Chip select line for SD card

//Use this reset pin for the shield!
//#define TFT_RST  -1  // you can also connect this to the Arduino reset!

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

//#define TFT_SCLK 13  
//#define TFT_MOSI 11   


float p = 3.1415926;

// Timer variables
unsigned long currentMillis = 0;
unsigned long previousMillis = 0; //will store last time
const long interval = 1000; //interval of 1 second
int currentSeconds = 0;
int currentMinutes = 0;

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;
const int chipSelect = 10;
File myFile;

//**********************************************************************
void setup(void) {
  sensors.begin();
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

//  Serial.print("Initializing data SD card...");
//  if (!SD.begin(4)) {
//    Serial.println("failed");
//    return;
//  }
//  Serial.println("DATA OK!");
  

  // Use this initializer (uncomment) if you're using a 1.44" TFT
  tft.initR(INITR_144GREENTAB);   // initialize a ST7735S chip, black tab

  tft.setRotation(1);

  Serial.print("Initializing picture SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("failed");
    return;
  } 
  Serial.println("PICTURES OK!");



//  Serial.print("\nInitializing data SD card...");
//  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
//    Serial.println("initialization failed. Things to check:");
//    Serial.println("* is a card inserted?");
//    Serial.println("* is your wiring correct?");
//    Serial.println("* did you change the chipSelect pin to match your shield or module?");
//    return;
//  } else {
//    Serial.println("Wiring is correct and a card is present.");
//  }
//  
//  myFile = SD.open("test.txt", FILE_WRITE);
//
//  // if the file opened okay, write to it:
//  if (myFile) {
//    Serial.print("Writing to test.txt...");
//    myFile.println("testing 1, 2, 3.");
//    // close the file:
//    myFile.close();
//    Serial.println("done.");
//  } else {
//    // if the file didn't open, print an error:
//    Serial.println("error opening test.txt");
//  }

  uint16_t time = millis();
  tft.fillScreen(ST7735_BLACK);
  time = millis() - time;
  
  Serial.println(time, DEC);
  delay(500);

  // tft print function!
  tftPrintTest();
  delay(2000);

  // a single pixel
  tft.drawPixel(tft.width()/2, tft.height()/2, ST7735_GREEN);
  delay(500);

  Serial.println("done");
  delay(1000);

}

//*****************************************************************
void loop() {

  //timer runs automatically on startup
//    currentMillis = millis();
//    if (currentMillis - previousMillis >= interval) {
//      currentSeconds = currentSeconds + 1;
//      previousMillis = currentMillis;
//    }
//    if (currentSeconds >= 60) {
//      currentMinutes++;
//      currentSeconds = 0;
//    }
  currentSeconds = millis()/1000;
  if (currentSeconds >= 60) {
      currentMinutes++;
      currentSeconds = 0;
  }

  currentSeconds = millis()/1000;
  if (currentSeconds >= 60) {
      currentMinutes++;
      currentSeconds = 0;
  }

  readTempSensor();
  
  // large block of text
  tft.fillScreen(ST7735_BLACK);
  tft.setTextSize(2);
  testdrawtext("Temp.",  ST7735_WHITE);
  tft.setTextSize(5);
  tft.setCursor(20,40);
  tft.println(fahrenheit,0);
  //tft.print(char(248));
  tft.setTextSize(2);
  tft.print("Fahrenheit");
  delay(1000);

  // large block of text
  tft.fillScreen(ST7735_BLACK);
  tft.setTextSize(2);
  testdrawtext("VOLUME",  ST7735_WHITE);
  tft.setTextSize(5);
  tft.setCursor(20,40);
  tft.println(currentMinutes);
  tft.setTextSize(2);
  tft.print("Gallons");
  delay(1000);

  // large block of text
  tft.fillScreen(ST7735_BLACK);
  tft.setTextSize(2);
  testdrawtext("DURATION",  ST7735_WHITE);
  tft.setTextSize(5);
  tft.setCursor(20,40);
  tft.println(currentSeconds);
  tft.setTextSize(2);
  tft.print("seconds");
  delay(1000); 

  // change the name here!
  bmpDraw("lily128.bmp", 0, 0);
  delay(2000);
  
}

//**********************************************************************

//SUBROUTINES

//**********************************************************************
void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

//**********************************************************************
void tftPrintTest() {
  tft.setTextWrap(false);
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(0, 30);
  tft.setTextSize(2);
  tft.setTextColor(ST7735_BLUE);
  tft.print("ezDroplet");
  delay(1500);
  tft.setCursor(0, 0);
  tft.setTextSize(1);
  tft.setTextColor(ST7735_WHITE);
  tft.println("Sketch has been");
  tft.println("running for: ");
  tft.setTextColor(ST7735_MAGENTA);
  tft.print(millis() / 1000);
  tft.setTextColor(ST7735_WHITE);
  tft.print(" seconds.");
}

//**********************************************************************
void readTempSensor() 
{
  sensors.requestTemperatures();
  celsius = sensors.getTempCByIndex(0);
  fahrenheit = sensors.toFahrenheit(celsius);
}

//**********************************************************************

#define BUFFPIXEL 20

void bmpDraw(char *filename, uint8_t x, uint8_t y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();

  if((x >= tft.width()) || (y >= tft.height())) return;

  Serial.println();
  Serial.print("Loading image '");
  Serial.print(filename);
  Serial.println('\'');

  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.print("File not found");
    return;
  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.print("File size: "); Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print("Image Offset: "); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print("Header size: "); Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print("Bit Depth: "); Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print("Image size: ");
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x+w-1, y+h-1);

        for (row=0; row<h; row++) { // For each scanline...

          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<w; col++) { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format, push to display
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            tft.pushColor(tft.Color565(r,g,b));
          } // end pixel
        } // end scanline
        Serial.print("Loaded in ");
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) Serial.println("BMP format not recognized.");
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}
