#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <base64.h>
#include <ESP32Servo.h>

#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

const char *ssid = "YOUR_WIFI_SSID";
const char *password = "YOUR_WIFI_PASSWORD";
const char *server_url = "http://192.168.225.86:5000/upload_interrupt";


#define TRIG_PIN 14
#define ECHO_PIN 12
#define DIST_THRESHOLD 100
#define LED_TX 2
#define SERVO_PIN 13

Servo myServo;
long lastServoMoveTime = 0;
bool sweepingForward = true;
int servoAngle = 0;

long getDistanceCM();
void sendImageWithDistance(long distance);
void moveServoStepwise();

void setup() {
  Serial.begin(115200);
  pinMode(LED_TX, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Initialize servo
  myServo.attach(SERVO_PIN);
  myServo.write(0); // Start at 0 degrees

  // Camera config
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 12;
  config.fb_count = 1;
  config.fb_location = CAMERA_FB_IN_PSRAM;

  if (psramFound()) {
    config.jpeg_quality = 10;
    config.fb_count = 2;
    config.grab_mode = CAMERA_GRAB_LATEST;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.println(WiFi.localIP());
}

void loop() {
  long distance = getDistanceCM();
  Serial.print("Distance: ");
  Serial.println(distance);

  if (distance < DIST_THRESHOLD && WiFi.status() == WL_CONNECTED) {
    sendImageWithDistance(distance);
    delay(5000);
  }

  moveServoStepwise();  // Handle servo movement
  delay(1000);
}

long getDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2;
}

void sendImageWithDistance(long distance) {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  String imageBase64 = base64::encode(fb->buf, fb->len);
  String imageName = "image_" + String(millis()) + ".jpg";

  String jsonPayload = "{\"distance\":" + String(distance) +
                       ",\"imagename\":\"" + imageName +
                       "\",\"image\":\"" + imageBase64 + "\"}";

  HTTPClient http;
  http.begin(server_url);
  http.addHeader("Content-Type", "application/json");

  int responseCode = http.POST(jsonPayload);

  if (responseCode > 0) {
    Serial.print("Response: ");
    Serial.println(responseCode);
    digitalWrite(LED_TX, HIGH);
    delay(200);
    digitalWrite(LED_TX, LOW);
  } else {
    Serial.print("Error sending POST: ");
    Serial.println(http.errorToString(responseCode));
  }

  http.end();
  esp_camera_fb_return(fb);
}

void moveServoStepwise() {
  long currentTime = millis();
  if (currentTime - lastServoMoveTime >= 5000) {  // Move every 5 seconds
    if (sweepingForward) {
      servoAngle += 10;
      if (servoAngle >= 180) {
        servoAngle = 180;
        sweepingForward = false;
      }
    } else {
      servoAngle -= 10;
      if (servoAngle <= 0) {
        servoAngle = 0;
        sweepingForward = true;
      }
    }
    myServo.write(servoAngle);
    Serial.print("Servo moved to: ");
    Serial.println(servoAngle);
    lastServoMoveTime = currentTime;
  }
}
