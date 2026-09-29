int LED = 7;

unsigned long pwmPeriod = 10000;
int pwmDuty = 0;

void set_period(int period) {
    pwmPeriod = period;
}

void set_duty(int duty) {
    pwmDuty = duty;
}

void PWM() {
    unsigned long onTime = pwmPeriod * pwmDuty / 100;
    unsigned long offTime = pwmPeriod - onTime;

    if (onTime > 0) {
        digitalWrite(LED, LOW);     
        delayMicroseconds(onTime);
    }

    if (offTime > 0) {
        digitalWrite(LED, HIGH);    
        delayMicroseconds(offTime);
    }
}

void setup() {
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);

    set_period(10000);  
}

void loop() {
    static unsigned long startTime = millis();

    unsigned long elapsed = (millis() - startTime) % 1000;

    int duty;


    if (elapsed < 500) {
        duty = elapsed * 100 / 500;
    }


    else {
        duty = (1000 - elapsed) * 100 / 500;
    }

    set_duty(duty);

    PWM();
}
