#include "DHT.h"

/*
    A
   ---
F |   | B
  | G |
   ---
E |   | C
  |   |
   ---
    D

02  A   11
03  B   07
04  C   04
05  D   02
06  E   01
07  F   10
08  G   05
09  D1  12
10  D2  09
11  D3  08
12  D4  06

*/

#define P_A    2
#define P_B    3
#define P_C    4
#define P_D    5
#define P_E    6
#define P_F    7
#define P_G    8
#define P_D1   9
#define P_D2  10
#define P_D3  11
#define P_D4  12
#define P_DHT 13

#define DHTTYPE DHT11

#define CHAR_C 10
#define CHAR_H 11

const int SELECTOR_PINS[4] = { P_D1, P_D2, P_D3, P_D4 };
const int NUMB_PINS[7] = { P_A, P_B, P_C, P_D, P_E, P_F, P_G };
const int NUMBER_PATTERNS[12][7] = {
//    A  B  C  D  E  F  G
    { 1, 1, 1, 1, 1, 1 },    // 0
    { 0, 1, 1 },             // 1
    { 1, 1, 0, 1, 1, 0, 1 }, // 2
    { 1, 1, 1, 1, 0, 0, 1 }, // 3
    { 0, 1, 1, 0, 0, 1, 1 }, // 4
    { 1, 0, 1, 1, 0, 1, 1 }, // 5
    { 1, 0, 1, 1, 1, 1, 1 }, // 6
    { 1, 1, 1 },             // 7
    { 1, 1, 1, 1, 1, 1, 1 }, // 8
    { 1, 1, 1, 1, 0, 1, 1 }, // 9
    { 1, 0, 0, 1, 1, 1 },    // 10 (C)
    { 0, 1, 1, 0, 1, 1, 1 }  // 11 (H)
};

// --------------------------------------------------------------------------------

int currSelected = -1;
int currStatus = 0;

DHT dht(P_DHT, DHTTYPE);

// --------------------------------------------------------------------------------

void setPinsMode(const int *pins, const size_t &cPins, const uint8_t &mode) {
    for (int i = 0; i < cPins; ++i) {
        pinMode(pins[i], mode);
    }
}

void select(const int &pos) {
    int pin = SELECTOR_PINS[pos];

    if (pin == currSelected)
        return;

    if (currSelected > -1)
        digitalWrite(currSelected, 1);

    digitalWrite(pin, 0);
    currSelected = pin;
}

void setNumber(const int &pos, const int &numb) {
    select(pos);
    const int *pattern = NUMBER_PATTERNS[numb];
    for (int i = 0; i < 7; ++i) {
        digitalWrite(NUMB_PINS[i], pattern[i]);
    }
}

void asDigets(const int &numb, int *digits) {
    digits[0] = numb / 10;
    digits[1] = numb % 10;
}

// --------------------------------------------------------------------------------

// the setup routine runs once when you press reset:
void setup() {
    setPinsMode(SELECTOR_PINS, sizeof(SELECTOR_PINS) / sizeof(int), OUTPUT);
    setPinsMode(NUMB_PINS, sizeof(NUMB_PINS) / sizeof(int), OUTPUT);

    digitalWrite(P_D1, 1);
    digitalWrite(P_D2, 1);
    digitalWrite(P_D3, 1);
    digitalWrite(P_D4, 1);

    Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
    int data;
    int specifier;
    
    if (currStatus <= 2000 / 15) {
        data = dht.readTemperature();
        specifier = CHAR_C;
    } else if (currStatus <= 4000 / 15) {
        data = dht.readHumidity();
        specifier = CHAR_H;
    } else {
        currStatus = 0;
    }

    currStatus++;

    int *digits;
    asDigets(data, digits);

    setNumber(1, digits[0]);
    delay(5);
    setNumber(2, digits[1]);
    delay(5);
    setNumber(3, specifier);
    delay(5);
}