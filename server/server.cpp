#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <signal.h>
#include <pthread.h>


#define MESSAGE_SIZE 1000
#define PLAYER_NAME_SIZE 32
#define BOARD_SIZE 8
#define MAX_PLAYERS 100
#define MAX_GAMES 50
#define ERROR_MESSAGE_SIZE 100
#define FAIL_MESSAGE "Failled to process command"

//defines for board
#define EMPTY_SQUARE 0
#define PLAYER1_SQUARE 1
#define KING1_SQUARE 2
#define PLAYER2_SQUARE 3
#define KING2_SQUARE 4

/* portul folosit */
#define PORT 2927

/* codul de eroare returnat de anumite apeluri */
extern int errno;

typedef struct thData{
    int idThread; //id-ul thread-ului tinut in evidenta de acest program
    int cl; //descriptorul intors de accept

}thData;

struct player{
    int id;
    char name[PLAYER_NAME_SIZE]="";
    char password[PLAYER_NAME_SIZE]="";
    bool isOnline = false;
    bool isChallenged = false;
    char isChallengedBy[PLAYER_NAME_SIZE]="";
    bool hasChallenged = false;
    char challenged[PLAYER_NAME_SIZE]="";
    int challengeResponse = 0;
    bool isPlaying = false;
    int score = 0;
}player[MAX_PLAYERS];
int playersCount=0; //contorul jucatorilor inregistrati

struct game{
    int id=-1;
    char player1[PLAYER_NAME_SIZE]="";
    char player2[PLAYER_NAME_SIZE]="";
    int turn = 1;
    int matrix[BOARD_SIZE][BOARD_SIZE];
    int status = -1; //-1 = joc neinceput, 0 = joc in desfasurare, 1 = player1 won, 2 = player2 won, 3 = draw
    char result[PLAYER_NAME_SIZE]="";
}game[MAX_GAMES];
int gamesCount = 0; //contorul jocurilor in desfasurare

int convertCharToInt(char id[])
{
    int response = 0;
    char idCopy[10];
    strcpy(idCopy, id);
    for(int j = 0; j < strlen(idCopy); j++)
    {
        response = int(idCopy[j]-'0')+ response * 10;
    }
    return response;
}

void readFromFile(){
    FILE* file;
    int lineLenght = 100;
    char line[lineLenght];

    printf("[server]Incep incarcarea jucatorilor din fisier... \n");

    file = fopen("players.txt", "r");

    while(fgets(line, lineLenght, file)){
        char lineCopy[lineLenght];
        strcpy(lineCopy, line);

        playersCount++;
        char * p = strtok(lineCopy, " ");
        char charId[10];
        strcpy(charId, p);
        player[playersCount].id = convertCharToInt(charId);
        p = strtok(NULL, " ");
        strcpy(player[playersCount].name, p);
        p = strtok(NULL, " ");
        strcpy(player[playersCount].password, p);
        p = strtok(NULL, ".");
        char charScore[10];
        strcpy(charScore, p);
        player[playersCount].score = convertCharToInt(charScore);
    }
    fclose(file);

    printf("[server]Jucatorii din fisier au fost incarcati cu succes \n");
}

void updateFile(){
    FILE* file;

    file = fopen("players.txt", "w");

    for(int i=1; i <= playersCount; i++){
        char charId[10];
        sprintf(charId, "%d", player[i].id);
        fputs(charId, file);
        fputs(" ", file);
        fputs(player[i].name, file);
        fputs(" ", file);
        fputs(player[i].password, file);
        fputs(" ", file);
        char charScore[10];
        sprintf(charScore, "%d", player[i].score);
        fputs(charScore, file);
        fputs(".", file);
        fputs("\n", file);
    }
    fclose(file);
}



bool isRegister(char msg[])
{
    char msgCopy[MESSAGE_SIZE];
    strcpy(msgCopy, msg);
    msgCopy[9]='\0';
    if(strcmp(msgCopy, "register:")==0)
    {
        printf("[server]Comanda primita este register: \n");
        return true;
    }
    return false;
}

bool isLogin(char msg[])
{
    char msgCopy[MESSAGE_SIZE];
    strcpy(msgCopy, msg);
    msgCopy[6]='\0';
    if(strcmp(msgCopy, "login:")==0)
    {
        printf("[server]Comanda primita este 'login:' \n");
        return true;
    }
    return false;
}

bool isLogout(char msg[])
{
    char msgCopy[MESSAGE_SIZE];
    strcpy(msgCopy, msg);
    msgCopy[7]='\0';
    if(strcmp(msgCopy, "logout:") == 0)
    {
        printf("[server]Comanda primita este 'logout:' \n");
        return true;
    }
    return false;
}

bool isOnlinePlayersList(char msg[])
{
    char msgCopy[MESSAGE_SIZE];
    strcpy(msgCopy, msg);
    msgCopy[17]='\0';
    if(strcmp(msgCopy,"onlinePlayersList") == 0)
    {
        printf("[server]Comanda primita este 'onlinePlayersList' \n");
        return true;
    }
    return false;
}

bool isGetLobbyStatus(char msg[])
{
    char msgCopy[MESSAGE_SIZE];
    strcpy(msgCopy, msg);
    msgCopy[15]='\0';
    if(strcmp(msgCopy, "getLobbyStatus:") == 0)
    {
        printf("[server]Comanda primita este 'getLobbyStatus:' \n");
        return true;
    }
    return false;
}

bool isGetLeaderboard(char msg[]){
    char msgCopy[MESSAGE_SIZE];
    strcpy(msgCopy, msg);
    msgCopy[15]='\0';
    if(strcmp(msgCopy, "getLeaderboard:") == 0)
    {
        printf("[server]Comanda primita este 'getLeaderboard:' \n");
        return true;
    }
    return false;
}

bool isChallenge(char msg[])
{
    char msgCopy[MESSAGE_SIZE];
    strcpy(msgCopy, msg);
    msgCopy[10]='\0';
    if(strcmp(msgCopy, "challenge:") == 0)
    {
        printf("[server]Comanda primita este 'challenge:' \n");
        return true;
    }
    return false;
}

bool isAccept(char msg[])
{
    char msgCopy[MESSAGE_SIZE];
    strcpy(msgCopy, msg);
    msgCopy[7]='\0';
    if(strcmp(msgCopy, "accept:") == 0)
    {
        printf("[server]Comanda primita este 'accept:' \n");
        return true;
    }
    return false;
}

bool isDecline(char msg[])
{
    char msgCopy[MESSAGE_SIZE];
    strcpy(msgCopy, msg);
    msgCopy[8]='\0';
    if(strcmp(msgCopy, "decline:") == 0)
    {
        printf("[server]Comanda primita este 'decline:' \n");
        return true;
    }
    return false;
}

bool isInitGame(char msg[])
{
    char msgCopy[MESSAGE_SIZE];
    strcpy(msgCopy, msg);
    msgCopy[9]='\0';

    if(strcmp(msgCopy, "initGame:") == 0)
    {
        printf("[server]Comanda primita este 'initGame:' \n");
        return true;
    }
    return false;
}

bool isSetMatrix(char msg[])
{
    char msgCopy[MESSAGE_SIZE];
    strcpy(msgCopy, msg);
    msgCopy[10]='\0';

    if(strcmp(msgCopy, "setMatrix:") == 0)
    {
        printf("[server]Comanda primita este 'setMatrix:' \n");
        return true;
    }
    return false;

}

bool isGetMatrixUpdate(char msg[])
{
    char msgCopy[MESSAGE_SIZE];
    strcpy(msgCopy, msg);
    msgCopy[16]='\0';

    if(strcmp(msgCopy, "getMatrixUpdate:") == 0)
    {
        printf("[server]Comanda primita este 'getMatrixUpdate:' \n");
        return true;
    }
    return false;
}

bool isForfeit (char msg[]){
    char msgCopy[MESSAGE_SIZE];
    strcpy(msgCopy, msg);
    msgCopy[8]='\0';

    if(strcmp(msgCopy, "forfeit:") == 0)
    {
        printf("[server]Comanda primita este 'forfeit:' \n");
        return true;
    }
    return false;
}

bool isReturnToLobby (char msg[]){
    char msgCopy[MESSAGE_SIZE];
    strcpy(msgCopy, msg);
    msgCopy[14]='\0';

    if(strcmp(msgCopy, "returnToLobby:") == 0)
    {
        printf("[server]Comanda primita este 'returnToLobby:' \n");
        return true;
    }
    return false;
}

bool isExit(char msg[])
{
    char msgCopy[MESSAGE_SIZE];
    strcpy(msgCopy, msg);
    msgCopy[4]='\0';
    if(strcmp(msgCopy, "exit")==0)
    {
        return true;
    }
    return false;
}

void swap(int &x, int &y)
{
    int temp = x;
    x = y;
    y = temp;
}

void clearCommandFromEndLine(char msg[], char command[])
{
    strcpy(command, msg);
    for(int j=strlen(command)-1; j>=0; j--)
    {
        if(command[j] == '.')
        {
            command[j]='\0';
            break;
        }
    }
}

void clearPlayerStats(char playerName[])
{
    for(int j=1; j<=playersCount; j++)
    {
        if(strcmp(player[j].name, playerName) == 0)
        {
            player[j].isOnline = false;
            player[j].isChallenged = false;
            strcpy(player[j].isChallengedBy, " ");
            player[j].hasChallenged = false;
            strcpy(player[j].challenged, " ");
            player[j].challengeResponse = 0;
            player[j].isPlaying = false;
        }
    }
}

void registerPlayer(char msg[], char response[])
{
    char playerName[MESSAGE_SIZE];
    strcpy(playerName, msg + 10);
    char *p;
    p = strtok(playerName, " ");
    /* verific daca numele este deja inregistrat */
    for(int j=1; j<=playersCount; j++)
        if(strcmp(player[j].name, "")!=0)
            if(strcmp(p, player[j].name) == 0)
            {
                strcpy(response, "error.nameAlreadyRegistered The name is already registered");
                return;
            }


    playersCount++; //incrementez nr jucatorilor inregistrati

    player[playersCount].id=playersCount; //asignez jucatorului id-ul unic

    strcpy(player[playersCount].name, p);
    printf("[server]numele este: '%s'\n", player[playersCount].name);

    p = strtok(NULL, " ");
    strcpy(player[playersCount].password, p);
    printf("[server]Parola lui '%s' este: '%s'\n", player[playersCount].name, player[playersCount].password);

    player[playersCount].isOnline = true; //autologin
    updateFile();
    strcpy(response, "success register"); //inregistrat cu succes
}

void loginPlayer(char msg[], char response[])
{
    char msgCopy[MESSAGE_SIZE];
    strcpy(msgCopy, msg + 7);
    char *p;

    p = strtok(msgCopy, " "); /*p = numele jucatorului*/
    printf("[server]Numele este '%s' \n", p);

    /*verific daca numele este inregistrat*/
    bool nameCheck = false;
    int poz =-1;
    for(int j=1; j<=playersCount; j++)
    {
        if(strcmp(p, player[j].name) == 0)
        {
            /*daca este inregistrat verifica daca este online deja*/
            if(player[j].isOnline == true)
            {
                strcpy(response, "error.alreadyLoggedIn Already logged in");
                return;
            }
            else
            {
                nameCheck = true; //inregistrat, offline
                poz = j;
            }
        }
    }//for

    if (nameCheck == false)
    {   //jucator neinregistrat
        strcpy(response, "error.playerUnregistered Player is unregistered");
        return;
    }

    /*verific daca parola este corecta*/
    p = strtok(NULL, " ");
    if(strcmp(p, player[poz].password) != 0)
    {   //parola incorecta;
        strcpy(response, "error.wrongPassword Wrong Password");
        return;
    }

    player[poz].isOnline = true;
    //logare cu succes.
    strcpy(response, "success login");
    return;
}

void logoutPlayer(char msg[], char response[])
{
    char msgCopy[MESSAGE_SIZE];
    strcpy(msgCopy, msg + 8);
    if(int(msgCopy[strlen(msgCopy)]) == 10) //endline
        msgCopy[strlen(msgCopy)-1]='\0';

    for(int j=1; j<=playersCount; j++)
    {
        printf("[server]Verific jucatorul %s \n", player[j].name);
        if(strcmp(player[j].name, msgCopy) == 0)
        {
            clearPlayerStats(msgCopy);
            printf("[server]Player %s was logged out successfully!\n", player[j].name);
            strcpy(response, "exit");
            return;
        }
    }
}

void challengePlayer(char msg[], char response[])
{
    char msgCopy[MESSAGE_SIZE];
    char player1[MESSAGE_SIZE], player2[MESSAGE_SIZE];
    char * p;
    strcpy(msgCopy, msg + 11);
    p = strtok(msgCopy, " ");
    strcpy(player1, p); //cel care provoaca
    p = strtok(NULL, " ");
    strcpy(player2, p);// cel care este provocat

    for(int j = 1; j <= playersCount; j++)
    {
        if(strcmp(player[j].name, player2) == 0)
        {
            if(player[j].hasChallenged == false) //daca nu a provocat pe alt jucator
            {
                if(player[j].isChallenged == true) //daca a fost provocat de alt jucator
                {
                    strcpy(response, "error.alreadyChallenged: Player is already challenged by another player.");
                    return;
                }
                else
                {
                    player[j].isChallenged = true; //jucatorul este provocat
                    strcpy(player[j].isChallengedBy, player1);

                }
            }
            else
                strcpy(response, "error.playerHasChallenged: Player has challenged another player.");
        }
    }
    for(int j = 1; j <= playersCount; j++)
    {
        if(strcmp(player[j].name, player1) == 0)
        {
            player[j].hasChallenged = true;
            strcpy(player[j].challenged, player2);
        }
    }
    strcpy(response, "success challenge");
}

void convertIntToChar(int id, char response[])
{
    sprintf(response, "%d", id);
}

void initMatrix(int id) {
    /// NORMAL GAME
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            if(i <= 2) {
                if((i+j) % 2 != 0) {
                    game[id].matrix[i][j] = PLAYER2_SQUARE;
                }
            } else if (i >= 5) {
                if ((i+j) % 2 != 0) {
                    game[id].matrix[i][j] = PLAYER1_SQUARE;
                }
            }
        }
    }
    ///

    ///WIN no move for player 2 (1)
//    game[id].matrix[0][1] = PLAYER2_SQUARE;
//    game[id].matrix[0][3] = PLAYER2_SQUARE;
//    game[id].matrix[1][0] = PLAYER2_SQUARE;
//    game[id].matrix[2][1] = PLAYER1_SQUARE;
    ///

    ///DRAW no move left for both players (2)
//    game[id].matrix[0][1] = PLAYER2_SQUARE;
//    game[id].matrix[0][3] = PLAYER2_SQUARE;
//    game[id].matrix[0][5] = PLAYER2_SQUARE;
//    game[id].matrix[0][7] = PLAYER2_SQUARE;
//    game[id].matrix[1][0] = PLAYER2_SQUARE;
//    game[id].matrix[1][2] = PLAYER2_SQUARE;
//    game[id].matrix[1][4] = PLAYER2_SQUARE;
//    game[id].matrix[1][6] = PLAYER2_SQUARE;
//    game[id].matrix[2][1] = PLAYER1_SQUARE;
//    game[id].matrix[2][3] = PLAYER1_SQUARE;
//    game[id].matrix[2][5] = PLAYER1_SQUARE;
//    game[id].matrix[2][7] = PLAYER1_SQUARE;
//    game[id].matrix[3][0] = PLAYER1_SQUARE;
//    game[id].matrix[3][2] = PLAYER1_SQUARE;
//    game[id].matrix[3][6] = PLAYER1_SQUARE;
//    game[id].matrix[4][3] = PLAYER1_SQUARE;
    ///

    ///DRAW 2 pieces left (3)
//    game[id].matrix[3][2] = PLAYER1_SQUARE;
//    game[id].matrix[4][3] = PLAYER1_SQUARE;
//    game[id].matrix[2][3] = PLAYER2_SQUARE;
//    game[id].matrix[3][4] = PLAYER2_SQUARE;
    ///

    ///WIN eat all pieces (4)
//    for(int i = 0; i < BOARD_SIZE; i++) {
//        for(int j = 0; j < BOARD_SIZE; j++) {
//            if (i >= 5) {
//                if ((i+j) % 2 != 0) {
//                    game[id].matrix[i][j] = PLAYER1_SQUARE;
//                }
//            }
//        }
//    }
//    game[id].matrix[4][1] = PLAYER2_SQUARE;
//    game[id].matrix[2][3] = PLAYER2_SQUARE;
    ///

    ///WIN no move for player1 (5)
//    game[id].matrix[7][0] = PLAYER1_SQUARE;
//    game[id].matrix[7][2] = PLAYER1_SQUARE;
//    game[id].matrix[7][4] = PLAYER1_SQUARE;
//    game[id].matrix[4][3] = PLAYER2_SQUARE;
    ///
}

void clearGame(int id){
    strcpy(game[id].player1, " ");
    strcpy(game[id].player2, " ");
    game[id].turn = 1;
    for(int i = 0; i < BOARD_SIZE; i++)
        for(int j = 0; j < BOARD_SIZE; j++)
            game[id].matrix[i][j] = 0;
    strcpy(game[id].result, " ");
    //game[id].status = -1;
}

void createGame( char player1[], char player2[], char response[])
{
    printf("[server]Creez o nou instanta de joc \n");
    for(int j=1; j<=gamesCount; j++)
    {
        if (game[j].status == -1)
        {
            printf("[server]Am gasit un joc inactiv cu ID: %d. \n", game[j].id);
            clearGame(game[j].id);
            strcpy(game[j].player1, player1);
            strcpy(game[j].player2, player2);
            game[j].status = 0;
            char gameId[MESSAGE_SIZE];
            convertIntToChar(game[j].id, gameId);
            strcpy(response, "gameId: ");
            strcat(response, gameId);
            printf("[server]Joc creat cu succes! ID: %s. \n", gameId);
            return;
        }
    }

    printf("[server]Nu am gasit joc inactiv \n");
    gamesCount++;
    game[gamesCount].id = gamesCount;
    printf("[server]Creez un joc nou cu ID: %d. \n", game[gamesCount].id);
    strcpy(game[gamesCount].player1, player1);
    strcpy(game[gamesCount].player2, player2);
    game[gamesCount].status = 0;
    char gameId[MESSAGE_SIZE];
    convertIntToChar(game[gamesCount].id, gameId);
    strcpy(response, "gameId: ");
    strcat(response, gameId);
    printf("[server]Joc creat cu succes! ID: %s. \n", gameId);
}

void acceptChallenge(char msg[], char response[])
{
    char challengedPlayer[MESSAGE_SIZE];
    char playerWhoChallenged[MESSAGE_SIZE];
    char gameId[MESSAGE_SIZE];
    strcpy(challengedPlayer, msg + 8);

    for(int j = 1; j <= playersCount; j++)
    {
        if(strcmp(player[j].name, challengedPlayer) == 0)
        {
            strcpy(playerWhoChallenged, player[j].isChallengedBy);
            player[j].isPlaying = true;
            break;
        }
    }
    for(int j = 1; j <= playersCount; j++)
    {
        if(strcmp(player[j].name, playerWhoChallenged) == 0)
        {
            player[j].challengeResponse = 1;
        }
    }
    createGame(challengedPlayer, playerWhoChallenged, gameId);
    strcpy(response, gameId);
}

void declineChallenge(char msg[], char response[])
{
    char challengedPlayer[MESSAGE_SIZE];
    char playerWhoChallenged[MESSAGE_SIZE];
    strcpy(challengedPlayer, msg + 9);

    for(int j = 1; j <= playersCount; j++)
    {
        if(strcmp(player[j].name, challengedPlayer) == 0)
        {
            strcpy(playerWhoChallenged, player[j].isChallengedBy);

            player[j].isChallenged = false;
            strcpy(player[j].isChallengedBy, " ");

            break;
        }
    }
    for(int j = 1; j <= playersCount; j++)
    {
        if(strcmp(player[j].name, playerWhoChallenged) == 0)
        {
            player[j].challengeResponse = -1;
        }
    }
    strcpy(response, "succes declined");
}

void createOnlinePlayersList(char msg[], char response[])
{
    char list[MESSAGE_SIZE]="";
    strcpy(list, "playerList: ");

    printf("[server]Creating online players list...\n");
    for(int j=1; j<=playersCount; j++)
        if(player[j].isOnline == true && player[j].isPlaying == false) //Daca este online si nu joaca
            if(strcmp(player[j].name, msg) != 0)//jucatorul care a cerut lista sa nu apara in ea
            {
                strcat(list, player[j].name);
                strcat(list, " ");
            }
    printf("[server]Online players list created successfully \n");
    strcpy(response, list);
}


void matrixToChar(char charMatrix[], int id)
{
    bzero(charMatrix, MESSAGE_SIZE);

    for(int k = 0; k < BOARD_SIZE; k++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            char type[5];
            sprintf(type, "%d ", game[id].matrix[k][j]);
            strcat(charMatrix, type);
        }
        charMatrix[strlen(charMatrix)-1] = '\n';
    }
    charMatrix[strlen(charMatrix)-1] = '\0';
}

void matrixToInt(char matrix[], int id)
{
    char *p = strtok(matrix, " \n");
    int i = 0, j = -1;
    while(p) {
        j++;
        game[id].matrix[i][j] = int(p[0]-'0');
        if(j == BOARD_SIZE -1)
        {
            i++;
            j = -1;
        }
        p = strtok(NULL, " \n");
    }
}

void getLobbyStatus(char msg[], char response[])
{
    char msgCopy[MESSAGE_SIZE];
    strcpy(msgCopy, msg + 16);
    // printf("[server]Un jucator solicita update la lobby \n");

    // for(int j=0; j<strlen(msgCopy); j++)
    //     printf("%d ", msgCopy[j]);
    // printf("\n");

    for(int j=1; j<=playersCount; j++)
    {
        printf("[server]Verfic jucatorul: %s \n", player[j].name);
        if(strcmp(player[j].name, msgCopy) == 0)
        {
            printf("[server]Jucatorul %s solicita update la lobby \n", msgCopy);

            if(player[j].hasChallenged == true)
            {
                printf("[server]Jucatorul %s a provocat pe cineva si asteapta un raspuns. \n", msgCopy);
                if(player[j].challengeResponse != 0)
                {
                    if(player[j].challengeResponse == -1)
                    {
                        printf("[server]Jucatorului %s i-a fost refuzata provocarea. \n", msgCopy);
                        strcpy(response, "declined:");
                        player[j].hasChallenged = false;
                        strcpy(player[j].challenged, " ");
                        player[j].challengeResponse = 0;
                    }
                    else if(player[j].challengeResponse == 1)
                    {
                        printf("[server]Jucatorului %s i-a fost acceptata provocarea. \n", msgCopy);
                        for(int k=1; k<=gamesCount; k++)
                        {
                            if(strcmp(game[k].player2, player[j].name) == 0)
                            {
                                char gameId[MESSAGE_SIZE];
                                convertIntToChar(game[k].id, gameId);
                                strcpy(response, "accepted: ");
                                strcat(response, gameId);
                                player[j].isPlaying = true;
                            }
                        }
                        player[j].challengeResponse = 0;
                    }
                }
                else
                {
                    printf("[server]Jucatorul %s nu a primit un raspuns pentru provocare. \n", msgCopy);
                    char playerList[MESSAGE_SIZE];
                    bzero(playerList, MESSAGE_SIZE);
                    createOnlinePlayersList(msgCopy, playerList);
                    strcpy(response, playerList);
                }

            }
            else
            {
                printf("[server]Jucatorul %s NU a provocat pe nimeni la duel. \n", msgCopy);
                if(player[j].isChallenged == false)
                {
                    printf("[server]Jucatorul %s NU este provocat la duel \n", msgCopy);
                    char playerList[MESSAGE_SIZE];
                    createOnlinePlayersList(msgCopy, playerList);
                    strcpy(response, playerList);
                }
                else
                {
                    printf("[server]Jucatorul %s ESTE provocat la duel \n", msgCopy);
                    strcpy(response, "challenge: ");
                    strcat(response, player[j].isChallengedBy);
                }
            }
        }

    }//for
}

void sentLeaderboard(char msg[], char response[]){
    char playerName[MESSAGE_SIZE];
    strcpy(playerName, msg + 16);
    strcpy(response, "leaderboard: ");
    struct {
        char playerName[PLAYER_NAME_SIZE];
        int playerScore;
    }leaderboard[MAX_PLAYERS];

    for(int i = 1; i <= playersCount; i++){
        strcpy(leaderboard[i].playerName, player[i].name);
        leaderboard[i].playerScore  = player[i].score;
    }

    char tempName[PLAYER_NAME_SIZE];
    int tempScore;

    for(int i = 1; i < playersCount; i++){
        for(int j = i+1; j <= playersCount; j++){
            if(leaderboard[i].playerScore < leaderboard[j].playerScore){
                strcpy(tempName, leaderboard[i].playerName);
                strcpy(leaderboard[i].playerName, leaderboard[j].playerName);
                strcpy(leaderboard[j].playerName, tempName);

                tempScore = leaderboard[i].playerScore;
                leaderboard[i].playerScore = leaderboard[j].playerScore;
                leaderboard[j].playerScore = tempScore;
            }
        }
    }

    char charScore[10];
    for(int i = 1; i<=playersCount; i++){
        strcat(response, leaderboard[i].playerName);
        strcat(response, " ");
        convertIntToChar(leaderboard[i].playerScore, charScore);
        strcat(response, charScore);
        strcat(response, " ");
    }

    response[strlen(response) - 1] = '\0';

}

void reverseMatrix(int id) {
    for(int i = 0; i < BOARD_SIZE/2; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            swap(game[id].matrix[i][j], game[id].matrix[BOARD_SIZE - i - 1][j]);
        }
    }

    for(int j = 0; j < BOARD_SIZE/2; j++) {
        for(int i = 0; i < BOARD_SIZE; i++) {
            swap(game[id].matrix[i][j], game[id].matrix[i][BOARD_SIZE - j - 1]);
        }
    }

    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            if(game[id].matrix[i][j] == PLAYER1_SQUARE) {
                game[id].matrix[i][j] = PLAYER2_SQUARE;
            } else if(game[id].matrix[i][j] == PLAYER2_SQUARE) {
                game[id].matrix[i][j] = PLAYER1_SQUARE;
            } else if(game[id].matrix[i][j] == KING1_SQUARE) {
                game[id].matrix[i][j] = KING2_SQUARE;
            } else if(game[id].matrix[i][j] == KING2_SQUARE) {
                game[id].matrix[i][j] = KING1_SQUARE;
            }
        }
    }
}

void initGame(char msg[], char response[]) {
    bzero(response, MESSAGE_SIZE);
    char msgCopy[MESSAGE_SIZE];
    strcpy(msgCopy, msg + 10);
    char *p = strtok(msgCopy, " ");
    char name[PLAYER_NAME_SIZE];
    strcpy(name, p);
    p = strtok(NULL, " ");
    char idChar[10];
    strcpy(idChar, p);
    int id = convertCharToInt(idChar);
    initMatrix(id);

    strcpy(response, "turn: ");
    strcat(response, game[id].player1);
    strcat(response, "\n");
    strcat(response, "matrix: ");

    if(strcmp(name, game[id].player1) == 0) {
        char charMatrix[MESSAGE_SIZE];
        matrixToChar(charMatrix, id);
        strcat(response, charMatrix);
    } else if(strcmp(name, game[id].player2) == 0) {
        reverseMatrix(id);
        char charMatrix[MESSAGE_SIZE];
        matrixToChar(charMatrix, id);
        strcat(response, charMatrix);
        reverseMatrix(id);
    }
}


bool isWithinBorderLimits(int i, int j){
    if(i >= 0 && i < BOARD_SIZE && j >= 0 && j < BOARD_SIZE){
        return true;
    }
    return false;
}

bool canPlayerMove(int id, char name[]){
    for(int i = 0; i < BOARD_SIZE; i++){
        for(int j = 0; j < BOARD_SIZE; j++){
            if(strcmp(game[id].player1, name) == 0){ //player1 - verde
                if(game[id].matrix[i][j] == PLAYER1_SQUARE || game[id].matrix[i][j] == KING1_SQUARE){ //daca sunt pe o piesa a player1
                    if(isWithinBorderLimits(i-1, j-1)){
                        if(game[id].matrix[i-1][j-1] == EMPTY_SQUARE){
                            return true; // poate muta in stanga-sus
                        }
                        else if(game[id].matrix[i-1][j-1] == PLAYER2_SQUARE || game[id].matrix[i-1][j-1] == KING2_SQUARE){
                            if(isWithinBorderLimits(i-2, j-2)){
                                if(game[id].matrix[i-2][j-2] == EMPTY_SQUARE){
                                    return true;//poate muta in stanga-sus si mananca o piesa a player2
                                }
                            }
                        }
                    }
                    if(isWithinBorderLimits(i-1, j+1)){
                        if(game[id].matrix[i-1][j+1] == EMPTY_SQUARE){
                            return true; //poate muta dreapta-sus
                        }
                        else if(game[id].matrix[i-1][j+1] == PLAYER2_SQUARE || game[id].matrix[i-1][j+1] == KING2_SQUARE){
                            if(isWithinBorderLimits(i-2, j+2)){
                                if(game[id].matrix[i-2][j+2] == EMPTY_SQUARE){
                                    return true; //poate muta in dreapta-sus si mananca o piesa a player2
                                }
                            }
                        }
                    }
                    if(game[id].matrix[i][j] == KING1_SQUARE){ //daca e rege al player 1 verific si pentru pozitiile de jos
                        if(isWithinBorderLimits(i+1, j-1)){
                            if(game[id].matrix[i+1][j-1] == EMPTY_SQUARE){
                                return true;// poate muta stanga-jos
                            }
                            else if(game[id].matrix[i+1][j-1] == PLAYER2_SQUARE || game[id].matrix[i+1][j-1] == KING2_SQUARE){
                                if(isWithinBorderLimits(i+2, j-2)){
                                    if(game[id].matrix[i+2][j-2] == EMPTY_SQUARE){
                                        return true;//poate muta in stanga-jos si mananca o piesa a player2
                                    }
                                }
                            }
                        }
                        if(isWithinBorderLimits(i+1, j+1)){
                            if(game[id].matrix[i+1][j+1] == EMPTY_SQUARE){
                                return true; //poate muta dreapta-jos
                            }
                            else if(game[id].matrix[i+1][j+1] == PLAYER2_SQUARE || game[id].matrix[i+1][j+1] == KING2_SQUARE){
                                if(isWithinBorderLimits(i+2, j+2)){
                                    if(game[id].matrix[i+2][j+2] == EMPTY_SQUARE){
                                        return true;//poate muta in dreapta-jos si mananca o piesa a player2
                                    }
                                }
                            }
                        }
                    }
                }
            } else if(strcmp(game[id].player2, name) == 0){ //player2 - rosii
                if(game[id].matrix[i][j] == PLAYER2_SQUARE || game[id].matrix[i][j] == KING2_SQUARE){
                    if(isWithinBorderLimits(i+1, j-1)){
                        if(game[id].matrix[i+1][j-1] == EMPTY_SQUARE){
                            return true; //poate muta stanga-jos
                        }
                        else if(game[id].matrix[i+1][j-1] == PLAYER1_SQUARE || game[id].matrix[i+1][j-1] == KING1_SQUARE){
                            if(isWithinBorderLimits(i+2, j-2)){
                                if(game[id].matrix[i+2][j-2] == EMPTY_SQUARE){
                                    return true; //poate muta stanga-jos si manca o piesa a player1
                                }
                            }
                        }
                    }
                    if(isWithinBorderLimits(i+1, j+1)){
                        if(game[id].matrix[i+1][j+1] == EMPTY_SQUARE){
                            return true; //poate muta dreapta-jos
                        }else if(game[id].matrix[i+1][j+1] == PLAYER1_SQUARE || game[id].matrix[i+1][j+1] == KING1_SQUARE){
                            if(isWithinBorderLimits(i+2, j+2)){
                                if(game[id].matrix[i+2][j+2] == EMPTY_SQUARE){
                                    return true; //poate muta dreapta-jos si manca o piesa a player1
                                }
                            }
                        }
                    }
                    if(game[id].matrix[i][j] == KING2_SQUARE){ //daca e rege al player2 verific daca poate muta in sus
                        if(isWithinBorderLimits(i-1, j-1)){
                            if(game[id].matrix[i-1][j-1] == EMPTY_SQUARE){
                                return true; //poate muta stanga-sus
                            } else if(game[id].matrix[i-1][j-1] ==  PLAYER1_SQUARE || game[id].matrix[i-1][j-1] == KING1_SQUARE){
                                if(isWithinBorderLimits(i-2, j-2)){
                                    if(game[id].matrix[i-2][j-2] == EMPTY_SQUARE){
                                        return true; //poate muta stanga-sus si manca o piesa a player1
                                    }
                                }
                            }
                        }
                        if(isWithinBorderLimits(i-1, j+1)){
                            if(game[id].matrix[i-1][j+1] == EMPTY_SQUARE){
                                return true; //poate muta dreapta-sus
                            } else if(game[id].matrix[i-1][j+1] == PLAYER1_SQUARE || game[id].matrix[i-1][j+1] == KING1_SQUARE){
                                if(isWithinBorderLimits(i-2, j+2)){
                                    if(game[id].matrix[i-2][j+2] == EMPTY_SQUARE){
                                        return true; //poate muta dreapta-sus si manca o piesa a player1
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

int evaluateMove(int id)
{
    printf("[server]Incep evaluarea ultimei mutari din jocul: %d. \n", id);
    //verific daca s-a deconectat cineva
    for(int i = 1; i <= playersCount; i++)
    {
        if(strcmp(player[i].name, game[id].player1) == 0){
            if(player[i].isPlaying == false){
                printf("[server]Jucatorul '%s' s-a deconectat de la jocul %d. \n", game[id].player1, id);
                printf("[server]Jocul %d s-a termint prin: forfeit. \n", id);
                game[id].status = 2;
                strcpy(game[id].result, "forfeit");
                return 1; //forfeit
            }
        }
        if(strcmp(player[i].name, game[id].player2) == 0){
            if(player[i].isPlaying == false){
                printf("[server]Jucatorul '%s' s-a deconectat de la jocul %d. \n", game[id].player2, id);
                printf("[server]Jocul %d s-a termint prin: forfeit. \n", id);
                game[id].status = 1;
                strcpy(game[id].result, "forfeit");
                return 1; //forfeit
            }
        }
    }


    if(canPlayerMove(id, game[id].player1) == false && canPlayerMove(id, game[id].player2) == false){ //nimeni nu poate muta
        printf("[server]Jocul %d s-a terminat printr-o remiza. \n", id);
        game[id].status = 3;
        strcpy(game[id].result, "draw");
        return 2; //draw
    }

    //numar cate piese are fiecare jucator
    int player1Pieces = 0;
    int player2Pieces = 0;
    for(int i = 0; i < BOARD_SIZE; i++)
        for(int j = 0; j < BOARD_SIZE; j++){
            if(game[id].matrix[i][j] == PLAYER1_SQUARE || game[id].matrix[i][j] == KING1_SQUARE)
                player1Pieces++;
            if(game[id].matrix[i][j] == PLAYER2_SQUARE || game[id].matrix[i][j] == KING2_SQUARE)
                player2Pieces++;
        }

    if(player1Pieces == 1 && player2Pieces == 1){
        printf("[server]Jocul %d s-a terminat cu o remiza. \n", id);
        game[id].status = 3;
        strcpy(game[id].result, "draw");
        return 2; //draw
    }

    if(player2Pieces == 0){
        printf("[server]Jocul %d a fost castigat de: %s. \n", id, game[id].player1);
        game[id].status = 1;
        strcpy(game[id].result, "won");
        return 3; //Player1 won
    }
    if(game[id].turn == 2){ //daca este randul player2
        if(canPlayerMove(id, game[id].player2) == false){ // este randul lui si nu poate muta
            printf("[server]Jocul %d a fost castigat de: %s. \n", id, game[id].player1);
            printf("[server]Jucatorul %s nu are nici o mutare disponibila. \n", game[id].player2);
            game[id].status = 1;
            strcpy(game[id].result, "won");
            return 3; //Player2 won
        }
    }

    if(player1Pieces == 0){
        printf("[server]Jocul %d a fost castigat de: %s. \n", id, game[id].player2);
        game[id].status = 2;
        strcpy(game[id].result, "won");
        return 4; //Player2 won
    }
    if(game[id].turn == 1){ //daca este randul player1
        if(canPlayerMove(id, game[id].player1) == false){ // este randul lui si nu poate muta
            printf("[server]Jocul %d a fost castigat de: %s. \n", id, game[id].player2);
            printf("[server]Jucatorul %s nu are nici o mutare disponibila. \n", game[id].player1);
            game[id].status = 2;
            strcpy(game[id].result, "won");
            return 4; //Player2 won
        }
    }

    printf("[server]Am terminat de evaluat ultima mutare din jocul: %d. \n", id);
    printf("\n");
    return 0; //continue game
}


void validateGame(int id)
{
    printf("[server]Incep validarea jocului: %d. \n", id);

    if(game[id].status == 1){ //player1 won
        printf("[server]Jocul %d a fost castigat de: %s. \n", id, game[id].player1);
        for(int i=1; i <= playersCount; i++){
            if(strcmp(game[id].player1, player[i].name) == 0){
                printf("[server]Jucatorului '%s' i-au fost adaugate doua puncte. \n", game[id].player1);
                player[i].score += 2;
            }
        }
    }
    if(game[id].status == 2){ //player2 won
        printf("[server]Jocul %d a fost castigat de: %s. \n", id, game[id].player1);
        for(int i=1; i <= playersCount; i++){
            if(strcmp(game[id].player2, player[i].name) == 0){
                printf("[server]Jucatorului '%s' i-au fost adaugate doua puncte. \n", game[id].player2);
                player[i].score += 2;
            }
        }
    }
    if(game[id].status == 3){ //draw
        printf("[server]Jocul %d s-a terminat printr-o remiza. \n", id);
        for(int i=1; i <= playersCount; i++){
            if(strcmp(game[id].player1, player[i].name) == 0){
                player[i].score += 1;
            }
            if(strcmp(game[id].player2, player[i].name) == 0){
                player[i].score += 1;
            }
        }
        printf("[server]Amndiblor jucatori le-a fost adaugat un punct. \n");
    }
    printf("[serve]Am termiant de validat jocul: %d. \n", id);

    updateFile();
    game[id].status = -1;
}

void setMatrix(char msg[], char response[]) {
    char msgCpy[MESSAGE_SIZE];
    strcpy(msgCpy, msg + 11);
    char *p = strtok(msgCpy, " ");
    char playerName[PLAYER_NAME_SIZE];
    strcpy(playerName, p);
    p = strtok(NULL, " ");
    char idChar[10];
    strcpy(idChar, p);
    int id = convertCharToInt(idChar);
    p = strtok(NULL, " ");
    char turn[10];
    strcpy(turn, p);
    p = strtok(NULL, "\0");
    char charMatrix[MESSAGE_SIZE];
    strcpy(charMatrix, p);

    matrixToInt(charMatrix, id);
    if(strcmp(game[id].player2, playerName) == 0) {
        reverseMatrix(id);
    }

    if(strcmp(turn, "pass") == 0) {
        if(game[id].turn == 1) {
            game[id].turn = 2;
        } else if(game[id].turn == 2) {
            game[id].turn = 1;
        }
    }

    int evaluateMoveResponse = evaluateMove(id);

    if(evaluateMoveResponse == 1) //forfeit
    {
        strcpy(response, "result: forfeit");
        strcat(response, "\n");
        strcat(response, "matrix: ");
        if(strcmp(playerName, game[id].player1) == 0) {
            char charMatrix[MESSAGE_SIZE];
            matrixToChar(charMatrix, id);
            strcat(response, charMatrix);
        } else if(strcmp(playerName, game[id].player2) == 0) {
            reverseMatrix(id);
            char charMatrix[MESSAGE_SIZE];
            matrixToChar(charMatrix, id);
            strcat(response, charMatrix);
            reverseMatrix(id);
        }
    }

    if(evaluateMoveResponse == 2) //draw
    {
        strcpy(response, "result: draw");
        strcat(response, "\n");
        strcat(response, "matrix: ");
        if(strcmp(playerName, game[id].player1) == 0) {
            char charMatrix[MESSAGE_SIZE];
            matrixToChar(charMatrix, id);
            strcat(response, charMatrix);
        } else if(strcmp(playerName, game[id].player2) == 0) {
            reverseMatrix(id);
            char charMatrix[MESSAGE_SIZE];
            matrixToChar(charMatrix, id);
            strcat(response, charMatrix);
            reverseMatrix(id);
        }
    }

    if(evaluateMoveResponse == 3){ //Player1 won
        strcpy(response, "result: ");
        strcat(response, game[id].player1);
        strcat(response, "\n");
        strcat(response, "matrix: ");
        if(strcmp(playerName, game[id].player1) == 0) {
            char charMatrix[MESSAGE_SIZE];
            matrixToChar(charMatrix, id);
            strcat(response, charMatrix);
        } else if(strcmp(playerName, game[id].player2) == 0) {
            reverseMatrix(id);
            char charMatrix[MESSAGE_SIZE];
            matrixToChar(charMatrix, id);
            strcat(response, charMatrix);
            reverseMatrix(id);
        }
    }

    if(evaluateMoveResponse == 4){ //Player2 won
        strcpy(response, "result: ");
        strcat(response, game[id].player2);
        strcat(response, "\n");
        strcat(response, "matrix: ");
        if(strcmp(playerName, game[id].player1) == 0) {
            char charMatrix[MESSAGE_SIZE];
            matrixToChar(charMatrix, id);
            strcat(response, charMatrix);
        } else if(strcmp(playerName, game[id].player2) == 0) {
            reverseMatrix(id);
            char charMatrix[MESSAGE_SIZE];
            matrixToChar(charMatrix, id);
            strcat(response, charMatrix);
            reverseMatrix(id);
        }
    }

    // if(evaluateMoveResponse != 0)
    //     validateGame(id);

    if(evaluateMoveResponse == 0){
        strcpy(response, "success: setMatrix");
    }

}

void playerForfeit(char msg[], char response[]){
    char playerName[MESSAGE_SIZE];
    strcpy(playerName, msg + 9);

    for(int i = 1; i<= playersCount; i++){
        if(strcmp(player[i].name, playerName) == 0){
            player[i].isChallenged = false;
            strcpy(player[i].isChallengedBy, " ");
            player[i].hasChallenged = false;
            strcpy(player[i].challenged, " ");
            player[i].challengeResponse = 0;
            player[i].isPlaying = false;

        }
    }

    strcpy(response, "success: forfeit");
}

void returnPlayerToLobby (char msg[], char response[]){
    char playerName[MESSAGE_SIZE];
    strcpy(playerName, msg + 15);

    for(int i = 1; i <= playersCount; i++){
        if(strcmp(player[i].name, playerName) == 0){
            player[i].isChallenged = false;
            strcpy(player[i].isChallengedBy, " ");
            player[i].hasChallenged = false;
            strcpy(player[i].challenged, " ");
            player[i].challengeResponse = 0;
            player[i].isPlaying = false;

        }
    }
    strcpy(response, "success: backToLobby");
}

void getMatrixUpdate(char msg[], char response[]) {
    char msgCpy[MESSAGE_SIZE];
    strcpy(msgCpy, msg + 17);
    char *p = strtok(msgCpy, " ");
    char playerName[PLAYER_NAME_SIZE];
    strcpy(playerName, p);
    p = strtok(NULL, " ");
    char idChar[10];
    strcpy(idChar, p);
    int id = convertCharToInt(idChar);

    int evaluateMoveResponse = evaluateMove(id);

    if(evaluateMoveResponse == 1) //forfeit
    {
        strcpy(response, "result: forfeit");
        strcat(response, "\n");
        strcat(response, "matrix: ");
        if(strcmp(playerName, game[id].player1) == 0) {
            char charMatrix[MESSAGE_SIZE];
            matrixToChar(charMatrix, id);
            strcat(response, charMatrix);
        } else if(strcmp(playerName, game[id].player2) == 0) {
            reverseMatrix(id);
            char charMatrix[MESSAGE_SIZE];
            matrixToChar(charMatrix, id);
            strcat(response, charMatrix);
            reverseMatrix(id);
        }
    }

    if(evaluateMoveResponse == 2) //draw
    {
        strcpy(response, "result: draw");
        strcat(response, "\n");
        strcat(response, "matrix: ");
        if(strcmp(playerName, game[id].player1) == 0) {
            char charMatrix[MESSAGE_SIZE];
            matrixToChar(charMatrix, id);
            strcat(response, charMatrix);
        } else if(strcmp(playerName, game[id].player2) == 0) {
            reverseMatrix(id);
            char charMatrix[MESSAGE_SIZE];
            matrixToChar(charMatrix, id);
            strcat(response, charMatrix);
            reverseMatrix(id);
        }
    }

    if(evaluateMoveResponse == 3){ //Player1 won
        strcpy(response, "result: ");
        strcat(response, game[id].player1);
        strcat(response, "\n");
        strcat(response, "matrix: ");
        if(strcmp(playerName, game[id].player1) == 0) {
            char charMatrix[MESSAGE_SIZE];
            matrixToChar(charMatrix, id);
            strcat(response, charMatrix);
        } else if(strcmp(playerName, game[id].player2) == 0) {
            reverseMatrix(id);
            char charMatrix[MESSAGE_SIZE];
            matrixToChar(charMatrix, id);
            strcat(response, charMatrix);
            reverseMatrix(id);
        }
    }

    if(evaluateMoveResponse == 4){ //Player2 won
        strcpy(response, "result: ");
        strcat(response, game[id].player2);
        strcat(response, "\n");
        strcat(response, "matrix: ");
        if(strcmp(playerName, game[id].player1) == 0) {
            char charMatrix[MESSAGE_SIZE];
            matrixToChar(charMatrix, id);
            strcat(response, charMatrix);
        } else if(strcmp(playerName, game[id].player2) == 0) {
            reverseMatrix(id);
            char charMatrix[MESSAGE_SIZE];
            matrixToChar(charMatrix, id);
            strcat(response, charMatrix);
            reverseMatrix(id);
        }
    }

    if(evaluateMoveResponse == 0){ //continue game
        strcpy(response, "turn: ");
        if(game[id].turn == 1) {
            strcat(response, game[id].player1);
        } else if(game[id].turn == 2) {
            strcat(response, game[id].player2);
        }
        strcat(response, "\n");
        strcat(response, "matrix: ");

        if(strcmp(playerName, game[id].player1) == 0) {
            char charMatrix[MESSAGE_SIZE];
            matrixToChar(charMatrix, id);
            strcat(response, charMatrix);
        } else if(strcmp(playerName, game[id].player2) == 0) {
            reverseMatrix(id);
            char charMatrix[MESSAGE_SIZE];
            matrixToChar(charMatrix, id);
            strcat(response, charMatrix);
            reverseMatrix(id);
        }
    }

    if(evaluateMoveResponse != 0)
        validateGame(id);

}

void exitClient(char msg[], char response[])
{
    strcpy(response, "exit");
}

void messageHandler(char msg[], char response[])
{
    char command[MESSAGE_SIZE];
    clearCommandFromEndLine(msg, command);

    if(isRegister(command))
    {
        registerPlayer(command, response);
        return;
    }
    if(isLogin(command))
    {
        loginPlayer(command, response);
        return;
    }
    if(isLogout(command))
    {
        logoutPlayer(command, response);
        return;
    }

    if(isGetLobbyStatus(command))
    {
        getLobbyStatus(command, response);
        return;
    }

    if(isGetLeaderboard(command)){
        sentLeaderboard(command, response);
        return;
    }

    if(isChallenge(command))
    {
        challengePlayer(command, response);
        return;
    }

    if(isAccept(command))
    {
        acceptChallenge(command, response);
        return;
    }

    if(isDecline(command))
    {
        declineChallenge(command, response);
        return;
    }

    if(isInitGame(command))
    {
        initGame(command, response);
        return;
    }

    if(isSetMatrix(command))
    {
        setMatrix(command, response);
        return;
    }
    if(isGetMatrixUpdate(command))
    {
        getMatrixUpdate(command, response);
        return;
    }

    if(isForfeit(command)){
        playerForfeit(command, response);
        return;
    }

    if(isReturnToLobby(command)){
        returnPlayerToLobby(command, response);
        return;
    }

    if(isExit(command))
    {
        exitClient(command, response);
        return;
    }

    strcpy(response, "error.invalidCommand Invalid Command");
}





int raspunde(void *arg)
{
    struct thData tdL;
    tdL= *((struct thData*)arg);
    char buffer[MESSAGE_SIZE];		/* mesajul */
    int bytes;			/* numarul de octeti cititi/scrisi */
    char msg[MESSAGE_SIZE];		//mesajul primit de la client
    char msgrasp[MESSAGE_SIZE]="";  //mesaj de raspuns pentru client

    bytes = read (tdL.cl, msg, sizeof (buffer));
    if (bytes < 0)
    {
        perror ("Eroare la read() de la client.\n");
        // return 0;
    }
    printf ("[server]Mesajul a fost receptionat...%s\n", msg);

    char messageHandlerResponse[MESSAGE_SIZE];
    messageHandler(msg, messageHandlerResponse);

    if(strcmp(messageHandlerResponse, "exit") == 0) //logout sau exit
        return -1;

    bzero(msgrasp, MESSAGE_SIZE);
    strcpy(msgrasp, messageHandlerResponse);

    /*pregatim mesajul de raspuns */
    // bzero(msgrasp, MESSAGE_SIZE);
    // strcpy(msgrasp, msg);

    printf("/////////////////////// JUCATORI //////////////////////\n");
    printf("[server]Numarul de clienti conectati este: %d \n", playersCount);
    for(int j=1; j<=playersCount; j++)
        printf("ID: %d - Nume: %s - Parola: %s - isOnline: %d - hasChallenged: %d - challenged: %s - isChallenged: %d - isChallengedBy: %s - challengeResponse: %d - isPlaying: %d - Score: %d \n", player[j].id, player[j].name, player[j].password, player[j].isOnline, player[j].hasChallenged, player[j].challenged, player[j].isChallenged, player[j].isChallengedBy, player[j].challengeResponse, player[j].isPlaying, player[j].score);
    printf("/////////////////////////////////////////////////////\n");
    printf("\n");
    printf("/////////////////////// GAMES //////////////////////\n");
    printf("[server]Numarul de jocuri active este: %d \n", gamesCount);
    for(int j=1; j<=gamesCount; j++)
        printf("ID: %d - Player1: %s - Player2: %s - turn: %d - status: %d \n", game[j].id, game[j].player1, game[j].player2, game[j].turn, game[j].status);
    printf("////////////////////////////////////////////////////////\n");
    printf("\n");
    printf("[server]Trimitem mesajul inapoi...%s\n",msgrasp);

    if (bytes && write (tdL.cl, msgrasp, bytes) < 0)
    {
        perror ("[server] Eroare la write() catre client.\n");
        // return 0;
    }
    return 0;
}//raspunde

static void *treat(void * arg)
{
    struct thData tdL = *((struct thData*)arg);

    while(1) {
        printf ("[thread]- %d - Asteptam mesajul...\n", tdL.idThread);
        pthread_detach(pthread_self());
        if(raspunde((struct thData*)arg) == -1) {
            /* am terminat cu acest client, inchidem conexiunea */
            close ((intptr_t)arg);
            return(NULL);
        }
    }
};

int main ()
{
    struct sockaddr_in server;	// structura folosita de server
    struct sockaddr_in from;
    int nr;		//mesajul primit de trimis la client
    int sd;		//descriptorul de socket
    int pid;
    pthread_t th[100];    //Identificatorii thread-urilor care se vor crea
    int i=0;

    readFromFile();

    /* crearea unui socket */
    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror ("[server]Eroare la socket().\n");
        return errno;
    }
    /* utilizarea optiunii SO_REUSEADDR */
    int on=1;
    setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

    /* pregatirea structurilor de date */
    bzero (&server, sizeof (server));
    bzero (&from, sizeof (from));

    /* umplem structura folosita de server */
    /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;
    /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl (INADDR_ANY);
    /* utilizam un port utilizator */
    server.sin_port = htons (PORT);

    /* atasam socketul */
    if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
        perror ("[server]Eroare la bind().\n");
        return errno;
    }

    /* punem serverul sa asculte daca vin clienti sa se conecteze */
    if (listen (sd, 2) == -1)
    {
        perror ("[server]Eroare la listen().\n");
        return errno;
    }
    /* servim in mod concurent clientii...folosind thread-uri */
    while (1)
    {
        int client;
        thData * td; //parametru functia executata de thread
        socklen_t length = sizeof (from);

        printf ("[server]Asteptam la portul %d...\n",PORT);
        fflush (stdout);

        // client= malloc(sizeof(int));
        /* acceptam un client (stare blocanta pina la realizarea conexiunii) */
        if ( (client = accept (sd, (struct sockaddr *) &from, &length)) < 0)
        {
            perror ("[server]Eroare la accept().\n");
            continue;
        }

        /* s-a realizat conexiunea, se astepta mesajul */

        // int idThread; //id-ul threadului
        // int cl; //descriptorul intors de accept

        td=(struct thData*)malloc(sizeof(struct thData));
        td->idThread=i++;
        td->cl=client;

        pthread_create(&th[i], NULL, &treat, td);
    }//while
};