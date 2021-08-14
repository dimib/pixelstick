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
#define UPSIDEDOWN 1


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


const int MODUS_OFF = 0;
const int MODUS_SELECT = 1;
const int MODUS_RUN = 2;
const int MODUS_CONTINUE = 3;

int selectedFileNumber = 1;
int modus = MODUS_SELECT;
char filename[20];


void setup() {

  pinMode(LEDPIN, OUTPUT);
  
  Serial.begin(9600);
//  while (!Serial) {
//    ; // wait for serial port to connect. Needed for native USB port only
//  }
//  
  
  Serial.println("Initializing neopixels...");
  stick.begin();
  stick.setBrightness(50);

  digitalWrite(BUTTON_A, HIGH);
  digitalWrite(BUTTON_B, HIGH);
  digitalWrite(BUTTON_C, HIGH);
  digitalWrite(BUTTON_D, HIGH);

  Serial.println("Initializing SD...");
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    showReady(COL_ERR);
    showReady(COL_ERR);
    showReady(COL_ERR);
    return;
  }
  
  showReady(COL_OK);;
}


void loop() {
  int button = readButton();

  switch (modus) {
    case MODUS_OFF:
      if (button == BUTTON_A) {
        Serial.println("Modus Run");
        modus = MODUS_RUN;
      }
      if (button == BUTTON_B) {
        Serial.println("Modus Continue");
        modus = MODUS_CONTINUE;
      }
      if (button == BUTTON_D) {
        Serial.println("Modus Select");
        modus = MODUS_SELECT;
      }
      delay(10);
      break;
      
    case MODUS_SELECT:
      inputNumber(selectedFileNumber);
      sprintf(filename, "%d/%d.PIX", NUMPIXELS, selectedFileNumber);
      Serial.println(filename);
      if (SD.exists(filename)) {
        showStatus(COL_OK);
        modus = MODUS_OFF;
      } else {
        showStatus(COL_ERR);
        modus = MODUS_SELECT;
      }
      break;
      
    case MODUS_RUN:
      clearStick();
      blinkFile(filename, 10);
      clearStick();
      modus = MODUS_OFF;
      break;
      
    case MODUS_CONTINUE:
      clearStick();
      if (blinkFile(filename, 10) == 1) {
        modus = MODUS_SELECT;
      }
      clearStick();
      break;
              
    default:
      break;
  }  
}

//  ==============================
//  Utils
//  ==============================

long fourToLong(unsigned char* b) {
  long l = b[0] << 24 | b[1] << 16 | b[2] << 8 | b[3];
  return l;
}

int readButton() {
  static boolean button = false;
  boolean a = analogRead(BUTTON_A) < 500;
  boolean b = analogRead(BUTTON_B) < 500;
  boolean c = analogRead(BUTTON_C) < 500;
  boolean d = analogRead(BUTTON_D) < 500;

  if (!button) {
    if (a || b || c || d) {
      button = true;
      if (a) return BUTTON_A;
      if (b) return BUTTON_B;
      if (c) return BUTTON_C;
      if (d) return BUTTON_D;
    }
  } else {
    if (!a && !b && !c && !d) {
      button = false; 
    }
  }
  return 0;
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
        if (number > 1) {
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

  return number;
}


//  =========================
//  Pixelstick
//  =========================

void showReady(long color) {

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

void showStatus(long color) {
  for (int i = 0; i < NUMPIXELS; i++) {
    stick.setPixelColor(i, color);
  }
  delay(500);
  stick.show();
  for (int i = 0; i < NUMPIXELS; i++) {
    stick.setPixelColor(i, 0);
  }
  stick.show();
  
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
  Serial.println("blinkFile");
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
  Serial.println(mem);
  if (pixel != NULL) {
    Serial.println("Malloc ok");
  } else {
    Serial.println("Malloc nicht ok");
  }
 

  int r = 0, g = 0, b = 0;

  boolean stop = false;

  for(int x = 0; x < w && !stop; x++) {
    Serial.println("Read");
    const unsigned long avail = pixFile.available();
    long bytes = pixFile.read(pixel, mem);
    for(int y = 0, p = 0; y < h && p < bytes; y++, p += 4) {
      if (y < NUMPIXELS) {
        stick.setPixelColor(NUMPIXELS - 1 - y, pixel[p], pixel[p+1], pixel[p+2]);
      }
    }
    stick.show();
    delay(sleep);
    stop = readButton() != 0;
  }
  Serial.println("Done or stop");
  delete [] pixel;
  pixFile.close();
  return stop ? 1 : 0;
}
