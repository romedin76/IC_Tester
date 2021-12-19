#include <Arduino.h>

/* PINS
IC   IC    Pin
14   16    Arduino

8    9  =  5
9    10 =  4
10   11 =  3
11   12 =  2
12   13 =  13
13   14 =  A0
14   15 =  A1
     16 =  A2
     1  =  6
1    2  =  7
2    3  =  8
3    4  =  9
4    5  =  10
5    6  =  11
6    7  =  12
7    8  =  GND
*/
// Pin Definitions
int pin14[14] = {7, 8, 9, 10, 11, 12, A7, 5, 4, 3, 2, 13, A0, A1};        // contains the correct allocation for a 14 Pins IC
int pin16[16] = {6, 7, 8, 9, 10, 11, 12, A7, 5, 4, 3, 2, 13, A0, A1, A2}; // contains the correct allocation for a 16 Pins IC

int screenStatus = 0, lastStatus = 0;
int *pin, pinCount = 0;
bool finish = false;

typedef struct
{
  int pins;
  char *pattern[10];
} IC;

typedef struct
{
  uint8_t lang;
  const char *IC_name;
  IC IC_object;
} ICDictionay;

IC ci_7406 = {14, {"0H0H0HGH0H0H0V", "1L1L1LGL1L1L1V"}};
IC ci_7442 = {16, {"LHHHHHHGHHH0000V", "HLHHHHHGHHH0001V", "HHLHHHHGHHH0010V", "HHHLHHHGHHH0011V", "HHHHLHHGHHH0100V", "HHHHHLHGHHH0101V", "HHHHHHLGHHH0110V", "HHHHHHHGLHH0111V", "HHHHHHHGHLH1000V", "HHHHHHHGHHL1001V"}};
IC ci_4076 = {16, {"00HHHHCG0011110V", "00HLHLCG0001010V", "00LHLHCG0010100V"}};
IC ci_7432 = {14, {"00L00LGL00L00V", "01H01HGH10H10V", "10H10HGH01H01V", "11H11HGH11H11V"}};
IC ci_7404 = {14, {"0H0H0HGH0H0H0V", "1L1L1LGL1L1L1V"}};
IC ci_74139 = {16, {"000LHHHGHHHL000V", "001HHLHGHLHH100V","010HLHHGHHLH010V","011HHHLGLHHH110V"}};

const ICDictionay myDictionaryArr[]{
    {0, "7406", ci_7406},
    {1, "7442", ci_7442},
    {2, "4076", ci_4076},
    {3, "7432", ci_7432},
    {4, "7404", ci_7404},
    {4, "74139", ci_74139},
};

void setup()
{
  Serial.begin(9600);

  delay(1000); // power-up safety delay
}

boolean testCase(String test, int pins)
{
  boolean result = true;
  int clkPin = -1;

  Serial.println("SignalIn : " + test);
  Serial.print("Response : ");

  // Setting Vcc, GND and INPUTS
  for (int i = 0; i < pins; i++)
  {
    switch (test[i])
    {
    case 'V':
      pinMode(pin[i], OUTPUT);
      digitalWrite(pin[i], HIGH);
      break;
    case 'G':
      pinMode(pin[i], OUTPUT);
      digitalWrite(pin[i], LOW);
      break;
    case 'L':
      digitalWrite(pin[i], LOW);
      pinMode(pin[i], INPUT_PULLUP);
      break;
    case 'H':
      digitalWrite(pin[i], LOW);
      pinMode(pin[i], INPUT_PULLUP);
      break;
    }
  }

  delay(5);

  // Setting Input Signals
  for (int i = 0; i < pins; i++)
  {
    switch (test[i])
    {
    case 'X':
    case '0':
      pinMode(pin[i], OUTPUT);
      digitalWrite(pin[i], LOW);
      break;
    case '1':
      pinMode(pin[i], OUTPUT);
      digitalWrite(pin[i], HIGH);
      break;
    case 'C':
      clkPin = pin[i];
      pinMode(pin[i], OUTPUT);
      digitalWrite(pin[i], LOW);
      break;
    }
  }

  if (clkPin != -1)
  {
    // Clock Trigger
    pinMode(clkPin, INPUT_PULLUP);
    delay(10);
    pinMode(clkPin, OUTPUT);
    digitalWrite(clkPin, LOW);
  }

  delay(5);

  // Reading Outputs
  for (int i = 0; i < pins; i++)
  {
    switch (test[i])
    {
    case 'H':
      if (!digitalRead(pin[i]))
      {
        result = false;
        Serial.print('L');
      }
      else
        Serial.print(' ');
      break;
    case 'L':
      if (digitalRead(pin[i]))
      {
        result = false;
        Serial.print('H');
      }
      else
        Serial.print(' ');
      break;
    default:
      Serial.print(' ');
    }
  }
  Serial.println(";");
  // Serial.println("\nCase Result : "+String(result));
  return result;
}

void test(String name, int numPins, char *pattern[])
{
  String newCase;
  int pins = -1;
  boolean result = true;

  Serial.print(name);
  Serial.print(" - ");
  Serial.println(numPins);

  pins = numPins;
  pin = new int[pins];

  if (pins == 16)
    pin = pin16;
  else if (pins == 14)
    pin = pin14;

  int i;

  for (i = 0; i < 10; i++)
  {
    if (pattern[i] != NULL)
    {
      if (testCase(pattern[i], pins) == false)
      {
        result = false;
      }
      delay(50);
    }
  }
  delete (pin);

  if (result == true)
  {
    Serial.println("IC is GOOD");
  }
  else
  {
    Serial.println("IC is BAD");
  }
}

IC searchCI(String ic_find)
{
  IC ic_found;
  bool found = false;
  for (uint8_t i = 0; i < sizeof(myDictionaryArr) / sizeof(ICDictionay); ++i)
  {
    if (ic_find == myDictionaryArr[i].IC_name)
    {
      ic_found = myDictionaryArr[i].IC_object;
      found = true;
      break;
    }
  }
  if (found)
  {
    Serial.println("Chip encontrado");
  }
  else
  {
    Serial.println("Chip NO encontrado");
  }
  return ic_found;
}

void loop()
{
  String ic_to_find = "7432";
  
  if (finish == 0)
  {
    IC ci_found = searchCI(ic_to_find);
    test(ic_to_find, ci_found.pins, ci_found.pattern);
    finish = true;
  }

  delay(10000);
}