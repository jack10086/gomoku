#pragma once
#include "AI.h"
#include "Man.h"
#include "Chess.h"
class ChessGame
{
public:
    ChessGame(Man *man, AI *ai, Chess *chess);
    // 开始对局
    void play();

private:
    Man *man;
    AI *ai;
    Chess *chess;
};