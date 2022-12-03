# Draughts
Draughts game for Computer Networks class

## To compile and run projects:
Go into each project's folder and run:
- server: `gcc server.cpp -o server && ./server`
- client: `g++ -c client.cpp && g++ client.o -o client -lsfml-graphics -lsfml-window -lsfml-system && ./client 0 2728`

You will need SFML to run the client. To install it, run this command: `sudo apt-get install libsfml-dev`
