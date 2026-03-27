#ifndef SNAKE_H
#define SNAKE_H

#include <QWidget>
#include <QList>
#include <QPoint>

class QTimer;
class QLabel;

class Snake : public QWidget
{
    Q_OBJECT

public:
    explicit Snake(QWidget *parent = nullptr);
    ~Snake();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void gameLoop();

private:
    void restartGame();
    void spawnFood();
    void move();
    bool checkCollision();
    void checkFoodCollision();
    void gameOver();

    static const int CELL_SIZE = 20;
    static const int GRID_WIDTH = 30;
    static const int GRID_HEIGHT = 20;
    static const int TIMER_INTERVAL = 150;

    QList<QPoint> snake;
    QPoint food;
    int direction;
    int nextDirection;
    int score;
    bool gameRunning;

    QTimer *timer;
    QLabel *scoreLabel;
};

#endif // SNAKE_H
