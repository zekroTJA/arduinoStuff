#include <DHT.h>
#include <DHT_U.h>

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

// Uncomment this to enable debug output
// via serial COM output
// #define DEBUG

// This defines the DHT Sensor type.
// For example: DHT11, DHT13, ...
#define DHTTYPE DHT11

// Defines the ammount of milli seconds between
// the draw of each didget on the display output.
#define TIMING 5

// Digital pins for controlling the
// sections of a numbe field
#define P_A    2
#define P_B    3
#define P_C    4
#define P_D    5
#define P_E    6
#define P_F    7
#define P_G    8

// Digital pins for selecting the
// number field to set the sections for
#define P_D1   9
#define P_D2  10
#define P_D3  11
#define P_D4  12
#define P_DHT 13

// Index of the two character fields
// for C and H
#define CHAR_C 10
#define CHAR_H 11

const int SELECTOR_PINS[4] = { P_D1, P_D2, P_D3, P_D4 };
const int NUMB_PINS[7] = { P_A, P_B, P_C, P_D, P_E, P_F, P_G };

// Matrix which defines which sections
// of a field must be lit for displaying
// the right number / character
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
int data = 0;
int specifier = 0;

// Initialization of the DHT sensor
// handler with the digital pin of the
// sensor and the type of th sensor
DHT dht(P_DHT, DHTTYPE);

// --------------------------------------------------------------------------------

/**
 * Sets the range of specified pins to
 * the specified pin mode (either OUTPUT or INPUT).
 * 
 * pins  : the array of pins to set
 * cPins : size of the pins array
 * mode  : the mode to specify
 */
void setPinsMode(const int *pins, const size_t &cPins, const uint8_t &mode) {
    for (int i = 0; i < cPins; ++i) {
        pinMode(pins[i], mode);
    }
}

/**
 * Selects the specified digit field
 * on the display (1st, 2nd, ...)
 * so that the definition of sections
 * will affect this field next.
 * 
 * pos : position of the digit
 */
void select(const int &pos) {
    int pin = SELECTOR_PINS[pos];

    if (pin == currSelected)
        return;

    if (currSelected > -1)
        digitalWrite(currSelected, 1);

    digitalWrite(pin, 0);
    currSelected = pin;
}

/**
 * Selects the defined position of the digit
 * and then sets the required pins to HIGH so
 * that the passed numer / character will be
 * shown on this digit.
 * 
 * pos :  position of the digit
 * numb : the number / character to be displayed
 */
void setNumber(const int &pos, const int &numb) {
    select(pos);
    const int *pattern = NUMBER_PATTERNS[numb];
    for (int i = 0; i < 7; ++i) {
        digitalWrite(NUMB_PINS[i], pattern[i]);
    }
}

/**
 * Splits a two-digit integer into two seperate
 * integers and writes them into the digits array.
 * 
 * numb   : number to be split
 * digits : array reference to write result to
 */
void asDigets(const int &numb, int *digits) {
    digits[0] = numb / 10;
    digits[1] = numb % 10;
}

// --------------------------------------------------------------------------------

// the setup routine runs once when you press reset
void setup() {
    setPinsMode(SELECTOR_PINS, sizeof(SELECTOR_PINS) / sizeof(int), OUTPUT);
    setPinsMode(NUMB_PINS, sizeof(NUMB_PINS) / sizeof(int), OUTPUT);

    digitalWrite(P_D1, 1);
    digitalWrite(P_D2, 1);
    digitalWrite(P_D3, 1);
    digitalWrite(P_D4, 1);

#ifdef DEBUG
    Serial.begin(9600);
#endif
}

// the loop routine runs over and over again forever
void loop() {
    if (currStatus == 1500 / (TIMING * 3)) {
        data = dht.readTemperature();
        specifier = CHAR_C;
#ifdef DEBUG
        Serial.println(data, specifier);
#endif
    } else if (currStatus == 4200 / (TIMING * 3)) {
        data = dht.readHumidity();
        specifier = CHAR_H;
#ifdef DEBUG
        Serial.println(data, specifier);
#endif
    } else if (currStatus == 6000 / (TIMING * 3)) {
        currStatus = 0;
    }

    currStatus++;

    int digits[2];
    asDigets(data, digits);

    setNumber(1, digits[0]);
    delay(TIMING);
    setNumber(2, digits[1]);
    delay(TIMING);
    setNumber(3, specifier);
    delay(TIMING);
}
