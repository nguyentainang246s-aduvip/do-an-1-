# 🌿 Dự Án Vườn Thông Minh (Smart Garden IoT) - ESP32 & Blynk

Dự án **Vườn Thông Minh (Smart Garden)** là một hệ thống IoT hoàn chỉnh dựa trên vi điều khiển **ESP32**, giúp tự động hóa việc chăm sóc cây trồng bằng cách giám sát các chỉ số môi trường đất và không khí, đồng thời điều khiển thiết bị thông minh (Đèn chiếu sáng, Máy bơm nước) qua ứng dụng di động **Blynk** và hiển thị trực quan thông tin lên màn hình **OLED SH1106**.

---

## 🚀 Các Tính Năng Nổi Bật

- **Giám sát thời gian thực:**
  - Nhiệt độ & Độ ẩm không khí (cảm biến DHT22).
  - Cường độ ánh sáng (cảm biến BH1750).
  - Độ ẩm đất (cảm biến độ ẩm đất điện dung chống ăn mòn).
- **Màn hình hiển thị tại chỗ:** Sử dụng OLED SH1106 hiển thị thông số môi trường tiếng Việt không dấu và trạng thái kết nối WiFi/Blynk, chế độ hoạt động cực kỳ trực quan.
- **Hai chế độ vận hành:**
  - **Chế độ Tự Động (Auto Mode):** Hệ thống tự động bật/tắt thiết bị dựa trên ngưỡng tối ưu đã được thiết lập sẵn.
  - **Chế độ Thủ Công (Manual Mode):** Điều khiển bật/tắt Đèn và Máy bơm độc lập từ xa bất kỳ lúc nào qua ứng dụng Blynk.
- **Giám sát & Điều khiển từ xa (Blynk IoT):** Theo dõi số liệu trực quan bằng biểu đồ và điều khiển thông qua giao diện ứng dụng Blynk trên điện thoại di động hoặc trình duyệt web.

---

## 🛠️ Danh Sách Linh Kiện Phần Cứng

| STT | Linh Kiện | Mô tả | Số lượng |
| :--- | :--- | :--- | :---: |
| 1 | **ESP32 NodeMCU** | Board mạch phát triển trung tâm tích hợp WiFi/Bluetooth | 1 |
| 2 | **Cảm biến DHT22** | Đo nhiệt độ và độ ẩm không khí (độ chính xác cao hơn DHT11) | 1 |
| 3 | **Cảm biến BH1750** | Đo cường độ ánh sáng thực tế (đơn vị Lux), giao tiếp I2C | 1 |
| 4 | **Cảm biến độ ẩm đất** | Loại cảm biến điện dung (Capacitive) tránh bị ăn mòn điện hóa | 1 |
| 5 | **Màn hình OLED SH1106** | Kích thước 1.3 inch, độ phân giải 128x64, giao tiếp I2C | 1 |
| 6 | **Module Relay 2 Kênh** | Điều khiển Đèn chiếu sáng (AC/DC) và Máy bơm nước mini | 1 |
| 7 | **Máy bơm nước mini & Đèn** | Tải thực thi của hệ thống | 1 |
| 8 | **Nguồn cấp 5V** | Adapter cấp nguồn ổn định cho hệ thống | 1 |
| 9 | **Dây nối & Testboard** | Dây cắm đực-cái, cái-cái và linh kiện hỗ trợ đấu nối | - |

---

## 📌 Sơ Đồ Đấu Nối Chân (Pinout Connection)

Dưới đây là cấu hình kết nối các chân phần cứng đã được tối ưu hóa trong mã nguồn:

### 1. Kết Nối Cảm Biến & Màn Hình

| Thiết bị | Chân trên Thiết bị | Chân trên ESP32 | Ghi chú |
| :--- | :---: | :---: | :--- |
| **DHT22** | VCC | 3.3V / 5V | Nguồn cấp cảm biến |
| | GND | GND | Cực âm nguồn |
| | DATA | **GPIO 4** | Chân tín hiệu số |
| **BH1750** | VCC | 3.3V | Giao tiếp I2C nguồn 3.3V |
| | GND | GND | Cực âm nguồn |
| | SDA | **GPIO 21** | Chân dữ liệu I2C |
| | SCL | **GPIO 22** | Chân xung nhịp I2C |
| **OLED SH1106** | VCC | 3.3V / 5V | Giao tiếp I2C |
| | GND | GND | Cực âm nguồn |
| | SDA | **GPIO 21** | Chung bus I2C với BH1750 |
| | SCL | **GPIO 22** | Chung bus I2C với BH1750 |
| **Cảm Biến Đất** | VCC | 3.3V / 5V | Cảm biến độ ẩm đất analog |
| | GND | GND | Cực âm nguồn |
| | AOUT (Analog Out) | **GPIO 34** | Chân đầu vào Analog (ADC1) |

### 2. Kết Nối Relay Thiết Bị

| Thiết bị điều khiển | Chân Relay | Chân trên ESP32 | Ghi chú |
| :--- | :---: | :---: | :--- |
| **Relay 1 (ĐÈN)** | IN1 | **GPIO 23** | Mức kích: **HIGH** để BẬT đèn |
| **Relay 2 (BƠM)** | IN2 | **GPIO 26** | Mức kích: **HIGH** để BẬT bơm |

---

## ⚙️ Cơ Chế Hoạt Động Của Hệ Thống

### 1. Chế độ Tự Động (Auto Mode)

Hệ thống sẽ liên tục cập nhật dữ liệu cảm biến sau mỗi **2 giây** và thực hiện các logic so sánh với các ngưỡng cài đặt sẵn:

- **Điều khiển Đèn chiếu sáng:**
  - Nếu cường độ ánh sáng đo được **< 30 Lux**: Hệ thống tự động **BẬT** Đèn.
  - Nếu cường độ ánh sáng đo được **> 50 Lux**: Hệ thống tự động **TẮT** Đèn.
- **Điều khiển Máy bơm nước:**
  - Nếu độ ẩm đất đo được **< 60%** (Đất khô): Hệ thống tự động **BẬT** Máy bơm.
  - Nếu độ ẩm đất đo được **> 80%** (Đất đủ ẩm): Hệ thống tự động **TẮT** Máy bơm để tiết kiệm nước và tránh ngập úng.

### 2. Chế độ Thủ Công (Manual Mode)

Khi người dùng chuyển sang chế độ Thủ công trên ứng dụng Blynk:
- Hệ thống ngừng tự động so sánh ngưỡng cảm biến.
- Người dùng có toàn quyền **Bật/Tắt Đèn** và **Bật/Tắt Máy bơm** trực tiếp từ màn hình điều khiển Blynk bất kể các chỉ số môi trường hiện tại là bao nhiêu.

---

## 📱 Cấu Hình Giao Diện Blynk IoT

Để đồng bộ dữ liệu giữa ESP32 và ứng dụng di động Blynk, bạn cần tạo một **Template** trên Blynk IoT Console với các chân ảo (**Virtual Pins**) được cấu hình như sau:

| Tên Dữ Liệu | Chân Ảo (V-Pin) | Kiểu Dữ Liệu | Giá Trị | Hướng Truyền dữ liệu |
| :--- | :---: | :---: | :---: | :--- |
| **Nhiệt độ (Temperature)** | **V5** | Double | -40 -> 80 °C | ESP32 gửi lên Blynk |
| **Độ ẩm không khí (Humidity)**| **V6** | Double | 0 -> 100 % | ESP32 gửi lên Blynk |
| **Cường độ ánh sáng (Lux)** | **V7** | Double | 0 -> 65535 lx | ESP32 gửi lên Blynk |
| **Chế độ hoạt động (Mode)** | **V8** | Integer | 0 (Manual) / 1 (Auto) | Blynk điều khiển xuống ESP32 |
| **Nút Đèn (Light Switch)** | **V9** | Integer | 0 (Tắt) / 1 (Bật) | Blynk điều khiển xuống ESP32 |
| **Trạng thái Đèn (Feedback)** | **V10** | Integer | 0 -> 255 | ESP32 phản hồi trạng thái Đèn |
| **Độ ẩm đất (Soil Moisture)** | **V11** | Integer | 0 -> 100 % | ESP32 gửi lên Blynk |
| **Nút Bơm (Pump Switch)** | **V12** | Integer | 0 (Tắt) / 1 (Bật) | Blynk điều khiển xuống ESP32 |

---

## 🛠️ Hướng Dẫn Cài Đặt & Sử Dụng

### 1. Chuẩn bị môi trường lập trình
1. Tải và cài đặt **Arduino IDE** (phiên bản 1.8.x hoặc mới nhất 2.x).
2. Thêm hỗ trợ board mạch ESP32 bằng cách vào `File -> Preferences -> Additional Board Manager URLs` và dán đường dẫn:
   ```text
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. Vào `Tools -> Board -> Boards Manager`, tìm kiếm `esp32` và tiến hành cài đặt.

### 2. Cài đặt các thư viện Arduino cần thiết
Tìm kiếm và cài đặt các thư viện sau thông qua thư viện tích hợp `Sketch -> Include Library -> Manage Libraries...`:
- **Blynk** (Bản mới nhất phù hợp với Blynk IoT)
- **DHT Sensor Library** (bởi Adafruit)
- **Adafruit Unified Sensor** (bởi Adafruit - thư viện bổ trợ cho DHT)
- **BH1750** (bởi Christopher Laws)
- **Adafruit GFX Library** (bởi Adafruit)
- **Adafruit SH110X** (bởi Adafruit - hỗ trợ màn hình OLED SH1106)

### 3. Cấu hình thông tin kết nối trong Code
Mở file `doan1.code.ino` trong Arduino IDE, tìm đến các dòng sau và thay đổi theo tài khoản Blynk và mạng WiFi nhà bạn:

```cpp
/* ---- Cấu hình Blynk ---- */
#define BLYNK_TEMPLATE_ID "TMPLxxxxxxxxx"     // Thay bằng Template ID của bạn trên Blynk
#define BLYNK_TEMPLATE_NAME "xxxxxxxxxxxx"    // Thay bằng tên Template của bạn
#define BLYNK_AUTH_TOKEN "xxxxxxxxxxxxxxxx"   // Thay bằng mã Token thiết bị của bạn

/* ---- THÔNG TIN WIFI ---- */
char ssid[] = "Tên Wifi của bạn";             // Thay bằng tên Wifi
char pass[] = "Mật khẩu Wifi";                // Thay bằng mật khẩu Wifi
```

### 4. Hiệu chuẩn Cảm Biến Độ Ẩm Đất (Tùy chọn)
Mỗi cảm biến độ ẩm đất điện dung có thông số phần cứng hơi khác nhau. Bạn có thể hiệu chuẩn hai giá trị sau trong code để hiển thị % độ ẩm chính xác nhất:
```cpp
#define MOISTURE_AIR_VALUE 3000     // Giá trị đọc được khi để cảm biến ngoài không khí (khô hoàn toàn)
#define MOISTURE_WATER_VALUE 1200   // Giá trị đọc được khi ngâm cảm biến ngập trong nước (ẩm 100%)
```

### 5. Nạp Code (Upload)
1. Kết nối board ESP32 vào máy tính qua cáp Micro-USB / Type-C.
2. Tại Arduino IDE, chọn đúng cổng COM của thiết bị (`Tools -> Port`).
3. Chọn đúng board mạch (`Tools -> Board -> ESP32 Arduino -> ESP32 Dev Module`).
4. Nhấn nút **Upload** (mũi tên hướng sang phải) để tiến hành biên dịch và nạp code xuống ESP32.
5. Mở **Serial Monitor** với tốc độ baud `115200` để theo dõi quá trình khởi động, kết nối mạng và hoạt động của hệ thống.

---

## 📂 Cấu Trúc Thư Mục Dự Án
```text
doan1.code/
├── doan1.code.ino      # Mã nguồn chính điều khiển hệ thống
└── wifiConfig.h        # Thư viện tùy chọn cấu hình wifi qua Web Server/EEPROM (Đã được tắt ở bản hiện tại)
```

---

## 🤝 Hỗ Trợ & Phát Triển
Dự án được xây dựng với mục tiêu cung cấp giải pháp nông nghiệp thông minh dễ tiếp cận. Nếu bạn gặp bất kỳ vấn đề gì trong quá trình đấu nối phần cứng hoặc nạp code, vui lòng kiểm tra lại sơ đồ chân hoặc mở một Issue để được hỗ trợ!

Chúc các bạn thực hiện dự án thành công! 🌾✨
