#ifndef GAME2048_H
#define GAME2048_H

#include <QWidget>
#include <QMap>

class QLabel;

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

private:
    void addNewTile();
    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();
    bool canMove();
    void checkGameOver();

    static const int SIZE = 4;
    static const int CELL_SIZE = 100;
    static const int PADDING = 10;

    int board[SIZE][SIZE];
    int score;
    bool gameOver;

    QMap<int, QColor> tileColors;
    QLabel *scoreLabel;
};

#endif // GAME2048_H
