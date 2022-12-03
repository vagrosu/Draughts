#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"
#pragma clang diagnostic ignored "-Wenum-compare-switch"
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-member-init"
#pragma ide diagnostic ignored "bugprone-branch-clone"
#include <SFML/Graphics.hpp>
#include "graphics.h"
#include "helperFunctions.h"

int sendMessage(sockaddr_in server, int socketDescriptor, char sentMessage[], char response[]) {
    fflush (stdout);

    /* trimiterea mesajului la server */
    code.serverData.length = sizeof(server);
    if (sendto (socketDescriptor, sentMessage, MESSAGE_SIZE, 0, (struct sockaddr*)&server, code.serverData.length) <= 0)
    {
        perror ("[client]Eroare la sendto() spre server.\n");
        return errno;
    }

    /* citirea raspunsului dat de server
      (apel blocant pina cind serverul raspunde) */
    if ((code.serverData.messageLength=recvfrom(socketDescriptor, response, MESSAGE_SIZE, 0, (struct sockaddr*)&server, &code.serverData.length)) < 0)
    {
        perror ("[client]Eroare la recvfrom() de la server.\n");
        return errno;
    }
}

void appWindow(sockaddr_in server, int socketDescriptor)
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Draughts Client");



    window.setFramerateLimit(60);

    drawTable();
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;

                case sf::Event::KeyPressed: // Keyboard keys
                    switch (event.key.code) {
                        case sf::Keyboard::Escape:
                            window.close();
                            break;
                        default:
                            break;
                    }
                    break;

                case sf::Event::TextEntered: // Keyboard text
                    break;

                case sf::Event::MouseButtonPressed: // Mouse pressed buttons
                    switch (event.key.code) {
                        case sf::Mouse::Left:
                            code.appProps.mouseIsPressed = true;
                            if(code.appProps.appMenu == PLAYING && code.appProps.turn == PLAYER1) {
                                int selectedPiece = mouseIsOnPiece();
                                if(selectedPiece != -1) {
                                    onPieceClickHandler(selectedPiece);
                                }
                                int selectedOption = mouseIsOnOption();
                                if(selectedOption != -1) {
                                    onOptionClickHandler(selectedOption);

                                    char res[MESSAGE_SIZE], msg[MESSAGE_SIZE];
                                    matrixToChar(msg);
                                    sendMessage(server, socketDescriptor, msg, res);
                                    printf ("%s", res);
                                }
                            }
                            break;
                        case sf::Mouse::Right:
                            break;
                        default:
                            break;
                    }
                    break;

                case sf::Event::MouseButtonReleased: // Mouse released buttons
//                    if(code.appProps.userInput.inputIsActive != false && mouseIsOnItem(code.appProps.userInput.userInputBackground.getPosition(), code.appProps.userInput.userInputBackground.getSize()) == false) {
//                        userInputButtonIsPressedHandler(1);
//                    }
                    switch (event.key.code) {
                        case sf::Mouse::Left:
//                            if(mouseIsOnUserInputButton() != -1) {
//                                userInputButtonIsPressedHandler(mouseIsOnUserInputButton());
//                            }
//                            if(code.appProps.userInput.inputIsActive == false) {
//                                if(mouseIsOnBlockMenuButton() != -1) {
//                                    blockMenuButtonIsPressedHandler(mouseIsOnBlockMenuButton(), arialMedium);
//                                }
//                                if(mouseIsOnAppOutputButton() != -1) {
//                                    appOutputButtonIsPressedHandler(mouseIsOnAppOutputButton(), arialMedium);
//                                }
//                                if(mouseIsOnAppMenuButton() != -1) {
//                                    menuButtonIsPressedHandler(mouseIsOnAppMenuButton(), arialMedium);
//                                }
//                            } else {
//                                if(mouseIsOnUserInputField() != -1) {
//                                    code.appProps.userInput.activeField = mouseIsOnUserInputField();
//                                }
//                            }
                            code.appProps.mouseIsPressed = false;
//                            code.appProps.block.blockIsBeingMoved = false;
//                            code.appProps.addBlockMenu.blockIsBeingAdded = false;
                            break;
                        case sf::Mouse::Right:
//                            code.appProps.blockMenu.blockMenuIsActive = false;
//                            if(code.appProps.userInput.inputIsActive == false) {
//                                if(mouseIsOnBlock() != -1) {
//                                    updateBlockMenu(mouseIsOnBlock(), arialMedium);
//                                }
//                            }
                            break;
                        default:
                            break;
                    }
                    break;

                case sf::Event::MouseMoved: // Mouse has moved
                    code.appProps.mousePos.x = event.mouseMove.x;
                    code.appProps.mousePos.y = event.mouseMove.y;
                    code.appProps.mouseIsMoving = true;
                    break;
                default:
                    break;
            }
        }
        code.appProps.mouseIsMoving = false;

        window.clear(sf::Color::WINDOW_COLOR);

        /// DRAWING ZONE ///
        displayBoardMatrix(window);
        drawGame();
        displayPieces(window);
        displayMoveOptions(window);
        /// END OF DRAWING ZONE ///

        window.display();

//        printf("   ");
//        for(int i = 0; i < BOARD_SIZE; i++) {
//            printf("%d: ", i);
//        }
//        printf("\n");
//        for(int j = 0; j < BOARD_SIZE; j++) {
//            printf("%d: ", j);
//            for(int i = 0; i < BOARD_SIZE; i++) {
//                printf("%d  ", code.boardMatrix[i][j].type);
//            }
//            printf("\n");
//        }
//        printf("\n");
    }

}
#pragma clang diagnostic pop