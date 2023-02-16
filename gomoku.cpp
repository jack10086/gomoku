#include <iostream>
#include "ChessGame.h"

int main(int argc, char *argv[])
{
    AI ai;
    Man man;
    Chess chess(13, 44, 43, 67.3);
    ChessGame game(&man, &ai, &chess);
    game.play();
    return 0;
}