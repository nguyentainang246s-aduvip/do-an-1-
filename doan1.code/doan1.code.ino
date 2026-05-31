//#include "wifiConfig.h" // Đã bỏ file này
#include "DHT.h"       
#include <Wire.h>       
#include <BH1750.h>    

/* ---- THÊM THƯ VIỆN OLED SH1106 ---- */
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

/* ---- Cấu hình Blynk ---- */
#define BLYNK_TEMPLATE_ID "TMPL60eS-otKb"
#define BLYNK_TEMPLATE_NAME "do nhiet do do am"
#define BLYNK_AUTH_TOKEN "qZaJoeTn0ivQM3l02n0VYfEQQuH7kROO"
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp32.h>

/* ---- THÔNG TIN WIFI CỦA BẠN (NHẬP VÀO ĐÂY) ---- */
char ssid[] = "ahihi"; // <--- Nhập tên Wifi
char pass[] = "31072009";    // <--- Nhập mật khẩu Wifi

/* ---- Cấu hình OLED SH1106 (1.3 inch) ---- */
#define i2c_Address 0x3c 
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET -1   
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/* ---- Cấu hình DHT22 ---- */
#define DHTPIN 4       
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

/* ---- Cấu hình BH1750 ---- */
BH1750 lightMeter;

/* ---- Cấu hình Cảm biến Độ ẩm Đất ---- */
#define MOISTURE_PIN 34
#define MOISTURE_AIR_VALUE 3000     
#define MOISTURE_WATER_VALUE 1200   

/* ---- Cấu hình RELAY ---- */
const int relayLightPin = 23; // Relay 1: Đèn
const int relayPumpPin = 26;  // Relay 2: Bơm

// Mức kích Relay
#define RELAY_ON HIGH   
#define RELAY_OFF LOW   

/* ---- Ngưỡng Tự Động ---- */
#define LIGHT_THRESHOLD_ON 30         
#define LIGHT_THRESHOLD_OFF 50         
#define SOIL_THRESHOLD_DRY 60  
#define SOIL_THRESHOLD_WET 80

/* ---- Biến điều khiển ---- */
bool isAutoMode = true;     
int manualLightState = 0;   
int manualPumpState = 0;    
bool isLightOn = false;     
bool isPumpOn = false;      

/* ---- Biến toàn cục & Timer ---- */
BlynkTimer timer;

/* ---- Đồng bộ Blynk khi kết nối lại ---- */
BLYNK_CONNECTED() {
  Serial.println("Syncing...");
  Blynk.syncVirtual(V8, V9, V12);
}

/* ---- V8: Chế độ Auto/Manual ---- */
BLYNK_WRITE(V8) {
  isAutoMode = param.asInt();
  Serial.println(isAutoMode ? "Mode: AUTO" : "Mode: MANUAL");
}

/* ---- V9: Nút Đèn (Manual) ---- */
BLYNK_WRITE(V9) {
  manualLightState = param.asInt();
  if (!isAutoMode) {
    digitalWrite(relayLightPin, manualLightState ? RELAY_ON : RELAY_OFF);
    isLightOn = manualLightState;
    Blynk.virtualWrite(V10, isLightOn ? 255 : 0);
  }
}

/* ---- V12: Nút Bơm (Manual) ---- */
BLYNK_WRITE(V12) {
  manualPumpState = param.asInt();
  if (!isAutoMode) {
    digitalWrite(relayPumpPin, manualPumpState ? RELAY_ON : RELAY_OFF);
    isPumpOn = manualPumpState;
  }
}

// --- HÀM CẬP NHẬT MÀN HÌNH OLED (TIẾNG VIỆT) ---
void updateOLED(float t, float h, float lux, int moisture, bool autoMode) {
  display.clearDisplay(); 
  
  // --- DÒNG 1: CHẾ ĐỘ & WIFI ---
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  
  display.setCursor(0, 0);
  display.print(autoMode ? "TU DONG" : "THU CONG");
  
  // Hiển thị trạng thái Wifi 
  display.setCursor(90, 0);
  // Nếu đã kết nối blynk thì hiển thị OK
  display.print(Blynk.connected() ? "WIFI:OK" : "MAT NET");

  // Dòng kẻ ngang
  display.drawLine(0, 10, 128, 10, SH110X_WHITE);

  // --- DÒNG 2: NHIỆT ĐỘ ---
  display.setCursor(0, 15);
  display.print("Nhiet do: "); display.print(t, 1); display.print("C");
  
  // --- DÒNG 3: ĐỘ ẨM KHÔNG KHÍ ---
  display.setCursor(0, 27);
  display.print("Do am KK: "); display.print(h, 1); display.print("%");

  // --- DÒNG 4: ÁNH SÁNG & TRẠNG THÁI ĐÈN ---
  display.setCursor(0, 39);
  display.print("A.Sang  : "); display.print(lux, 0); 
  
  // Trạng thái Đèn
  display.setCursor(95, 39);
  display.print(isLightOn ? "D:BAT" : "D:TAT");

  // --- DÒNG 5: ĐỘ ẨM ĐẤT & TRẠNG THÁI BƠM ---
  display.setCursor(0, 51);
  display.print("Dat     : "); display.print(moisture); display.print("%");

  // Trạng thái Bơm
  display.setCursor(95, 51);
  display.print(isPumpOn ? "B:BAT" : "B:TAT");

  display.display(); 
}

// --- HÀM XỬ LÝ CHÍNH (ĐỌC CẢM BIẾN & LOGIC) ---
void sendSensorData()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float lux = lightMeter.readLightLevel();

  int rawMoisture = analogRead(MOISTURE_PIN);
  int moisturePercent = map(rawMoisture, MOISTURE_AIR_VALUE, MOISTURE_WATER_VALUE, 0, 100);
  moisturePercent = constrain(moisturePercent, 0, 100);

  // Kiểm tra lỗi DHT
  if (isnan(h) || isnan(t)) {
    Serial.println("Loi doc DHT!");
    t = 0; h = 0; 
  }

  Serial.print("T:"); Serial.print(t, 1);
  Serial.print("|H:"); Serial.print(h, 1);
  Serial.print("|L:"); Serial.print(lux, 0);
  Serial.print("|M:"); Serial.print(moisturePercent); Serial.print("%");

  // --- LOGIC TỰ ĐỘNG ---
  if (isAutoMode) {
    // 1. Đèn 
    if (lux >= 0) {
      if (!isLightOn && lux < LIGHT_THRESHOLD_ON) {
        digitalWrite(relayLightPin, RELAY_ON); 
        isLightOn = true;
      } else if (isLightOn && lux > LIGHT_THRESHOLD_OFF) {
        digitalWrite(relayLightPin, RELAY_OFF); 
        isLightOn = false;
      }
    }

    // 2. Bơm 
    if (!isPumpOn && moisturePercent < SOIL_THRESHOLD_DRY) {
      digitalWrite(relayPumpPin, RELAY_ON); 
      isPumpOn = true;
      Serial.println(" | Bom ON");
    } else if (isPumpOn && moisturePercent > SOIL_THRESHOLD_WET) {
      digitalWrite(relayPumpPin, RELAY_OFF); 
      isPumpOn = false;
      Serial.println(" | Bom OFF");
    } else {
      Serial.println();
    }
  }
  // --- LOGIC THỦ CÔNG ---
  else {
    digitalWrite(relayLightPin, isLightOn ? RELAY_ON : RELAY_OFF);
    digitalWrite(relayPumpPin, isPumpOn ? RELAY_ON : RELAY_OFF);
    Serial.println(" | Manual");
  }
  
  // --- GỬI DỮ LIỆU LÊN BLYNK (Chỉ gửi nếu có kết nối) ---
  if (Blynk.connected()) {
    Blynk.virtualWrite(V10, isLightOn ? 255 : 0);
    if (!isnan(h) && !isnan(t)) {
      Blynk.virtualWrite(V5, t);
      Blynk.virtualWrite(V6, h);
    }
    if (lux >= 0) Blynk.virtualWrite(V7, lux);
    Blynk.virtualWrite(V11, moisturePercent);
  }

  // --- CẬP NHẬT OLED ---
  updateOLED(t, h, lux, moisturePercent, isAutoMode);
}

void setup() {
  Serial.begin(115200);
  
  // Khởi động giao tiếp I2C 
  Wire.begin();       
  
  dht.begin();        
  lightMeter.begin();
  
  // --- KHỞI ĐỘNG OLED SH1106 ---
  delay(100);
  if(!display.begin(i2c_Address, true)) { 
    Serial.println(F("Khong tim thay OLED SH1106"));
  } else {
    Serial.println(F("OLED SH1106 Started"));
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    
    // Màn hình chào
    display.setCursor(15, 20); 
    display.println("VUON THONG MINH"); 
    display.setCursor(30, 35);
    display.println("Dang khoi dong...");
    display.display();
    delay(2000); 
  }

  pinMode(MOISTURE_PIN, INPUT);
  pinMode(relayLightPin, OUTPUT);
  pinMode(relayPumpPin, OUTPUT);
  
  digitalWrite(relayLightPin, RELAY_OFF);
  digitalWrite(relayPumpPin, RELAY_OFF);

  // --- KẾT NỐI WIFI & BLYNK (Code mới) ---
  // Hàm này sẽ cố gắng kết nối Wifi và Blynk server.
  // Lưu ý: Nó có thể làm dừng chương trình ở đây nếu không có mạng.
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  
  // Thiết lập timer 
  timer.setInterval(2000L, sendSensorData);
}

void loop() {
  Blynk.run(); // Xử lý kết nối Blynk
  timer.run(); // Chạy bộ đếm thời gian đọc cảm biến
}