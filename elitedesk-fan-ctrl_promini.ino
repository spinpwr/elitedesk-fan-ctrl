#define  FAN1_PIN      9      // PWM pin for the FAN1, duty = OCRA1A
#define  FAN2_PIN     10      // PWM pin for the FAN2, duty = OCRA1B
#define  NTC_PIN      A4      // analog pin of the NTC sensor

#define  LOOP_TIME  3000      // time between measurements
#define  NTC_MIN     420      // NTC reading units of maximum duty (~5mV/unit)
#define  NTC_MAX     720      // NTC reading units of minimum duty (~5mV/unit)
#define  FAN_MIN      80      // minimum duty of the fans

#define  DEBUG      // turn ON serial debug msgs

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
int duty;

void setup() {
  // based on: https://github.com/adolfintel/NanoFanController/blob/master/NanoFanController/NanoFanController.ino#L121
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,HIGH);
  pinMode(FAN1_PIN,OUTPUT);
  pinMode(FAN2_PIN,OUTPUT);
  //Set PWM frequency to about 25khz on pins 9,10 (timer 1 mode 10, no prescale, count to 320)
  TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
  TCCR1B = (1 << CS10) | (1 << WGM13);
  ICR1 = 320;
  //Set PWM frequency to about 25khz on pin 3 (timer 2 mode 5, prescale 8, count to 79)
  // TIMSK2 = 0;
  // TIFR2 = 0;
  // TCCR2A = (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);
  // TCCR2B = (1 << WGM22) | (1 << CS21);
  // OCR2A = 79;
#ifdef DEBUG 
  //Startup pulse
  Serial.begin(9600);
  Serial.println(F("FAN CONTROLLER"));
  Serial.print(F("Startup pulse... "));
  OCR1A = 120;
  OCR1B = 180;
  delay(1000);
  OCR1A = FAN_MIN;
  Serial.println(F("done"));
#endif
  // Set initial PWM duty
  OCR1A = FAN_MIN;
  OCR1B = FAN_MIN;
  digitalWrite(LED_BUILTIN,LOW);
}

void loop() {
  ntc_val = analogRead(NTC_PIN);
  duty = map(ntc_val, NTC_MIN, NTC_MAX, 320, 0);
  OCR1A = duty > FAN_MIN ? duty : FAN_MIN;
  OCR1B = duty > FAN_MIN ? duty : FAN_MIN;
#ifdef DEBUG
    Serial.print("NTC = ");
    Serial.print(int(ntc_val));
    Serial.print(" = ");
    Serial.print( ntc_val*4700 / (1023-ntc_val));
    Serial.print(" ohm");
    Serial.print(" | dutyA = ");
    Serial.print(OCR1A);
    Serial.print(" | dutyB = ");
    Serial.println(OCR1B);
#endif
  delay(LOOP_TIME);
}
