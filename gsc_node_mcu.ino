#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <TinyGPS.h>
#include <math.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <DFRobotDFPlayerMini.h>

#define LED_PIN D4

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
TinyGPS gps;
DFRobotDFPlayerMini myDFPlayer;

SoftwareSerial ss(D2, D1);
SoftwareSerial gps_serial(D6, D5);
SoftwareSerial speaker_serial(D3, D7);
LiquidCrystal_I2C lcdMonitor(0x27, 16, 2);

const char *host = "ajou-epas.xyz";
uint16_t port = 7002;
const char *urlPath = "/ws/my-location";

bool socketConnected = false;
unsigned long lastUpdateTime = 0;
const long updateInterval = 2000;

float latitude, longitude, speed, course, altitude, satellites;
unsigned long hdop, age;
int year;
byte month, day, hour, minute, second, hundredths;
char timestamp[30];

void connectWifi()
{
  lcdMonitor.clear();
  lcdMonitor.setCursor(0, 0);
  lcdMonitor.print("Connecting Wifi");
  Serial.println("[WIFI] Connecting WiFi...");

  WiFiMulti.addAP("Bandall2.4G", "sunf6678");

  while (WiFiMulti.run() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("[WIFI] WiFi connected");
  Serial.print("[WIFI] IP address: ");
  Serial.println(WiFi.localIP());
  lcdMonitor.setCursor(0, 1);
  lcdMonitor.print(WiFi.localIP());
  delay(1000);
}

void setLcd()
{
  lcdMonitor.begin();
  lcdMonitor.backlight();
  lcdMonitor.setCursor(0, 0);
  lcdMonitor.print("Waiting....");
}

void setSpeaker()
{
  if (!myDFPlayer.begin(speaker_serial, false))
  {
    Serial.println("[MP3] Unable to Start");
  }

  myDFPlayer.volume(25);

  pinMode(LED_PIN, OUTPUT);
}

void setup()
{
  Serial.begin(115200);
  gps_serial.begin(9600);
  speaker_serial.begin(9600);

  setLcd();
  setSpeaker();
  delay(3000);
  connectWifi();

  latitude = 127.105997;
  longitude = 37.352690;
  course = 10;
  // webSocket.begin(host, port, urlPath);
  webSocket.beginSSL(host, port, urlPath);
  webSocket.onEvent(onWebSocketEvent);
  webSocket.setReconnectInterval(5000);
}

void sendInitMessage()
{
  StaticJsonDocument<200> doc;
  doc["requestType"] = "INIT";
  String message;
  serializeJson(doc, message);
  webSocket.sendTXT(message);
}

void sendUpdateMessage(double longitude, double latitude, bool isUsingNavi, double meterPerSec, double direction, const char *timestamp)
{
  StaticJsonDocument<512> doc;
  doc["requestType"] = "UPDATE";
  JsonObject data = doc.createNestedObject("data");
  data["longitude"] = longitude;
  data["latitude"] = latitude;
  data["isUsingNavi"] = isUsingNavi;
  data["meterPerSec"] = meterPerSec;
  data["direction"] = direction;
  data["timestamp"] = timestamp;
  String message;
  serializeJson(doc, message);
  webSocket.sendTXT(message);
}

void onWebSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case WStype_DISCONNECTED:
    Serial.println("[WSc] Disconnected from Server. Trying to Reconnect");
    socketConnected = false;
    break;
  case WStype_CONNECTED:
    Serial.println("[WSc] Connected to Server");
    sendInitMessage();
    break;
  case WStype_TEXT:
    Serial.printf("[WSc] get text: %s\n", payload);
    delegateHandleResponse(payload, length);
    break;
  }
}

void delegateHandleResponse(uint8_t *payload, size_t length)
{
  StaticJsonDocument<4096> doc;
  DeserializationError error = deserializeJson(doc, payload, length);

  if (error)
  {
    Serial.print(F("[MSG] deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  const char *messageType = doc["messageType"].as<const char *>();
  JsonObject data = doc["data"].as<JsonObject>();

  if (strcmp(messageType, "RESPONSE") == 0)
  {
    handleUpdateResponse(data);
  }
  else if (strcmp(messageType, "ALERT") == 0)
  {
    handleAlert(data);
  }
  else if (strcmp(messageType, "ALERT_UPDATE") == 0)
  {
  }
  else if (strcmp(messageType, "ALERT_END") == 0)
  {
    handleAlertEnd(data);
  }
  else
  {
    Serial.println("[MSG] Unknown messageType");
  }
}

void handleUpdateResponse(const JsonObject &data)
{
  if (data["vehicleStatusId"])
  {
    handleVehicleStatusId(data);
  }
  else
  {
    handleLocationConfidence(data);
  }
}

void handleVehicleStatusId(const JsonObject &data)
{
  const char *vehicleStatusId = data["vehicleStatusId"].as<const char *>();
  Serial.println("[MSG] Vehicle status ID received. Connection successful.");
  socketConnected = true;
}

void handleLocationConfidence(const JsonObject &data)
{
  if (data["location"]["confidence"].isNull())
  {
    Serial.println("[MSG] Confidence is null");
  }
  else
  {
    float confidence = data["location"]["confidence"].as<float>();
    Serial.print("[MSG] confidence: ");
    Serial.println(confidence);
  }
}

void handleAlert(const JsonObject &data)
{
  // emergencyEventId, licenseNumber, vehicleType 추출
  int emergencyEventId = data["emergencyEventId"];
  const char *licenseNumber = data["licenseNumber"];
  const char *vehicleType = data["vehicleType"];

  int currentPathPoint = data["currentPathPoint"];
  double currentLatitude = data["pathPoints"][currentPathPoint]["location"][0];
  double currentLongitude = data["pathPoints"][currentPathPoint]["location"][1];

  Serial.print("[MSG] Emergency Event ID: ");
  Serial.println(emergencyEventId);
  Serial.print("[MSG] License Number: ");
  Serial.println(licenseNumber);
  Serial.print("[MSG] Vehicle Type: ");
  Serial.println(vehicleType);
  Serial.print("[MSG] Current Latitude: ");
  Serial.println(currentLatitude, 6);
  Serial.print("[MSG] Current Longitude: ");
  Serial.println(currentLongitude, 6);

  double relativeBearing = calculateBearing(latitude, longitude, currentLatitude, currentLongitude, course);
  Serial.print("[MSG] Relative Bearing to Emergency Vehicle: ");
  Serial.println(relativeBearing, 2);

  // 상대적 방위각을 문자열로 변환
  String direction = determineDirection(relativeBearing);
  addEmergencyVehicle(data, direction);
}

void handleAlertEnd(const JsonObject &data)
{
  const char *licenseNumber = data["licenseNumber"];
  removeEmergencyVehicle(data);
}

void loop()
{
  bool isDataReady = smartdelay(2000);
  // sendUpdateMessage(127.105997, 37.352690, false, 11, 10, "2024-04-13T12:34:56.789Z");
  // displayVehicleInfo();
  // findNotAlertedEmergencyVehicle();
  // return;

  if (myDFPlayer.available())
  {
    printDetail(myDFPlayer.readType(), myDFPlayer.read());
  }

  if (!isDataReady || !socketConnected)
  {
    return;
  }

  extractGpsData();
  displayGpsData();
  printGpsVariables();
  sendUpdateMessage(longitude, latitude, false, speed / 3.6, course, timestamp);
  displayVehicleInfo();
  findNotAlertedEmergencyVehicle();
}

void printDetail(uint8_t type, int value)
{
  switch (type)
  {
  case TimeOut:
    Serial.println(F("Time Out!"));
    break;
  case WrongStack:
    Serial.println(F("Stack Wrong!"));
    break;
  case DFPlayerCardInserted:
    Serial.println(F("Card Inserted!"));
    break;
  case DFPlayerCardRemoved:
    Serial.println(F("Card Removed!"));
    break;
  case DFPlayerCardOnline:
    Serial.println(F("Card Online!"));
    break;
  case DFPlayerUSBInserted:
    Serial.println("USB Inserted!");
    break;
  case DFPlayerUSBRemoved:
    Serial.println("USB Removed!");
    break;
  case DFPlayerPlayFinished:
    Serial.print(F("Number:"));
    Serial.print(value);
    Serial.println(F(" Play Finished!"));
    break;
  case DFPlayerError:
    Serial.print(F("DFPlayerError:"));
    switch (value)
    {
    case Busy:
      Serial.println(F("Card not found"));
      break;
    case Sleeping:
      Serial.println(F("Sleeping"));
      break;
    case SerialWrongStack:
      Serial.println(F("Get Wrong Stack"));
      break;
    case CheckSumNotMatch:
      Serial.println(F("Check Sum Not Match"));
      break;
    case FileIndexOut:
      Serial.println(F("File Index Out of Bound"));
      break;
    case FileMismatch:
      Serial.println(F("Cannot Find File"));
      break;
    case Advertise:
      Serial.println(F("In Advertise"));
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

static bool smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  bool result = false;
  while (millis() - start < ms)
  {
    if (gps_serial.available())
    {
      char c = gps_serial.read();
      result |= gps.encode(c);
    }
    webSocket.loop();
  }
  return result;
}

void displayGpsData()
{
  lcdMonitor.clear();
  lcdMonitor.setCursor(0, 0);
  lcdMonitor.print("Lat: ");
  lcdMonitor.print(latitude, 6);
  lcdMonitor.setCursor(0, 1);
  lcdMonitor.print("Lon: ");
  lcdMonitor.print(longitude, 6);
}

void extractGpsData()
{
  gps.f_get_position(&latitude, &longitude);
  speed = gps.f_speed_kmph();
  course = gps.f_course();
  altitude = gps.f_altitude();
  satellites = gps.satellites();
  hdop = gps.hdop();
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);

  if (age == TinyGPS::GPS_INVALID_AGE)
  {
    Serial.println("Time data is INVALID");
  }
  else
  {
    // byte 타입을 int 타입으로 변환
    int yearInt = year;
    int monthInt = month;
    int dayInt = day;
    int hourInt = hour;
    int minuteInt = minute;
    int secondInt = second;

    // int 타입 변수를 사용하여 sprintf 함수 호출
    sprintf(timestamp, "%04d-%02d-%02dT%02d:%02d:%02dZ", yearInt, monthInt, dayInt, hourInt, minuteInt, secondInt);
  }
}

void printGpsVariables()
{
  Serial.print("Lat: ");
  Serial.print(latitude, 6);
  Serial.print(", Lon: ");
  Serial.print(longitude, 6);
  Serial.print(", Speed: ");
  Serial.print(speed, 2);
  Serial.print(" km/h, Dir: ");
  Serial.print(course, 2);
  Serial.print(" deg, Alt: ");
  Serial.print(altitude, 2);
  Serial.print(" m, Sats: ");
  Serial.print(satellites);
  Serial.print(", HDOP: ");
  Serial.print(hdop, 2);
  Serial.print(", Time: ");
  Serial.println(timestamp);
}

double calculateBearing(double lat1, double lon1, double lat2, double lon2, double currentBearing)
{
  double dLon = (lon2 - lon1) * PI / 180.0;
  lat1 = lat1 * PI / 180.0;
  lat2 = lat2 * PI / 180.0;
  lon1 = lon1 * PI / 180.0;
  lon2 = lon2 * PI / 180.0;

  double y = sin(dLon) * cos(lat2);
  double x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dLon);
  double bearing = atan2(y, x) * 180.0 / PI - currentBearing;

  bearing = fmod((bearing + 360.0), 360.0);

  return bearing;
}

String determineDirection(double bearing)
{
  if (bearing >= 337.5 || bearing < 22.5)
  {
    return "N";
  }
  else if (bearing >= 22.5 && bearing < 67.5)
  {
    return "NE";
  }
  else if (bearing >= 67.5 && bearing < 112.5)
  {
    return "E";
  }
  else if (bearing >= 112.5 && bearing < 157.5)
  {
    return "SE";
  }
  else if (bearing >= 157.5 && bearing < 202.5)
  {
    return "S";
  }
  else if (bearing >= 202.5 && bearing < 247.5)
  {
    return "SW";
  }
  else if (bearing >= 247.5 && bearing < 292.5)
  {
    return "W";
  }
  else if (bearing >= 292.5 && bearing < 337.5)
  {
    return "NW";
  }
  else
  {
    return "--";
  }
}

// -----------

struct EmergencyVehicle
{
  int eventId;
  String licenseNumber;
  String vehicleType;
  double latitude;
  double longitude;
  String direction;
  bool isAlerted;
};

const int MAX_VEHICLES = 20;
EmergencyVehicle emergencyVehicles[MAX_VEHICLES];
int vehicleCount = 0;
int currentPrintedVehicleIdx = 0;
unsigned long lastAlertTime = 0;

void addEmergencyVehicle(const JsonObject &data, String direction)
{
  if (vehicleCount >= MAX_VEHICLES)
  {
    Serial.println("[ERROR] Emergency vehicle storage is full.");
    return;
  }

  String newLicenseNumber = data["licenseNumber"].as<String>();
  int currentPathPoint = data["currentPathPoint"];

  // 이미 차량 정보가 존재할 때
  for (int i = 0; i < vehicleCount; i++)
  {
    if (emergencyVehicles[i].licenseNumber == newLicenseNumber)
    {
      emergencyVehicles[i].latitude = data["pathPoints"][currentPathPoint]["location"][0];
      emergencyVehicles[i].longitude = data["pathPoints"][currentPathPoint]["location"][1];
      emergencyVehicles[i].direction = direction;
      emergencyVehicles[vehicleCount].isAlerted = false;
      Serial.println("[INFO] Updated existing vehicle's location.");
      return;
    }
  }

  emergencyVehicles[vehicleCount].eventId = data["emergencyEventId"];
  emergencyVehicles[vehicleCount].licenseNumber = data["licenseNumber"].as<String>();
  emergencyVehicles[vehicleCount].vehicleType = data["vehicleType"].as<String>();
  emergencyVehicles[vehicleCount].latitude = data["pathPoints"][currentPathPoint]["location"][0];
  emergencyVehicles[vehicleCount].longitude = data["pathPoints"][currentPathPoint]["location"][1];
  emergencyVehicles[vehicleCount].direction = direction;
  emergencyVehicles[vehicleCount].isAlerted = false;
  vehicleCount++;
}

void removeEmergencyVehicle(const JsonObject &data)
{
  int index = -1;
  // 삭제할 차량의 인덱스 찾기
  for (int i = 0; i < vehicleCount; i++)
  {
    if (emergencyVehicles[i].licenseNumber == data["licenseNumber"].as<String>())
    {
      index = i;
      break;
    }
  }

  // 차량이 배열에 없는 경우
  if (index == -1)
  {
    Serial.println("[ERROR] Emergency vehicle not found.");
    return;
  }

  // 차량 정보 삭제 및 나머지 차량 정보 당기기
  for (int i = index; i < vehicleCount - 1; i++)
  {
    emergencyVehicles[i] = emergencyVehicles[i + 1];
  }

  emergencyVehicles[vehicleCount - 1] = EmergencyVehicle{};
  vehicleCount--;
  currentPrintedVehicleIdx--;
  Serial.println("[INFO] Emergency vehicle removed.");
}

void displayVehicleInfo()
{
  if (vehicleCount == 0)
  {
    displayGpsData();
    return;
  }

  currentPrintedVehicleIdx = (currentPrintedVehicleIdx + 1) % vehicleCount;

  lcdMonitor.clear();
  lcdMonitor.setCursor(0, 0);
  lcdMonitor.print("Emergency Alert!");
  lcdMonitor.setCursor(0, 1);
  lcdMonitor.print(emergencyVehicles[currentPrintedVehicleIdx].licenseNumber);

  if (emergencyVehicles[currentPrintedVehicleIdx].licenseNumber.length() > 11)
  {
    return;
  }

  String direction = emergencyVehicles[currentPrintedVehicleIdx].direction;
  if (direction.length() == 1)
  {
    lcdMonitor.setCursor(16 - 4, 1);
  }
  else
  {
    lcdMonitor.setCursor(16 - 5, 1);
  }

  lcdMonitor.print(emergencyVehicles[currentPrintedVehicleIdx].direction);

  if (vehicleCount > 9)
  {
    lcdMonitor.setCursor(16 - 3, 1);
    lcdMonitor.print("(+)");
  }
  else
  {
    lcdMonitor.setCursor(16 - 3, 1);
    lcdMonitor.print("(");
    lcdMonitor.print(vehicleCount);
    lcdMonitor.print(")");
  }
}

// ---------
void printAlertSpeaker(int vehicleIndex)
{
  int directionCode = 1;
  String direction = emergencyVehicles[vehicleIndex].direction;

  if (direction == "N")
    directionCode = 9;
  else if (direction == "NE")
    directionCode = 8;
  else if (direction == "E")
    directionCode = 7;
  else if (direction == "SE")
    directionCode = 6;
  else if (direction == "S")
    directionCode = 5;
  else if (direction == "SW")
    directionCode = 4;
  else if (direction == "W")
    directionCode = 3;
  else if (direction == "NW")
    directionCode = 2;
  else
    directionCode = 1;

  myDFPlayer.play(directionCode);
  Serial.print("[ALERT] Alerting ");
  Serial.println(emergencyVehicles[vehicleIndex].licenseNumber);
}

void findNotAlertedEmergencyVehicle()
{
  unsigned long currentTime = millis();
  if (currentTime - lastAlertTime >= 1000)
  {
    digitalWrite(LED_PIN, HIGH);
  }
  // 5초가 지나지 않았다면 스킵
  if (currentTime - lastAlertTime < 3300)
  {
    return;
  }

  for (int i = 0; i < vehicleCount; i++)
  {
    if (!emergencyVehicles[i].isAlerted)
    {
      printAlertSpeaker(i);
      emergencyVehicles[i].isAlerted = true;
      lastAlertTime = currentTime;
      digitalWrite(LED_PIN, LOW);
      break;
    }
  }
}
