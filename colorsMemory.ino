#include <EEPROM.h>

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(OLED_ADDR);

int leds[] = {3, 6, 9, 12};
int buttons[] = {2 , 5, 8, 11, 4};
int Tone[] = {262, 196, 220, 247};

int animationStage = 0;
double dotStageTimer = 0;

int record;

int buzzer = 7;
bool wait = true;

String currentSequence = "";

byte Alto, Baixo;

void setup()
{
  for (int i = 0; i < 4; i++) {
    pinMode(leds[i], OUTPUT);
    pinMode(buttons[i], INPUT);
    Serial.begin(9600);
  }


  if (EEPROM.read(2) == 255) {
    EEPROM.write(0, 0);
    EEPROM.write(1, 0);
    EEPROM.write(2, 0);
  }

  record = EEPROM.read(0) * 256 + EEPROM.read(1);
  Serial.print("Record atual:");
  Serial.println(record);

  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.display();


} // void setup()

void loop() {
  wait = waitLoop(wait);
  if (wait) {
    return;
  }//if

  currentSequence = addNumber(currentSequence);

  showSequence();

  wait = playerAttempt();

  delay(500);
} // void loop()

void lightUp(int color, bool press) {

  digitalWrite(leds[color], HIGH);
  tone(buzzer, Tone[color], 200);
  delay(200);

  digitalWrite(leds[color], LOW);
  if (!press) {
    delay(300);
  }// if
}

void showSequence() {
  int sequenceSize = currentSequence.length();
  for (int i = 0; i < sequenceSize; i++) {
    int currentColor = currentSequence[i];
    currentColor = currentColor - 48;

    display.clearDisplay();

    display.setCursor(0, 0);
    display.print("O        record:");
    display.print(record);

    if (record < 10) {
      display.print(" ");
    }// if

    display.println("  O");

    display.println("O                   O");

    display.print("O   Sequencia: "); //15

    int sequenceSize = currentSequence.length();

    if (sequenceSize < 10) {
      display.print(" ");
    }//if

    display.print(sequenceSize);
    display.println("   O");


    display.print("O      prontos:");
    if (sequenceSize - 1 < 10) {
      display.print(" ");
    }//if

    display.print(sequenceSize -1);
    display.println("   O");
    

    display.display();
    lightUp(currentColor, false);
  }//for
}// void showSequence

int waitButton(bool isWaittingPlayer) {
  for (int i = 0; i < 5; i++) {
    if (digitalRead(buttons[i])) {
      while (digitalRead(buttons[i])) {
        if (!isWaittingPlayer) {
          // Liga a led e reproduz o som
          // caso seja a vez do jogador
          lightUp(i, true);
        }// if
        delay(1);
      }//while
      return i;
    } // if
  }// for
  return -1;
} // int waitButton

void animation(int stage) {

  display.clearDisplay();

  display.setCursor(0, 0);
  display.print("         record:");
  display.println(record);
  display.println("      Aguardando");
  display.print("      jogador");

  dotStageTimer = dotStageTimer + 0.25;

  int dotStage = dotStageTimer;

  if (dotStageTimer == 3.25) {
    dotStageTimer = 0;
  }// if

  for (int i = 0; i < dotStage; i++) {
    display.print(".");
  }// for


  display.println();

  display.setCursor(0, stage);
  display.println("O");
  display.setCursor(121, 28 - stage);
  display.println("O");


  display.display();

}// void animation();

bool waitLoop(bool isWaittingPlayer) {
  if (!isWaittingPlayer) {
    return isWaittingPlayer;
  }// if

  char consoleInput = Serial.read();
  if (consoleInput == 'r') {
    record = 0;
    EEPROM.write(1, 0);
    EEPROM.write(2, 0);

    Serial.println("Record reiniciado!");
  }

  bool isLightOn = false;
  for (int count = 0; count < 1001; count++) {
    int buttonPressed = waitButton(true);

    int stageOnTime = count / 37; // 37 para dividir o 1000 do timer para até 28 partes
    if (stageOnTime != animationStage) {
      animationStage = stageOnTime;
      animation(animationStage);
    } //if

    if (count % 200 == 0) {
      isLightOn = !isLightOn;
    }//if

    if (buttonPressed == 4) {
      isWaittingPlayer = false;
      isLightOn = false;
      Serial.println("Jogo inciiado");
    }//if

    for (int led : leds) {
      digitalWrite(led, isLightOn ? HIGH : LOW);
    }//for

    if (!isWaittingPlayer) {
      break;
    }//if
  }// for

  if (!isWaittingPlayer) {
    delay(200);
  }// if
  return isWaittingPlayer;
} //bool waitLoop()


void endMusic() {
  display.clearDisplay();
  display.setCursor(0,0);

  display.println("O                   O");
  display.println("O   Voce perdeu!!   O");
  display.println("O       T-T         O");
  display.println("O                   O");

  display.display();
  
  delay(300);
  tone(buzzer, 220, 300);

  delay(400);
  tone(buzzer, 208, 300);

  delay(450);
  tone(buzzer, 196, 300);

  delay(450);
  tone(buzzer, 185, 1500);
  delay(1000);

}// void endMusic

void breakRecord() {
  display.clearDisplay();
  display.setCursor(0,0);

  display.println("O                   O");
  display.println("O Record quebrado!! O");
  display.println("O       ^w^         O");
  display.println("O                   O");

  display.display();

  delay(300);
  tone(buzzer, 185, 180);

  delay(200);
  tone(buzzer, 233, 180);

  delay(200);
  tone(buzzer, 277, 180);

  delay(200);
  tone(buzzer, 740, 1500);

  delay(1000);

}// void breakRecord



bool playerAttempt() {
  String playerSequence = "";

  while (true) {
    int buttonPressed = waitButton(false);
    if (buttonPressed == 4) {
      currentSequence = "";
      return true;
    }//if

    if (buttonPressed == -1) {
      continue;
    }//if

    playerSequence = playerSequence + buttonPressed;

    //    Serial.print("Sequencia do player: ");
    //    Serial.println(playerSequence);

    int lastPressedIndex = playerSequence.length() - 1;

    if (playerSequence[lastPressedIndex] != currentSequence[lastPressedIndex]) {
      int sequenceSize = currentSequence.length() - 1;
      currentSequence = "";

      if (sequenceSize > record) {
        Alto = sequenceSize / 256;
        Baixo = sequenceSize;
        EEPROM.write(0, Alto);
        EEPROM.write(1, Baixo);
        Serial.println(EEPROM.read(0) * 256 + EEPROM.read(1));

        breakRecord();
        record = sequenceSize;
        return true;
      }// if
      endMusic();
      delay(300);
      return true;
    }//if

    if (playerSequence.length() == currentSequence.length()) {
      return false;
    }//if

  }//while
}// bool playerAttempt()


String addNumber(String currentList) {
  int New = rand() % 4;
  return currentList + New;
}// char addNumber