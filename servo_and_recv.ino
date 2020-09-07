// Include IR Remote Library 
#include <IRremote.h>
#include <avr/sleep.h>
#include <avr/power.h>

// Include Servo Library
#include <Servo.h>

const byte LED_PIN = 8;

const unsigned long BAUD_RATE = 115200;

// Define Receiver Pin
const byte RECV_PIN = 11;

// Define servo initial position
int pos = 90;

// Define Servo Pin
const int SERVO_PIN = 5;

// no-activity timeout in milliseconds
const unsigned long NO_ACTIVITY_TIMEOUT = 2000;

// Define IR Receiver, results and servo
IRrecv irrecv(RECV_PIN);
Servo myservo;
decode_results results;

EMPTY_INTERRUPT (PCINT0_vect);

void setup() {
  // put your setup code here, to run once:
  // start the receiver
  Serial.begin(BAUD_RATE);
  irrecv.enableIRIn();

  // attach the servo
  myservo.attach(SERVO_PIN);

  // start with Servo in center
  myservo.write(90);

  pinMode(LED_BUILTIN, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
    static unsigned long lastActivity;  // when we last woke
  // sleep if no activity for a while
  if (millis () - lastActivity >= NO_ACTIVITY_TIMEOUT)
    {
    Serial.flush ();  // wait for Serial to finish outputting
    Serial.end ();    // shut down Serial
    
    noInterrupts ();  // timed sequence coming up

    // pin change interrupt for D11
    PCMSK0 |= bit (PCINT3);  // want pin 11
    PCIFR  |= bit (PCIF0);   // clear any outstanding interrupts
    PCICR  |= bit (PCIE0);   // enable pin change interrupts for D8 to D13

    set_sleep_mode (SLEEP_MODE_STANDBY);  
    sleep_enable();

    byte old_ADCSRA = ADCSRA;
    // disable ADC to save power
    ADCSRA = 0;  
    digitalWrite(LED_BUILTIN, LOW);
    power_all_disable ();  // turn off various modules
    interrupts ();
    sleep_cpu ();  
    sleep_disable();
    digitalWrite(LED_BUILTIN, HIGH);
    power_all_enable ();   // enable modules again
    ADCSRA = old_ADCSRA;   // re-enable ADC conversion
    Serial.begin(BAUD_RATE);
    lastActivity = millis ();
    }  // end of no activity for a couple of seconds


  if (irrecv.decode(&results)) // chech if a code has been received
  {
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
    
    Serial.println(results.value, HEX);
    if (results.value == 0xFF6897) {
      if (pos == 90 || pos == 60) {
        pos = 130;
      }
      else if (pos == 130 ) {
        pos = 60;
      }
      myservo.write(pos);
    }
  pinMode(LED_PIN, INPUT);
  digitalWrite(LED_PIN, LOW);  
  delay(500);
  irrecv.resume();
  }
}
