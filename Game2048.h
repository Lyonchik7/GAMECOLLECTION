#ifndef GAME2048_H
#define GAME2048_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVector>
#include <QMap>

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
    void showHighScores();

private:
    void addNewTile();

    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();

    bool canMove();
    void checkGameState();

    void updateScore(int points);

    void saveData();
    void loadData();

private:
    static const int SIZE = 4;

    int board[SIZE][SIZE];

    int score;
    int bestScore;

    bool gameOver;
    bool win;

    QVector<int> highScores;

    QLabel *scoreLabel;
    QLabel *bestScoreLabel;
    QLabel *statusLabel;

    QPushButton *restartButton;
    QPushButton *recordsButton;

    QMap<int, QColor> tileColors;
};

#endif // GAME2048_H
