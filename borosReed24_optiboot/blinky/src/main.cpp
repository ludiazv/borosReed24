#include<Arduino.h>

void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
}
void loop(){
  digitalWrite(LED,LOW);
  delay(1000);
  digitalWrite(LED,HIGH);
  delay(1000);
} 