#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"
#include <WiFiClientSecure.h>
#include <Wire.h>

// Fill ssid and password with your network credentials
const char* ssid = "iPhone";
const char* password = "tuantuan";
const char* host = "script.google.com";
// Replace with your own script id to make server side changes
const char *GScriptId = "AKfycbzWmnf0WaHeXdrMMmnKFKQXZihB1gtHCkam7WS_tSdd_fapcMp3LlIR4WwpJKGBO5cC"; // Receiving data from google script address
String payload_base = "{\"command\": \"insert_row\", \"sheet_name\": \"Sheet1\", \"values\": ";

const int httpsPort = 443;

HTTPSRedirect* client = nullptr;
String value = "";  // String variable to hold the value
bool newData = false;  // Flag to check if data needs to be sent

void setup() {
  Serial.begin(9600);
  Serial.flush();
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  Serial.flush();
  WiFi.begin(ssid, password);
  
  // Wait for WiFi connection
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {  // Limit to 30 attempts
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
    //Serial.print("IP address: ");
    
  } else {
    Serial.println("\nFailed to connect to WiFi!");
    return;  // Stop if WiFi is not connected
  }

  // Initialize HTTPS client
  client = new HTTPSRedirect(httpsPort);
  client->setInsecure();
  client->setPrintResponseBody(false);
  client->setContentTypeHeader("application/json");

  // Ensure connection to the server
  if (!client->connected()) {
    client->connect(host, httpsPort);
  }
}

void loop() {
  // Nhận giá trị từ bàn phím qua UART
  if (Serial.available() > 0) {
    value = Serial.readStringUntil('\n'); // Đọc chuỗi cho đến khi gặp dấu xuống dòng
    value.trim(); // Loại bỏ khoảng trắng hoặc ký tự không mong muốn

    if (value.length() > 0) {
      newData = true;  // Đánh dấu có dữ liệu mới cần gửi
    } else {
      Serial.println("Vui lòng nhập chuỗi hợp lệ.");
    }
  }

  // Chỉ gửi dữ liệu khi có yêu cầu mới từ bàn phím
  if (newData) {
    Serial.print("Đang gửi dữ liệu '");
    Serial.print(value);
    Serial.println("' lên Google Sheets...");

    sendData(value);  // Gọi hàm gửi chuỗi từ UART lên Google Sheets
    newData = false;  // Reset lại cờ sau khi gửi dữ liệu
  }

  delay(100);  // Thêm delay để giảm tải CPU
}

// Subroutine for sending data to Google Sheets
void sendData(String value) {
  String url = String("/macros/s/") + GScriptId + "/exec"; // Gửi dữ liệu tới Google Script
  String payload = payload_base + "\"" + value + "\"}"; // Tạo payload để gửi

  // Publish data to Google Sheets
  Serial.println("Đang xuất bản dữ liệu...");

  if (client->POST(url, host, payload)) {
    // Do something here if publish was successful
    Serial.println("Dữ liệu đã được xuất bản thành công!");
  } else {
    // Do something here if publish was not successful
    Serial.println("Lỗi khi kết nối!");
  }

  // A delay of several seconds is required before publishing again    
  delay(100);
}
