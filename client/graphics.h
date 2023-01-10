#include <SFML/Graphics.hpp>
#include "helperFunctions.h"

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

bool mouseIsOnItem(sf::RectangleShape item) {
    const sf::Vector2f itemPos = item.getPosition();
    const sf::Vector2f itemSize = item.getSize();
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
            if(mouseIsOnItem(code.playingMenu.boardMatrix[i][j].piece.getPosition(), sf::Vector2f(code.playingMenu.boardMatrix[i][j].piece.getRadius()*2, code.playingMenu.boardMatrix[i][j].piece.getRadius()*2))) {
                if(isPlayer1Piece(i, j) || isPlayer1King(i, j)) {
                    return code.playingMenu.boardMatrix[i][j].squareIndex;
                }
            }
        }
    }
    return -1;
}

int mouseIsOnOption() {
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            if(mouseIsOnItem(code.playingMenu.boardMatrix[i][j].square.getPosition(), sf::Vector2f(code.playingMenu.boardMatrix[i][j].square.getSize()))) {
                if(isOption(i, j)) {
                    return code.playingMenu.boardMatrix[i][j].squareIndex;
                }
            }
        }
    }
    return -1;
}

int mouseIsOnPlayerCard() {
    if(!code.mainMenu.challenge.isModalOpen && !code.mainMenu.response.isModalOpen) {
        for(int i = 0; i < code.mainMenu.playersCount; i++) {
            if(mouseIsOnItem(code.mainMenu.playerCard[i].getPosition(), code.mainMenu.playerCard[i].getSize())) {
                return i;
            }
        }
    }
    return -1;
}

bool mouseIsOnQuitGame() {
    if(mouseIsOnItem(code.playingMenu.info.quitButton.getPosition(), code.playingMenu.info.quitButton.getSize())) {
        return true;
    }
    return false;
}

/// /////////// END OF CHECK MOUSE POSITION ////////////////////////

bool canEatPiece(int i, int j) {
    if(isPlayer1Piece(i, j)) {
        if(isWithinGame(i-1, j-1)) {
            if(!isEmptySquare(i-1, j-1) && isPlayer2Piece(i-1, j-1) || isPlayer2King(i-1, j-1)){
                if(isWithinGame(i-2, j-2) && isEmptySquare(i-2, j-2)) {
                    return true;
                }
            }
        }
        if(isWithinGame(i-1, j+1)) {
            if(!isEmptySquare(i-1, j+1) && isPlayer2Piece(i-1, j+1) || isPlayer2King(i-1, j+1)) {
                if(isWithinGame(i-2, j+2) && isEmptySquare(i-2, j+2)) {
                    return true;
                }
            }
        }
    } else if(isPlayer1King(i, j)) {
        if(isWithinGame(i-1, j-1)) {
            if(!isEmptySquare(i-1, j-1) && isPlayer2Piece(i-1, j-1) || isPlayer2King(i-1, j-1)){
                if(isWithinGame(i-2, j-2) && isEmptySquare(i-2, j-2)) {
                    return true;
                }
            }
        }
        if(isWithinGame(i-1, j+1)) {
            if(!isEmptySquare(i-1, j+1) && isPlayer2Piece(i-1, j+1) || isPlayer2King(i-1, j+1)) {
                if(isWithinGame(i-2, j+2) && isEmptySquare(i-2, j+2)) {
                    return true;
                }
            }
        }
        if(isWithinGame(i+1, j-1)) {
            if(!isEmptySquare(i+1, j-1) && isPlayer2Piece(i+1, j-1) || isPlayer2King(i+1, j-1)) {
                if(isWithinGame(i+2, j-2) && isEmptySquare(i+2, j-2)) {
                    return true;
                }
            }
        }
        if(isWithinGame(i+1, j+1)) {
            if(!isEmptySquare(i+1, j+1) && isPlayer2Piece(i+1, j+1) || isPlayer2King(i+1, j+1)) {
                if(isWithinGame(i+2, j+2) && isEmptySquare(i+2, j+2)) {
                    return true;
                }
            }
        }
    }
    return false;
}

/// /////////// ACTION HANDLERS ////////////////////////

void onPieceClickHandler(int pieceIndex) {
    int foundI = 0, foundJ = 0;
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            if(code.playingMenu.boardMatrix[i][j].squareIndex == pieceIndex) {
                foundI = i;
                foundJ = j;
                break;
            }
        }
    }
    code.playingMenu.selectedPieceIndex = code.playingMenu.boardMatrix[foundI][foundJ].squareIndex;
    clearAllMoveOptions();
    printf("Selected piece: %d:%d.\n", foundI, foundJ);
    if(isPlayer1Piece(foundI, foundJ)) {
        if(isWithinGame(foundI-1, foundJ-1)) {
            if(isEmptySquare(foundI-1, foundJ-1)) {
                code.playingMenu.boardMatrix[foundI-1][foundJ-1].type = MOVE_OPTION_SQUARE;
            }
            else if(isPlayer2Piece(foundI-1, foundJ-1) || isPlayer2King(foundI-1, foundJ-1)){
                if(isWithinGame(foundI-2, foundJ-2) && isEmptySquare(foundI-2, foundJ-2)) {
                    code.playingMenu.boardMatrix[foundI-2][foundJ-2].type = MOVE_OPTION_SQUARE;
                }
            }
        }
        if(isWithinGame(foundI-1, foundJ+1)) {
            if(isEmptySquare(foundI-1, foundJ+1)) {
                code.playingMenu.boardMatrix[foundI-1][foundJ+1].type = MOVE_OPTION_SQUARE;
            }
            else if(isPlayer2Piece(foundI-1, foundJ+1) || isPlayer2King(foundI-1, foundJ+1)) {
                if(isWithinGame(foundI-2, foundJ+2) && isEmptySquare(foundI-2, foundJ+2)) {
                    code.playingMenu.boardMatrix[foundI-2][foundJ+2].type = MOVE_OPTION_SQUARE;
                }
            }
        }
    } else if(isPlayer1King(foundI, foundJ)) {
        if(isWithinGame(foundI-1, foundJ-1)) {
            if(isEmptySquare(foundI-1, foundJ-1)) {
                code.playingMenu.boardMatrix[foundI-1][foundJ-1].type = MOVE_OPTION_SQUARE;
            }
            else if(isPlayer2Piece(foundI-1, foundJ-1) || isPlayer2King(foundI-1, foundJ-1)){
                if(isWithinGame(foundI-2, foundJ-2) && isEmptySquare(foundI-2, foundJ-2)) {
                    code.playingMenu.boardMatrix[foundI-2][foundJ-2].type = MOVE_OPTION_SQUARE;
                }
            }
        }
        if(isWithinGame(foundI-1, foundJ+1)) {
            if(isEmptySquare(foundI-1, foundJ+1)) {
                code.playingMenu.boardMatrix[foundI-1][foundJ+1].type = MOVE_OPTION_SQUARE;
            }
            else if(isPlayer2Piece(foundI-1, foundJ+1) || isPlayer2King(foundI-1, foundJ+1)) {
                if(isWithinGame(foundI-2, foundJ+2) && isEmptySquare(foundI-2, foundJ+2)) {
                    code.playingMenu.boardMatrix[foundI-2][foundJ+2].type = MOVE_OPTION_SQUARE;
                }
            }
        }
        if(isWithinGame(foundI+1, foundJ-1)) {
            if(isEmptySquare(foundI+1, foundJ-1)) {
                code.playingMenu.boardMatrix[foundI+1][foundJ-1].type = MOVE_OPTION_SQUARE;
            }
            else if(isPlayer2Piece(foundI+1, foundJ-1) || isPlayer2King(foundI+1, foundJ-1)) {
                if(isWithinGame(foundI+2, foundJ-2) && isEmptySquare(foundI+2, foundJ-2)) {
                    code.playingMenu.boardMatrix[foundI+2][foundJ-2].type = MOVE_OPTION_SQUARE;
                }
            }
        }
        if(isWithinGame(foundI+1, foundJ+1)) {
            if(isEmptySquare(foundI+1, foundJ+1)) {
                code.playingMenu.boardMatrix[foundI+1][foundJ+1].type = MOVE_OPTION_SQUARE;
            }
            else if(isPlayer2Piece(foundI+1, foundJ+1) || isPlayer2King(foundI+1, foundJ+1)) {
                if(isWithinGame(foundI+2, foundJ+2) && isEmptySquare(foundI+2, foundJ+2)) {
                    code.playingMenu.boardMatrix[foundI+2][foundJ+2].type = MOVE_OPTION_SQUARE;
                }
            }
        }
    }
}

void onOptionClickHandler(int optionIndex) {
    int pieceI = 0, pieceJ = 0, optionI = 0, optionJ = 0;
    char keepTurnMessage[] = "keep ";
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            if(code.playingMenu.boardMatrix[i][j].squareIndex == optionIndex) {
                optionI = i;
                optionJ = j;
            } else if(code.playingMenu.boardMatrix[i][j].squareIndex == code.playingMenu.selectedPieceIndex) {
                pieceI = i;
                pieceJ = j;
            }
        }
    }
    printf("Move piece %d:%d to option %d:%d", pieceI, pieceJ, optionI, optionJ);
    if(isPlayer1Piece(pieceI, pieceJ) && optionI != 0) {
        code.playingMenu.boardMatrix[optionI][optionJ].type = PLAYER1_SQUARE;
    } else {
        code.playingMenu.boardMatrix[optionI][optionJ].type = KING1_SQUARE;
    }
    code.playingMenu.boardMatrix[pieceI][pieceJ].type = EMPTY_SQUARE;
    if(abs(pieceI - optionI) == 2) {
        int toEatI = (pieceI+optionI)/2, toEatJ = (pieceJ+optionJ)/2;
        if(isPlayer2Piece(toEatI, toEatJ) || isPlayer2King(toEatI, toEatJ)) {
            printf(" and eat piece %d:%d", toEatI, toEatJ);
            code.playingMenu.boardMatrix[toEatI][toEatJ].type = EMPTY_SQUARE;
        }
//        else if(isPlayer1Piece(toEatI, toEatJ) || isPlayer1King(toEatI, toEatJ)) {
//            printf(" and jump over piece %d:%d", toEatI, toEatJ);
//        }
        if(!canEatPiece(optionI, optionJ)) {
            code.playingMenu.turn = PLAYER2;
            strcpy(keepTurnMessage, "pass ");
        }
    } else {
        code.playingMenu.turn = PLAYER2;
        strcpy(keepTurnMessage, "pass ");
    }
    clearAllMoveOptions();
    printf(".\n");
    code.playingMenu.selectedPieceIndex = -1;

    char res[MESSAGE_SIZE], charMatrix[MESSAGE_SIZE];
    intMatrixToChar(charMatrix);
    sendReadMessage(appendChars("setMatrix: ", code.appProps.playerName, " ", idToChar(code.playingMenu.gameId), " ", keepTurnMessage, charMatrix), res);
    if(isResponse(res, "result")) {
        gameResultHandler(res);
    }
}

void onPlayerNameChange(char inputChar) {
    if((int)inputChar == 8 && strlen(code.appProps.playerName) > 0) { // if backspace is pressed
        code.appProps.playerName[strlen(code.appProps.playerName)-1] = '\0';
    } else if((int)inputChar > 32 && strlen(code.appProps.playerName) < PLAYER_NAME_SIZE-1) {
        code.appProps.playerName[strlen(code.appProps.playerName)] = inputChar;
    }
    code.loginMenu.nameInput.setString(code.appProps.playerName);
}

void onPasswordChange(char inputChar) {
    if((int)inputChar == 8 && strlen(code.loginMenu.password) > 0) { // if backspace is pressed
        code.loginMenu.password[strlen(code.loginMenu.password)-1] = '\0';
    } else if((int)inputChar > 32 && strlen(code.loginMenu.password) < PLAYER_NAME_SIZE-1) {
        code.loginMenu.password[strlen(code.loginMenu.password)] = inputChar;
    }
    code.loginMenu.passwordInput.setString(convertPassword(code.loginMenu.password));
}

void onConfirmPasswordChange(char inputChar) {
    if((int)inputChar == 8 && strlen(code.loginMenu.confirmPassword) > 0) { // if backspace is pressed
        code.loginMenu.confirmPassword[strlen(code.loginMenu.confirmPassword)-1] = '\0';
    } else if((int)inputChar > 32 && strlen(code.loginMenu.confirmPassword) < PLAYER_NAME_SIZE-1) {
        code.loginMenu.confirmPassword[strlen(code.loginMenu.confirmPassword)] = inputChar;
    }
    code.loginMenu.confirmInput.setString(convertPassword(code.loginMenu.confirmPassword));
}

void onLoginFirstButtonClickHandler(sf::RenderWindow &window) {
    char res[MESSAGE_SIZE] = {'\0'};
    if(code.loginMenu.isLoginActive) {
        if(strlen(code.appProps.playerName) && strlen(code.loginMenu.password)) {
            sendReadMessage(appendChars("login: ", code.appProps.playerName, " ", code.loginMenu.password), res);
        } else {
            setError("Please fill in all the fields");
        }
    } else {
        if(strlen(code.appProps.playerName) && strlen(code.loginMenu.password) && strlen(code.loginMenu.confirmPassword)) {
            if(strcmp(code.loginMenu.password, code.loginMenu.confirmPassword) == 0) {
                sendReadMessage(appendChars("register: ", code.appProps.playerName, " ", code.loginMenu.password) ,res);
            } else {
                setError("Passwords do not match");
            }
        } else {
            setError("Please fill in all the fields");
        }
    }
    if(res[0] != '\0') {
        if(isBackendSuccess(res)) {
            code.appProps.appMenu = MAIN_MENU;
            window.setTitle(appendChars("Draughts - ", code.appProps.playerName));
        }
    }
}

void handleGetLobbyStatusResponse(char resMsg[]) {
    if(isResponse(resMsg, "playerList")) {
        char resCpy[MESSAGE_SIZE];
        strcpy(resCpy, resMsg + 12);
        char *player = strtok(resCpy, " ");
        code.mainMenu.playersCount = 0;
        while(player) {
            strcpy(code.mainMenu.players[code.mainMenu.playersCount], player);
            code.mainMenu.playersCount++;
            player = strtok(NULL, " ");
        }
    } else {
        if (isResponse(resMsg, "challenge")) {
            strcpy(resMsg, resMsg + 11);
            strcpy(code.appProps.enemyName, resMsg);
            code.mainMenu.challenge.isModalOpen = true;
        } else {
            if(isResponse(resMsg, "accepted")) {
                code.mainMenu.response.isModalOpen = false;
                initGame(resMsg);
            } else if(isResponse(resMsg, "declined")) {
                code.mainMenu.response.isModalOpen = false;
                setError("Player has declined your challenge");
            }
        }
    }
}

void onLeaderboardSwitch() {
    code.mainMenu.isLeaderboard = !code.mainMenu.isLeaderboard;
    code.mainMenu.playersCount = 0;
    if(code.mainMenu.isLeaderboard) {
        char resMsg[MESSAGE_SIZE];
        sendReadMessage(appendChars("getLeaderboard: ", code.appProps.playerName), resMsg);
        if(isResponse(resMsg, "leaderboard")) {
            char resCpy[MESSAGE_SIZE];
            strcpy(resCpy, resMsg + 13);

            code.mainMenu.playersCount = 0;
            char *playerInfo = strtok(resCpy, " ");
            while(playerInfo) {
                strcpy(code.mainMenu.players[code.mainMenu.playersCount], playerInfo);
                playerInfo = strtok(NULL, " ");
                strcpy(code.mainMenu.scores[code.mainMenu.playersCount], playerInfo);
                playerInfo = strtok(NULL, " ");
                code.mainMenu.playersCount++;
            }
        } else {
            setError("Can't fetch leaderboard data");
        }
    } else {
        char res[MESSAGE_SIZE];
        sendReadMessage(appendChars("getLobbyStatus: ", code.appProps.playerName), res);
        handleGetLobbyStatusResponse(res);
    }
}

void onResponseInviteHandler(bool hasAccepted) {
    char resMsg[MESSAGE_SIZE];
    if(hasAccepted) {
        sendReadMessage(appendChars("accept: ", code.appProps.playerName), resMsg);
        initGame(resMsg);
    } else {
        sendReadMessage(appendChars("decline: ", code.appProps.playerName), resMsg);
    }
    code.mainMenu.challenge.isModalOpen = false;
}

void handleGetMatrixUpdateResponse(char resMsg[]) {
    if(isResponse(resMsg, "turn")) {
        fetchMatrixInfo(resMsg);
    } else if(isResponse(resMsg, "result")) {
        gameResultHandler(resMsg);
    } else {
        setError("Unknown command");
    }
}

void onQuitGameHandler() {
    char res[MESSAGE_SIZE];
    if(code.playingMenu.winner == NO_WINNER) {
        sendReadMessage(appendChars("forfeit: ", code.appProps.playerName), res);
    } else {
        sendReadMessage(appendChars("returnToLobby: ", code.appProps.playerName), res);
    };
    code.appProps.appMenu = MAIN_MENU;
}

/// /////////// END OF ACTION HANDLERS ////////////////////////

void loadImages() {
    if(!code.appProps.backgroundTexture.loadFromFile("images/background.jpeg")) {
        printf("Failed to load texture");
    }
}

void loadFonts() {
    if (!code.appProps.arialMedium.loadFromFile("fonts/Arial-Medium.ttf"))
    {
        printf("Failed to load font");
    }
}

/// /////////// DRAW ITEMS ////////////////////////


void drawTable() {
    int index = 0;
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            code.playingMenu.boardMatrix[i][j].squareIndex = ++index;
            code.playingMenu.boardMatrix[i][j].square.setPosition(BOARD_OFFSET_X + j*SQUARE_SIZE, BOARD_OFFSET_Y*2 + i*SQUARE_SIZE);
            code.playingMenu.boardMatrix[i][j].square.setSize(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
            if((i+j) % 2 == 0) {
                code.playingMenu.boardMatrix[i][j].square.setFillColor(SQUARE_COLOR1);
            } else {
                code.playingMenu.boardMatrix[i][j].square.setFillColor(SQUARE_COLOR2);
            }
        }
    }
}

void drawPiece(int i, int j) {
    if(isPlayerSquare(i, j)) {
        code.playingMenu.boardMatrix[i][j].piece.setRadius(PIECE_SIZE);

        code.playingMenu.boardMatrix[i][j].piece.setPosition(
            code.playingMenu.boardMatrix[i][j].square.getPosition().x + (code.playingMenu.boardMatrix[i][j].square.getSize().x - code.playingMenu.boardMatrix[i][j].piece.getRadius()*2) / 2,
            code.playingMenu.boardMatrix[i][j].square.getPosition().y + (code.playingMenu.boardMatrix[i][j].square.getSize().y - code.playingMenu.boardMatrix[i][j].piece.getRadius()*2) / 2
        );
        if(isPlayer1Piece(i, j)) {
            code.playingMenu.boardMatrix[i][j].piece.setOutlineThickness(0);
            if(code.playingMenu.boardMatrix[i][j].squareIndex != code.playingMenu.selectedPieceIndex) {
                if(code.playingMenu.playerColor == PLAYER1) {
                    code.playingMenu.boardMatrix[i][j].piece.setFillColor(PIECE_COLOR1);
                } else {
                    code.playingMenu.boardMatrix[i][j].piece.setFillColor(PIECE_COLOR2);
                }
            } else {
                if(code.playingMenu.playerColor == PLAYER1) {
                    code.playingMenu.boardMatrix[i][j].piece.setFillColor(SELECTED_PIECE_COLOR1);
                } else {
                    code.playingMenu.boardMatrix[i][j].piece.setFillColor(SELECTED_PIECE_COLOR2);
                }
            }
        } else if (isPlayer2Piece(i, j)) {
            code.playingMenu.boardMatrix[i][j].piece.setOutlineThickness(0);
            if(code.playingMenu.boardMatrix[i][j].squareIndex != code.playingMenu.selectedPieceIndex) {
                if(code.playingMenu.playerColor == PLAYER1) {
                    code.playingMenu.boardMatrix[i][j].piece.setFillColor(PIECE_COLOR2);
                } else {
                    code.playingMenu.boardMatrix[i][j].piece.setFillColor(PIECE_COLOR1);
                }
            } else {
                if(code.playingMenu.playerColor == PLAYER1) {
                    code.playingMenu.boardMatrix[i][j].piece.setFillColor(SELECTED_PIECE_COLOR2);
                } else {
                    code.playingMenu.boardMatrix[i][j].piece.setFillColor(SELECTED_PIECE_COLOR1);
                }
            }
        } else if(isPlayer1King(i, j)) {
            code.playingMenu.boardMatrix[i][j].piece.setOutlineColor(KING_OUTLINE_COLOR);
            code.playingMenu.boardMatrix[i][j].piece.setOutlineThickness(8);
            if(code.playingMenu.boardMatrix[i][j].squareIndex != code.playingMenu.selectedPieceIndex) {
                if(code.playingMenu.playerColor == PLAYER1) {
                    code.playingMenu.boardMatrix[i][j].piece.setFillColor(KING_COLOR1);
                } else {
                    code.playingMenu.boardMatrix[i][j].piece.setFillColor(KING_COLOR2);
                }
            } else {
                if(code.playingMenu.playerColor == PLAYER1) {
                    code.playingMenu.boardMatrix[i][j].piece.setFillColor(SELECTED_KING_COLOR1);
                } else {
                    code.playingMenu.boardMatrix[i][j].piece.setFillColor(SELECTED_KING_COLOR2);
                }
            }
        } else if(isPlayer2King(i, j)) {
            code.playingMenu.boardMatrix[i][j].piece.setOutlineColor(KING_OUTLINE_COLOR);
            code.playingMenu.boardMatrix[i][j].piece.setOutlineThickness(8);
            if(code.playingMenu.boardMatrix[i][j].squareIndex != code.playingMenu.selectedPieceIndex) {
                if(code.playingMenu.playerColor == PLAYER1) {
                    code.playingMenu.boardMatrix[i][j].piece.setFillColor(KING_COLOR2);
                } else {
                    code.playingMenu.boardMatrix[i][j].piece.setFillColor(KING_COLOR1);
                }
            } else {
                if(code.playingMenu.playerColor == PLAYER1) {
                    code.playingMenu.boardMatrix[i][j].piece.setFillColor(SELECTED_KING_COLOR2);
                } else {
                    code.playingMenu.boardMatrix[i][j].piece.setFillColor(SELECTED_KING_COLOR1);
                }
            }
        }
    }
}

void drawMoveOption(int i, int j) {
    if(code.playingMenu.boardMatrix[i][j].type == MOVE_OPTION_SQUARE) {
        code.playingMenu.boardMatrix[i][j].moveOption.setRadius(MOVE_OPTION_SIZE);
        code.playingMenu.boardMatrix[i][j].moveOption.setPosition(
            code.playingMenu.boardMatrix[i][j].square.getPosition().x + (code.playingMenu.boardMatrix[i][j].square.getSize().x - code.playingMenu.boardMatrix[i][j].moveOption.getRadius()*2) / 2,
            code.playingMenu.boardMatrix[i][j].square.getPosition().y + (code.playingMenu.boardMatrix[i][j].square.getSize().y - code.playingMenu.boardMatrix[i][j].moveOption.getRadius()*2) / 2
        );
        code.playingMenu.boardMatrix[i][j].moveOption.setFillColor(MOVE_OPTION_COLOR);
    }
}

void drawGame() {
    code.appProps.background.setTexture(&code.appProps.backgroundTexture);
    code.appProps.background.setSize(sf::Vector2f(float(WINDOW_WIDTH), float(WINDOW_HEIGHT)));

    drawTable();
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            drawPiece(i, j);
            drawMoveOption(i, j);
        }
    }

    code.playingMenu.info.topBackground.setSize(sf::Vector2f(WINDOW_WIDTH, 2*BOARD_OFFSET_Y));
    code.playingMenu.info.topBackground.setPosition(sf::Vector2f(0, 0));
    if(code.playingMenu.playerColor == PLAYER1) {
        code.playingMenu.info.topBackground.setFillColor(SELECTED_PIECE_COLOR1);
    } else {
        code.playingMenu.info.topBackground.setFillColor(SELECTED_PIECE_COLOR2);
    }
    if(code.playingMenu.winner == NO_WINNER) {
        code.playingMenu.info.turnMessage.setString(code.playingMenu.turn == PLAYER1 ? "Your turn" : appendChars(code.appProps.enemyName, "'s turn"));
    } else if(code.playingMenu.winner == PLAYER1) {
        code.playingMenu.info.turnMessage.setString("You won!");
    } else if(code.playingMenu.winner == PLAYER2) {
        code.playingMenu.info.turnMessage.setString("You lost!");
    } else if(code.playingMenu.winner == DRAW) {
        code.playingMenu.info.turnMessage.setString("It's a draw!");
    }
    code.playingMenu.info.turnMessage.setFillColor(sf::Color::White);
    code.playingMenu.info.turnMessage.setStyle(sf::Text::Bold);
    code.playingMenu.info.turnMessage.setCharacterSize(35);
    code.playingMenu.info.turnMessage.setOutlineColor(sf::Color::Black);
    code.playingMenu.info.turnMessage.setOutlineThickness(1);
    code.playingMenu.info.turnMessage.setFont(code.appProps.arialMedium);
    code.playingMenu.info.turnMessage.setPosition(center(code.playingMenu.info.topBackground, code.playingMenu.info.turnMessage));

    code.playingMenu.info.quitButton.setFillColor(sf::Color::Red);
    code.playingMenu.info.quitButton.setPosition(sf::Vector2f(0, 0));
    code.playingMenu.info.quitButton.setSize(sf::Vector2f(BOARD_OFFSET_X, code.playingMenu.info.topBackground.getSize().y));
    code.playingMenu.info.quitButtonText.setPosition(center(code.playingMenu.info.quitButton, code.playingMenu.info.quitButtonText));
    code.playingMenu.info.quitButtonText.setFillColor(sf::Color::White);
    code.playingMenu.info.quitButtonText.setStyle(sf::Text::Bold);
    code.playingMenu.info.quitButtonText.setCharacterSize(30);
    code.playingMenu.info.quitButtonText.setOutlineColor(sf::Color::Black);
    code.playingMenu.info.quitButtonText.setOutlineThickness(1);
    code.playingMenu.info.quitButtonText.setFont(code.appProps.arialMedium);
    code.playingMenu.info.quitButtonText.setString("QUIT");

    code.playingMenu.info.piecesLeft.setFillColor(sf::Color::Red);
    code.playingMenu.info.piecesLeft.setSize(sf::Vector2f(BOARD_OFFSET_X, code.playingMenu.info.topBackground.getSize().y));
    code.playingMenu.info.piecesLeft.setPosition(sf::Vector2f(WINDOW_WIDTH - code.playingMenu.info.piecesLeft.getSize().x, 0));
    code.playingMenu.info.piecesLeftText.setPosition(center(code.playingMenu.info.piecesLeft, code.playingMenu.info.piecesLeftText));
    code.playingMenu.info.piecesLeftText.setFillColor(sf::Color::White);
    code.playingMenu.info.piecesLeftText.setStyle(sf::Text::Bold);
    code.playingMenu.info.piecesLeftText.setCharacterSize(30);
    code.playingMenu.info.piecesLeftText.setOutlineColor(sf::Color::Black);
    code.playingMenu.info.piecesLeftText.setOutlineThickness(1);
    code.playingMenu.info.piecesLeftText.setFont(code.appProps.arialMedium);
    char remainingPieces[50];
    sprintf(remainingPieces, "Pieces: %d", countLeftPieces());
    code.playingMenu.info.piecesLeftText.setString(remainingPieces);
}

void drawLoginMenu() {
    code.appProps.modalTitle.setString(code.loginMenu.isLoginActive ? "LOGIN" : "REGISTER");

    code.loginMenu.nameText.setString("Username: ");
    code.loginMenu.nameText.setFillColor(sf::Color::Black);
    code.loginMenu.nameText.setStyle(sf::Text::Bold);
    code.loginMenu.nameText.setCharacterSize(30);
    code.loginMenu.nameText.setFont(code.appProps.arialMedium);
    code.loginMenu.nameText.setPosition(sf::Vector2f(
            code.appProps.modal.getPosition().x + (MODAL_WIDTH - code.loginMenu.nameText.getGlobalBounds().width - code.loginMenu.nameInputBackground.getSize().x-30)/2,
            code.appProps.modal.getPosition().x + (MODAL_HEIGHT - code.loginMenu.nameText.getGlobalBounds().height - code.loginMenu.passwordText.getGlobalBounds().height - (code.loginMenu.isLoginActive ? (code.loginMenu.confirmText.getGlobalBounds().height - 120) : 0) - 300)/2));
    code.loginMenu.nameInputBackground.setSize(sf::Vector2f(600, 70));
    code.loginMenu.nameInputBackground.setPosition(sf::Vector2f(code.loginMenu.nameText.getPosition().x + code.loginMenu.nameText.getGlobalBounds().width + 30, code.loginMenu.nameText.getPosition().y-15));
    code.loginMenu.nameInputBackground.setFillColor(sf::Color::White);
    code.loginMenu.nameInput.setPosition(sf::Vector2f(code.loginMenu.nameInputBackground.getPosition().x+10, center(code.loginMenu.nameInputBackground, code.loginMenu.nameInput).y));
    code.loginMenu.nameInput.setFillColor(sf::Color::Black);
    code.loginMenu.nameInput.setStyle(sf::Text::Bold);
    code.loginMenu.nameInput.setCharacterSize(30);
    code.loginMenu.nameInput.setFont(code.appProps.arialMedium);

    code.loginMenu.passwordText.setString("Password: ");
    code.loginMenu.passwordText.setFillColor(sf::Color::Black);
    code.loginMenu.passwordText.setStyle(sf::Text::Bold);
    code.loginMenu.passwordText.setCharacterSize(30);
    code.loginMenu.passwordText.setFont(code.appProps.arialMedium);
    code.loginMenu.passwordText.setPosition(sf::Vector2f(code.loginMenu.passwordInputBackground.getPosition().x - code.loginMenu.passwordText.getGlobalBounds().width - 30, code.loginMenu.nameText.getPosition().y + 120));
    code.loginMenu.passwordInputBackground.setSize(sf::Vector2f(600, 70));
    code.loginMenu.passwordInputBackground.setPosition(sf::Vector2f(code.loginMenu.nameInputBackground.getPosition().x, code.loginMenu.passwordText.getPosition().y-15));
    code.loginMenu.passwordInputBackground.setFillColor(sf::Color::White);
    code.loginMenu.passwordInput.setPosition(sf::Vector2f(code.loginMenu.passwordInputBackground.getPosition().x + 10, center(code.loginMenu.passwordInputBackground, code.loginMenu.passwordInput).y));
    code.loginMenu.passwordInput.setFillColor(sf::Color::Black);
    code.loginMenu.passwordInput.setStyle(sf::Text::Bold);
    code.loginMenu.passwordInput.setCharacterSize(30);
    code.loginMenu.passwordInput.setFont(code.appProps.arialMedium);

    code.loginMenu.confirmText.setString("Confirm password: ");
    code.loginMenu.confirmText.setFillColor(sf::Color::Black);
    code.loginMenu.confirmText.setStyle(sf::Text::Bold);
    code.loginMenu.confirmText.setCharacterSize(30);
    code.loginMenu.confirmText.setFont(code.appProps.arialMedium);
    code.loginMenu.confirmText.setPosition(sf::Vector2f(code.loginMenu.confirmInputBackground.getPosition().x - code.loginMenu.confirmText.getGlobalBounds().width - 30, code.loginMenu.passwordText.getPosition().y + 120));
    code.loginMenu.confirmInputBackground.setSize(sf::Vector2f(600, 70));
    code.loginMenu.confirmInputBackground.setPosition(sf::Vector2f(code.loginMenu.nameInputBackground.getPosition().x, code.loginMenu.confirmText.getPosition().y-15));
    code.loginMenu.confirmInputBackground.setFillColor(sf::Color::White);
    code.loginMenu.confirmInput.setPosition(sf::Vector2f(code.loginMenu.confirmInputBackground.getPosition().x + 10, center(code.loginMenu.confirmInputBackground, code.loginMenu.confirmInput).y));
    code.loginMenu.confirmInput.setFillColor(sf::Color::Black);
    code.loginMenu.confirmInput.setStyle(sf::Text::Bold);
    code.loginMenu.confirmInput.setCharacterSize(30);
    code.loginMenu.confirmInput.setFont(code.appProps.arialMedium);

    code.loginMenu.secondButton.setSize(sf::Vector2f(MODAL_WIDTH*0.4, 70));
    code.loginMenu.secondButton.setPosition(sf::Vector2f(center(code.appProps.modal, code.loginMenu.secondButton).x,  code.appProps.modal.getPosition().y+MODAL_HEIGHT-code.loginMenu.firstButton.getSize().y-50));
    code.loginMenu.secondButton.setFillColor(sf::Color::Red);
    code.loginMenu.secondButtonText.setString(code.loginMenu.isLoginActive ? "Go to Register" : "Go to Login");
    code.loginMenu.secondButtonText.setFillColor(sf::Color::Black);
    code.loginMenu.secondButtonText.setStyle(sf::Text::Bold);
    code.loginMenu.secondButtonText.setCharacterSize(30);
    code.loginMenu.secondButtonText.setFont(code.appProps.arialMedium);
    code.loginMenu.secondButtonText.setPosition(sf::Vector2f(center(code.loginMenu.secondButton, code.loginMenu.secondButtonText)));

    code.loginMenu.firstButton.setSize(sf::Vector2f(MODAL_WIDTH*0.5, 70));
    code.loginMenu.firstButton.setPosition(sf::Vector2f(center(code.appProps.modal, code.loginMenu.firstButton).x, code.loginMenu.secondButton.getPosition().y-code.loginMenu.firstButton.getSize().y-30));
    code.loginMenu.firstButton.setFillColor(sf::Color::Green);
    code.loginMenu.firstButtonText.setString(code.loginMenu.isLoginActive ? "Login" : "Register");
    code.loginMenu.firstButtonText.setFillColor(sf::Color::Black);
    code.loginMenu.firstButtonText.setStyle(sf::Text::Bold);
    code.loginMenu.firstButtonText.setCharacterSize(30);
    code.loginMenu.firstButtonText.setFont(code.appProps.arialMedium);
    code.loginMenu.firstButtonText.setPosition(sf::Vector2f(center(code.loginMenu.firstButton, code.loginMenu.firstButtonText)));
}

void drawMainMenu(sf::RenderWindow &window) {
    code.appProps.modalTitle.setString(!code.mainMenu.isLeaderboard ? "ONLINE PLAYERS" : "LEADERBOARD");
    code.appProps.modalTitle.setPosition(sf::Vector2f(code.appProps.modalTitle.getPosition().x, code.appProps.modal.getPosition().y + 30));

    code.mainMenu.playersBackground.setSize(sf::Vector2f(code.appProps.modal.getSize().x*0.75, code.appProps.modal.getSize().y*0.7));
    code.mainMenu.playersBackground.setPosition(sf::Vector2f(center(code.appProps.modal, code.mainMenu.playersBackground).x, code.appProps.modalTitle.getPosition().y + 90));
    code.mainMenu.playersBackground.setFillColor(sf::Color::Black);

    code.mainMenu.button.setSize(sf::Vector2f(MODAL_WIDTH*0.4, 70));
    code.mainMenu.button.setPosition(sf::Vector2f(center(code.appProps.modal, code.mainMenu.button).x, code.mainMenu.playersBackground.getPosition().y + code.mainMenu.playersBackground.getSize().y + 30));
    code.mainMenu.button.setFillColor(sf::Color(179, 179, 179));
    code.mainMenu.buttonText.setString(!code.mainMenu.isLeaderboard ? "Go to Leaderboards" : "Go to Players");
    code.mainMenu.buttonText.setFillColor(sf::Color::Black);
    code.mainMenu.buttonText.setStyle(sf::Text::Bold);
    code.mainMenu.buttonText.setCharacterSize(30);
    code.mainMenu.buttonText.setFont(code.appProps.arialMedium);
    code.mainMenu.buttonText.setPosition(center(code.mainMenu.button, code.mainMenu.buttonText));

    for(int i = 0; i < MAX_CARDS; i++) {
        code.mainMenu.playerScore[i].setString("");
        code.mainMenu.playerText[i].setString("");
        if(i < code.mainMenu.playersCount) {
            code.mainMenu.playerText[i].setString(code.mainMenu.players[i]);
            if(code.mainMenu.isLeaderboard) {
                code.mainMenu.playerScore[i].setString(appendChars("score: ", code.mainMenu.scores[i]));
            }
        }

        code.mainMenu.playerCard[i].setSize(sf::Vector2f(code.mainMenu.playersBackground.getSize().x-10, (code.mainMenu.playersBackground.getSize().y - 5*(MAX_CARDS+1))/MAX_CARDS));
        code.mainMenu.playerCard[i].setPosition(sf::Vector2f(
                center(code.mainMenu.playersBackground,  code.mainMenu.playerCard[i]).x,
                i == 0 ? code.mainMenu.playersBackground.getPosition().y + 5 : (code.mainMenu.playerCard[i-1].getPosition().y + code.mainMenu.playerCard[i-1].getSize().y + 5)));
        code.mainMenu.playerCard[i].setFillColor(sf::Color::White);

        code.mainMenu.playerText[i].setFillColor(sf::Color::Black);
        code.mainMenu.playerText[i].setStyle(sf::Text::Bold);
        code.mainMenu.playerText[i].setCharacterSize(30);
        code.mainMenu.playerText[i].setFont(code.appProps.arialMedium);
        code.mainMenu.playerText[i].setPosition(code.mainMenu.playerCard[i].getPosition().x + 20,center( code.mainMenu.playerCard[i], code.mainMenu.playerText[i]).y);

        code.mainMenu.playerScore[i].setFillColor(sf::Color::Black);
        code.mainMenu.playerScore[i].setStyle(sf::Text::Bold);
        code.mainMenu.playerScore[i].setCharacterSize(30);
        code.mainMenu.playerScore[i].setFont(code.appProps.arialMedium);
        code.mainMenu.playerScore[i].setPosition(code.mainMenu.playerCard[i].getPosition().x + 650,center( code.mainMenu.playerCard[i], code.mainMenu.playerText[i]).y);
    }
}

void drawError() {
    code.appProps.errorBackground.setSize(sf::Vector2f(WINDOW_WIDTH/1.5, 60));
    code.appProps.errorBackground.setPosition(sf::Vector2f(center(code.appProps.background, code.appProps.errorBackground).x, 0));
    code.appProps.errorBackground.setFillColor(sf::Color::Red);
    code.appProps.errorBackground.setOutlineThickness(3);
    code.appProps.errorBackground.setOutlineColor(sf::Color::Black);
    code.appProps.errorText.setString(code.appProps.error);
    code.appProps.errorText.setFillColor(sf::Color::White);
    code.appProps.errorText.setStyle(sf::Text::Bold);
    code.appProps.errorText.setCharacterSize(30);
    code.appProps.errorText.setFont(code.appProps.arialMedium);
    code.appProps.errorText.setPosition(center(code.appProps.errorBackground, code.appProps.errorText));

}

void drawModal() {
    code.appProps.background.setTexture(&code.appProps.backgroundTexture);
    code.appProps.background.setSize(sf::Vector2f(float(WINDOW_WIDTH), float(WINDOW_HEIGHT)));

    code.appProps.modal.setSize(sf::Vector2f(MODAL_WIDTH, MODAL_HEIGHT));
    code.appProps.modal.setPosition(sf::Vector2f((WINDOW_WIDTH - MODAL_WIDTH)/2, (WINDOW_HEIGHT - MODAL_HEIGHT)/2));
    code.appProps.modal.setFillColor(MODAL_COLOR);
    code.appProps.modalTitle.setFillColor(sf::Color::Black);
    code.appProps.modalTitle.setStyle(sf::Text::Bold);
    code.appProps.modalTitle.setCharacterSize(50);
    code.appProps.modalTitle.setFont(code.appProps.arialMedium);
    code.appProps.modalTitle.setPosition(sf::Vector2f(center(code.appProps.modal, code.appProps.modalTitle).x, code.appProps.modal.getPosition().y + 75));
}

void drawChallengeModal() {
    code.mainMenu.challenge.challengeBackground.setSize(sf::Vector2f(700, 300));
    code.mainMenu.challenge.challengeBackground.setPosition(center(code.appProps.background, code.mainMenu.challenge.challengeBackground));
    code.mainMenu.challenge.challengeBackground.setFillColor(MODAL_COLOR);

    code.mainMenu.challenge.playerName.setString(appendChars("Player ", code.appProps.enemyName));
    code.mainMenu.challenge.playerName.setFillColor(sf::Color::Black);
    code.mainMenu.challenge.playerName.setStyle(sf::Text::Bold);
    code.mainMenu.challenge.playerName.setCharacterSize(30);
    code.mainMenu.challenge.playerName.setFont(code.appProps.arialMedium);
    code.mainMenu.challenge.playerName.setPosition(sf::Vector2f(
        center(code.mainMenu.challenge.challengeBackground, code.mainMenu.challenge.playerName).x,
        code.mainMenu.challenge.challengeBackground.getPosition().y + 30));
    code.mainMenu.challenge.title.setString("has challenged you!");
    code.mainMenu.challenge.title.setFillColor(sf::Color::Black);
    code.mainMenu.challenge.title.setStyle(sf::Text::Bold);
    code.mainMenu.challenge.title.setCharacterSize(30);
    code.mainMenu.challenge.title.setFont(code.appProps.arialMedium);
    code.mainMenu.challenge.title.setPosition(sf::Vector2f(
            center(code.mainMenu.challenge.challengeBackground, code.mainMenu.challenge.title).x,
            code.mainMenu.challenge.playerName.getPosition().y + 50));

    code.mainMenu.challenge.acceptButton.setSize(sf::Vector2f(200, 80));
    code.mainMenu.challenge.acceptButton.setPosition(sf::Vector2f(
            code.mainMenu.challenge.challengeBackground.getPosition().x + (code.mainMenu.challenge.challengeBackground.getSize().x - (2*code.mainMenu.challenge.acceptButton.getSize().x))/3,
            code.mainMenu.challenge.title.getPosition().y + 110));
    code.mainMenu.challenge.acceptButton.setFillColor(sf::Color(71, 209, 71));
    code.mainMenu.challenge.accept.setString("ACCEPT");
    code.mainMenu.challenge.accept.setFillColor(sf::Color::Black);
    code.mainMenu.challenge.accept.setStyle(sf::Text::Bold);
    code.mainMenu.challenge.accept.setCharacterSize(30);
    code.mainMenu.challenge.accept.setFont(code.appProps.arialMedium);
    code.mainMenu.challenge.accept.setPosition(center(code.mainMenu.challenge.acceptButton, code.mainMenu.challenge.accept));

    code.mainMenu.challenge.declineButton.setSize(sf::Vector2f(200, 80));
    code.mainMenu.challenge.declineButton.setPosition(sf::Vector2f(code.mainMenu.challenge.acceptButton.getPosition().x + code.mainMenu.challenge.acceptButton.getSize().x + (code.mainMenu.challenge.challengeBackground.getSize().x - (2*code.mainMenu.challenge.declineButton.getSize().x))/3, code.mainMenu.challenge.acceptButton.getPosition().y));
    code.mainMenu.challenge.declineButton.setFillColor(sf::Color(255, 51, 51));
    code.mainMenu.challenge.decline.setString("DECLINE");
    code.mainMenu.challenge.decline.setFillColor(sf::Color::Black);
    code.mainMenu.challenge.decline.setStyle(sf::Text::Bold);
    code.mainMenu.challenge.decline.setCharacterSize(30);
    code.mainMenu.challenge.decline.setFont(code.appProps.arialMedium);
    code.mainMenu.challenge.decline.setPosition(center(code.mainMenu.challenge.declineButton, code.mainMenu.challenge.decline));
}

void drawResponseModal() {
    code.mainMenu.response.challengeBackground.setSize(sf::Vector2f(700, 300));
    code.mainMenu.response.challengeBackground.setPosition(center(code.appProps.background, code.mainMenu.response.challengeBackground));
    code.mainMenu.response.challengeBackground.setFillColor(MODAL_COLOR);

    code.mainMenu.response.playerName.setString("Waiting for a response from player");
    code.mainMenu.response.playerName.setFillColor(sf::Color::Black);
    code.mainMenu.response.playerName.setStyle(sf::Text::Bold);
    code.mainMenu.response.playerName.setCharacterSize(30);
    code.mainMenu.response.playerName.setFont(code.appProps.arialMedium);
    code.mainMenu.response.playerName.setPosition(sf::Vector2f(
            center(code.mainMenu.response.challengeBackground, code.mainMenu.response.playerName).x,
            code.mainMenu.response.challengeBackground.getPosition().y + 100));
    code.mainMenu.response.title.setString(code.appProps.enemyName);
    code.mainMenu.response.title.setFillColor(sf::Color::Black);
    code.mainMenu.response.title.setStyle(sf::Text::Bold);
    code.mainMenu.response.title.setCharacterSize(30);
    code.mainMenu.response.title.setFont(code.appProps.arialMedium);
    code.mainMenu.response.title.setPosition(sf::Vector2f(
            center(code.mainMenu.response.challengeBackground, code.mainMenu.response.title).x,
            code.mainMenu.response.playerName.getPosition().y + 50));
}

/// /////////// END OF DRAW ITEMS ////////////////////////


/// /////////// DISPLAY ////////////////////////

void dislayResponseModal(sf::RenderWindow &window) {
    window.draw(code.mainMenu.response.challengeBackground);
    window.draw(code.mainMenu.response.title);
    window.draw(code.mainMenu.response.playerName);
}

void dislayChallengeModal(sf::RenderWindow &window) {
    window.draw(code.mainMenu.challenge.challengeBackground);
    window.draw(code.mainMenu.challenge.title);
    window.draw(code.mainMenu.challenge.playerName);
    window.draw(code.mainMenu.challenge.acceptButton);
    window.draw(code.mainMenu.challenge.accept);
    window.draw(code.mainMenu.challenge.declineButton);
    window.draw(code.mainMenu.challenge.decline);
}

void displayModal(sf::RenderWindow &window) {
    window.draw(code.appProps.background);
    window.draw(code.appProps.modal);
    window.draw(code.appProps.modalTitle);
}

void displayLoginMenu(sf::RenderWindow &window) {
    window.draw(code.loginMenu.nameText);
    window.draw(code.loginMenu.nameInputBackground);
    window.draw(code.loginMenu.nameInput);
    window.draw(code.loginMenu.passwordText);
    window.draw(code.loginMenu.passwordInputBackground);
    window.draw(code.loginMenu.passwordInput);
    if(!code.loginMenu.isLoginActive) {
        window.draw(code.loginMenu.confirmText);
        window.draw(code.loginMenu.confirmInputBackground);
        window.draw(code.loginMenu.confirmInput);
    }
    window.draw(code.loginMenu.firstButton);
    window.draw(code.loginMenu.firstButtonText);
    window.draw(code.loginMenu.secondButton);
    window.draw(code.loginMenu.secondButtonText);
}

void displayMainMenu(sf::RenderWindow &window) {
    window.draw(code.mainMenu.playersBackground);
    window.draw(code.mainMenu.button);
    window.draw(code.mainMenu.buttonText);
    for(int i = 0; i < MAX_CARDS; i++) {
        window.draw(code.mainMenu.playerCard[i]);
        window.draw(code.mainMenu.playerText[i]);
        window.draw(code.mainMenu.playerScore[i]);
    }
}

void displayError(sf::RenderWindow &window) {
    window.draw(code.appProps.errorBackground);
    window.draw(code.appProps.errorText);
}

void displayBoardMatrix(sf::RenderWindow &window) {
    for(int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            window.draw(code.playingMenu.boardMatrix[i][j].square);
        }
    }
}

void displayPieces(sf::RenderWindow &window) {
    for(int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if(isPlayerSquare(i, j)) {
                window.draw(code.playingMenu.boardMatrix[i][j].piece);
            }
        }
    }
}

void displayMoveOptions(sf::RenderWindow &window) {
    for(int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if(code.playingMenu.boardMatrix[i][j].type == MOVE_OPTION_SQUARE) {
                window.draw(code.playingMenu.boardMatrix[i][j].moveOption);
            }
        }
    }
}

void displayGame(sf::RenderWindow &window) {
    window.draw(code.appProps.background);
    window.draw(code.playingMenu.info.topBackground);
    window.draw(code.playingMenu.info.turnMessage);
    window.draw(code.playingMenu.info.quitButton);
    window.draw(code.playingMenu.info.quitButtonText);
    window.draw(code.playingMenu.info.piecesLeft);
    window.draw(code.playingMenu.info.piecesLeftText);
}

/// /////////// END OF DISPLAY ////////////////////////