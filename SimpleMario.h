#ifndef SIMPLEMARIO_H
#define SIMPLEMARIO_H

#include <QWidget>
#include <QList>
#include <QRectF>
#include <QSet>

class QTimer;
class QLabel;

class SimpleMario : public QWidget
{
    Q_OBJECT

public:
    explicit SimpleMario(QWidget *parent = nullptr);
    ~SimpleMario();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void gameLoop();

private:
    void restartGame();
    void updatePlayer();
    void checkCollisions();
    void checkCoinCollision();
    void checkEnemyCollision();
    void gameOver();

    static const int CANVAS_WIDTH = 800;
    static const int CANVAS_HEIGHT = 500;
    static const int GROUND_LEVEL = 450;
    static const int PLAYER_SIZE = 40;
    static const float GRAVITY;
    static const float JUMP_POWER;
    static const float MOVE_SPEED;

    QRectF playerRect;
    float velocityX;
    float velocityY;
    bool onGround;

    QList<QRectF> platforms;
    QRectF ground;
    QRectF coin;
    QRectF enemy;
    int enemyDirection;

    QSet<int> pressedKeys;
    bool gameOver_;
    int score;

    QTimer *timer;
    QLabel *scoreLabel;
};

#endif // SIMPLEMARIO_H
