#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <time.h>

#define Row0 0
#define Row1 1
#define Row2 2
#define Row3 3
#define A_ 10
#define B_ 11
#define C_ 12
#define D_ 13
#define E_ 14
#define F_ 15
#define G_ 16
#define H_ 17
#define I_ 18
#define J_ 19
#define K_ 20
#define L_ 21
#define M_ 22
#define N_ 23
#define O_ 24
#define P_ 25
#define Q_ 26
#define R_ 27
#define S_ 28
#define T_ 29
#define U_ 30
#define V_ 31
#define W_ 32
#define X_ 33
#define Y_ 34
#define Z_ 35
#define B 0x20

LiquidCrystal_I2C lcd(0x27, 20, 4);
// NodeMCU Dev Kit => D1 = SCL, D2 = SDA

const char *Mon[] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
const char *Day[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

//Wi-Fi
#define ssid1 "hogeid1"
#define password1 "hogepw1"
#define ssid2 "hogeid2"
#define password2 "hogepw2"
String newHostname = "Clock2004LCD";

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
ESP8266WiFiMulti wifiMulti;

//NTP client
int timezone = 9;
char ntp_server1[20] = "ntp.nict.jp";
char ntp_server2[20] = "ntp.jst.mfeed.ad.jp";
char ntp_server3[20] = "time.aws.com";
int dst = 0;

int msecond, second, minute, hour, dayOfWeek, dayOfMonth, month, year, dots;

unsigned long lastSecond = millis();
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
unsigned long scrollMillis = 0;
unsigned long lastUpdateTime = 0;
unsigned long lastUpdateData = 0;
unsigned int stringLength;
char timeBuffer[59];
const int MESSAGE_LENGTH = sizeof(timeBuffer);
const int DISPLAY_WIDTH = 20;
int g_nPosition = 0;
String upperSec = "0";
String underSec = "0";


void initWiFi() {
  WiFi.hostname(newHostname.c_str());
  WiFi.mode(WIFI_STA);
//  WiFi.begin(ssid1, password1);
  wifiMulti.addAP(ssid1,password1);
  wifiMulti.addAP(ssid2,password2);
  Serial.print("Connecting to WiFi ..");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
}

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  Serial.println("Connected to Wi-Fi sucessfully.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());
  Serial.printf("hostname: %s\n", WiFi.hostname().c_str());
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  Serial.println("Disconnected from Wi-Fi, trying to connect...");
  WiFi.disconnect();
  WiFi.hostname(newHostname.c_str());
  while (wifiMulti.run() != WL_CONNECTED) {
  Serial.print('.');
  delay(1000);
  }
}

void DefineLargeChar() {

  uint8_t cc1[8] = { // Custom Character 1
    B11100,
    B11110,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111
  };

  uint8_t cc2[8] = { // Custom Character 2
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B00000,
    B00000,
    B00000
  };

  uint8_t cc3[8] = { // Custom Character 3
    B00000,
    B00000,
    B00000,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111
  };

  uint8_t cc4[8] = { // Custom Character 4
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B01111,
    B00111
  };

  uint8_t cc5[8] = { // Custom Character 5
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B00000,
    B00000
  };

  uint8_t cc6[8] = { // Custom Character 6
    B00000,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B00000,
    B00000
  };

  uint8_t cc7[8] = { // Custom Character 7
    B00000,
    B11100,
    B11110,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111
  };

  uint8_t cc8[8] = { // Custom Character 8
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111
  };

  // send custom characters to the display
  lcd.createChar(1, cc1);
  lcd.createChar(2, cc2);
  lcd.createChar(3, cc3);
  lcd.createChar(4, cc4);
  lcd.createChar(5, cc5);
  lcd.createChar(6, cc6);
  lcd.createChar(7, cc7);
  lcd.createChar(8, cc8);
}

// 0 1 2 3 4 5 6 7 8 9 A B C D E F G H I J K L M N O P Q R S T U V W X Y Z BLANK
char bn1[] = {
  8, 2, 1, 2, 1, B, 2, 2, 1, 2, 2, 1, 3, B, 8, 8, 2, 2, 8, 2, 2, 2, 2, 8, 8, 2, 1, 8, 2, 1, 8, 2, 1, 8, 2, 1, 3, 2, 2, 8, 2, 1, 8, 2, 2, 8, 2, 2, 8, 2, 1, 8, B, 8, B, 8, B, B, B, 8, 8, B, 8, 8, B, B, 8, 3, 1, 8, B, 1, 8, 2, 1, 8, 2, 1, 8, 2, 1, 8, 2, 1, 8, 2, 5, 2, 8, 2, 8, B, 8, 8, B, 8, 8, B, 8, 8, B, 8, 8, B, 8, 2, 2, 1, B, B, B
};
char bn2[] = {
  8, B, 8, B, 8, B , 3, 2, 2, B, 6, 1, 4, 3, 8, 5, 6, 7, 8, 6, 7, B, 3, 2, 8, 6, 8, 5, 6, 8, 8, 6, 8, 8, 6, 1, 8, B, B, 8, B, 8, 8, 6, B, 8, 6, B, 8, B, 3, 8, 6, 8, B, 8, B, B, B, 8, 8, 6, 2, 8, B, B, 8, B, 8, 8, 4, 8, 8, B, 8, 8, 2, 2, 8, B, 8, 8, 3, 8, 2, 2, 1, B, 8, B, 8, B, 8, 8, B, 8, 8, B, 8, B, 6, B, 2, 3, 2, B, 6, B, B, B, B
};
char bn3[] = {
  4, 3, 8, 3, 8, 3, 8, 3, 3, 3, 3, 8, B, B, 8, 4, 3, 8, 4, 3, 8, B, 8, B, 4, 3, 8, B, B, 8, 8, B, 8, 8, 3, 8, 4, 3, 3, 8, 3, 8, 8, 3, 3, 8, B, B, 4, 3, 8, 8, B, 8, B, 8, B, 4, 8, B, 8, B, 1, 8, 3, 3, 8, B, 8, 8, B, 8, 4, 3, 8, 8, B, B, 4, 3, 7, 8, B, 1, 4, 3, 8, B, 8, B, 4, 3, 8, 2, 3, 2, 4, 2, 8, 8, B, 8, B, 8, B, 4, 3, 3, B, B, B
};

void printTwoNumber(uint8_t number, uint8_t position)//13
{
  // Print position is hardcoded
  int digit0; // To represent the ones
  int digit1; // To represent the tens
  digit0 = number % 10;
  digit1 = number / 10;

  // Line 1 of the two-digit number
  lcd.setCursor(position, 0);
  lcd.write(bn1[digit1 * 3]);
  lcd.write(bn1[digit1 * 3 + 1]);
  lcd.write(bn1[digit1 * 3 + 2]);
  lcd.write(B); // Blank
  lcd.write(bn1[digit0 * 3]);
  lcd.write(bn1[digit0 * 3 + 1]);
  lcd.write(bn1[digit0 * 3 + 2]);

  // Line 2 of the two-digit number
  lcd.setCursor(position, 1);
  lcd.write(bn2[digit1 * 3]);
  lcd.write(bn2[digit1 * 3 + 1]);
  lcd.write(bn2[digit1 * 3 + 2]);
  lcd.write(B); // Blank
  lcd.write(bn2[digit0 * 3]);
  lcd.write(bn2[digit0 * 3 + 1]);
  lcd.write(bn2[digit0 * 3 + 2]);

  // Line 3 of the two-digit number
  lcd.setCursor(position, 2);
  lcd.write(bn3[digit1 * 3]);
  lcd.write(bn3[digit1 * 3 + 1]);
  lcd.write(bn3[digit1 * 3 + 2]);
  lcd.write(B); // Blank
  lcd.write(bn3[digit0 * 3]);
  lcd.write(bn3[digit0 * 3 + 1]);
  lcd.write(bn3[digit0 * 3 + 2]);
}

void printColons(uint8_t position)
{
  lcd.setCursor(position, 0);
  lcd.write (3);
  lcd.setCursor(position, 2);
  lcd.write (6);
}

void printNoColons(uint8_t position)
{
  lcd.setCursor(position, 0);
  lcd.write (B);
  lcd.setCursor(position, 2);
  lcd.write (B);
}

void setup() {
  Serial.begin(115200);

  //Register event handlers
  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);
   
  initWiFi();

  configTime(timezone * 3600, dst, ntp_server1, ntp_server2, ntp_server3);
  Serial.println("Waiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(500);
  }

  lcd.init();
  lcd.backlight();
  DefineLargeChar(); // Create the custom characters
  lcd.clear();

  updateTime();
  updateData();
}

void loop() {
  currentMillis = millis();

  if ((currentMillis - lastUpdateTime) > 300000) updateTime();
  if ((currentMillis - lastUpdateData) > 600000) updateData();
  if (year < 2000) updateTime();

  dots = (currentMillis % 1000) < 500;
  if (dots) printColons(9); else printNoColons(9);

  if ((millis() - lastSecond) > 1000) {
    lastSecond = millis();
    second++;
    if (second > 59) {
      second = 0;
      minute++;
      if (minute > 59) {
        minute = 0;
        hour++;
        if (hour > 23) hour = 0;
      }
    }

    if (second < 10) {
      upperSec = "0";
      underSec = (String(second)).substring(0, 1);
    } else {
      upperSec = (String(second)).substring(0, 1);
      underSec = (String(second)).substring(1, 2);
    }

    printTwoNumber(hour, 1);
    printTwoNumber(minute, 11);
    lcd.setCursor(19, 0); lcd.print(upperSec);
    lcd.setCursor(19, 1); lcd.print(underSec);
  }

  concatenate();
  scrollDisplay(3, 350);
}

void updateData() {
  lastUpdateData = millis();
}

void updateTime() {
  lastUpdateTime = millis();
  time_t now = time(nullptr);
  struct tm* newtime = localtime(&now);

  year = String(newtime->tm_year + 1900).toInt();
  month = String(newtime->tm_mon + 1).toInt();
  dayOfMonth = String(newtime->tm_mday).toInt();
  dayOfWeek = String(newtime->tm_wday).toInt();
  hour = String(newtime->tm_hour).toInt();
  minute = String(newtime->tm_min).toInt();
  second = String(newtime->tm_sec).toInt();
}

void concatenate() {
  int i;
  memset(timeBuffer, ' ', (MESSAGE_LENGTH - 1));
//  snprintf(timeBuffer, (MESSAGE_LENGTH - 1), "  %s %02d % s %02d %02d\337C % s ", Day[dayOfWeek], dayOfMonth, Mon[month], year, temperature, conChar);
  snprintf(timeBuffer, (MESSAGE_LENGTH - 1), " %02d/%02d/%02d %s", year, month, dayOfMonth, Day[dayOfWeek]);

  for (i = 0; i < 3; i++) {
    timeBuffer[(MESSAGE_LENGTH - 2) + i] = ' ';
  }

  String timestr = String(timeBuffer);
  stringLength = timestr.length();
}

void scrollDisplay(int line, int speed) {
  int i;
  if (stringLength < DISPLAY_WIDTH) {
  for (i = 0; i < DISPLAY_WIDTH; i++) {
    lcd.setCursor(i, line);
    lcd.print(timeBuffer[g_nPosition + i]);
    lcd.print(timeBuffer[stringLength] = ' ');  
  }
  }
  else if ((currentMillis - scrollMillis) > speed) {
    scrollMillis = millis();

    if (g_nPosition < stringLength - DISPLAY_WIDTH) {
      for (i = 0; i < DISPLAY_WIDTH; i++) {
        lcd.setCursor(i, line);
        lcd.print(timeBuffer[g_nPosition + i]);
        lcd.print(timeBuffer[stringLength] = ' ');
      }
    } else {
      int nChars = stringLength - g_nPosition;
      for (i = 0; i < nChars; i++) {
        lcd.setCursor(i, line);
        lcd.print(timeBuffer[g_nPosition + i]);
        lcd.print(timeBuffer[stringLength] = ' ');
      }
      for (i = 0; i < (DISPLAY_WIDTH - nChars); i++) {
        lcd.setCursor(nChars + i, line);
        lcd.print(timeBuffer[i]);
        lcd.print(timeBuffer[stringLength] = ' ');
      }
    }

    g_nPosition++;
    if (g_nPosition >= stringLength) {
      g_nPosition = 0;
    }
  }
}
