#include <EEPROM.h>

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 6*10; i++){
    EEPROM.update(i, 0);
  }
  Serial.print("Cleared!");
}

void loop() {
  // put your main code here, to run repeatedly:

}
