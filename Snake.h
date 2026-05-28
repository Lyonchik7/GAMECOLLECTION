#ifndef SNAKE_H
#define SNAKE_H

#include <QWidget>
#include <QTimer>
#include <QPoint>
#include <QPointF>
#include <QVector>
#include <QColor>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QPropertyAnimation>

struct SnakeSegment {
    QPoint gridPos;
    QPointF renderPos;
    double width;
    int direction;
};

class Snake : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(float previewSmooth READ getPreviewSmooth WRITE setPreviewSmooth)

public:
    explicit Snake(QWidget *parent = nullptr);
    ~Snake();

    static constexpr int GRID_WIDTH = 20;
    static constexpr int GRID_HEIGHT = 20;
    static constexpr int CELL_SIZE = 25;
    static constexpr int BUTTON_PANEL_HEIGHT = 60;
    static constexpr int GAME_UPDATE_INTERVAL = 180;
    static constexpr int RENDER_INTERVAL = 16;

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void startGame();
    void restartGame();
    void togglePause();
    void gameLoop();
    void changeSnakeColor(int index);

private:
    void updateUIGeometry();
    void spawnFood();
    void move();
    void applyDirection();
    void updateRenderPositions();
    void checkFoodCollision();
    bool checkCollision();
    void gameOver();

    void drawSnakePreview(QPainter &painter, int centerX, int centerY, double scale, int dir);
    QColor getSnakeGradientStart(int index, bool isHead) const;
    QColor getSnakeGradientEnd(int index, bool isHead) const;

    int getGameAreaY() const { return BUTTON_PANEL_HEIGHT; }
    float getPreviewSmooth() const { return previewSmooth; }
    void setPreviewSmooth(float value) { previewSmooth = value; update(); }

    QVector<SnakeSegment> snake;
    QPoint foodGrid;
    QPointF foodPos;

    int direction;
    int nextDirection;
    int score;

    bool gameRunning;
    bool gameStarted;
    bool isPaused;
    bool pendingMove;

    bool isMoving;
    double animationProgress;
    QVector<QPointF> segmentStartPositions;
    QVector<QPointF> segmentTargetPositions;
    QVector<int> prevDirections;

    // Анимация поедания
    bool isEating;
    float eatAnimProgress;

    QLabel *scoreLabel;
    QLabel *colorLabel;
    QComboBox *colorCombo;
    QPushButton *startBtn;
    QPushButton *pauseBtn;
    QPushButton *restartBtn;

    QTimer *gameTimer;
    QTimer *renderTimer;
    QPropertyAnimation *previewSmoothAnimation;

    int selectedColor;
    int previewDirection;
    float previewSmooth;
    float previewPhase;
};

#endif
