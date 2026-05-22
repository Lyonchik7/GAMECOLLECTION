#ifndef GAME2048_H
#define GAME2048_H

#include <QWidget>
#include <QMap>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

class QLabel;
class QGraphicsOpacityEffect;

class Tile {
public:
    int value;
    int x, y;
    int targetX, targetY;
    bool isNew;
    bool isMerged;
    
    Tile() : value(0), x(0), y(0), targetX(0), targetY(0), isNew(false), isMerged(false) {}
};

class Game2048 : public QWidget
{
    Q_OBJECT

public:
    explicit Game2048(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void restartGame();
    void animateTile(int fromX, int fromY, int toX, int toY);

private:
    void addNewTile();
    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();
    bool canMove();
    void checkGameOver();
    void updateScore(int points);
    void createScoreAnimation(int x, int y, int points);
    void animateTileAppearance(int x, int y);
    void saveBoardState();
    bool compareBoardState();

    static const int SIZE = 4;
    static const int CELL_SIZE = 110;
    static const int PADDING = 15;

    int board[SIZE][SIZE];
    int savedBoard[SIZE][SIZE];
    int score;
    int bestScore;
    bool gameOver;
    bool win;

    QMap<int, QColor> tileColors;
    QLabel *scoreLabel;
    QLabel *bestScoreLabel;
    QLabel *gameStatusLabel;
    QParallelAnimationGroup *animationGroup;
    
    QVector<QPair<QPoint, QPoint>> movingTiles;
};

#endif // GAME2048_H
