#pragma once
#include <graphics.h>
#include <vector>

struct ChessPos // 表示落子位置
{
    int row;
    int col;
    ChessPos(int i = 5, int j = 5) : row(i), col(j) {}
};
typedef enum
{
    CHESS_WHITE = -1,
    CHESS_BLACK = 1
} chess_kind_t;
class Chess
{
public:
    Chess(int gradeSize, int marginX, int marginY, float chessSize);
    // 获取棋盘大小（13,15,19）
    int getGradeSize();

    // 初始化：加载棋盘图片，初始化相关数据
    void init();

    // 检查棋局是否结束
    bool checkOver();

    // 检查是否有效点击，是则保存在参数pos中
    bool clickBoard(int x, int y, ChessPos *pos);

    // 在指定位置pos落子kind
    void chessDown(ChessPos *pos, chess_kind_t kind);

    // 获取指定位置的棋子颜色
    int getChessData(ChessPos *pos);
    int getChessData(int row, int col);

private:
    IMAGE chessBlackImg;
    IMAGE chessWhiteImg;

    int gradeSize;                          // 棋盘的大小
    int margin_x, margin_y;                 // 棋盘左右边界
    float chessSize;                        // 棋子的大小
    std::vector<std::vector<int>> chessMap; // 当前棋局的棋子分布数据：-1,0,1

    bool playerFlag; // 1:黑方走，0：白方走

    void updateGameMap(ChessPos *pos);

    bool checkWin();

    ChessPos lastPos; // 最后的落子点
};