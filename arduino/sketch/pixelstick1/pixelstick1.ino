#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <SD.h>

//#include <Wire.h>
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>


// ========================================================
// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;
// ========================================================

// ========================================================
// Display
//Adafruit_SSD1306 display = Adafruit_SSD1306();
// ========================================================

// ========================================================
// Which pin on the Arduino is connected to the NeoPixels?
#define PIN            10
#define NUMPIXELS      144


// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.

Adafruit_NeoPixel stick = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
// ========================================================


// ========================================================
// Other...
// ========================================================
#define LEDPIN 13



int selectFile(char*);
int blinkFile(char*, int);
void clearStick();

void printDisp(char* text, int x, int y, bool);

void setup() {

  pinMode(LEDPIN, OUTPUT);
  
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.print("Initializing neopixels...");
  
  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  
  stick.begin();

  Serial.println("Display...");
  
  //display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // initialize with the I2C addr 0x3C (for the 128x32)
 // display.display();
  
//  delay(1000);
//  display.clearDisplay();
//  display.display();
//  display.setTextSize(1);
//  display.setTextColor(WHITE);
//  displayPrint("Pixelstick\nVersion 0.1", 0, 0, true);
//  delay(1000);
  
  Serial.println("initialization done.");
}

void loop() {
  Serial.println("loop");
  clearStick();

  Serial.println("File...");
  char filename[80];
//  if (selectFile(filename)) {
//    blinkFile(filename, 10);
//  }

blinkFile("first.pix", 1);  
  clearStick();
  delay(10000);
}

//  ==============================
//  Utils
//  ==============================

long fourToLong(unsigned char* b) {
  long l = b[0] << 24 | b[1] << 16 | b[2] << 8 | b[3];
  return l;
}


int selectFile(char* filename) {
  Serial.println("selectFile");
  displayPrint("Read files", 0, 0, true);
  File root = SD.open("/");
  root.rewindDirectory();

  int count = 0;
  char* filenames[80];
  
  while(true && count < 80) {
    File entry = root.openNextFile();
    if (!entry) {
      Serial.println("Ende");
      Serial.println(count, DEC);
      break;
    }
    filenames[count] = new char[strlen(entry.name() + 1)];
    strcpy(filenames[count], entry.name());
    entry.close();
    Serial.println(filenames[count]);
    count++;
  }
  root.close();

  for (int i = 0; i < count; i++) {
    displayPrint(filenames[i], 0, 0, true);
    delay(1000);
  }
  strncpy(filename, filenames[0], 79);
  for (int i = 0; i < count; i++) {
    delete [] filenames[i];
  }
  return 1;
}




//  =========================
//  Display
//  =========================
void displayPrint(char* text, int x, int y, bool cls) {
//  if (cls) {
//    display.clearDisplay();
//  }
//  display.setCursor(x, y);
//  display.println(text);
//  display.setCursor(x, y);
//  display.display();
}

//  =========================
//  Pixelstick
//  =========================
void show() {
  stick.show();
}

void updateStick(long *px, int num, bool show) {
  for (int i = 0; i < num; i++) {
    unsigned char r = (px[i] >> 24) & 0xff;
    unsigned char g = (px[i] >> 16) & 0xff;
    unsigned char b = (px[i] >> 8) & 0xff;
    stick.setPixelColor(i, stick.Color(r, g, b));
  }
  if (show) {
      stick.show();
    }
}
void clearStick() {
  for(int i = 0; i <NUMPIXELS; i++) {
    stick.setPixelColor(i, 0);
  }
  stick.show();
}

void printStick(long *px, int num) {
  char hex[8];
  for (int i = 0; i < num; i++) {
    sprintf(hex, "%02X%02X%02X ", (px[i] >> 24), (px[i] >> 16), (px[i] >> 8));
    Serial.print(hex);
    Serial.print(" ");
  }
  Serial.println();
}

int blinkFile(char* path, int sleep) {
  File pixFile = SD.open(path, FILE_READ);
  if (!pixFile) {
    Serial.println("file not found");
    return -1;
  }


  
  unsigned char header[4];
  unsigned char width[4];
  unsigned char height[4];
  unsigned char data[5];
  memset(header, 0, 4);
  memset(width, 0, 4);
  memset(height, 0, 4);
  memset(data, 0, 5);
  pixFile.read(header, 3);
  pixFile.read(width, 4);
  pixFile.read(height, 4);
  pixFile.read(data, 4);

  Serial.println((char*) header);
  Serial.println((char*) data);
  
  long w = fourToLong(width);
  long h = fourToLong(height);

  Serial.println(w);
  Serial.println(h);
  
  //long* lbuf = new long(h);
  unsigned char pixel[10];

  for(int x = 0; x < w; x++) {
    //Serial.print(x, DEC);
    //Serial.print(": ");
    for(int y = 0; y < h; y++) {
      pixFile.read(pixel, 4);
     // char hex[8];
     // sprintf(hex, "%02X%02X%02X ", pixel[0], pixel[1], pixel[2]);
     // Serial.print(hex);
      // lbuf[y] = fourToLong(pixel);  
      if (y < NUMPIXELS) {
        stick.setPixelColor(y, stick.Color(pixel[0], pixel[1], pixel[2]));
      }
    }
    Serial.print(x); Serial.print(" ");
    //Serial.println();
    //printStick(lbuf, h);
    // digitalWrite(LEDPIN, HIGH);
    // updateStick(lbuf, h < NUMPIXELS ? h : NUMPIXELS, true);
    stick.show();
    //digitalWrite(LEDPIN, LOW);
    
    delay(sleep);
    // clearStick();
    //Serial.print(".");
  }

  //delete[] lbuf;
  pixFile.close();
}





