#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

// Configurare pini
#define TFT_CS 10
#define TFT_DC 9
#define TFT_RST 8
#define JOYSTICK1_Y A0
#define JOYSTICK2_Y A1
#define BUTTON_PIN 3
#define BUZZER_PIN 7

// Culori definite
#define TFT_BLACK ST77XX_BLACK
#define TFT_WHITE ST77XX_WHITE
#define TFT_BLUE ST77XX_BLUE
#define TFT_RED ST77XX_RED

// Timpi definiți
const unsigned long DEBOUNCE_DELAY_MS = 100;
const unsigned long COUNTDOWN_INTERVAL_MS = 700;
const unsigned long WELCOME_SCREEN_TIME_MS = 2000;
const unsigned long WINNER_DISPLAY_TIME_MS = 1500;

// Scor țintă
const int MAX_SCORE = 5;

// Creare obiect ecran
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Variabile globale
int currentOption = 0;
int difficulty = 1;
int player1Score = 0, player2Score = 0;
bool buttonPressed = false;
bool gameStarted = false;
bool countdownActive = false;
bool welcomeScreenActive = true;

unsigned long lastDebounceTime = 0;
unsigned long lastCountdownTime = 0;
unsigned long welcomeScreenStartTime = 0;
unsigned long winnerDisplayStartTime = 0;

// Structuri pentru palete și minge
struct Paddle {
  int x, y, width, height;
} player1, player2;

struct Ball {
  int x, y, radius, dx, dy;
} ball;

// Numărătoare inversă
int countdownValue = 3;

// Prototipuri funcții
void displayWelcomeMessage();
void displayDifficultyMenu();
void confirmSelection();
void buttonPressISR();
void startGameCountdown();
void updateCountdown();
void startPongGame();
void updateGame();
void drawField();
void drawGame();
void resetBall();
void displayScores();
void showWinner(int player);
void resetGame();
void handleMenuNavigation();



void displayWelcomeMessage() {
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(1, 40);
  tft.print("Bine ai venit!");
}

void displayDifficultyMenu() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(20, 30);
  tft.print(currentOption == 0 ? "> Usor" : "  Usor");
  tft.setCursor(20, 50);
  tft.print(currentOption == 1 ? "> Mediu" : "  Mediu");
  tft.setCursor(20, 70);
  tft.print(currentOption == 2 ? "> Dificil" : "  Dificil");
}

void confirmSelection() {
  difficulty = currentOption + 1;
  tone(BUZZER_PIN, 1000, 200);
}

void startGameCountdown() {
  countdownActive = true;
  countdownValue = 3; // Resetăm numărătoarea la 3
  lastCountdownTime = millis(); // Setăm timpul de pornire
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(3);
  tft.setCursor(50, 50);
  tft.print(countdownValue); // Afișăm primul număr
}

void updateCountdown() {
  if (millis() - lastCountdownTime >= COUNTDOWN_INTERVAL_MS) {
    lastCountdownTime = millis();
    countdownValue--;

    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(3);
    tft.setCursor(50, 50);

    if (countdownValue > 0) {
      tft.print(countdownValue); // Afișăm următorul număr
      tone(BUZZER_PIN, 1000, 250);
    } else {
      tft.print("Start!");
      tone(BUZZER_PIN, 1500, 500);
      countdownActive = false; // Încheiem numărătoarea
      startPongGame();
    }
  }
}
// Poziția paletelor
void startPongGame() {
  player1 = {5, 60, 8, 30};
  player2 = {tft.width() - 13, 60, 8, 30};
  resetBall();
  drawField();
  gameStarted = true;
}

void resetBall() {
  ball = {tft.width() / 2, tft.height() / 2, 4, 2, 2};
}

void updateGame() {
  // Actualizăm pozițiile jucătorilor
  player1.y = map(analogRead(JOYSTICK1_Y), 0, 1023, 0, tft.height() - player1.height);
  player2.y = map(analogRead(JOYSTICK2_Y), 0, 1023, 0, tft.height() - player2.height);

  // Actualizăm poziția mingii
  ball.x += ball.dx * difficulty;
  ball.y += ball.dy * difficulty;

  // Coliziuni cu marginea de sus și de jos a ecranului
  if (ball.y <= 0 || ball.y >= tft.height() - ball.radius) {
    ball.dy *= -1;
  }

  // Coliziune cu paleta 1
  if (ball.x - ball.radius <= player1.x + player1.width &&
      ball.x - ball.radius >= player1.x &&
      ball.y + ball.radius >= player1.y &&
      ball.y - ball.radius <= player1.y + player1.height) {
    ball.dx *= -1;
    tone(BUZZER_PIN, 1500, 100);
  }

  // Coliziune cu paleta 2
  if (ball.x + ball.radius >= player2.x &&
      ball.x + ball.radius <= player2.x + player2.width &&
      ball.y + ball.radius >= player2.y &&
      ball.y - ball.radius <= player2.y + player2.height) {
    ball.dx *= -1;
    tone(BUZZER_PIN, 1500, 100);
  }

  // Goluri și resetare minge
  if (ball.x <= 0) {
    player2Score++;
    resetBall();
  } else if (ball.x >= tft.width()) {
    player1Score++;
    resetBall();
  }

  // Verificăm dacă un jucător a câștigat
  if (player1Score >= MAX_SCORE) {
    showWinner(1);
  } else if (player2Score >= MAX_SCORE) {
    showWinner(2);
  }

  // Desenăm starea jocului
  drawGame();
}



// Desenare teren
void drawField() {
  tft.drawRect(0, 0, tft.width(), tft.height(), TFT_WHITE);
  for (int i = 0; i < tft.height(); i += 10) {
    tft.fillRect(tft.width() / 2 - 1, i, 2, 5, TFT_WHITE);
  }
}

void drawGame() {
  tft.fillScreen(TFT_BLACK);
  drawField();

  tft.fillRect(player1.x, player1.y, player1.width, player1.height, TFT_BLUE);
  tft.fillRect(player2.x, player2.y, player2.width, player2.height, TFT_RED);
  tft.fillCircle(ball.x, ball.y, ball.radius, TFT_WHITE);

  displayScores();
}
// Afișare scor
void displayScores() {
  tft.setCursor(20, 10);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.print(player1Score);

  tft.setCursor(tft.width() - 40, 10);
  tft.print(player2Score);
}

void showWinner(int player) {
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(20, 50);
  tft.setTextSize(2);
  tft.print("Jucatorul ");
  tft.print(player);
  tft.print(" a castigat!");
  winnerDisplayStartTime = millis();
  while (millis() - winnerDisplayStartTime < WINNER_DISPLAY_TIME_MS) {
    // Menținem mesajul pe ecran
  }
  resetGame();
}

void resetGame() {
  player1Score = 0;
  player2Score = 0;
  gameStarted = false;
  countdownActive = false; 
  displayDifficultyMenu();
}

void buttonPressISR() {
  buttonPressed = true;
}

void handleMenuNavigation() {
  int joystickVal = analogRead(JOYSTICK2_Y);
  if (millis() - lastDebounceTime > DEBOUNCE_DELAY_MS) {
    if (joystickVal < 400 && currentOption > 0) {
      currentOption--;
      displayDifficultyMenu();
    } else if (joystickVal > 600 && currentOption < 2) {
      currentOption++;
      displayDifficultyMenu();
    }
    lastDebounceTime = millis();
  }
  if (buttonPressed) {
    buttonPressed = false;
    confirmSelection();
    startGameCountdown();
  }
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonPressISR, FALLING);

  tft.initR(INITR_BLACKTAB);
  tft.setRotation(2);
  tft.fillScreen(TFT_BLACK);

  displayWelcomeMessage();
  welcomeScreenStartTime = millis(); // Salvăm timpul de pornire pentru ecranul de bun venit
}

void loop() {
  if (welcomeScreenActive) {
    // Gestionăm afișarea mesajului de bun venit
    if (millis() - welcomeScreenStartTime >= WELCOME_SCREEN_TIME_MS) {
      welcomeScreenActive = false;
      displayDifficultyMenu();
    }
  } else if (countdownActive == true) {
    updateCountdown(); // Actualizăm numărătoarea inversă
  } else if (gameStarted == true) {
    updateGame();
  } else {
    handleMenuNavigation();
  }
}