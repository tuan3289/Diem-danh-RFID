Yêu cầu phần cứng

Vi điều khiển PIC (ví dụ PIC16F877A)

Module đọc RFID (ví dụ RC522 MFRC522)

Module Wi-Fi ESP8266 (ESP-01 / NodeMCU / Wemos — tùy bạn)

Cập nguồn 5V cho PIC/RC522, 3.3V cho ESP8266 (hoặc dùng mức chuyển đổi tín hiệu cho UART)

Bộ chuyển mức TTL (TX/RX) nếu dùng PIC (5V) ↔ ESP8266 (3.3V)

Dây nối, breadboard, nguồn ổn định

Sơ đồ nối dây (ví dụ)

Đây là sơ đồ logic (tùy module/hardware bạn có thể thay đổi chân)

RC522 ↔ PIC16F877A

SDA → RC0/không cố định (theo SPI bạn đã cấu hình)

SCK → RC3

MOSI → RC4

MISO → RC5

RST → 5V ou 其他 chân

VCC → 3.3V (hoặc 5V tùy module)

GND → GND

PIC UART ↔ ESP8266

PIC TX (TXᴘɪᴄ) → [Level shifter] → ESP8266 RX

PIC RX (RXᴘɪᴄ) ← [Level shifter] ← ESP8266 TX

GND chung giữa PIC và ESP8266

Thường dùng 9600 bps, 8N1 (nếu code PIC/ESP set như vậy). (Bạn có thể dùng 115200 nếu phần mềm hỗ trợ, nhưng 9600 ổn định hơn với PIC cổ.)

Ghi chú quan trọng: ESP8266 chạy ở 3.3V — KHÔNG nối trực tiếp chân TX của PIC (5V) vào RX của ESP8266 mà không có bộ chia điện áp hoặc chuyển mức.

Luồng hoạt động (flow)

Thẻ RFID được chạm vào module MFRC522.

PIC đọc UID thẻ (dùng thư viện MFRC522/RC522 cho PIC).

Khi phát hiện UID hợp lệ, PIC gửi UID qua UART theo định dạng ví dụ: UID:0EFE3002\r\n hoặc 0E FE 30 02\n.

ESP8266 nhận chuỗi UID, parse, và thực hiện request HTTP (GET/POST) tới WebApp / Google Apps Script để ghi dòng mới lên Google Sheets (hoặc gửi tới server của bạn).
