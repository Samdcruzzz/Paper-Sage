#include <Servo.h>
#include <CheapStepper.h>

// Sensor and actuator pin definitions
#define IR_SENSOR A0
#define SERVO_PIN 9
#define SERVO_PIN1 12

#define BUZZER A2
#define S0 7
#define S1 6
#define S2 11
#define S3 10
#define OUT 8

// Ultrasonic and Soil moisture sensors
const int TRIG_PIN = A5;  //12
const int ECHO_PIN = 13;
const int potPin = A1;
const int MAX_DRY_VALUE = 1;    // Dry/Wet threshold
const int ULTRA_DISTANCE = 20;  // Object detection range

const int trigPin = A4;
const int echoPin = A3;
//const int POT_PIN1 = A4;
const int maxDryValue = 1;
const int Ultra_Distance = 8;

Servo s, s1;
CheapStepper stepper(2, 3, 4, 5);

int red, green, blue;
long duration, duration1;
int distance = 0, distance1 = 0;
int irValue = 0;
int soil = 0, fsoil;
int soil1 = 0, fsoil1;

// Function Prototypes
void program1();
void program2();
void program3();
void program4();
int getColor(int s2State, int s3State);

void setup() {
  Serial.begin(9600);

  pinMode(BUZZER, OUTPUT);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(OUT, INPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  s.attach(SERVO_PIN);
  s.write(120);  // Neutral position

  s1.attach(SERVO_PIN1);
  s1.write(150);

  stepper.setRpm(15);
}

void loop() {
  program1();
  program2();
  program3();
}

void program1() {
  irValue = analogRead(IR_SENSOR);

  if (irValue > 10) {
    Serial.print("IR Value: ");
    Serial.println(irValue);

    if (irValue >= 10 && irValue <= 130) {
      Serial.println("✅ Recyclable Paper Detected!");
      digitalWrite(BUZZER, HIGH);
      delay(1500);
      digitalWrite(BUZZER, LOW);

      stepper.moveDegreesCW(195);
      delay(3000);
      s.write(0);
      delay(3000);
      s.write(120);
      delay(2000);
      stepper.moveDegreesCCW(200);
    }
  }
  delay(1000);
}

void program2() {
  red = getColor(LOW, LOW);
  green = getColor(HIGH, HIGH);
  blue = getColor(LOW, HIGH);

  Serial.print("R: ");
  Serial.print(red);
  Serial.print(" G: ");
  Serial.print(green);
  Serial.print(" B: ");
  Serial.println(blue);

  if (red >= 10 && red <= 100 && green >= 10 && green <= 100 && blue >= 10 && blue <= 100) {
    Serial.println("❌Non-Recyclable Paper Detected!");
    digitalWrite(BUZZER, HIGH);
    delay(500);
    digitalWrite(BUZZER, HIGH);
    delay(1000);
    digitalWrite(BUZZER, LOW);

    stepper.moveDegreesCW(140);
    delay(2000);
    s.write(0);
    delay(3000);
    s.write(125);
    stepper.moveDegreesCCW(145);
  } else {
    s.write(125);
  }
  delay(1000);
}

int getColor(int s2State, int s3State) {
  digitalWrite(S2, s2State);
  digitalWrite(S3, s3State);
  return pulseIn(OUT, LOW);
}

void program3() {
  {  // Infinite loop to check every 5 seconds
    distance = 0;

    for (int i = 0; i < 2; i++) {
      digitalWrite(TRIG_PIN, LOW);
      delayMicroseconds(7);
      digitalWrite(TRIG_PIN, HIGH);
      delayMicroseconds(10);
      digitalWrite(TRIG_PIN, LOW);
      duration = pulseIn(ECHO_PIN, HIGH);
      distance += duration * 0.034 / 2;
      delay(10);
    }
    distance /= 2;

    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    if ((distance > 0 && distance <= 5) || (distance >= 8 && distance <= 15)) {  // Detect object within 4 cm
      Serial.println("⭕ Object detected!");

      digitalWrite(BUZZER, HIGH);
      delay(500);
      digitalWrite(BUZZER, HIGH);
      delay(500);
      digitalWrite(BUZZER, HIGH);
      delay(1000);
      digitalWrite(BUZZER, LOW);

      delay(2000);
      s.write(0);
      delay(3000);
      s.write(125);
      delay(35000);
    }
    if ((distance > 0 && distance <= 5) || (distance >= 8 && distance <= 15)) {  // Measure soil humidity
      delay(1000);
      fsoil = 0;
      for (int i = 0; i < 3; i++) {
        soil = analogRead(potPin);
        soil = constrain(soil, 485, 1023);
        fsoil += map(soil, 485, 1023, 100, 0);
        delay(75);
      }
      fsoil /= 3;

      Serial.print("Humidity: ");
      Serial.print(fsoil);
      Serial.print("%    Distance: ");
      Serial.print(distance);
      Serial.println(" cm");

      digitalWrite(BUZZER, HIGH);
      delay(200);
      digitalWrite(BUZZER, HIGH);
      delay(200);
      digitalWrite(BUZZER, LOW);

      if (fsoil > MAX_DRY_VALUE) {
        Serial.println("==> DRY Waste");
        digitalWrite(BUZZER, HIGH);
        delay(200);
        digitalWrite(BUZZER, HIGH);
        delay(200);
        digitalWrite(BUZZER, LOW);
        s1.write(100);
        delay(2000);
        s1.write(150);
        delay(2000);
      } else {
        Serial.println("==> DRY Waste");
        digitalWrite(BUZZER, HIGH);
        delay(3000);
        digitalWrite(BUZZER, LOW);

        delay(3000);
        s1.write(100);
        delay(2000);
        s1.write(150);
        delay(2000);
      }
    }
    delay(5000);  // Wait for 5 seconds before the next check
  }
}
