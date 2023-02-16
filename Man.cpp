#include "Man.h"

void Man::go()
{
    MOUSEMSG msg;
    ChessPos pos;

    while (1)
    {
        // 获取鼠标点击消息
        msg = GetMouseMsg();

        // 通过chess对象，判断落子是否有效
        if (msg.uMsg == WM_LBUTTONDOWN && chess->clickBoard(msg.x, msg.y, &pos))
            break;
    }

    // 落子
    chess->chessDown(&pos, CHESS_BLACK);
}
void Man::init(Chess *chess)
{
    this->chess = chess;
}
