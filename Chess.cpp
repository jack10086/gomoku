#include "Chess.h"
#include <cmath>
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <mmsystem.h>
#pragma comment(lib, "Winmm.lib")

void putimagePNG(int x, int y, IMAGE *picture) // x为载入图片的X坐标，y为Y坐标
{
    // 变量初始化
    DWORD *dst = GetImageBuffer(); // GetImageBuffer()函数，用于获取绘图设备的显存指针，EASYX自带
    DWORD *draw = GetImageBuffer();
    DWORD *src = GetImageBuffer(picture);      // 获取picture的显存指针
    int picture_width = picture->getwidth();   // 获取picture的宽度，EASYX自带
    int picture_height = picture->getheight(); // 获取picture的高度，EASYX自带
    int graphWidth = getwidth();               // 获取绘图区的宽度，EASYX自带
    int graphHeight = getheight();             // 获取绘图区的高度，EASYX自带
    int dstX = 0;                              // 在显存里像素的角标

    // 实现透明贴图 公式： Cp=αp*FP+(1-αp)*BP ， 贝叶斯定理来进行点颜色的概率计算
    for (int iy = 0; iy < picture_height; iy++)
    {
        for (int ix = 0; ix < picture_width; ix++)
        {
            int srcX = ix + iy * picture_width;        // 在显存里像素的角标
            int sa = ((src[srcX] & 0xff000000) >> 24); // 0xAArrggbb;AA是透明度
            int sr = ((src[srcX] & 0xff0000) >> 16);   // 获取RGB里的R
            int sg = ((src[srcX] & 0xff00) >> 8);      // G
            int sb = src[srcX] & 0xff;                 // B
            if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
            {
                dstX = (ix + x) + (iy + y) * graphWidth; // 在显存里像素的角标
                int dr = ((dst[dstX] & 0xff0000) >> 16);
                int dg = ((dst[dstX] & 0xff00) >> 8);
                int db = dst[dstX] & 0xff;
                draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  // 公式： Cp=αp*FP+(1-αp)*BP  ； αp=sa/255 , FP=sr , BP=dr
                             | ((sg * sa / 255 + dg * (255 - sa) / 255) << 8) // αp=sa/255 , FP=sg , BP=dg
                             | (sb * sa / 255 + db * (255 - sa) / 255);       // αp=sa/255 , FP=sb , BP=db
            }
        }
    }
}

Chess::Chess(int gradeSize, int marginX, int marginY, float chessSize)
{
    this->gradeSize = gradeSize;
    this->margin_x = marginX;
    this->margin_y = marginY;
    this->chessSize = chessSize;
    playerFlag = CHESS_BLACK;

    std::vector<int> tmp(gradeSize, 0);
    for (int i = 0; i < gradeSize; i++)
        chessMap.push_back(tmp);
}
int Chess::getGradeSize()
{
    return gradeSize;
}
void Chess::init()
{
    // 创建游戏窗口
    initgraph(900, 900, EW_SHOWCONSOLE);

    // 棋盘图片加载
    loadimage(0, _T("res/chess2.jpg"));

    // 播放开始提示音
    mciSendString(_T("play res/start.WAV"), 0, 0, 0);

    // 加载棋子图片
    loadimage(&chessBlackImg, _T("res/black.png"), chessSize, chessSize, true);
    loadimage(&chessWhiteImg, _T("res/white.png"), chessSize, chessSize, true);

    // 棋盘清零
    for (int i = 0; i < gradeSize; i++)
        for (int j = 0; j < gradeSize; j++)
            chessMap[i][j] = 0;

    playerFlag = true;
}
bool Chess::checkOver()
{
    if (checkWin())
    {
        Sleep(1000);
        if (playerFlag)
        { // 白方胜，即AI胜
            mciSendString(_T("play res/defeat.mp3"), 0, 0, 0);
            loadimage(0, _T("res/defeat.jpg"));
        }
        else
        {
            mciSendString(_T("play res/good.mp3"), 0, 0, 0);
            loadimage(0, _T("res/victory.jpg"));
        }
        _getch(); // 暂停
        return true;
    }
    return false;
}

bool Chess::clickBoard(int x, int y, ChessPos *pos)
{
    int col = (x - margin_x) / chessSize;
    int row = (y - margin_y) / chessSize;
    int leftTopPosX = margin_x + chessSize * col;
    int leftTopPosY = margin_y + chessSize * row;
    int offset = chessSize * 0.3;

    // 左上角的判断
    int len = sqrt((x - leftTopPosX) * (x - leftTopPosX) + (y - leftTopPosY) * (y - leftTopPosY));
    if (len < offset)
    {
        pos->row = row;
        pos->col = col;
        if (!chessMap[row][col])
            return true;
    }

    // 右上角判断
    len = sqrt((leftTopPosX + chessSize - x) * (leftTopPosX + chessSize - x) + (y - leftTopPosY) * (y - leftTopPosY));
    if (len < offset)
    {
        pos->row = row;
        pos->col = col + 1;
        if (!chessMap[row][col + 1])
            return true;
    }

    // 左下角判断
    len = sqrt((x - leftTopPosX) * (x - leftTopPosX) + (leftTopPosY + chessSize - y) * (leftTopPosY + chessSize - y));
    if (len < offset)
    {
        pos->row = row + 1;
        pos->col = col;
        if (!chessMap[row + 1][col])
            return true;
    }

    // 右下角判断
    len = sqrt((leftTopPosX + chessSize - x) * (leftTopPosX + chessSize - x) + (leftTopPosY + chessSize - y) * (leftTopPosY + chessSize - y));
    if (len < offset)
    {
        pos->row = row + 1;
        pos->col = col + 1;
        if (!chessMap[row + 1][col + 1])
            return true;
    }
    return false;
}

void Chess::chessDown(ChessPos *pos, chess_kind_t kind)
{
    mciSendString(_T("play res/down7.wav"), 0, 0, 0);
    int x = margin_x + (pos->col - 0.5) * chessSize;
    int y = margin_y + (pos->row - 0.5) * chessSize;

    if (kind == CHESS_WHITE)
        putimagePNG(x, y, &chessWhiteImg);
    else
        putimagePNG(x, y, &chessBlackImg);

    updateGameMap(pos);
}

int Chess::getChessData(ChessPos *pos)
{
    return chessMap[pos->row][pos->col];
}

int Chess::getChessData(int row, int col)
{
    return chessMap[row][col];
}

void Chess::updateGameMap(ChessPos *pos)
{
    lastPos = ChessPos(pos->row, pos->col);
    chessMap[pos->row][pos->col] = playerFlag ? CHESS_BLACK : CHESS_WHITE;
    playerFlag = !playerFlag; // 黑白方交换行棋
}

bool Chess::checkWin()
{
    // 横竖斜四种大情况，每种情况都根据当前落子往后遍历5个棋子，有一种符合就算赢
    // 水平方向
    int row = lastPos.row;
    int col = lastPos.col;

    for (int i = 0; i < 5; i++)
    {
        // 往左5个，往右匹配4个子，20种情况
        if (col - i >= 0 &&
            col - i + 4 < gradeSize &&
            chessMap[row][col - i] == chessMap[row][col - i + 1] &&
            chessMap[row][col - i] == chessMap[row][col - i + 2] &&
            chessMap[row][col - i] == chessMap[row][col - i + 3] &&
            chessMap[row][col - i] == chessMap[row][col - i + 4])
            return true;
    }

    // 竖直方向(上下延伸4个)
    for (int i = 0; i < 5; i++)
    {
        if (row - i >= 0 &&
            row - i + 4 < gradeSize &&
            chessMap[row - i][col] == chessMap[row - i + 1][col] &&
            chessMap[row - i][col] == chessMap[row - i + 2][col] &&
            chessMap[row - i][col] == chessMap[row - i + 3][col] &&
            chessMap[row - i][col] == chessMap[row - i + 4][col])
            return true;
    }

    // “/"方向
    for (int i = 0; i < 5; i++)
    {
        if (row + i < gradeSize &&
            row + i - 4 >= 0 &&
            col - i >= 0 &&
            col - i + 4 < gradeSize &&
            // 第[row+i]行，第[col-i]的棋子，与右上方连续4个棋子都相同
            chessMap[row + i][col - i] == chessMap[row + i - 1][col - i + 1] &&
            chessMap[row + i][col - i] == chessMap[row + i - 2][col - i + 2] &&
            chessMap[row + i][col - i] == chessMap[row + i - 3][col - i + 3] &&
            chessMap[row + i][col - i] == chessMap[row + i - 4][col - i + 4])
            return true;
    }

    // “\“ 方向
    for (int i = 0; i < 5; i++)
    {
        // 第[row+i]行，第[col-i]的棋子，与右下方连续4个棋子都相同
        if (row - i >= 0 &&
            row - i + 4 < gradeSize &&
            col - i >= 0 &&
            col - i + 4 < gradeSize &&
            chessMap[row - i][col - i] == chessMap[row - i + 1][col - i + 1] &&
            chessMap[row - i][col - i] == chessMap[row - i + 2][col - i + 2] &&
            chessMap[row - i][col - i] == chessMap[row - i + 3][col - i + 3] &&
            chessMap[row - i][col - i] == chessMap[row - i + 4][col - i + 4])
            return true;
    }

    return false;
}