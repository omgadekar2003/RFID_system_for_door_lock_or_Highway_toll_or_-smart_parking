#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9
#define SERVO_PIN 3
#define LED_G 5
#define LED_R 4
#define BUZZER 2

MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo myServo;

String authorizedUID = "C3 3D 0B 05";

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(LED_G, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  myServo.attach(SERVO_PIN);
  myServo.write(0);  // Gate closed

  Serial.println("Scan RFID card...");
}

void loop() {

  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  Serial.print("UID tag : ");
  String content = "";

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);

    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  Serial.println();
  content.toUpperCase();

  // -------- AUTHORIZATION CHECK --------
  if (content.substring(1) == authorizedUID) {

    Serial.println("Authorized Access");

    digitalWrite(LED_G, HIGH);
    tone(BUZZER, 1200);
    delay(150);
    noTone(BUZZER);

    myServo.write(180);   // Open gate
    delay(5000);          // Keep open 5 sec
    myServo.write(0);     // Close gate

    digitalWrite(LED_G, LOW);
  }
  else {

    Serial.println("Access Denied");

    digitalWrite(LED_R, HIGH);

    for (int i = 0; i < 2; i++) {
      tone(BUZZER, 600);
      delay(150);
      noTone(BUZZER);
      delay(100);
    }

    digitalWrite(LED_R, LOW);
  }

  Serial.println();
}

