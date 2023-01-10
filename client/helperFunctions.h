#ifndef PROIECT_HELPERFUNCTIONS_H
#define PROIECT_HELPERFUNCTIONS_H

void intMatrixToChar(char charMatrix[]) {
    bzero(charMatrix, MESSAGE_SIZE);

    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            char type[5];
            sprintf(type, "%d ", code.playingMenu.boardMatrix[i][j].type);
            strcat(charMatrix, type);
        }
        charMatrix[strlen(charMatrix)-1] = '\n';
    }
    charMatrix[strlen(charMatrix)-1] = '\0';
}

void charMatrixToInt(char charMatrix[]) {
    char *p = strtok(charMatrix, " \n");
    int i = 0, j = -1;
    while(p) {
        j++;
        code.playingMenu.boardMatrix[i][j].type = int(p[0]-'0');
        if(j == BOARD_SIZE-1) {
            i++;
            j = -1;
        }
        p = strtok(NULL, " \n");
    }
}

char* convertPassword(char password[]) {
    char* passwordStars = (char*)malloc(strlen(password) + 5);
    int i;
    for(i = 0; i < strlen(password); i++) {
        passwordStars[i] = '*';
    }
    passwordStars[i] = '\0';
    return passwordStars;
}

char* appendChars(char msg1[], char msg2[]) {
    char* resultChar = (char*)malloc((strlen(msg1) + strlen(msg2) + 1) * sizeof(char));
    strcpy(resultChar, msg1);
    strcat(resultChar, msg2);
    return resultChar;
}

char* appendChars(char msg1[], char msg2[], char msg3[]) {
    char* resultChar = (char*)malloc((strlen(msg1) + strlen(msg2) + strlen(msg3) + 1) * sizeof(char));
    strcpy(resultChar, msg1);
    strcat(resultChar, msg2);
    strcat(resultChar, msg3);
    return resultChar;
}

char* appendChars(char msg1[], char msg2[], char msg3[], char msg4[]) {
    char* resultChar = (char*)malloc((strlen(msg1) + strlen(msg2) + strlen(msg3) + strlen(msg4) + 1) * sizeof(char));
    strcpy(resultChar, msg1);
    strcat(resultChar, msg2);
    strcat(resultChar, msg3);
    strcat(resultChar, msg4);
    return resultChar;
}

char* appendChars(char msg1[], char msg2[], char msg3[], char msg4[], char msg5[]) {
    char* resultChar = (char*)malloc((strlen(msg1) + strlen(msg2) + strlen(msg3) + strlen(msg4) + strlen(msg5) + 1) * sizeof(char));
    strcpy(resultChar, msg1);
    strcat(resultChar, msg2);
    strcat(resultChar, msg3);
    strcat(resultChar, msg4);
    strcat(resultChar, msg5);
    return resultChar;
}

char* appendChars(char msg1[], char msg2[], char msg3[], char msg4[], char msg5[], char msg6[]) {
    char* resultChar = (char*)malloc((strlen(msg1) + strlen(msg2) + strlen(msg3) + strlen(msg4) + strlen(msg5)  + strlen(msg6) + 1) * sizeof(char));
    strcpy(resultChar, msg1);
    strcat(resultChar, msg2);
    strcat(resultChar, msg3);
    strcat(resultChar, msg4);
    strcat(resultChar, msg5);
    strcat(resultChar, msg6);
    return resultChar;
}

char* appendChars(char msg1[], char msg2[], char msg3[], char msg4[], char msg5[], char msg6[], char msg7[]) {
    char* resultChar = (char*)malloc((strlen(msg1) + strlen(msg2) + strlen(msg3) + strlen(msg4) + strlen(msg5) + strlen(msg6) + strlen(msg7) + 1) * sizeof(char));
    strcpy(resultChar, msg1);
    strcat(resultChar, msg2);
    strcat(resultChar, msg3);
    strcat(resultChar, msg4);
    strcat(resultChar, msg5);
    strcat(resultChar, msg6);
    strcat(resultChar, msg7);
    return resultChar;
}

bool isResponse(char res[], char command[]) {
    char commandCpy[strlen(command)+2];
    strcpy(commandCpy, command);
    strcat(commandCpy, ":");

    if(strlen(res) < strlen(commandCpy)) {
        return false;
    }
    for(int i = 0; i < strlen(commandCpy); i++) {
        if(commandCpy[i] != res[i]) {
            return false;
        }
    }
    return true;
}

bool isBackendError(char msg[]) {
    char msgCopy[MESSAGE_SIZE];
    strcpy(msgCopy, msg);
    msgCopy[6] = '\0';
    return strcmp(msgCopy,"error.") == 0;
}

bool isBackendSuccess(char msg[]) {
    char msgCopy[MESSAGE_SIZE];
    strcpy(msgCopy, msg);
    msgCopy[7] = '\0';
    return strcmp(msgCopy,"success") == 0;
}

void clearError() {
    strcpy(code.appProps.error, "\0");
}

char* getErrorMessage(char errorMsg[]) {
    return isBackendError(errorMsg) ? errorMsg + (strchr(errorMsg, ' ') - errorMsg) + 1 : nullptr;
}

char* getErrorCode(char errorMsg[]) {
    static char msgCopy[MESSAGE_SIZE];
    strcpy(msgCopy, errorMsg);
    char *p = strtok(msgCopy, " ");
    return isBackendError(errorMsg) ? p + 6 : nullptr;
}

void setError(char errorMsg[]) {
    code.appProps.errorTime = 0;
    printf("Error: %s \n", errorMsg);
    if(isBackendError(errorMsg)) {
        strcpy(code.appProps.error, getErrorMessage(errorMsg));
    } else {
        strcpy(code.appProps.error, errorMsg);
    }
}

void updateErrorTime() {
    code.appProps.errorTime++;
    if(code.appProps.errorTime == 300) {
        clearError();
    }
}

int sendMessage(char sentMsg[]) {
    if (write (code.server.sd, sentMsg, MESSAGE_SIZE) <= 0) {
        perror ("[client]Eroare la sendto() spre server.\n");
        return errno;
    }
    printf("Sent message: %s \n", sentMsg);
}

int sendReadMessage(char sentMsg[], char resMsg[]) {
    sendMessage(appendChars(sentMsg, "."));

    bzero(resMsg, MESSAGE_SIZE);
    if (read (code.server.sd, resMsg, MESSAGE_SIZE) < 0) {
        perror ("[client]Eroare la read() de la server.\n");
        return errno;
    }
    printf("Response: /%s/\n", resMsg);
    if(isBackendError(resMsg)) {
        setError(resMsg);
    }
}

bool isWithinGame(int i, int j) {
    return i >= 0 && i < BOARD_SIZE && j >= 0 && j < BOARD_SIZE;
}

bool isEmptySquare(int i, int j) {
    return code.playingMenu.boardMatrix[i][j].type != PLAYER1_SQUARE &&
           code.playingMenu.boardMatrix[i][j].type != PLAYER2_SQUARE &&
           code.playingMenu.boardMatrix[i][j].type != KING1_SQUARE &&
           code.playingMenu.boardMatrix[i][j].type != KING2_SQUARE;
}

bool isPlayerSquare(int i, int j) {
    return code.playingMenu.boardMatrix[i][j].type == PLAYER1_SQUARE ||
           code.playingMenu.boardMatrix[i][j].type == PLAYER2_SQUARE ||
           code.playingMenu.boardMatrix[i][j].type == KING1_SQUARE ||
           code.playingMenu.boardMatrix[i][j].type == KING2_SQUARE;
}

bool isOption(int i, int j) {
    return code.playingMenu.boardMatrix[i][j].type == MOVE_OPTION_SQUARE;
}

bool isPlayer1Piece(int i, int j) {
    return code.playingMenu.boardMatrix[i][j].type == PLAYER1_SQUARE;
}

bool isPlayer1King(int i, int j) {
    return code.playingMenu.boardMatrix[i][j].type == KING1_SQUARE;
}

bool isPlayer2Piece(int i, int j) {
    return code.playingMenu.boardMatrix[i][j].type == PLAYER2_SQUARE;
}

bool isPlayer2King(int i, int j) {
    return code.playingMenu.boardMatrix[i][j].type == KING2_SQUARE;
}

void clearAllMoveOptions() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if(code.playingMenu.boardMatrix[i][j].type == MOVE_OPTION_SQUARE) {
                code.playingMenu.boardMatrix[i][j].type = EMPTY_SQUARE;
            }
        }
    }
}

char* idToChar(int num) {
    int numCpy = num, numSize = 0;
    do {
        numCpy /= 10;
        numSize++;
    } while(numCpy);

    char* gameId = (char*) malloc(numSize+1);
    sprintf(gameId, "%d", code.playingMenu.gameId);
    return gameId;
}

int countLeftPieces() {
    int piecesLeft = 0;
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            if(isPlayer1Piece(i, j) || isPlayer1King(i, j)) {
                piecesLeft++;
            }
        }
    }
    return piecesLeft;
}

void fetchMatrixInfo(char boardInfo[]) {
    if(isResponse(boardInfo, "turn")) {
        char* resInfo = strtok(boardInfo, "\n") + 6;
//        printf("Turn: /%s/\n", resInfo);
        if(strcmp(resInfo, code.appProps.playerName) == 0) {
            code.playingMenu.turn = PLAYER1;
        } else if(strcmp(resInfo, code.appProps.enemyName) == 0) {
            code.playingMenu.turn = PLAYER2;
        } else {
            setError("Player turn error");
        }

        resInfo = strtok(NULL, "\0");
        if(isResponse(resInfo, "matrix")) {
            resInfo += 8;
//            printf("Matrix info: /%s/\n", resInfo);
            charMatrixToInt(resInfo);
        } else {
            setError("Can't fetch board info");
        }
    } else {
        setError("Unknown command");
    }
}

void gameResultHandler(char resMsg[]) {
    if(isResponse(resMsg, "result")) {
        char msgCpy[MESSAGE_SIZE];
        strcpy(msgCpy, resMsg + 8);
        strcpy(msgCpy, msgCpy);
        char *p = strtok(msgCpy, "\n");
        char result[PLAYER_NAME_SIZE];
        strcpy(result, p);
//        printf("Result: /%s/\n", result);
        if(strcmp(result, code.appProps.playerName) == 0) {
            printf("[client]Player 1 won\n");
            code.playingMenu.winner = PLAYER1;
            setError(appendChars("Player ", code.appProps.playerName, " won!"));
        } else if(strcmp(result, code.appProps.enemyName) == 0) {
            printf("[client]Player 2 won\n");
            code.playingMenu.winner = PLAYER2;
            setError(appendChars("Player ", code.appProps.enemyName, " won!"));
        } else if(strcmp(result, "draw") == 0) {
            printf("[client]It's a draw\n");
            code.playingMenu.winner = DRAW;
            setError("It's a draw!");
        } else if(strcmp(result, "forfeit") == 0) {
            printf("[client]Forfeit\n");
            code.playingMenu.winner = PLAYER1;
            setError(appendChars("Player ", code.appProps.enemyName, " quitted!"));
        }

        p = strtok(NULL, "\0");
        if(isResponse(p, "matrix")) {
            char charMatrix[MESSAGE_SIZE];
            strcpy(charMatrix, p + 8);
            charMatrixToInt(charMatrix);
        } else {
            setError("Can't fetch board info");
        }
    } else {
        setError("Unknown command");
    }
}

void initGame(char command[]) {
    code.playingMenu.winner = -1;
    int gameId = -1;
    if(isResponse(command, "gameId")) {
        gameId = atoi(command + 8);
    } else if(isResponse(command, "accepted")) {
        gameId = atoi(command + 10);
    } else {
        setError("No game id provided");
    }
    code.playingMenu.gameId = gameId;

    char res[MESSAGE_SIZE];
    sendReadMessage(appendChars("initGame: ", code.appProps.playerName, " ", idToChar(code.playingMenu.gameId)), res);
    if(isResponse(res, "turn")) {
        char* resInfo = strtok(res, "\n") + 6;
//        printf("Turn: /%s/\n", resInfo);
        if(strcmp(resInfo, code.appProps.playerName) == 0) {
            code.playingMenu.playerColor = PLAYER1;
            code.playingMenu.turn = PLAYER1;
        } else if(strcmp(resInfo, code.appProps.enemyName) == 0) {
            code.playingMenu.playerColor = PLAYER2;
            code.playingMenu.turn = PLAYER2;
        } else {
            setError("Player turn error");
        }

        resInfo = strtok(NULL, "\0");
        if(isResponse(resInfo, "matrix")) {
            resInfo += 8;
//            printf("Matrix info: /%s/\n", resInfo);
            charMatrixToInt(resInfo);
        } else {
            setError("Can't fetch board info");
        }
    } else {
        setError("Unknown command");
    }
    code.appProps.appMenu = PLAYING;
}

sf::Vector2f center(sf::RectangleShape rect, sf::Text text) {
    return sf::Vector2f(rect.getPosition().x + rect.getSize().x/2 - text.getGlobalBounds().width/2, rect.getPosition().y + rect.getSize().y/2 - text.getGlobalBounds().height/2 - 3);
}

sf::Vector2f center(sf::RectangleShape outside, sf::RectangleShape inside) {
    return sf::Vector2f(outside.getPosition().x + outside.getSize().x/2 - inside.getSize().x/2, outside.getPosition().y + outside.getSize().y/2 - inside.getSize().y/2);
}

#endif //PROIECT_HELPERFUNCTIONS_H
