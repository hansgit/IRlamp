// Checks for signal on IR sensor on pin irPin and adjusts LED brightness
// based on detected lightning conditions

#include <IRremote.h>
#define MAX_TIME 150

const int ledPin = 6;     // LED bank/transistor pin
const int lightPin  = 0;  // Light PR pin
const int irPin = 2;      // IR sensor pin
int   luminosite;
long lastPressTime = 0;
boolean state = 0;
long irValue;
const int samplingWindow = 10; //number of samples to smooth light sensor vals
int samplingReadings[samplingWindow];
int samplingIndex = 0;
int samplingTotal = 0;
int samplingAvg = 0;

IRrecv irrecv(irPin); // annonce le pin du recepteur IR
decode_results results;

void setup() {
  pinMode(ledPin, OUTPUT);
  irrecv.enableIRIn(); // start receiver
  Serial.begin(9600);
}

void loop() {
  samplingTotal = samplingTotal - samplingReadings[samplingIndex];
  samplingReadings[samplingIndex] = analogRead(lightPin);
  samplingTotal = samplingTotal + samplingReadings[samplingIndex];
  samplingIndex++;
  if (samplingIndex >= samplingWindow) {
    samplingIndex = 0;
  }
  if (irrecv.decode(&results)) {
    irValue = results.value;
    Serial.print("irValue = ");
    Serial.println(irValue);
    if (irValue == 551536695) {
      Serial.println("ONE pressed");
      if (millis() - lastPressTime > MAX_TIME) {
        state = 1 - state;
        Serial.print("state = ");
        Serial.println(state);
      }
      lastPressTime = millis();
    }
    irrecv.resume();
  }
  manageLight();
}


void manageLight() {
  if (state == 1) {
    luminosite = map((samplingTotal/samplingWindow), 0, 1023, 10, 255);
    analogWrite(ledPin, luminosite);
    Serial.println(analogRead(lightPin));
    Serial.println(luminosite);
    Serial.println();
  }
  if (state == 0) {
    analogWrite(ledPin, 0);
  }
}

