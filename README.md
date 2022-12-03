# Draughts
Draughts game for Computer Networks class

## To compile and run projects:

- client: `g++ -c client.cpp && g++ client.o -o client -lsfml-graphics -lsfml-window -lsfml-system && ./client 0 2728`
- server: `gcc server.cpp -o server && ./server`

You will need SFML to run the client. To install it, run this command: `sudo apt-get install libsfml-devz`
