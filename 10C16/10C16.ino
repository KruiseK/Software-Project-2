#include <Servo.h>
#include <math.h>

#define PIN_SERVO 10
#define PIN_TRIG 12
#define PIN_ECHO 13

#define USE_SIGMOID 0
#define DETECT_DISTANCE 180.0
#define RELEASE_DISTANCE 230.0
#define SENSOR_INTERVAL 50
#define MOVING_TIME 3000UL

#define ANGLE_CLOSED 0
#define ANGLE_OPEN 90

Servo myServo;

bool vehicleDetected = false;
float currentAngle = ANGLE_CLOSED;
float startAngle = ANGLE_CLOSED;
float targetAngle = ANGLE_CLOSED;
unsigned long moveStartTime = 0;
unsigned long lastSensorTime = 0;
bool moving = false;

float easing(float t) {
  t = constrain(t, 0.0f, 1.0f);
#if USE_SIGMOID
  const float k = 12.0f;
  const float low = 1.0f / (1.0f + exp(k * 0.5f));
  const float high = 1.0f / (1.0f + exp(-k * 0.5f));
  float y = 1.0f / (1.0f + exp(-k * (t - 0.5f)));
  return (y - low) / (high - low);
#else
  return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
#endif
}

float readDistanceMM() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  unsigned long duration = pulseIn(PIN_ECHO, HIGH, 12000UL);
  if (duration == 0) return -1.0f;
  return duration * 0.346f / 2.0f;
}

void updateServo(unsigned long now) {
  if (!moving) return;
  float t = (now - moveStartTime) / (float)MOVING_TIME;
  if (t >= 1.0f) {
    currentAngle = targetAngle;
    moving = false;
  } else {
    currentAngle = startAngle + (targetAngle - startAngle) * easing(t);
  }
  myServo.write((int)round(currentAngle));
}

void moveTo(float angle, unsigned long now) {
  if (targetAngle == angle) return;
  startAngle = currentAngle;
  targetAngle = angle;
  moveStartTime = now;
  moving = true;
}

void setup() {
  Serial.begin(9600);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  myServo.attach(PIN_SERVO);
  myServo.write(ANGLE_CLOSED);
}

void loop() {
  unsigned long now = millis();
  updateServo(now);

  if (now - lastSensorTime >= SENSOR_INTERVAL) {
    lastSensorTime = now;
    float distance = readDistanceMM();

    if (distance >= 0) {
      if (!vehicleDetected && distance <= DETECT_DISTANCE) {
        vehicleDetected = true;
        moveTo(ANGLE_OPEN, millis());
        Serial.println("Vehicle detected: opening");
      } else if (vehicleDetected && distance >= RELEASE_DISTANCE) {
        vehicleDetected = false;
        moveTo(ANGLE_CLOSED, millis());
        Serial.println("Vehicle passed: closing");
      }
      Serial.print("Distance (mm): ");
      Serial.println(distance);
    }
  }
}
