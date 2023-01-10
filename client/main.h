#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"
#pragma clang diagnostic ignored "-Wenum-compare-switch"
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-member-init"
#pragma ide diagnostic ignored "bugprone-branch-clone"
#include <SFML/Graphics.hpp>
#include "graphics.h"
#include "helperFunctions.h"

void appWindow() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Draughts");
    window.setFramerateLimit(60);
    loadImages();
    loadFonts();

    while (window.isOpen())
    {
        updateErrorTime();
        if(code.appProps.appMenu == MAIN_MENU && !code.mainMenu.challenge.isModalOpen && !code.mainMenu.isLeaderboard) {
            if(code.appProps.wait == 50) {
                char res[MESSAGE_SIZE];
                sendReadMessage(appendChars("getLobbyStatus: ", code.appProps.playerName), res);
                handleGetLobbyStatusResponse(res);
                code.appProps.wait = 0;
            }
            code.appProps.wait++;
        } else if(code.appProps.appMenu == PLAYING && code.playingMenu.turn == PLAYER2 && code.playingMenu.winner == NO_WINNER) {
            if(code.appProps.wait == 50) {
                char res[MESSAGE_SIZE];
                sendReadMessage(appendChars("getMatrixUpdate: ", code.appProps.playerName, " ", idToChar(code.playingMenu.gameId)), res);
                handleGetMatrixUpdateResponse(res);
                code.appProps.wait = 0;
            }
            code.appProps.wait++;
        }

        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    if(code.appProps.appMenu != LOGIN_MENU) {
                        sendMessage(appendChars("logout: ", code.appProps.playerName));
                    } else {
                        sendMessage("exit.");
                    }
                    close(code.server.sd);
                    break;

                case sf::Event::KeyPressed: // Keyboard keys
                    switch (event.key.code) {
                        case sf::Keyboard::Escape:
                            window.close();
                            if(code.appProps.appMenu != LOGIN_MENU) {
                                sendMessage(appendChars("logout: ", code.appProps.playerName));
                            } else {
                                sendMessage("exit.");
                            }
                            close(code.server.sd);
                            break;
                        default:
                            break;
                    }
                    break;

                case sf::Event::TextEntered: // Keyboard text
                    if(code.appProps.appMenu == LOGIN_MENU) {
                        if(code.loginMenu.activeInputField == USERNAME) {
                            onPlayerNameChange(event.text.unicode);
                        } else if(code.loginMenu.activeInputField == PASSWORD) {
                            onPasswordChange(event.text.unicode);
                        } else if(code.loginMenu.activeInputField == CONFIRM_PASSWORD) {
                            onConfirmPasswordChange(event.text.unicode);
                        }
                    }
                    break;

                case sf::Event::MouseButtonPressed: // Mouse pressed buttons
                    switch (event.key.code) {
                        case sf::Mouse::Left:
                            code.appProps.mouseIsPressed = true;
                            if(code.appProps.appMenu == LOGIN_MENU) {
                                if(mouseIsOnItem(code.loginMenu.nameInputBackground)) {
                                    code.loginMenu.activeInputField = USERNAME;
                                } else if(mouseIsOnItem(code.loginMenu.passwordInputBackground)) {
                                    code.loginMenu.activeInputField = PASSWORD;
                                } if(mouseIsOnItem(code.loginMenu.confirmInputBackground)) {
                                    code.loginMenu.activeInputField = CONFIRM_PASSWORD;
                                } else if(mouseIsOnItem(code.loginMenu.firstButton)) {
                                    onLoginFirstButtonClickHandler(window);
                                } else if(mouseIsOnItem(code.loginMenu.secondButton)) {
                                    code.loginMenu.isLoginActive = !code.loginMenu.isLoginActive;
                                }
                            } else if(code.appProps.appMenu == MAIN_MENU) {
                                if(code.mainMenu.challenge.isModalOpen) {
                                    if(mouseIsOnItem(code.mainMenu.challenge.acceptButton)) {
                                        onResponseInviteHandler(true);
                                    } else if(mouseIsOnItem(code.mainMenu.challenge.declineButton)) {
                                        onResponseInviteHandler(false);
                                    }
                                } else {
                                    int challengedPlayerId = mouseIsOnPlayerCard();
                                    if(challengedPlayerId != -1 && !code.mainMenu.isLeaderboard) {
                                        char res[MESSAGE_SIZE];
                                        strcpy(code.appProps.enemyName, code.mainMenu.players[challengedPlayerId]);
                                        sendReadMessage(appendChars("challenge: ", code.appProps.playerName, " ",code.mainMenu.players[challengedPlayerId]), res);
                                        if(!isBackendError(res)) {
                                            code.mainMenu.response.isModalOpen = true;
                                        }
                                    } else if(mouseIsOnItem(code.mainMenu.button)) {
                                        onLeaderboardSwitch();
                                    }
                                }
                            } else if(code.appProps.appMenu == PLAYING) {
                                if(code.playingMenu.turn == PLAYER1 && code.playingMenu.winner == NO_WINNER){
                                    int selectedPiece = mouseIsOnPiece();
                                    if(selectedPiece != -1) {
                                        onPieceClickHandler(selectedPiece);
                                    }
                                    int selectedOption = mouseIsOnOption();
                                    if(selectedOption != -1) {
                                        onOptionClickHandler(selectedOption);
                                    }
                                }
                                if(mouseIsOnQuitGame()) {
                                    onQuitGameHandler();
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
                    switch (event.key.code) {
                        case sf::Mouse::Left:
                            code.appProps.mouseIsPressed = false;

                            break;
                        case sf::Mouse::Right:
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
        if(code.appProps.appMenu == LOGIN_MENU){
            drawLoginMenu();
            drawModal();
            displayModal(window);
            displayLoginMenu(window);
        } else if(code.appProps.appMenu == MAIN_MENU) {
            drawModal();
            drawMainMenu(window);
            displayModal(window);
            displayMainMenu(window);
            if(code.mainMenu.challenge.isModalOpen) {
                drawChallengeModal();
                dislayChallengeModal(window);
            } else if(code.mainMenu.response.isModalOpen) {
                drawResponseModal();
                dislayResponseModal(window);
            }
        } else if(code.appProps.appMenu == PLAYING) {
            drawGame();
            displayGame(window);
            displayBoardMatrix(window);
            displayPieces(window);
            displayMoveOptions(window);
        }
        if(code.appProps.error[0] != '\0') {
            drawError();
            displayError(window);
        }
        /// END OF DRAWING ZONE ///

        window.display();

//        printf("\n");
//        for(int i = 0; i < BOARD_SIZE; i++) {
//            printf("%d: ", i);
//        }
//        printf("\n");
//        for(int j = 0; j < BOARD_SIZE; j++) {
//            printf("%d: ", j);
//            for(int i = 0; i < BOARD_SIZE; i++) {
//                printf("%d  ", code.playingMenu.boardMatrix[i][j].type);
//            }
//            printf("\n");
//        }
//        printf("\n");
    }

}
#pragma clang diagnostic pop