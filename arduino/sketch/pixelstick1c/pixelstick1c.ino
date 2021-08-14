#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <SD.h>

// ========================================================
// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;
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

#define ANALOG

#ifdef DISPLAY
#define BUTTON_A 9
#define BUTTON_B 6
#define BUTTON_C 5
#define LED      13
#endif

#ifdef ANALOG
#define BUTTON_A A1
#define BUTTON_B A0
#define BUTTON_C A3
#define BUTTON_D A2
#define LED      13
#endif


int selectFile(char*);
int blinkFile(char*, int);
void clearStick();

const long COL_OK = stick.Color(0, 255, 0);
const long COL_ERR = stick.Color(255, 0, 0);
const long COL_WARN = stick.Color(0, 80, 80);

void setup() {

  pinMode(LEDPIN, OUTPUT);
  
  Serial.begin(9600);
//  while (!Serial) {
//    ; // wait for serial port to connect. Needed for native USB port only
//  }
  Serial.print("Initializing neopixels...");
  
  Serial.print("Initializing SD card...");

  
  stick.begin();
  stick.setBrightness(50);

  Serial.println("Display...");
  Serial.println("initialization done.");

  //pinMode(BUTTON_A, INPUT_PULLUP);
  //pinMode(BUTTON_B, INPUT_PULLUP);
  //pinMode(BUTTON_C, INPUT_PULLUP);
  digitalWrite(BUTTON_A, HIGH);
  digitalWrite(BUTTON_B, HIGH);
  digitalWrite(BUTTON_C, HIGH);
  digitalWrite(BUTTON_D, HIGH);

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    showStatus(COL_ERR);
    showStatus(COL_ERR);
    showStatus(COL_ERR);
    return;
  }
  
  showStatus(COL_OK);;
}



void loop() {
  static int selectedFileNumber = 1;

  Serial.println("loop");
  clearStick();

  inputNumber(selectedFileNumber);
  
  char filename[80];
  sprintf(filename, "%d.PIX", selectedFileNumber);
  Serial.println(filename);
  
  if (SD.exists(filename)) {
    showStatus(COL_OK);
    countdown();
    blinkFile(filename, 10);
    clearStick();
    delay(10000);
  } else {
    showStatus(COL_ERR);    
  }
  
}

//  ==============================
//  Utils
//  ==============================

long fourToLong(unsigned char* b) {
  long l = b[0] << 24 | b[1] << 16 | b[2] << 8 | b[3];
  return l;
}

int inputNumber(int& number) {

  showNumber(number);

  Serial.println("Input...");
  
  bool button = false;
  bool done = false;
  while (!done) {

    bool up = analogRead(BUTTON_A) < 500;
    bool down = analogRead(BUTTON_D) < 500;
    bool ok = analogRead(BUTTON_B) < 500;

    if (!button && (up||ok||down)) {

      button = true;
      if (up) {
        Serial.println("Button UP");
        number++;
      }
      
      if (down) {
        Serial.println("Button DOWN");
        if (number > 0) {
          number--;
        }
      }
      if (ok) {
        Serial.println("Button OK");
        done = true;
      }
      showNumber(number);
    }
    if (!up && !ok &&! down) {
      button = false;
    }
    delay(100);
  }
}


//  =========================
//  Pixelstick
//  =========================
void show() {
  stick.show();
}

void showStatus(long color) {

  for(int i = 0, j = NUMPIXELS - 1; i < NUMPIXELS; i++, j--) {
    if (i < j) {
      stick.setPixelColor(i, color);
      stick.setPixelColor(j, color);
    } else {
      stick.setPixelColor(i, 0, 0, 0);
      stick.setPixelColor(j, 0, 0, 0);      
    }
    stick.show();  
  }
}

void countdown() {

  for(int i = 9; i >= 0; i--) {
    stick.clear();
    for (int j = i; j >= 0; j--) {
      stick.setPixelColor(j, j < 3 ? stick.Color(0, 0, 10) : stick.Color(80, 80, 80));
    }
    stick.show();
    delay(1000);
  }
  clearStick();
}

void showNumber(int number) {
  Serial.print("number="); Serial.println(number);
  stick.clear();

  int n = number;
  int z = 10;
  while (n > 9) {
    stick.setPixelColor(z, 0, 0, 80);
    n-=10;;
    z++;
  }

  while (n > 0) {
    stick.setPixelColor(n, 80, 80, 80);
    n--;
  }

  stick.show();
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

void hexPrint(byte a, byte b, byte c) {
  char hex[8];
  sprintf(hex, "%02x%02x%02x ", a, b, c);
  Serial.print(hex);
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

  const int mem = h * 4;
  unsigned char* pixel = malloc(mem);

  int r = 0, g = 0, b = 0;
  
  for(int x = 0; x < w; x++) {
    const int avail = pixFile.available();
    int bytes = pixFile.read(pixel, avail < mem ? avail : mem);
    Serial.print(avail); Serial.print(" "); Serial.println(bytes);
    for(int y = 0, p = 0; y < h && p < bytes; y++, p += 4) {
      
      if (y < NUMPIXELS) {
        stick.setPixelColor(NUMPIXELS - 1 - y, pixel[p], pixel[p+1], pixel[p+2]);
      }
    }
    stick.show();
    delay(sleep);
  }
  delete [] pixel;
  pixFile.close();
}





