#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal.h>

#define DHTPIN 1
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int displayMode = 2;
boolean displayCelcius = false;

byte customCharTorn[8] = {
  0b11111,
  0b11110,
  0b01110,
  0b00111,
  0b00011,
  0b00110,
  0b01000,
  0b00100
};

byte customCharUpUp[8] = {
  0b00100,
  0b01110,
  0b11011,
  0b10001,
  0b00100,
  0b01110,
  0b11011,
  0b10001
};

// up
byte customCharUp[8] = {
  0b00000,
  0b00000,
  0b00100,
  0b01110,
  0b11011,
  0b10001,
  0b00000,
  0b00000
};

byte customCharDn[8] = {
  0b00000,
  0b00000,
  0b10001,
  0b11011,
  0b01110,
  0b00100,
  0b00000,
  0b00000
};

byte customCharDnDn[8] = {
  0b10001,
  0b11011,
  0b01110,
  0b00100,
  0b10001,
  0b11011,
  0b01110,
  0b00100
};

void setup()
{
  lcd.createChar(0, customCharTorn);
  lcd.createChar(1, customCharUpUp);
  lcd.createChar(2, customCharUp);
  lcd.createChar(3, customCharDn);
  lcd.createChar(4, customCharDnDn);
  lcd.begin(16, 2);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("dewpointer v0.1");
  lcd.setCursor(0, 1);
  lcd.print("2017 jason zahn");
  
  delay(2000);
}

void loop()
{
  double humidity = dht.readHumidity();
  double tempC = dht.readTemperature();
  double tempF = farenheight(tempC);
  double dewpC = dewPoint(tempC, humidity);
  double dewpF = farenheight(dewpC);
  double deprC = tempC - dewpC;
  double deprF = tempF - dewpF;
  
  processInput();
  
  if (displayCelcius)
    printDisplay(tempC, dewpC, humidity, deprC);
  else
    printDisplay(tempF, dewpF, humidity, deprF);
  
  delay(2000);
}

void processInput()
{
  
}

void printDisplay(const double temp, const double dewp, const double humidity, const double depr)
{
  if (displayMode == 0)
  {
    lcd.clear();
    printDisplayMode0(temp, dewp);
  }
  else if (displayMode == 1)
  {
    lcd.clear();
    printDisplayMode1(temp, humidity);
  }
  else if (displayMode == 2)
  {
    lcd.clear();
    printDisplayMode2(temp, dewp, depr);
  }
  else
  {
    lcd.clear();
    printDisplayMode3(temp, dewp, humidity, depr);
  }
}

void printDisplayMode0(const double temp, const double dewp)
{
  lcd.setCursor(0, 0);
  lcd.print("TEMP");
  lcd.setCursor(9, 0);
  lcd.print("DEWP");

  lcd.setCursor(0, 1);
  lcd.print(temp);
  lcd.setCursor(9, 1);
  lcd.print(dewp);
}

void printDisplayMode1(const double temp, const double humidity)
{
  lcd.setCursor(0, 0);
  lcd.print("TEMP");
  lcd.setCursor(9, 0);
  lcd.print("HUMI");

  lcd.setCursor(0, 1);
  lcd.print(temp);
  lcd.setCursor(9, 1);
  lcd.print(humidity);
}

void printDisplayMode2(const double temp, const double dewp,  const double depr)
{
  char tempStr[4];
  char dewpStr[4];
  char deprStr[4];

  dtostrf(temp, 4, 1, tempStr);
  dtostrf(dewp, 4, 1, dewpStr);
  dtostrf(depr, 4, 1, deprStr);
  
  lcd.setCursor(0, 0);
  lcd.print("TEMP");
  lcd.write((uint8_t)1);
  lcd.setCursor(6, 0);
  lcd.print("DEWP");
  lcd.write((uint8_t)4);
  lcd.setCursor(12, 0);
  //lcd.print("DEPR");
  printMoneyDisplay(depr);

  lcd.setCursor(0, 1);
  lcd.print(tempStr);
  lcd.setCursor(6, 1);
  lcd.print(dewpStr);
  lcd.setCursor(12, 1);
  //lcd.print();
  lcd.print(deprStr);
}

void printDisplayMode3(const double temp, const double dewp, const double humidity, const double depr) {
  lcd.setCursor(0, 0);
  lcd.print("T");
  lcd.setCursor(2, 0);
  lcd.print(temp);

  lcd.setCursor(9, 0);
  lcd.print("D");
  lcd.setCursor(11, 0);
  lcd.print(dewp);

  lcd.setCursor(0, 1);
  lcd.print("H");
  lcd.setCursor(2, 1);
  lcd.print(humidity);

  lcd.setCursor(9, 1);
  lcd.print("d");
  lcd.setCursor(11, 1);
  lcd.print(depr);
}

void printMoneyDisplay(const double depr)
{
  if (depr < 5) {
    lcd.write((uint8_t)0);
    lcd.write((uint8_t)0);
    lcd.write((uint8_t)0);
    lcd.write((uint8_t)0);
  }
  else if (depr < 10) {
    lcd.write((uint8_t)0);
    lcd.write((uint8_t)0);
    lcd.write((uint8_t)0);
  }
  else if (depr < 20) {
    lcd.write((uint8_t)0);
    lcd.write((uint8_t)0);
  }
  else {
    lcd.write((uint8_t)0);
  }
}

double dewPoint(const double celsius, const double humidity)
{
  // NOAA dewpoint calculation /////////////
  // (1) Saturation Vapor Pressure = ESGG(T)
  double ratio = 373.15 / (273.15 + celsius);
  double rhs = -7.90298 * (ratio - 1);
  rhs += 5.02808 * log10(ratio);
  rhs += -1.3816e-7 * (pow(10, (11.344 * (1 - 1/ratio ))) - 1) ;
  rhs += 8.1328e-3 * (pow(10, (-3.49149 * (ratio - 1))) - 1) ;
  rhs += log10(1013.246);

  // factor -3 is to adjust units - Vapor Pressure SVP * humidity
  double VP = pow(10, rhs - 3) * humidity;

  // (2) DEWPOINT = F(Vapor Pressure)
  double T = log(VP/0.61078);   // temp var
  return (241.88 * T) / (17.558 - T);
}

double farenheight(const double celcius)
{
  return celcius * 1.8 + 32;
}

int dblToInt(const double input)
{
  return (int)(input + 0.5f);
}

