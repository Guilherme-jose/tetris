#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define NUMPIXELS 64
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

const int COLS = 8;
const int ROWS = 8;

int board[ROWS][COLS];
int currentPiece[4][4];
int pieceRow, pieceCol;
bool running = false;

struct I = {
  int shape[4][4] = {
    {1, 1, 1, 1},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  };
  String color = "purple";
};

struct J = {
  int shape[4][4] = {
    {1, 0, 0, 0},
    {1, 1, 1, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  };
  String color = "blue";
};

struct L = {
  int shape[4][4] = {
    {0, 0, 1, 0},
    {1, 1, 1, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  };
  String color = "orange";
};

struct O = {
  int shape[4][4] = {
    {1, 1, 0, 0},
    {1, 1, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  };
  String color = "yellow";
};

struct S = {
  int shape[4][4] = {
    {0, 1, 1, 0},
    {1, 1, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  };
  String color = "green";
};

struct T = {
  int shape[4][4] = {
    {0, 1, 0, 0},
    {1, 1, 1, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  };
  String color = "magenta";
};

struct Z = {
  int shape[4][4] = {
    {1, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  };
  String color = "red";
};




void drawPiece() {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (currentPiece[i][j] == 1) {
        pixels.setPixelColor((i + pieceRow) * COLS + j + pieceCol, pixels.Color(255, 0, 0));
      }
    }
  }
}

void createPiece() {
  int piece = random(7);
  switch (piece) {
    case 0:
      currentPiece = I.shape;
      break;
    case 1:
      currentPiece = J.shape;
      break;
    case 2:
      currentPiece = L.shape;
      break;
    case 3:
      currentPiece = O.shape;
      break;
    case 4:
      currentPiece = S.shape;
      break;
    case 5:
      currentPiece = T.shape;
      break;
    case 6:
      currentPiece = Z.shape;
      break;
  }
  pieceRow = 0;
  pieceCol = 3;
}

void drawBoard() {
  pixels.clear();
  for (int i = 0; i < ROWS; i++) {
    if (i % 2 == 0) {
      for (int j = 0; j < COLS; j++) {
        pixels.setPixelColor(i * COLS + j, pixels.Color(0, 0, 255));
      }
    } else {
      for (int j = COLS - 1; j >= 0; j--) {
        pixels.setPixelColor(i * COLS + j, pixels.Color(0, 0, 255));
      }
  pixels.show();
}

bool isCollision() {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (currentPiece[i][j] == 1) {
        if (i + pieceRow >= ROWS || j + pieceCol >= COLS || j + pieceCol < 0 || board[i + pieceRow][j + pieceCol] == 1) {
          return true;
        }
      }
    }
  }
  return false;
}

void moveLeft() {
  pieceCol--;
  if (isCollision()) {
    pieceCol++;
  }
  drawBoard();
}

void moveRight() {
  pieceCol++;
  if (isCollision()) {
    pieceCol--;
  }
  drawBoard();
}

void mergePiece() {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (currentPiece[i][j] == 1) {
        board[i + pieceRow][j + pieceCol] = 1;
      }
    }
  }
}

void clearLines() {
  for (int i = 0; i < ROWS; i++) {
    bool full = true;
    for (int j = 0; j < COLS; j++) {
      if (board[i][j] == 0) {
        full = false;
        break;
      }
    }
    if (full) {
      for (int j = 0; j < COLS; j++) {
        board[i][j] = 0;
      }
      for (int k = i; k > 0; k--) {
        for (int j = 0; j < COLS; j++) {
          board[k][j] = board[k - 1][j];
        }
      }
    }
  }
}

void rotatePiece() {
  int temp[4][4];
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      temp[i][j] = currentPiece[i][j];
    }
  }
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      currentPiece[i][j] = temp[3 - j][i];
    }
  }
  if (isCollision()) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        currentPiece[i][j] = temp[i][j];
      }
    }
  }
  drawBoard();
}

void movePieceDown() {
  pieceRow++;
  if (isCollision()) {
    pieceRow--;
    mergePiece();
    clearLines();
    createPiece();
    if (isCollision()) {
      running = false;
    }
  }
  drawBoard();
}

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
      board[i][j] = 0;
    }
  }
  pixels.begin();
  createPiece();
  running = true;
}

void loop() {
  if (running) {
    drawBoard();
    drawPiece();
    delay(500);
    movePieceDown();
  }
}

