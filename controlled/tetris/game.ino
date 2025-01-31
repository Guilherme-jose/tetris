#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define PIN        D3
#define NUMPIXELS 64
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

const int COLS = 8;
const int ROWS = 8;

int board[ROWS][COLS];
int currentPiece[4][4][4];
int pieceRotation = 0;
int pieceRow, pieceCol;
bool running = false;

int score = 0;

struct Piece {
  int shape[4][4][4];
};

Piece I = {
  {{ 
    {0, 0, 0, 0},
    {1, 1, 1, 1},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
    
  },{
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0}
  },
  {
    {0, 0, 0, 0},
    {1, 1, 1, 1},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  },
  {
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0}
  }},
};

Piece J = {
  {{ 
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 1, 1, 0},
    {0, 0, 0, 0}
  },
  {
    {0, 0, 0, 0},
    {1, 1, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 0}
  },
  {
    {0, 1, 1, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 0, 0}
  },
  {
    {0, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 1, 1, 0},
    {0, 0, 0, 0}
  }},
};

Piece L = {
    {{
        {0, 1, 0, 0},
        {0, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0}
    },
    {
        {0, 0, 0, 0},
        {1, 1, 1, 0},
        {1, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {
        {1, 1, 0, 0},
        {0, 1, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 0, 0}
    },
    {
        {0, 0, 1, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }}
};

Piece O = {
    {{
        {1, 1, 0, 0},
        {1, 1, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {
        {1, 1, 0, 0},
        {1, 1, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {
        {1, 1, 0, 0},
        {1, 1, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {
        {1, 1, 0, 0},
        {1, 1, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }
    },
};

Piece S = {
    {{
        {0, 1, 1, 0},
        {1, 1, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }, 
    {
        {0, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 0}
    },
    {
        {0, 0, 0, 0},
        {0, 1, 1, 0},
        {1, 1, 0, 0},
        {0, 0, 0, 0}
    },
    {
        {1, 0, 0, 0},
        {1, 1, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 0, 0}
    }}
};

Piece T = {
  {{
    {0, 1, 0, 0},
    {1, 1, 1, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  },
  {
    {0, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 1, 0, 0},
    {0, 0, 0, 0}
  },
  {
    {0, 0, 0, 0},
    {1, 1, 1, 0},
    {0, 1, 0, 0},
    {0, 0, 0, 0}
  },
  {
    {0, 1, 0, 0},
    {1, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 0, 0}
  }}
};

Piece Z = {
    {{
        {1, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    {
        {0, 0, 1, 0},
        {0, 1, 1, 0},
        {0, 1, 0, 0},
        {0, 0, 0, 0}
    },
    {
        {0, 0, 0, 0},
        {1, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0}
    },
    {
        {0, 1, 0, 0},
        {1, 1, 0, 0},
        {1, 0, 0, 0},
        {0, 0, 0, 0}
    }
    }
};


void drawPiece() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (currentPiece[pieceRotation][i][j] == 1) {
                int pixelIndex;
                if ((pieceRow + i) % 2 == 0) {
                    pixelIndex = (pieceRow + i) * COLS + (COLS - (pieceCol + j) - 1);
                } else {
                    pixelIndex = (pieceRow + i) * COLS + (pieceCol + j);
                }
                pixels.setPixelColor(pixelIndex, pixels.Color(255, 0, 0));
            }
        }
    }
}

void createPiece() {
  int piece = random(7);    
  int shape[4][4][4];
  switch (piece) {
    case 0:
      memcpy(shape, I.shape, sizeof(I.shape));
      break;
    case 1:
      memcpy(shape, J.shape, sizeof(J.shape));
      break;
    case 2:
      memcpy(shape, L.shape, sizeof(L.shape));
      break;
    case 3:
      memcpy(shape, O.shape, sizeof(O.shape));
      break;
    case 4:
      memcpy(shape, S.shape, sizeof(S.shape));
      break;
    case 5:
      memcpy(shape, T.shape, sizeof(T.shape));
      break;
    case 6:
      memcpy(shape, Z.shape, sizeof(Z.shape));
      break;
  }
    for (int k = 0; k < 4; k++) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                currentPiece[k][i][j] = shape[k][i][j];
            }
        }
    }

  pieceRow = 0;
  pieceCol = 3;
  pieceRotation = 0;
  score += 1;
}

void drawBoard() {
    pixels.clear();
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                int pixelIndex;
                if (i % 2 == 0) {
                    pixelIndex = i * COLS + (COLS - j - 1);
                } else {
                    pixelIndex = i * COLS + j;
                }
                if (board[i][j] == 1) {
                    pixels.setPixelColor(pixelIndex, pixels.Color(0, 255, 0));
                } else {
                    pixels.setPixelColor(pixelIndex, pixels.Color(0, 0, 0));
                }
            }
        }
    drawPiece();
    pixels.show();
}

bool isCollision() {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (currentPiece[pieceRotation][i][j] == 1) {
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
      if (currentPiece[pieceRotation][i][j] == 1) {
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
    pieceRotation = (pieceRotation + 1) % 4;
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
      reset();
    }
  }
  drawBoard();
}

void reset() {
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
      board[i][j] = 0;
    }
  }
  score = 0;
  createPiece();
  running = true;
}

void move(const String& direction) {
  if (direction == "left") {
    moveLeft();
  } else if (direction == "right") {
    moveRight();
  } else if (direction == "rotate") {
    rotatePiece();
  } else if (direction == "down") {
    movePieceDown();
  } else if (direction == "rotate") {
    rotatePiece();
  } else if (direction == "reset") {
    reset();
  }
}

void game_setup() {
    pixels.begin();
    randomSeed(analogRead(0));
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
        board[i][j] = 0;
        }
    }
    createPiece();
    running = true;
}

void game_loop() {
    static unsigned long lastUpdate = 0;
    unsigned long currentMillis = millis();
    if (currentMillis - lastUpdate >= 1000) {
        lastUpdate = currentMillis;
        movePieceDown();
        drawBoard();
        drawPiece();
    }
        
}

