//g++ -c client.cpp && g++ client.o -o client -lsfml-graphics -lsfml-window -lsfml-system && ./client 0 2927
//gcc server.cpp -o server && ./server

#include <SFML/Graphics.hpp>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>

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
#define MODAL_WIDTH 0.87*WINDOW_WIDTH
#define MODAL_HEIGHT 0.87*WINDOW_HEIGHT
#define MODAL_COLOR sf::Color(230, 230, 230)
#define LOGIN_MENU 0
#define MAIN_MENU 1
#define PLAYING 2

//Char sizes
#define MESSAGE_SIZE 1000
#define PLAYER_NAME_SIZE 32
#define ERROR_MESSAGE_SIZE 101
#define MAX_ACTIVE_PLAYERS 100

//Move Turn/Winner
#define NO_WINNER -1
#define PLAYER1 0
#define PLAYER2 1
#define DRAW 2

//Main Menu
#define MAX_CARDS 10

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
#define PIECE_COLOR1 sf::Color(0, 255, 0)
#define SELECTED_PIECE_COLOR1 sf::Color(51, 153, 51)
#define KING_COLOR1 PIECE_COLOR1
#define SELECTED_KING_COLOR1 SELECTED_PIECE_COLOR1
#define PIECE_COLOR2 sf::Color(255, 0, 0)
#define SELECTED_PIECE_COLOR2 sf::Color(179, 0, 0)
#define KING_COLOR2 PIECE_COLOR2
#define SELECTED_KING_COLOR2 SELECTED_PIECE_COLOR2
#define MOVE_OPTION_COLOR sf::Color::Blue
#define KING_OUTLINE_COLOR sf::Color::Black


struct ServerData {
    int port;
    int sd;			// descriptorul de socket
    struct sockaddr_in serverData;	// structura folosita pentru conectare
};

struct AppProps {
    bool mouseIsPressed = false;
    bool mouseIsMoving = false;
    int appMenu = LOGIN_MENU;
    sf::Vector2f mousePos;
    sf::Texture backgroundTexture;
    sf::Font arialMedium;
    sf::RectangleShape background;
    sf::RectangleShape modal;
    sf::RectangleShape errorBackground;
    sf::Text errorText;
    sf::Text modalTitle;
    char playerName[PLAYER_NAME_SIZE] = {'\0'};
    char enemyName[PLAYER_NAME_SIZE] = {'\0'};
    char error[ERROR_MESSAGE_SIZE] = {'\0'};
    int errorTime = 0;
    int wait = 0;
};

struct BoardMatrix {
    int type = EMPTY_SQUARE;
    int squareIndex = 0;
    sf::RectangleShape square;
    sf::CircleShape piece;
    sf::CircleShape moveOption;
};

struct LoginMenuProps {
    #define USERNAME 0
    #define PASSWORD 1
    #define CONFIRM_PASSWORD 2
    bool isLoginActive = true;
    int activeInputField = USERNAME;
    sf::Text nameText;
    sf::RectangleShape nameInputBackground;
    sf::Text nameInput;
    sf::Text passwordText;
    sf::RectangleShape passwordInputBackground;
    sf::Text passwordInput;
    char password[PLAYER_NAME_SIZE] = {'\0'};
    sf::Text confirmText;
    sf::RectangleShape confirmInputBackground;
    sf::Text confirmInput;
    char confirmPassword[PLAYER_NAME_SIZE] = {'\0'};
    sf::RectangleShape firstButton;
    sf::Text firstButtonText;
    sf::RectangleShape secondButton;
    sf::Text secondButtonText;
};

struct MainMenuProps {
    bool isLeaderboard = false;
    sf::RectangleShape playersBackground;
    sf::RectangleShape playerCard[MAX_CARDS];
    sf::Text playerText[MAX_CARDS];
    char players[MAX_ACTIVE_PLAYERS][PLAYER_NAME_SIZE];
    sf::Text playerScore[MAX_CARDS];
    char scores[MAX_ACTIVE_PLAYERS][10];
    int playersCount = 0;
    sf::RectangleShape button;
    sf::Text buttonText;
    struct {
        sf::RectangleShape challengeBackground;
        sf::Text playerName;
        sf::Text title;
        sf::RectangleShape acceptButton;
        sf::Text accept;
        sf::RectangleShape declineButton;
        sf::Text decline;
        bool isModalOpen = false;
    } challenge;
    struct {
        sf::RectangleShape challengeBackground;
        sf::Text playerName;
        sf::Text title;
        bool isModalOpen = false;
    }response;

};

struct PlayingMenuProps {
    int gameId;
    int selectedPieceIndex = -1;
    int turn = PLAYER1;
    int winner = NO_WINNER;
    int playerColor = PLAYER1;
    BoardMatrix boardMatrix[BOARD_SIZE][BOARD_SIZE];
    struct {
        sf::Text turnMessage;
        sf::RectangleShape topBackground;
        sf::RectangleShape quitButton;
        sf::Text quitButtonText;
        sf::RectangleShape piecesLeft;
        sf::Text piecesLeftText;
    } info;
};

struct {
    AppProps appProps;
    LoginMenuProps loginMenu;
    MainMenuProps mainMenu;
    PlayingMenuProps playingMenu;
    ServerData server;
}code;

#include "main.h"

void initConnection(int argc, char *argv[]) {
    if (argc != 3) {
        printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
        exit(-1);
    }
    code.server.port = atoi(argv[2]);
    /* cream socketul */
    if ((code.server.sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror ("[client] Eroare la socket().\n");
        exit(errno);
    }
    /* umplem structura folosita pentru realizarea dialogului cu serverul */
    code.server.serverData.sin_family = AF_INET; //familia socket-ului
    code.server.serverData.sin_addr.s_addr = inet_addr(argv[1]); //adresa IP a serverului
    code.server.serverData.sin_port = htons(code.server.port); //portul de conectare
    if (connect(code.server.sd, (struct sockaddr *) &code.server.serverData,sizeof (struct sockaddr)) == -1)
    {
        perror ("[client]Eroare la connect().\n");
        exit(errno);
    }
}

int main(int argc, char *argv[]) {
    initConnection(argc, argv);
    appWindow();
    return 0;
}
