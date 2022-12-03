//g++ -c client.cpp && g++ client.o -o client -lsfml-graphics -lsfml-window -lsfml-system && ./client 0 2728
//gcc server.cpp -o server && ./server

#include <SFML/Graphics.hpp>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
//#include <sys/types.h>
//#include <errno.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <netdb.h>

//Board Square types
#define EMPTY_SQUARE 0
#define PLAYER1_SQUARE 1
#define KING1_SQUARE 2
#define PLAYER2_SQUARE 3
#define KING2_SQUARE 4
#define MOVE_OPTION_SQUARE 5

//Window & app
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 960
#define WINDOW_COLOR Black
#define MAIN_MENU 0
#define PLAYING 1

//Server communication
#define MESSAGE_SIZE 500

//Move Turn
#define PLAYER1 1
#define PLAYER2 2

//Board
#define BOARD_SIZE 8
#define SQUARE_SIZE 110
#define BOARD_OFFSET_X (WINDOW_WIDTH-BOARD_SIZE*SQUARE_SIZE)/2 //The offset for the left/right of the board
#define BOARD_OFFSET_Y (WINDOW_HEIGHT-BOARD_SIZE*SQUARE_SIZE)/2 //The offset for the tob/bottom of the board
#define SQUARE_COLOR1 sf::Color::Yellow//()
#define SQUARE_COLOR2 sf::Color::White//()

//Pieces
#define PIECE_SIZE (SQUARE_SIZE-35)/2
#define MOVE_OPTION_SIZE (SQUARE_SIZE-70)/2
#define PIECE_COLOR1 sf::Color::Green
#define KING_COLOR1 sf::Color::Magenta
#define PIECE_COLOR2 sf::Color::Red
#define KING_COLOR2 sf::Color::Cyan
#define MOVE_OPTION_COLOR sf::Color::Blue


struct ServerData {
    int port;
    char message[100];		// mesajul trimis
    unsigned int messageLength=0;
    unsigned int length=0;
};

struct WindowProps {
    bool mouseIsPressed = false;
    bool mouseIsMoving = false;
    int appMenu = PLAYING;
    int selectedPieceIndex = -1;
    int turn = PLAYER1;
    sf::Vector2f mousePos; //Mouse position
};

struct BoardMatrix {
    int type = EMPTY_SQUARE;
    int squareIndex = 0;
    sf::RectangleShape square;
    sf::CircleShape piece;
    sf::CircleShape moveOption;
};

struct {
    BoardMatrix boardMatrix[BOARD_SIZE][BOARD_SIZE];
    WindowProps appProps; //Props for graphics
    ServerData serverData;
}code;

#include "main.h"

int main(int argc, char *argv[]) {
    //FOR TESTING TODO: Delete
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            if(i <= 2) {
//                if((i+j) % 2 != 0) {
//                    code.boardMatrix[i][j].type = PLAYER2_SQUARE;
//                }
            } else if (i >= 5) {
                if ((i+j) % 2 != 0) {
                    code.boardMatrix[i][j].type = PLAYER1_SQUARE;
                }
            }
        }
    }
    code.boardMatrix[4][5].type = PLAYER2_SQUARE;
    code.boardMatrix[3][4].type = PLAYER2_SQUARE;
    //END OF TESTING

    int socketDescriptor;			// descriptorul de socket
    struct sockaddr_in server;	// structura folosita pentru conectare


    if (argc != 3)
    {
        printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
        return -1;
    }

    code.serverData.port = atoi (argv[2]);

    /* cream socketul */
    if ((socketDescriptor = socket (AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror ("Eroare la socket().\n");
        return errno;
    }

    /* umplem structura folosita pentru realizarea dialogului cu serverul */
    /* familia socket-ului */
    server.sin_family = AF_INET;
    /* adresa IP a serverului */
    server.sin_addr.s_addr = inet_addr(argv[1]);
    /* portul de conectare */
    server.sin_port = htons (code.serverData.port);

    appWindow(server, socketDescriptor);
    close(socketDescriptor);

    return 0;
}