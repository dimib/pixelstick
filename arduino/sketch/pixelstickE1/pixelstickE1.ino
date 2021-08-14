#include <Adafruit_NeoPixel.h>
#include <SPI.h>

// ========================================================
// set up variables using the SD utility library functions:

// ========================================================

// ========================================================
// Which pin on the Arduino is connected to the NeoPixels?
#define PIN            11
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


void clearStick();

const long COL_OK = stick.Color(0, 255, 0);
const long COL_ERR = stick.Color(255, 0, 0);
const long COL_WARN = stick.Color(0, 80, 80);


const int MODUS_OFF = 0;
const int MODUS_SELECT = 1;
const int MODUS_RUN = 2;
// const int MODUS_CONTINUE = 3;

int selectedProgramNumber = 1;
int modus = MODUS_SELECT;

const int MP = 16;

void setup() {

  pinMode(LEDPIN, OUTPUT);
  
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Hello");
  
  
  Serial.println("Initializing neopixels...");
  stick.begin();
  stick.setBrightness(50);

  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  pinMode(BUTTON_D, INPUT_PULLUP);

  digitalWrite(BUTTON_A, HIGH);
  digitalWrite(BUTTON_B, HIGH);
  digitalWrite(BUTTON_C, HIGH);
  digitalWrite(BUTTON_D, HIGH);
  
  showReady(COL_OK);
}


void loop() {
  int button = readButton();
  Serial.println(button);

  switch (modus) {
    case MODUS_OFF:
      if (button == BUTTON_A) {
        Serial.println("Modus Run");
        modus = MODUS_RUN;
      }
      if (button == BUTTON_B) {
        Serial.println("Modus Run");
        modus = MODUS_RUN; // MODUS_CONTINUE;
      }
      if (button == BUTTON_D) {
        Serial.println("Modus Select");
        modus = MODUS_SELECT;
      }
      delay(10);
      break;
      
    case MODUS_SELECT:
      inputNumber(selectedProgramNumber);
      showStatus(COL_OK);
      modus = MODUS_OFF;
      break;
      
    case MODUS_RUN:
      clearStick();
      startSelectedProgram();
      clearStick();
      modus = MODUS_OFF;
      break;
/*      
    case MODUS_CONTINUE:
      clearStick();
      clearStick();
      break;
*/              
    default:
      break;
  }  
}

// ---------
// Propeller
// ---------
struct propdata {
  unsigned int pixel[5][2];
  uint32_t color[5];
  int num;
  int rounds;
  unsigned int showPixel;
  unsigned int hidePixel;
};

struct fadedata {
  uint32_t color[10];
  int num;
  int rounds;
};

const struct propdata p1 = {
  { {MP, 144}, {MP, 120}, {MP, 100}, {MP, 80}, {MP, 60} }, { 0x800000, 0x808000, 0x808080, 0x008080, 0x000080 }, 5, 5, 1000, 500
};

const struct propdata p2 = {
  { {MP, 144}, {MP,72}, {MP,36}, {0,0}, {0,0} }, { 0x808080, 0x404040, 0x202020, 0, 0 }, 3, 3, 1000, 500
};

const struct propdata p3 = {
  { {120, 144}, {100,120}, {80,100}, {60,80}, {40,60} }, { 0xff0000, 0xFF0000, 0xFF0000, 0xF00000, 0xf00000 }, 5, 3, 50, 20
};

const struct propdata p4 = {
  { {120, 144}, {100,120}, {80,100}, {60,80}, {40,60} }, { 0xff00af, 0xef00bf, 0xdf00cf, 0xcf00df, 0xbf00ff }, 5, 3, 50, 20
};

const struct propdata p5 = {
  { {120, 144}, {100,120}, {80,100}, {60,80}, {40,60} }, { 0xff0000, 0x00ff00, 0xff0000, 0x00ff00, 0xff0000 }, 5, 3, 50, 20
};

const struct propdata p6 = {
  { {MP, NUMPIXELS}, {MP + 20, NUMPIXELS - 20}, {MP + 40, NUMPIXELS - 40}, {0,0}, {0,0} }, { 0xff0000, 0x00ff00, 0x0000ff, 0x00ff00, 0xff0000 }, 3, 3, 25, 20
};
const struct propdata p7 = {
  { {MP, NUMPIXELS}, {MP + 20, NUMPIXELS - 20}, {MP + 40, NUMPIXELS - 40}, {0,0}, {0,0} }, { 0xff0000, 0xa000a0, 0x0000ff, 0x00ff00, 0xff0000 }, 3, 3, 25, 20
};

const struct propdata p8 = {
  { {MP, 130}, {MP + 5, 135}, {MP + 10, 140}, {0,0}, {0,0} }, { 0x800000, 0x008000, 0x000080, 0, 0 }, 3, 1, 40, 40
};

const struct fadedata f1 = { { 0x900000, 0x009000, 0x000090, 0, 0, 0, 0, 0, 0, 0 }, 3, 5 };
const struct fadedata f2 = { { 0xb00000, 0xb0b000, 0xb0b0b0, 0, 0, 0, 0, 0, 0, 0 }, 3, 5 };
const struct fadedata f3 = { { 0xc00000, 0xc000a0, 0xc000b0, 0xc000c0, 0xc000d0, 0, 0, 0, 0, 0 }, 5, 3 };
const struct fadedata f4 = { { 0x0000f0, 0x0000e0, 0x0000d0, 0x0000c0, 0x0000b0, 0, 0, 0, 0, 0 }, 5, 3 };
const struct fadedata f5 = { { 0x00a0ff, 0x00b0ff, 0x00c0ff, 0x00d0ff, 0x00e0ff, 0, 0, 0, 0, 0 }, 5, 3 };
const struct fadedata f6 = { { 0x0060f0, 0x0070e0, 0x0080d0, 0x0090c0, 0x00a0b0, 0x00b0a0, 0x00c090, 0x00d080, 0x00e070, 0x00f060 }, 10, 1 };
const struct fadedata f7 = { { 0x60f000, 0x70e000, 0x80d000, 0x90c000, 0xa0b000, 0xb0a000, 0xc09000, 0xd08000, 0xe07000, 0xf06000 }, 10, 1 };
const struct fadedata f8 = { { 0xff0f00, 0xff1f00, 0xff2f00, 0xff3f00, 0xff4f00, 0xff5f00, 0xff6f00, 0xff7f00, 0xff8f00, 0xff9f00 }, 10, 1 };

void startSelectedProgram() {
  switch(selectedProgramNumber) {
    case 1: propeller(&p1); break;
    case 2: propeller(&p2); break;
    case 3: screw(&f1, 2); break;
    case 4: screw(&f2, 2); break;
    case 5: screw(&f1, 1); break;
    case 6: screw(&f2, 1); break;
    case 7: screw(&f3, 2); break;
    case 8: screw(&f4, 2); break;
    case 9: screw(&f5, 2); break;
    case 10: screw(&f6, 2); break;
    case 11: screw(&f7, 2); break;
    case 12: screw(&f8, 2); break;
    case 13: screw(&f8, 1); break;
    case 14: rainbow(10); break;  
    case 15: rainbow(100); break;
    case 16: propeller2(&p3); break;
    case 17: propeller2(&p4); break;
    case 18: propeller2(&p5); break;
    case 19: propeller3(&p5, 1); break;
    case 20: propeller3(&p6, 2); break;
    case 21: propeller3(&p7, 2); break;
    case 22: spiral(&p7); break;
    case 23: spiral(&p8); break;
    case 24: circle(); break;
    default: break;    
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
  boolean a = analogRead(BUTTON_A) == 0;
  boolean b = analogRead(BUTTON_B) == 0;
  boolean c = analogRead(BUTTON_C) == 0;
  boolean d = analogRead(BUTTON_D) == 0;
  
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

    int a = analogRead(BUTTON_A);
    int d = analogRead(BUTTON_D);
    int b = analogRead(BUTTON_B);
    
    char deb[30];
//    sprintf(deb, "%d %d %d", a, d, b);
//    Serial.println(deb);
    
    bool up = d == 0;
    bool down = a == 0;
    bool ok = b == 0;

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

//  ==================================================
//  Pixel Programme
//  ==================================================

// Propeller
void propeller(const struct propdata* p) {
  Serial.println("propeller");
  delay(2000);

  for (int i = 0; i < p->rounds; i++) {
    for (int ii = 0; ii < p->num; ii++) {
      fill(p->pixel[ii][0], p->pixel[ii][1], p->color[ii]);
      stick.show();
      delay(p->showPixel);
      clearStick();
      delay(p->hidePixel);
  
      if (readButton()) {
        delay(1000);
        return;
      }
    }
  }
}

void propeller2(const struct propdata* p) {
  Serial.println("propeller2");
  delay(2000);

  for (int i = 0; i < p->rounds; i++) {
    for (int ii = 0; ii < p->num; ii++) {
      fadeAllOut(p->pixel[ii][0], p->pixel[ii][1], 10, p->showPixel, p->color[ii]);
      clearStick();
      if (readButton()) {
        delay(1000);
        return;
      }
    }
  }
}

void propeller3(const struct propdata* p, int which) {
  Serial.println("propeller3");
  delay(2000);

  for (int i = 0; i < p->rounds; i++) {
    for (int ii = 0; ii < p->num; ii++) {
      if (which == 1) removeOneByOne(p->pixel[ii][0], p->pixel[ii][1], p->showPixel, p->color[ii]);
      if (which == 2) removeTwoByOne(p->pixel[ii][0], p->pixel[ii][1], p->showPixel, p->color[ii]);
      clearStick();
      if (readButton()) {
        delay(1000);
        return;
      }
    }
  }
}

void screw(const struct fadedata* p, int which) {
  Serial.println("screw");
  delay(2000);

  for (int i = 0; i < p->rounds; i++) {
    for (int ii = 0; ii < p->num; ii++) {
      for (int px = MP; px < NUMPIXELS; px++) {
        if (which == 1) fadeMove(px, p->color[ii]);
        if (which == 2) fadeMove2(px, p->color[ii]);
        stick.show();
        clearStick();
        if (readButton()) {
          delay(1000);
          return;
        }      
      }
    }
  }
}

void spiral(const struct propdata* p) {
  Serial.println("spiral");
  delay(2000);
  
  for (int i = 0; i < p->rounds; i++) {
    for (int ii = 0; ii < p->num; ii++) {
      uint32_t cv = stick.Color((p->color[ii] >> 16) & 0xff, (p->color[ii] >> 8) & 0xff, (p->color[ii] & 0xff));
      for (int px = p->pixel[ii][0]; px < p->pixel[ii][1]; px++) {
        stick.setPixelColor(px, cv);
        stick.show();
        delay(p->showPixel);
        stick.setPixelColor(px, 0);
        stick.show();
      }
    }
  }
}


void circle() {
  Serial.println("p2");
  clearStick();
  fill(80, 100, stick.Color(100, 0, 0));
  fill(50, 70, stick.Color(0, 0, 100));
  stick.show();
  delay(5000);
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  while(1) {
    for(j=0; j<256; j++) {
      for(i=0; i<stick.numPixels(); i++) {
        stick.setPixelColor(i, Wheel((i+j) & 255));
      }
      stick.show();
      delay(wait);
      if (readButton()) {
        delay(1000); return;
      }
    }
  }
}

void test() {
  clearStick();
  stick.setPixelColor(20, stick.Color(128, 0, 0));
  stick.setPixelColor(21, stick.Color(128 - 20, 0, 0));
  stick.setPixelColor(22, stick.Color(128 - 40, 0, 0));
  stick.setPixelColor(23, stick.Color(128 - 60, 0, 0));
  stick.setPixelColor(24, stick.Color(128 - 80, 0, 0));
  stick.setPixelColor(25, stick.Color(128 - 100, 0, 0));
  stick.setPixelColor(25, stick.Color(128 - 120, 0, 0));
  stick.setPixelColor(25, stick.Color(128 - 140, 0, 0));
  stick.show();
  delay(2000);
}

// --------------------
// Helper usw.
// --------------------
void fill(int from, int to, unsigned long value) {
  for(int i = from; i <= to; i++) {
    unsigned long cv = stick.Color((value >> 16) & 0xff, (value >> 8) & 0xff, value & 0xff);
    stick.setPixelColor(i, cv);
  }
}

void fadeMove(int from, unsigned long value) {
  for (int i = 0, j = 200; i < 20; i++, j-=20) {
    int px = from + i;
    if (px >= MP && px < NUMPIXELS) {
      int vr = ((value >> 16) & 0xff) - j;
      int vg = ((value >> 8) & 0xff) - j;
      int vb = (value & 0xff) - j;
      uint32_t cv = stick.Color((uint8_t) (vr > 0 ? vr : 0), (uint8_t) (vg > 0 ? vg : 0), (uint8_t) (vb > 0 ? vb : 0));
      stick.setPixelColor(px, cv);
    }
  }
}

void fadeMove2(int from, unsigned long value) {
  for (int i = 0, j = 200; i < 20; i++, j-=10) {
    int px = from + i;
    if (px >= MP && px < NUMPIXELS) {
      int vr = ((value >> 16) & 0xff) - j;
      int vg = ((value >> 8) & 0xff) - j;
      int vb = (value & 0xff) - j;
      uint32_t cv = stick.Color((vr > 0 ? vr : 0), (vg > 0 ? vg : 0), (vb > 0 ? vb : 0));
      stick.setPixelColor(px, cv);
    }
  }
}

void fadeAllOut(int from, int to, int steps, int showDelay, unsigned long value) {
    int vr = ((value >> 16) & 0xff);
    int vg = ((value >> 8) & 0xff);
    int vb = (value & 0xff);

    while((vr > 0) || (vg > 0) || (vb > 0)) {
      for (int ii = from; ii < to; ii++) {
        stick.setPixelColor(ii, stick.Color((vr > 0 ? vr : 0), (vg > 0 ? vg : 0), (vb > 0 ? vb : 0)));
      }
      stick.show();
      delay(showDelay);
      vr -= steps;
      vg -= steps;
      vb -= steps;
    }
}
void removeOneByOne(int from, int to, int showDelay, unsigned long value) {
    int vr = ((value >> 16) & 0xff);
    int vg = ((value >> 8) & 0xff);
    int vb = (value & 0xff);

    for (int ii = from; ii < to; ii++) {
      stick.setPixelColor(ii, stick.Color(vr, vg, vb));
    }
    stick.show();

    for (int ii = from; ii < to; ii++) {
      stick.setPixelColor(ii, stick.Color(0, 0, 0));
      stick.show();
      delay(showDelay);
    }  
}
void removeTwoByOne(int from, int to, int showDelay, unsigned long value) {
    int vr = ((value >> 16) & 0xff);
    int vg = ((value >> 8) & 0xff);
    int vb = (value & 0xff);

    for (int ii = from; ii < to; ii++) {
      stick.setPixelColor(ii, stick.Color(vr, vg, vb));
    }
    stick.show();

    int num = (to - from) / 2;
    for (int ii = 0; ii < num; ii++) {
      stick.setPixelColor(from + ii, stick.Color(0, 0, 0));
      stick.setPixelColor(to - ii, stick.Color(0, 0, 0));
      stick.show();
      delay(showDelay);
    }  
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return stick.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return stick.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return stick.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}



