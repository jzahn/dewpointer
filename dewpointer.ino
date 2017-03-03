#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal.h>

#define DHTPIN 1
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

// select the pins used on the LCD panel
//LiquidCrystal lcd(8, 9, 4, 5, 6, 7); // osepp
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // standard

// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

int display_mode = 2;

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
}

void loop()
{
  double humidity = dht.readHumidity();
  double tempC = dht.readTemperature();
  double tempF = farenheight(tempC);
  double dewpC = dewPoint(tempC, humidity);
  double dewpF = farenheight(dewpC);
  double temp_dp_deltaF = tempF - dewpF;
  getInput();
  doDisplay(tempF, dewpF, humidity, temp_dp_deltaF);
  delay(2000);
}

void getInput()
{
  lcd_key = read_lcd_buttons();

  switch (lcd_key)
  {
    case btnUP:
      display_mode = 0;
      break;

    case btnDOWN:
      display_mode = 1;
      break;

    case btnRIGHT:
      display_mode = 3;
      break;

    case btnLEFT:
      display_mode = 2;
      break;

    case btnSELECT:
      display_mode = 0;
      break;
  }
}

int read_lcd_buttons()
{
  adc_key_in = analogRead(0);      // read the value from the sensor
  // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
  // we add approx 50 to those values and check to see if we are close
  if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
  if (adc_key_in < 50)   return btnRIGHT;
  if (adc_key_in < 195)  return btnUP;
  if (adc_key_in < 380)  return btnSELECT;
  if (adc_key_in < 555)  return btnDOWN;
  if (adc_key_in < 790)  return btnLEFT;

  return btnNONE;  // when all others fail, return this...
}

void doDisplay(const double tempF, const double dewpF, const double humidity, const double temp_dp_deltaF)
{
  if (display_mode == 0)
  {
    lcd.clear();
    display_mode_0(tempF, dewpF);
  }
  else if (display_mode == 1)
  {
    lcd.clear();
    display_mode_1(tempF, humidity);
  }
  else if (display_mode == 2)
  {
    lcd.clear();
    display_mode_2(tempF, dewpF, temp_dp_deltaF);
  }
  else
  {
    lcd.clear();
    display_mode_3(tempF, dewpF, humidity, temp_dp_deltaF);
  }
}

void display_mode_0(const double tempF, const double dewpF)
{
  lcd.setCursor(0, 0);
  lcd.print("TEMP");
  lcd.setCursor(9, 0);
  lcd.print("DEWP");

  lcd.setCursor(0, 1);
  lcd.print(tempF);
  lcd.setCursor(9, 1);
  lcd.print(dewpF);
}

void display_mode_1(const double tempF, const double humidity)
{
  lcd.setCursor(0, 0);
  lcd.print("TEMP");
  lcd.setCursor(9, 0);
  lcd.print("HUMI");

  lcd.setCursor(0, 1);
  lcd.print(tempF);
  lcd.setCursor(9, 1);
  lcd.print(humidity);
}

void display_mode_2(const double tempF, const double dewpF,  const double temp_dp_deltaF)
{
  lcd.setCursor(0, 0);
  lcd.print("TEMP");
  lcd.write((uint8_t)1);
  lcd.setCursor(6, 0);
  lcd.print("DEWP");
  lcd.write((uint8_t)4);
  lcd.setCursor(12, 0);
  //lcd.print("DEPR");
  printMoneyDisplay(temp_dp_deltaF);

  lcd.setCursor(0, 1);
  lcd.print(tempF);
  lcd.setCursor(6, 1);
  lcd.print(dewpF);
  lcd.setCursor(12, 1);
  lcd.print(temp_dp_deltaF);
}

void display_mode_3(const double tempF, const double dewpF, const double humidity, const double temp_dp_deltaF) {
  lcd.setCursor(0, 0);
  lcd.print("T");
  lcd.setCursor(2, 0);
  lcd.print(tempF);

  lcd.setCursor(9, 0);
  lcd.print("D");
  lcd.setCursor(11, 0);
  lcd.print(dewpF);

  lcd.setCursor(0, 1);
  lcd.print("H");
  lcd.setCursor(2, 1);
  lcd.print(humidity);

  lcd.setCursor(9, 1);
  lcd.print("d");
  lcd.setCursor(11, 1);
  lcd.print(temp_dp_deltaF);
}

void printMoneyDisplay(const double temp_dp_deltaF)
{
  if (temp_dp_deltaF < 5) {
    lcd.write((uint8_t)0);
    lcd.write((uint8_t)0);
    lcd.write((uint8_t)0);
    lcd.write((uint8_t)0);
  }
  else if (temp_dp_deltaF < 10) {
    lcd.write((uint8_t)0);
    lcd.write((uint8_t)0);
    lcd.write((uint8_t)0);
  }
  else if (temp_dp_deltaF < 20) {
    lcd.write((uint8_t)0);
    lcd.write((uint8_t)0);
  }
  else {
    lcd.write((uint8_t)0);
  }
}

