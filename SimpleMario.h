#ifndef SIMPLEMARIO_H
#define SIMPLEMARIO_H

#include <QWidget>
#include <QList>
#include <QRectF>
#include <QSet>

class QTimer;
class QLabel;
class QFrame;
class QPushButton;

class SimpleMario : public QWidget
{
    Q_OBJECT

public:
    explicit SimpleMario(int startLevel = 1, QWidget *parent = nullptr);
    ~SimpleMario();

    void startGame();
    void stopGame();

signals:
    void gameFinished();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void gameLoop();
    void updateAnimation();
    void showPauseMenu();
    void hidePauseMenu();
    void restartLevel();
    void restartGame();
    void quitToMainMenu();
    void showDeathMenu();
    void showVictoryMenu();

private:
    void loadLevel(int level);
    void nextLevel();
    void killPlayer();
    void stompEnemy(int enemyIndex);
    void updatePlayer();
    void checkHorizontalCollisions();
    void checkVerticalCollisions();
    void checkCoinCollision();
    void checkEnemyCollision();
    void checkFlagCollision();
    void addDust(float x, float y);
    void addExplosion(float x, float y);
    void addCoinSparkle(float x, float y);
    void changeCharacter();

    void drawMario(QPainter &painter, const QRectF &rect);
    void drawGoomba(QPainter &painter, const QRectF &rect);
    void drawSnail(QPainter &painter, const QRectF &rect);
    void drawFlag(QPainter &painter, const QRectF &rect);
    void drawClouds(QPainter &painter);
    void drawCoin(QPainter &painter, const QRectF &rect, int frame);
    void drawStars(QPainter &painter);
    void drawBushes(QPainter &painter);
    void drawHills(QPainter &painter);
    void drawWoodenPlatform(QPainter &painter, const QRectF &rect);
    void drawPipe(QPainter &painter, const QRectF &rect);
    void drawTorch(QPainter &painter, const QPointF &pos);
    void drawLava(QPainter &painter, const QRectF &rect);

    static const int CANVAS_WIDTH = 800;
    static const int CANVAS_HEIGHT = 500;
    static const int GROUND_Y = 450;
    static const int PLAYER_SIZE = 40;
    static const float GRAVITY;
    static const float JUMP_POWER;
    static const float MOVE_SPEED;

    int currentLevel;
    int lives;
    int score;
    int coinsCollected;
    bool gameActive;
    bool levelComplete;
    bool victoryShown;
    float respawnInvincibility;
    float starInvincibility;
    int cloudOffset;
    int coinAnimFrame;
    int starTwinkle;
    float flagWave;
    bool showLevelNotify;
    int notifyAlpha;
    QTimer *notifyTimer;
    QTimer *starInvTimer;

    int characterColor;

    QRectF playerRect;
    float velocityX, velocityY;
    bool onGround;
    int walkFrame;
    QTimer *animationTimer;
    QTimer *cloudTimer;
    QTimer *coinAnimTimer;
    QTimer *starTimer;
    QTimer *flagTimer;

    struct Particle {
        float x, y, vx, vy, life;
        int type;
    };
    QList<Particle> particles;

    QRectF ground;
    QList<QRectF> platforms;
    QList<QRectF> coins;
    QList<QRectF> enemies;
    QList<int> enemyDirs;
    QList<int> enemyType;     // 0 - Goomba, 1 - Snail
    QRectF flagPole;
    QList<QRectF> pipes;
    QList<QPointF> torches;
    QRectF lavaZone;
    QList<QRectF> movingPlatforms;
    QList<int> movingPlatformDirs;

    QSet<int> pressedKeys;

    QFrame *topPanel;
    QLabel *scoreLabel;
    QLabel *livesLabel;
    QLabel *levelLabel;
    QTimer *gameTimer;

    QWidget *pauseOverlay;
    QWidget *deathOverlay;
    QWidget *victoryOverlay;
    QPushButton *resumeBtn, *restartLevelBtn, *restartGameBtn, *quitBtn;
    QPushButton *deathRestartLevelBtn, *deathRestartGameBtn, *deathQuitBtn;
    QPushButton *victoryRestartBtn, *victoryQuitBtn;
};

#endif // SIMPLEMARIO_H
