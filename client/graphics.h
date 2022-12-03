#include <SFML/Graphics.hpp>

/// /////////// HELPER FUNCTIONS ////////////////////////

bool isWithinGame(int i, int j) {
    return i >= 0 && i < BOARD_SIZE && j >= 0 && j < BOARD_SIZE;
}

bool isEmptySquare(int i, int j) {
    return code.boardMatrix[i][j].type != PLAYER1_SQUARE &&
        code.boardMatrix[i][j].type != PLAYER2_SQUARE &&
        code.boardMatrix[i][j].type != KING1_SQUARE &&
        code.boardMatrix[i][j].type != KING2_SQUARE;
}

bool isPlayerSquare(int i, int j) {
    return code.boardMatrix[i][j].type == PLAYER1_SQUARE ||
           code.boardMatrix[i][j].type == PLAYER2_SQUARE ||
           code.boardMatrix[i][j].type == KING1_SQUARE ||
           code.boardMatrix[i][j].type == KING2_SQUARE;
}

bool isOption(int i, int j) {
    return code.boardMatrix[i][j].type == MOVE_OPTION_SQUARE;
}

bool isPlayer1Piece(int i, int j) {
    return code.boardMatrix[i][j].type == PLAYER1_SQUARE;
}

bool isPlayer1King(int i, int j) {
    return code.boardMatrix[i][j].type == KING1_SQUARE;
}

bool isPlayer2Piece(int i, int j) {
    return code.boardMatrix[i][j].type == PLAYER2_SQUARE;
}

bool isPlayer2King(int i, int j) {
    return code.boardMatrix[i][j].type == KING2_SQUARE;
}

void clearAllMoveOptions() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if(code.boardMatrix[i][j].type == MOVE_OPTION_SQUARE) {
                code.boardMatrix[i][j].type = EMPTY_SQUARE;
            }
        }
    }
}

/// /////////// END OF HELPER FUNCTIONS ////////////////////////

/// /////////// CHECK MOUSE POSITION ////////////////////////

bool mouseIsOnItem(sf::Vector2f itemPos, sf::Vector2f itemSize) {
    if(code.appProps.mousePos.x < itemPos.x || code.appProps.mousePos.y < itemPos.y){
        return false;
    }
    if(code.appProps.mousePos.x > itemPos.x+itemSize.x || code.appProps.mousePos.y > itemPos.y+itemSize.y) {
        return false;
    }
    return true;
}

int mouseIsOnPiece() {
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            if(mouseIsOnItem(code.boardMatrix[i][j].piece.getPosition(), sf::Vector2f(code.boardMatrix[i][j].piece.getRadius()*2, code.boardMatrix[i][j].piece.getRadius()*2))) {
                if(isPlayer1Piece(i, j) || isPlayer1King(i, j)) {
                    return code.boardMatrix[i][j].squareIndex;
                }
            }
        }
    }
    return -1;
}

int mouseIsOnOption() {
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            if(mouseIsOnItem(code.boardMatrix[i][j].square.getPosition(), sf::Vector2f(code.boardMatrix[i][j].square.getSize()))) {
                if(isOption(i, j)) {
                    return code.boardMatrix[i][j].squareIndex;
                }
            }
        }
    }
    return -1;
}

/// /////////// END OF CHECK MOUSE POSITION ////////////////////////

/// /////////// ACTION HANDLERS ////////////////////////

void onPieceClickHandler(int pieceIndex) {
    int foundI = 0, foundJ = 0;
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            if(code.boardMatrix[i][j].squareIndex == pieceIndex) {
                foundI = i;
                foundJ = j;
                break;
            }
        }
    }
    code.appProps.selectedPieceIndex = code.boardMatrix[foundI][foundJ].squareIndex;
    clearAllMoveOptions();
    printf("Selected piece: %d:%d.\n", foundI, foundJ);
    if(isPlayer1Piece(foundI, foundJ)) {
        if(isWithinGame(foundI-1, foundJ-1)) {
            if(isEmptySquare(foundI-1, foundJ-1)) {
                code.boardMatrix[foundI-1][foundJ-1].type = MOVE_OPTION_SQUARE;
            } else if(isPlayer2Piece(foundI-1, foundJ-1) || isPlayer2King(foundI-1, foundJ-1)) {
                if(isWithinGame(foundI-2, foundJ-2) && isEmptySquare(foundI-2, foundJ-2)) {
                    code.boardMatrix[foundI-2][foundJ-2].type = MOVE_OPTION_SQUARE;
                }
            }
        }
        if(isWithinGame(foundI-1, foundJ+1)) {
            if(isEmptySquare(foundI-1, foundJ+1)) {
                code.boardMatrix[foundI-1][foundJ+1].type = MOVE_OPTION_SQUARE;
            } else if(isPlayer2Piece(foundI-1, foundJ+1) || isPlayer2King(foundI-1, foundJ+1)) {
                if(isWithinGame(foundI-2, foundJ+2) && isEmptySquare(foundI-2, foundJ+2)) {
                    code.boardMatrix[foundI-2][foundJ+2].type = MOVE_OPTION_SQUARE;
                }
            }
        }
    } else if(isPlayer1King(foundI, foundJ)) {
        if(isWithinGame(foundI+1, foundJ-1)) {
            if(isEmptySquare(foundI+1, foundJ-1)) {
                code.boardMatrix[foundI+1][foundJ-1].type = MOVE_OPTION_SQUARE;
            } else if(isPlayer2Piece(foundI+1, foundJ-1) || isPlayer2King(foundI+1, foundJ-1)) {
                if(isWithinGame(foundI+2, foundJ-2) && isEmptySquare(foundI+2, foundJ-2)) {
                    code.boardMatrix[foundI+2][foundJ-2].type = MOVE_OPTION_SQUARE;
                }
            }
        }
        if(isWithinGame(foundI+1, foundJ+1)) {
            if(isEmptySquare(foundI+1, foundJ+1)) {
                code.boardMatrix[foundI+1][foundJ+1].type = MOVE_OPTION_SQUARE;
            } else if(isPlayer2Piece(foundI+1, foundJ+1) || isPlayer2King(foundI+1, foundJ+1)) {
                if(isWithinGame(foundI+2, foundJ+2) && isEmptySquare(foundI+2, foundJ+2)) {
                    code.boardMatrix[foundI+2][foundJ+2].type = MOVE_OPTION_SQUARE;
                }
            }
        }
    }
}

void onOptionClickHandler(int optionIndex) {
    int pieceI = 0, pieceJ = 0, optionI = 0, optionJ = 0;
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            if(code.boardMatrix[i][j].squareIndex == optionIndex) {
                optionI = i;
                optionJ = j;
            } else if(code.boardMatrix[i][j].squareIndex == code.appProps.selectedPieceIndex) {
                pieceI = i;
                pieceJ = j;
            }
        }
    }
    printf("Move piece %d:%d to option %d:%d", pieceI, pieceJ, optionI, optionJ);
    if(isPlayer1Piece(pieceI, pieceJ) && optionI != 0) {
        code.boardMatrix[optionI][optionJ].type = PLAYER1_SQUARE;
    } else {
        code.boardMatrix[optionI][optionJ].type = KING1_SQUARE;
    }
    code.boardMatrix[pieceI][pieceJ].type = EMPTY_SQUARE;
    if(abs(pieceI - optionI) == 2) {
        int toEatI = (pieceI+optionI)/2, toEatJ = (pieceJ+optionJ)/2;
        if(isPlayer2Piece(toEatI, toEatJ) || isPlayer2King(toEatI, toEatJ)) {
            printf(" and eat piece %d:%d", toEatI, toEatJ);
            code.boardMatrix[toEatI][toEatJ].type = EMPTY_SQUARE;
        }
    } else {
//        code.appProps.turn = PLAYER2;
    }
    clearAllMoveOptions();
    printf(".\n");
}

/// /////////// END OF ACTION HANDLERS ////////////////////////


/// /////////// DRAW ITEMS ////////////////////////


void drawTable() {
    int index = 0;
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            code.boardMatrix[i][j].squareIndex = ++index;
            code.boardMatrix[i][j].square.setPosition(BOARD_OFFSET_X + j*SQUARE_SIZE, BOARD_OFFSET_Y*2 + i*SQUARE_SIZE);
            code.boardMatrix[i][j].square.setSize(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
            if((i+j) % 2 == 0) {
                code.boardMatrix[i][j].square.setFillColor(SQUARE_COLOR1);
            } else {
                code.boardMatrix[i][j].square.setFillColor(SQUARE_COLOR2);
            }
        }
    }
}

void drawPiece(int i, int j) {
//    if(code.boardMatrix[i][j].type == EMPTY_SQUARE) {
//        code.boardMatrix[i][j].piece.setRadius(0);
//    } else
    if(isPlayerSquare(i, j)) {
        code.boardMatrix[i][j].piece.setRadius(PIECE_SIZE);
        code.boardMatrix[i][j].piece.setPosition(
            code.boardMatrix[i][j].square.getPosition().x + (code.boardMatrix[i][j].square.getSize().x - code.boardMatrix[i][j].piece.getRadius()*2) / 2,
            code.boardMatrix[i][j].square.getPosition().y + (code.boardMatrix[i][j].square.getSize().y - code.boardMatrix[i][j].piece.getRadius()*2) / 2
        );
        if(isPlayer1Piece(i, j)) {
            code.boardMatrix[i][j].piece.setFillColor(PIECE_COLOR1);
        } else if (isPlayer2Piece(i, j)) {
            code.boardMatrix[i][j].piece.setFillColor(PIECE_COLOR2);
        } else if(isPlayer1King(i, j)) {
            code.boardMatrix[i][j].piece.setFillColor(KING_COLOR1);
        } else if(isPlayer2King(i, j)) {
            code.boardMatrix[i][j].piece.setFillColor(KING_COLOR2);
        }
    }
}

void drawMoveOption(int i, int j) {
    if(code.boardMatrix[i][j].type == MOVE_OPTION_SQUARE) {
        code.boardMatrix[i][j].moveOption.setRadius(MOVE_OPTION_SIZE);
        code.boardMatrix[i][j].moveOption.setPosition(
            code.boardMatrix[i][j].square.getPosition().x + (code.boardMatrix[i][j].square.getSize().x - code.boardMatrix[i][j].moveOption.getRadius()*2) / 2,
            code.boardMatrix[i][j].square.getPosition().y + (code.boardMatrix[i][j].square.getSize().y - code.boardMatrix[i][j].moveOption.getRadius()*2) / 2
        );
        code.boardMatrix[i][j].moveOption.setFillColor(MOVE_OPTION_COLOR);
    }
}

void drawGame() {
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            drawPiece(i, j);
            drawMoveOption(i, j);
        }
    }
}

/// /////////// END OF DRAW ITEMS ////////////////////////


/// /////////// DISPLAY ////////////////////////

void displayBoardMatrix(sf::RenderWindow &window) {
    for(int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            window.draw(code.boardMatrix[i][j].square);
        }
    }
}

void displayPieces(sf::RenderWindow &window) {
    for(int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if(isPlayerSquare(i, j)) {
                window.draw(code.boardMatrix[i][j].piece);
            }
        }
    }
}

void displayMoveOptions(sf::RenderWindow &window) {
    for(int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if(code.boardMatrix[i][j].type == MOVE_OPTION_SQUARE) {
                window.draw(code.boardMatrix[i][j].moveOption);
            }
        }
    }
}

/// /////////// END OF DISPLAY ////////////////////////