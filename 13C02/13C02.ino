
#include <Servo.h>

// Arduino pin assignment
#define PIN_SERVO 10

// configurable parameters
#define _DUTY_MIN 550
#define _DUTY_NEU 1500
#define _DUTY_MAX 2450

#define _POS_START _DUTY_MIN
#define _POS_END   _DUTY_MAX

// Experiment 1: 3.0
// Experiment 2: 0.3
#define _SERVO_SPEED 0.3

#define INTERVAL 20

// global variables
unsigned long last_sampling_time;

Servo myservo;

float duty_change_per_interval;
int duty_target;
float duty_curr;

int toggle_interval, toggle_interval_cnt;

void setup() {
  myservo.attach(PIN_SERVO);

  duty_target = duty_curr = _POS_START;
  myservo.writeMicroseconds((int)duty_curr);

  Serial.begin(57600);

  // Convert angular speed into duty change per interval
  duty_change_per_interval =
    (_DUTY_MAX - _DUTY_MIN) *
    (_SERVO_SPEED / 180.0) *
    (INTERVAL / 1000.0);

  Serial.print("duty_change_per_interval:");
  Serial.println(duty_change_per_interval, 4);

  // Initialize variables for servo update
  toggle_interval = (180.0 / _SERVO_SPEED) * 1000 / INTERVAL;
  toggle_interval_cnt = toggle_interval;

  last_sampling_time = millis();
}

void loop() {
  if (millis() - last_sampling_time < INTERVAL)
    return;

  // Adjust current duty toward target
  if (duty_target > duty_curr) {
    duty_curr += duty_change_per_interval;
    if (duty_curr > duty_target)
      duty_curr = duty_target;
  } else if (duty_target < duty_curr) {
    duty_curr -= duty_change_per_interval;
    if (duty_curr < duty_target)
      duty_curr = duty_target;
  }

  // Update servo position
  myservo.writeMicroseconds((int)duty_curr);

  // Serial plotter output
  Serial.print("Min:550");
  Serial.print(",duty_target:");
  Serial.print(duty_target);
  Serial.print(",duty_curr:");
  Serial.print(duty_curr);
  Serial.println(",Max:2450");

  // Toggle duty target
  if (toggle_interval_cnt >= toggle_interval) {
    toggle_interval_cnt = 0;

    if (duty_target == _POS_START)
      duty_target = _POS_END;
    else
      duty_target = _POS_START;
  } else {
    toggle_interval_cnt++;
  }

  last_sampling_time += INTERVAL;
}
