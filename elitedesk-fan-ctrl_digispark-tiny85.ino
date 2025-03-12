//
//  ATtiny 85
//
#define  FAN1_PIN  PB1    // PWM pin for the FAN1, duty = OCR1A
#define  FAN2_PIN  PB4    // PWM pin for the FAN2, duty = OCR1B
#define  NTC_PIN   PB3    // analog pin of the NTC sensor

#define  LOOP_TIME 3000   // time between measurements
#define  NTC_MIN   420    // NTC reading units of maximum duty (~5mV/unit)
#define  NTC_MAX   720    // NTC reading units of minimum duty (~5mV/unit)
#define  FAN_MIN    20    // minimum duty of the fans

#define  DEBUG            // switch serial debug msgs ON

// Clock at 16.5MHz
#define F_CPU 16500000  // This is used by delay.h library
// Normal delay() won't work because we are changing Timer1 behavior
// Adds _delay_ms and _delay_us functions
#include <util/delay.h>

#ifdef DEBUG
#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, PB0);  // RX, TX
#endif

/* --==  Some temp measurements  ==-- 
Resistance values measured in ohm by multimeter vs CPU temp by HWiNFO64.
NTC was set next to CPU lid in the middle of the side facing the CPU fan.
From factory there is some foam under the heatsink, so the fan doesn't
blow directly to the CPU, so neither the NTC.
NTC used: 10K epoxy thermistor B3435 from aliexpress

    21˚C   12040   OFF roomtemp 
    45˚C    5370   IDLE cold start
    52˚C    4350   IDLE after load
    64˚C    3820   CPU 100% load
    72˚C    3310   CPU+GPU 100% load 
*/

float ntc_val = 0;
int duty = FAN_MIN;
int fade = 20;

void setup() {
  pinMode(FAN1_PIN,OUTPUT);
  pinMode(FAN2_PIN,OUTPUT);
  //Set PWM frequency to 25khz on pins 5, 6
  //Enable PLL low speed mode 32MHz
  PLLCSR = (1 << LSM) | (1 << PLLE);
  while ((PLLCSR & (1<<PLOCK)) == 0x00) {
    //Do nothing until PLL lock bit is set ~100us
  }
  //Enable PLL clock source after freqency lock
  PLLCSR |= (1 << PCKE);
  //Output mode A pwm 01: set at BOTTOM clear at OCR1A match, /8 prescale
  TCCR1 = (1 << PWM1A) | (1 << COM1A1) | (1 << CS12);
  //Output mode B pwm 01: set at BOTTOM clear at OCR1B match
  GTCCR = (1 << PWM1B) | (1 << COM1B1);
  //Count to TOP=159
  OCR1C = 159;
  //Serial debug
#ifdef DEBUG
  mySerial.begin(9600);
  Serial.println(F("Serial done"));
#endif
  // Set initial PWM duty
  OCR1A = FAN_MIN;
  OCR1B = FAN_MIN;
}

void loop() {
  ntc_val = analogRead(NTC_PIN);
  duty = map(ntc_val, NTC_MIN, NTC_MAX, OCR1C, 0);
  OCR1A = duty > FAN_MIN ? duty : FAN_MIN;
  OCR1B = duty > FAN_MIN ? duty : FAN_MIN;
#ifdef DEBUG
  mySerial.print(ntc_val*4700 / (1023-ntc_val));
  mySerial.print(F(" ohm => duty: "));
  mySerial.println(duty);
#endif
  _delay_ms(LOOP_TIME);
}
