#ifndef PROIECT_HELPERFUNCTIONS_H
#define PROIECT_HELPERFUNCTIONS_H

void matrixToChar(char charMatrix[]) {
    bzero(charMatrix, MESSAGE_SIZE);
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            char type[5];
            sprintf(type, "%d ", code.boardMatrix[i][j].type);
            strcat(charMatrix, type);
        }
        charMatrix[strlen(charMatrix)-1] = '\n';
    }
    charMatrix[strlen(charMatrix)] = NULL;
}

#endif //PROIECT_HELPERFUNCTIONS_H
