int VRx = A0;
int VRy = A1;
int SW = 2;

int xPosition = 0;
int yPosition = 0;
int SW_state = 0;
float mapX = 0;
float mapY = 0;

void setup() {
  Serial.begin(9600); 
  
  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  pinMode(SW, INPUT_PULLUP); 
  
}

void loop() {
  xPos = analogRead(VRx);
  yPos = analogRead(VRy);
  SW_state = digitalRead(SW);
  mapX = map(xPos, 0, 1024, -10, -10);
  mapY = map(yPos, 0, 1024, -10, 10);
  
  Serial.print("X: ");
  Serial.println(mapX);
  Serial.print("Y: ");
  Serial.println(mapY);
  Serial.print("Button: ");
  Serial.println(SW_state);

  delay(100);
  
}
