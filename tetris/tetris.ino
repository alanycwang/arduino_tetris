#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <EEPROM.h>

//LCD
#define TFT_CS   10
#define TFT_RST  9
#define TFT_DC   8

//Joystick
#define VRx      A0
#define VRy      A1
#define SW       2

//Buttons
#define B1       3
#define B2       4

#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF
#define ORANGE   0xFD20
#define GRAY     0x8410
#define LGRAY    0xC618
#define DGRAY    0x4208

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

//input states
int16_t vrx = 5; //inverted
int16_t vry = 5;
bool sw = false;
bool b1 = false;
bool b2 = false;

bool rotateOK = true; //makes sure that piece rotates only once per button press

bool game_over = false;

int16_t screen[2][10][16]; //10x16 grid, keeps last screen state so that we don't need to refresh the entire LCD, which is slow
int8_t refresh = 0;

uint32_t lines = 0; //number of lines cleared
int8_t level = 0; //determines fall speed
uint32_t score = 0;

int8_t currentPiece[4]; //{piece, rotation, x, y} X AND Y VALUES CAN BE NEGATIVE

int8_t delayTime = 0; //used to delay different actions, stays constant
int8_t dropTime = 0; //used to delay falling, decreases at higher levels

//stored in hex for memory efficiency, each bit represents a row, the number in the bit is the sum of the columns(values from left to right: 8, 4, 2, 1)
int16_t pieces[7][4] = {{0x0F00, 0x2222, 0x00F0, 0x4444},  //I
                         {0x8E00, 0x6440, 0x0E20, 0x44C0},  //J
                         {0x2E00, 0x4460, 0x0E80, 0xC440},  //L
                         {0x6600, 0x6600, 0x6600, 0x6600},  //O
                         {0x6C00, 0x4620, 0x06C0, 0x8C40},  //S
                         {0x4E00, 0x4640, 0x0E40, 0x4C40},  //T
                         {0xC600, 0x2640, 0x0C60, 0x4C80}}; //Z

                         
int16_t colors[7] = {CYAN, BLUE, ORANGE, YELLOW, GREEN, MAGENTA, RED};

//stores leaderboard scores
uint32_t scores[10];
uint8_t initls[10][2];                     

void setup() {
Serial.begin(9600);
  
  tft.initR(INITR_144GREENTAB);

  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  pinMode(SW, INPUT_PULLUP);
  pinMode(B1, INPUT_PULLUP);
  pinMode(B2, INPUT_PULLUP);
  pinMode(A3, INPUT);
  pinMode(A5, INPUT);
  
  tft.setRotation(1);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);

  //reads leaderoard scores from EEPROM
  for (int i = 0; i < 10; i++) {
    EEPROM.get(6*i, initls[i][0]); // 1 byte
    EEPROM.get(6*i + 1, initls[i][1]); // 1 byte
    EEPROM.get(6*i + 2, scores[i]);// 4 bytes
  }
}

void loop() {
  play();
  for (int i = 127; i >= 0; i--) {
    tft.fillRect(0, i, 128, 1, BLACK);
    delay(5);
  }
  tft.setCursor(10, 30);
  tft.setTextSize(2);
  tft.print("GAME OVER");
  tft.setTextSize(1);
  tft.setCursor (30, 60);
  tft.print("Score: ");
  tft.print(score);
  while(true) {
    updateInput();
    if (b1 == true || b2 == true || sw == true) {
      break;
    }
  }
  tft.fillScreen(BLACK);
  for (int i = 0; i < 10; i++) {
    if (score > scores[i]){
      saveScore(i);
      break;
    }
  }
  showLeaderboard();
  delay(1000);
  while(true) {
    updateInput();
    if (b1 == true || b2 == true || sw == true) {
      break;
    }
  }
}

void showLeaderboard() {
  tft.fillScreen(BLACK);
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  for (int i = 0; i < 10; i++){
    tft.setCursor(10, 24 + 8*i);
    tft.print(char('A' + initls[i][0]));
    tft.print(char('A' + initls[i][1]));
    tft.print(": ");
    tft.print(scores[i]);
  }
}

void saveScore(uint8_t place){
  tft.setCursor(7, 30);
  int initials[2] = {0, 25};
  tft.print("Enter your initials");
  tft.setCursor(7, 38);
  tft.print("Left button: Change");
  tft.setCursor(7, 46);
  tft.print("Right button: Enter");
  tft.setTextSize(2);
  bool selected = true;
  uint8_t xlocs[2] = {64, 52};
  bool pressOK = true;
  while (true) {
    if (b2) break;
    updateInput();
    bool change = false;
    if (vrx <= 3 && selected) {
      selected = false;
      change = true;
    }
    if (vrx >= 7 && !selected) {
      selected = true;
      change = true;
    }
    if (b1 && pressOK){
      initials[selected] = (initials[selected] + 1)%26;
      change = true;
      pressOK = false;
    }

    else if (!b1 && !pressOK) pressOK = true;
    
    if (change) {
      tft.fillRect(52, 80, 24, 32, BLACK);
      tft.setCursor(xlocs[!selected], 80);
      tft.setTextColor(WHITE);
      tft.print(char('A' + initials[!selected]));
      tft.setCursor(xlocs[selected], 80);
      tft.setTextColor(YELLOW);
      tft.print(char('A' + initials[selected]));
    }
  }

  //each score takes up 4(score) + 2(initials) = 6(total) bytes
  for (int i = place + 1; i < 10; i++) {
    scores[i] = scores[i-1];
    initls[i][0] = initls[i-1][0];
    initls[i][1] = initls[i-1][1];
  }
  scores[place] = score;
  initls[place][0] = initials[1];
  initls[place][1] = initials[0];

  update_EEPROM();
}

void update_EEPROM () {
  for (int i = 0; i < 10; i++){
    uint8_t a, b;
    uint32_t s;
    EEPROM.get(6*i, a);
    EEPROM.get(6*i + 1, b);
    EEPROM.get(6*i + 2, s);

    if (a != initls[i][0]) EEPROM.put(6*i, initls[i][0]);
    if (b != initls[i][1]) EEPROM.put(6*i + 1, initls[i][1]);
    if (s != scores[i]) EEPROM.put(6*i + 2, scores[i]);
  }
}

void play() {
  tft.fillScreen(DGRAY);
  tft.fillRect(0, 0, 80, 128, BLACK);

  clearScreen();
  clearScreen();

  score = 0;
  level = 0;
  lines = 0;
  game_over = false;

  spawnPiece();
  printStats();
  
  while (true) {
    if (game_over) break;
    int temp = score; //helps check to see if score changed, so we don't need to refresh the screen every frame, which again, is slow
    updateInput();
    checkActions();
  
    if (dropTime == 0) { //moves block down (done by "down" function)
      if(!down()) {
        deleteRows(); //clears full rows
        delay(200);
        spawnPiece();
      }
    }
    
    delay(10);
    delayTime = (delayTime + 1)%100;
    dropTime = (dropTime + 1)%(100 - 10*level);
    level = int(lines/10)%10;
    if (temp != score) printStats();
  }
}

void updateInput(){ //updates input value variables
  if (abs(int(analogRead(VRx)/100 + 0.5) - vrx) >= 1) vrx = int(analogRead(VRx)/100); //rounds value down and divides by 10
  if (abs(int(analogRead(VRy)/100 + 0.5) - vry) >= 1) vry = int(analogRead(VRy)/100);
  if (digitalRead(SW) == sw) sw = !digitalRead(SW);
  if (digitalRead(B1) == b1) b1 = !digitalRead(B1);
  if (digitalRead(B2) == b2) b2 = !digitalRead(B2);
}

void checkActions() { //checks input to see if anything needs to be moved, rotated, etc
  if (b1 && rotateOK) {
    rotate();
    rotateOK = false;
  }
  else if (!b1) rotateOK = true;

  if (b2) {
    clearPiece(currentPiece[0], currentPiece[1], currentPiece[2], currentPiece[3]);
    currentPiece[0] = (currentPiece[0] + 1)%7;
    printPiece(currentPiece[0], currentPiece[1], currentPiece[2], currentPiece[3]);
    printScreen();
  }

  if (delayTime%10 == 0 && vry <= 3) {
    if(!down()) {
      deleteRows();
      delay(200);
      spawnPiece();
    }
  }

  if (delayTime%10== 0 && vrx >= 7) {
    left();
  }
  else if (delayTime%10 == 0 && vrx <= 3) {
    right();
  }
}

void printScreen() { //actually tells the lcd what to print based on screen array
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 16; j++) {
      if (screen[refresh][i][j] != screen[(refresh + 1)%2][i][j]) {
        if (screen[refresh][i][j] != BLACK) {
          tft.fillRect(1 + i*8, 1 + j*8, 6, 6, screen[refresh][i][j]);
          //edges of block
          tft.fillRect(i*8, j*8, 8, 1, LGRAY);
          tft.fillRect(i*8, j*8 + 1, 1, 6, GRAY);
          tft.fillRect((i + 1)*8 - 1, j*8 + 1, 1, 6, GRAY);
          tft.fillRect(i*8, (j + 1)*8 - 1, 8, 1, DGRAY); 
        }
        else tft.fillRect(i*8, j*8, 8, 8, BLACK);
      }
    }
  }

  //update refresh by overwriting old, but keeps previous
  refresh = (refresh + 1)%2;
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 16; j++) {
      screen[refresh][i][j] = screen[(refresh + 1)%2][i][j];
    }
  }
}

void printStats() {
  tft.fillRect(88, 0, 40, 72, DGRAY);
  tft.setCursor(88, 8);
  tft.print("Score:");
  tft.setCursor(88, 16);
  tft.print(score);
  tft.setCursor(88, 32);
  tft.print("Level:");
  tft.setCursor(88, 40);
  tft.print(level);
  tft.setCursor(88, 56);
  tft.print("Lines:");
  tft.setCursor(88, 64);
  tft.print(lines);
}

void clearScreen() {
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 16; j++) {
      screen[refresh][i][j] = BLACK;
    }
  }
}

void printPiece(int8_t piece, int8_t rotation, int8_t x, int8_t y){ //modifies "screen" to contain piece
  for (int i = 0; i < 4; i++){
    for (int j = 0; j < 4; j++){
      if (occupied(piece, rotation, i, j)) screen[refresh][x + j][y + i] = colors[piece];
    }
  }
}

void clearPiece(int8_t piece, int8_t rotation, int8_t x, int8_t y){
  for (int i = 0; i < 4; i++){
    for (int j = 0; j < 4; j++){
      if (occupied(piece, rotation, i, j)) screen[refresh][x + j][y + i] = BLACK;
    }
  }
}

bool occupied(int8_t piece, int8_t rotation, int8_t x, int8_t y){ //Checks if a certain spot on the 4x4 grid in which the piece is stored actually has a block or not
  if (x > 3 || y > 3 || x < 0 || y < 0) return false;
  return (pieces[piece][rotation] & power(2, 4*(3 - x) + (3 - y)));
}

bool spaceBelow(){ //if there is space for the block to fall
  for (int i = 3; i >= 0; i--) {
    for (int j = 0; j < 4; j++) {
      if (occupied(currentPiece[0], currentPiece[1], i, j) && !occupied(currentPiece[0], currentPiece[1], i + 1, j) && (currentPiece[3] + i >= 15 || screen[refresh][currentPiece[2] + j][currentPiece[3] + i + 1] != BLACK)) return false;
    }
  }
  return true;
}

bool down(){ //moves the block down
  if (spaceBelow()) {
    clearPiece(currentPiece[0], currentPiece[1], currentPiece[2], currentPiece[3]);
    currentPiece[3]++;
    printPiece(currentPiece[0], currentPiece[1], currentPiece[2], currentPiece[3]);
    printScreen();
    return true;
  }
  return false;
}

int8_t randomPiece() { //just a random function
  return absval((analogRead(A3)+analogRead(A5))%7); //second pin for more variation
}

//the following are similar to spaceBelow and down
bool spaceLeft() {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (occupied(currentPiece[0], currentPiece[1], i, j) && !occupied(currentPiece[0], currentPiece[1], i, j - 1) && ((currentPiece[2] + j <= 0) || screen[refresh][currentPiece[2] + j - 1][currentPiece[3] + i] != BLACK)) return false;
    }
  }
  return true;
}

bool spaceRight() {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (occupied(currentPiece[0], currentPiece[1], i, j) && !occupied(currentPiece[0], currentPiece[1], i, j + 1) && ((currentPiece[2] + j >= 9) || screen[refresh][currentPiece[2] + j + 1][currentPiece[3] + i] != BLACK)) return false;
    }
  }
  return true;
}

bool left(){
  if (spaceLeft()) {
    clearPiece(currentPiece[0], currentPiece[1], currentPiece[2], currentPiece[3]);
    currentPiece[2]--;
    printPiece(currentPiece[0], currentPiece[1], currentPiece[2], currentPiece[3]);
    printScreen();
    return true;
  }
  return false;
}

bool right(){
  if (spaceRight()) {
    clearPiece(currentPiece[0], currentPiece[1], currentPiece[2], currentPiece[3]);
    currentPiece[2]++;
    printPiece(currentPiece[0], currentPiece[1], currentPiece[2], currentPiece[3]);
    printScreen();
    return true;
  }
  return false;
}

int power(int base, int expnt) { //builtin exponent function is broken (somtimes returns 2^3 = 7.9999, which turns into 7 when we cast to int)
  if (expnt == 0) return 1;
  return (base*(power(base, expnt - 1)));
}

bool rotate() {
  //moves the piece away if there is no space to rotate into, called "wall kick"
  int8_t offset = 0;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (occupied(currentPiece[0], (currentPiece[1] + 1)%4, i, j)) {
        if (currentPiece[2] + j + offset < 0) offset -= (currentPiece[2] + j + offset);
        else if (currentPiece[2] + j + offset > 9) offset -= ((currentPiece[2] + j + offset) - 9);
      }
    }
  }
  //actually rotates
  clearPiece(currentPiece[0], currentPiece[1], currentPiece[2], currentPiece[3]);
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (occupied(currentPiece[0], (currentPiece[1] + 1)%4, i, j) && !occupied(currentPiece[0], currentPiece[1], i, j)) {
        if (screen[refresh][currentPiece[2] + j + offset][currentPiece[3] + i] != BLACK) {
          return false;
          printPiece(currentPiece[0], currentPiece[1], currentPiece[2], currentPiece[3]);
        }
      }
    }
  }
 
  currentPiece[1] = (currentPiece[1] + 1)%4;
  currentPiece[2] += offset;
  printPiece(currentPiece[0], currentPiece[1], currentPiece[2], currentPiece[3]);
  printScreen();
}

void deleteRows() {
  int temp = 0;
  for (int i = 0; i < 16; i++) {
    bool del = true;
    for (int j = 0; j < 10; j++) {
      if (screen[refresh][j][i] == BLACK){
        del = false;
        break;
      }
    }
    if (del) {
      deleteRow(i);
      temp++;
    }
  }

  //update score and lines
  lines += temp;
  if (temp == 1) score += 4*(level + 1);
  else if (temp == 2) score += 10*(level + 1);
  else if (temp == 3) score += 30*(level + 1);
  else if (temp >= 4) score += 120*(level + 1);
}

void deleteRow(int row) {
  for (int i = 0; i < 10; i++) {
    screen[refresh][i][row] = BLACK;
  }

  for (int i = row; i > 0; i--) {
    for (int j = 0; j < 10; j++) {
       screen[refresh][j][i] = screen[refresh][j][i - 1];
    }
  }

  for (int i = 0; i < 10; i++) {
    screen[refresh][i][0] = BLACK;
  }

  printScreen();
}

void spawnPiece() {
  currentPiece[0] = randomPiece();
  currentPiece[1] = 0;
  currentPiece[2] = 3;
  currentPiece[3] = 0;

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (occupied(currentPiece[0], 0, i, j) && screen[refresh][3 + j][i] != BLACK) {
        game_over = true;
        return;
      }
    }
  }

  printPiece(currentPiece[0], currentPiece[1], currentPiece[2], currentPiece[3]);
  printScreen();
}

int absval(int x) { //so abs() is somehow also broken, probably because it uses pow()
  if (x < 0) return 0 - x;
  return x;
}
