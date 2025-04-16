#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_APDS9960.h>
#include <MPU6050.h>
#include <math.h> // for sqrt()

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define BUZZER_PIN 12 // Connect buzzer positive to pin 9

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
Adafruit_APDS9960 apds;
MPU6050 mpu;

const uint8_t PROXIMITY_THRESHOLD = 50;
const float FALL_THRESHOLD = 3.0; // Acceleration magnitude in g (can be tuned)

void setup() {
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed");
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Initializing...");
  display.display();

  if (!apds.begin()) {
    Serial.println("APDS9960 init failed");
    display.clearDisplay();
    display.println("APDS9960 Error");
    display.display();
    while (true);
  }
  apds.enableProximity(true);

  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed");
    display.clearDisplay();
    display.println("MPU6050 Error");
    display.display();
    while (true);
  }
  mpu.initialize();

  display.clearDisplay();
  display.println("System Ready");
  display.display();
}

void loop() {
  uint8_t proximity = apds.readProximity();
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  // Convert raw values to g
  float ax_g = ax / 16384.0;
  float ay_g = ay / 16384.0;
  float az_g = az / 16384.0;

  float acc_magnitude = sqrt(ax_g * ax_g + ay_g * ay_g + az_g * az_g);

  display.clearDisplay();
  display.setCursor(0, 0);

  // Fall Detection
  if (acc_magnitude < FALL_THRESHOLD) {
    Serial.println("Fall detected!");
    display.println("Fall Detected!");
    digitalWrite(BUZZER_PIN, HIGH);
    delay(1000); // Alert duration
    digitalWrite(BUZZER_PIN, LOW);
  }
  // Obstacle Detection
  else if (proximity >= PROXIMITY_THRESHOLD) {
    Serial.println("Obstacle detected!");
    display.println("Obstacle Ahead!");
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500);
    digitalWrite(BUZZER_PIN, LOW);
  } else {
    display.println("All Clear");
    digitalWrite(BUZZER_PIN, LOW);
  }

  display.display();
  delay(300);
}
