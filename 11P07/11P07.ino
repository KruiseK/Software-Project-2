#include <Servo.h>

// Arduino pin assignment
#define PIN_LED   9
#define PIN_TRIG  12
#define PIN_ECHO  13
#define PIN_SERVO 10

// configurable parameters for sonar
#define SND_VEL 346.0
#define INTERVAL 25
#define PULSE_DURATION 10
#define _DIST_MIN 180.0
#define _DIST_MAX 360.0

#define TIMEOUT ((INTERVAL / 2) * 1000.0)
#define SCALE (0.001 * 0.5 * SND_VEL)

#define _EMA_ALPHA 0.3

// Target Distance
#define _TARGET_LOW  250.0
#define _TARGET_HIGH 290.0

// duty duration for myservo.writeMicroseconds()
#define _DUTY_MIN 550
#define _DUTY_MAX 2450
#define _DUTY_NEU ((_DUTY_MIN + _DUTY_MAX) / 2)

// global variables
float dist_ema, dist_prev = _DIST_MIN;
unsigned long last_sampling_time = 0;
unsigned long duty_control;
Servo myservo;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  digitalWrite(PIN_TRIG, LOW);
  digitalWrite(PIN_LED, HIGH);

  myservo.attach(PIN_SERVO);
  myservo.writeMicroseconds(_DUTY_NEU);

  dist_prev = _DIST_MIN;
  dist_ema = _DIST_MIN;

  Serial.begin(57600);
}

void loop() {
  float dist_raw, dist_filtered;

  if (millis() - last_sampling_time < INTERVAL)
    return;

  dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);

  // Range filter
  if ((dist_raw == 0.0) || (dist_raw > _DIST_MAX)) {
    dist_filtered = dist_prev;
    digitalWrite(PIN_LED, HIGH);
  }
  else if (dist_raw < _DIST_MIN) {
    dist_filtered = dist_prev;
    digitalWrite(PIN_LED, HIGH);
  }
  else {
    dist_filtered = dist_raw;
    dist_prev = dist_raw;
    digitalWrite(PIN_LED, LOW);
  }

  // EMA filter
  dist_ema = (dist_filtered * _EMA_ALPHA)
           + (1 - _EMA_ALPHA) * dist_ema;

  // Servo control
  if (dist_ema <= _DIST_MIN) {
    duty_control = _DUTY_MIN;
  }
  else if (dist_ema < _DIST_MAX) {
    duty_control = _DUTY_MIN
                 + (_DUTY_MAX - _DUTY_MIN)
                 * (dist_ema - _DIST_MIN)
                 / (_DIST_MAX - _DIST_MIN);
  }
  else {
    duty_control = _DUTY_MAX;
  }

  myservo.writeMicroseconds(duty_control);

  // Serial monitor
  Serial.print("Min:");
  Serial.print(_DIST_MIN);

  Serial.print(",dist:");
  Serial.print(dist_raw);

  Serial.print(",ema:");
  Serial.print(dist_ema);

  Serial.print(",Servo:");
  Serial.print(myservo.read());

  Serial.print(",Max:");
  Serial.print(_DIST_MAX);

  Serial.println("");

  last_sampling_time += INTERVAL;
}

// Ultrasonic sensor measurement
float USS_measure(int TRIG, int ECHO) {
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);

  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE;
}
