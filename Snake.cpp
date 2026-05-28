#include "Snake.h"
#include <QPainter>
#include <QKeyEvent>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QPainterPath>
#include <cmath>
#include <QFont>      
#include <QRect>      
#include <cmath>

Snake::Snake(QWidget *parent)
    : QWidget(parent)
    , direction(3)
    , nextDirection(3)
    , score(0)
    , gameRunning(false)
    , gameStarted(false)
    , isPaused(false)
    , pendingMove(false)
    , animationProgress(1.0)
    , isMoving(false)
    , selectedColor(0)
    , previewDirection(3)
    , previewSmooth(0.0f)
    , previewPhase(0.0f)
    , isEating(false)          // ← Инициализация
    , eatAnimProgress(0.0f)    // ← Инициализация
{
    setWindowTitle("Snake Game");
    setFixedSize(GRID_WIDTH * CELL_SIZE,
                 GRID_HEIGHT * CELL_SIZE + BUTTON_PANEL_HEIGHT);

    setAttribute(Qt::WA_TranslucentBackground, false);
    setFocusPolicy(Qt::StrongFocus);

    colorLabel = new QLabel("🎨 COLOR:", this);
    colorLabel->setFont(QFont("Segoe UI", 11, QFont::Bold));
    colorLabel->setAlignment(Qt::AlignCenter);
    colorLabel->setStyleSheet(
        "QLabel {"
        "background:#1f2229;"
        "color:#6ee7b7;"
        "border:2px solid #2c313c;"
        "border-radius:19px;"
        "padding:5px;"
        "}"
        );

    colorCombo = new QComboBox(this);
    colorCombo->addItem("  🐍 GREEN");
    colorCombo->addItem("  💙 BLUE");
    colorCombo->addItem("  ❤️ RED");
    colorCombo->addItem("  🧡 ORANGE");
    colorCombo->addItem("  🤎 BROWN");
    colorCombo->setFont(QFont("Segoe UI", 11, QFont::Bold));
    colorCombo->setStyleSheet(
        "QComboBox {"
        "background:#1f2229;"
        "color:#6ee7b7;"
        "border:2px solid #2c313c;"
        "border-radius:19px;"
        "padding:8px 12px;"
        "font-weight: bold;"
        "min-width: 200px;"
        "}"
        "QComboBox:hover {"
        "border-color:#6ee7b7;"
        "background:#2c313c;"
        "}"
        "QComboBox::drop-down {"
        "border: none;"
        "width: 35px;"
        "}"
        "QComboBox::down-arrow {"
        "image: none;"
        "border-left: 6px solid transparent;"
        "border-right: 6px solid transparent;"
        "border-top: 6px solid #6ee7b7;"
        "margin-right: 12px;"
        "}"
        "QComboBox QAbstractItemView {"
        "background:#1f2229;"
        "color:#6ee7b7;"
        "border:2px solid #2c313c;"
        "selection-background-color:#2c313c;"
        "selection-color:#6ee7b7;"
        "padding: 5px;"
        "min-width: 220px;"
        "}"
        "QComboBox QAbstractItemView::item {"
        "padding: 10px 15px;"
        "}"
        "QComboBox QAbstractItemView::item:hover {"
        "background:#2c313c;"
        "}"
        );
    connect(colorCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &Snake::changeSnakeColor);

    previewSmoothAnimation = new QPropertyAnimation(this, "previewSmooth");
    previewSmoothAnimation->setDuration(400);
    previewSmoothAnimation->setEasingCurve(QEasingCurve::InOutCubic);

    startBtn = new QPushButton("START GAME", this);
    startBtn->setFont(QFont("Segoe UI", 14, QFont::Bold));
    startBtn->setStyleSheet(
        "QPushButton {"
        "background:#2ecc71;"
        "color:#1f2229;"
        "border: none;"
        "border-radius:25px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background:#27ae60;"
        "}"
        "QPushButton:pressed {"
        "background:#229954;"
        "}"
        );
    connect(startBtn, &QPushButton::clicked, this, &Snake::startGame);

    pauseBtn = new QPushButton("⏸", this);
    pauseBtn->setFont(QFont("Segoe UI", 16, QFont::Bold));
    pauseBtn->setStyleSheet(
        "QPushButton {"
        "background:#1f2229;"
        "color:#6ee7b7;"
        "border:2px solid #2c313c;"
        "border-radius:19px;"
        "}"
        "QPushButton:hover {"
        "background:#2c313c;"
        "border-color:#6ee7b7;"
        "}"
        );
    pauseBtn->setEnabled(false);
    pauseBtn->hide(); // Скрыто по умолчанию
    connect(pauseBtn, &QPushButton::clicked, this, &Snake::togglePause);

    restartBtn = new QPushButton("⟳", this);
    restartBtn->setFont(QFont("Segoe UI", 16, QFont::Bold));
    restartBtn->setStyleSheet(
        "QPushButton {"
        "background:#1f2229;"
        "color:#6ee7b7;"
        "border:2px solid #2c313c;"
        "border-radius:19px;"
        "}"
        "QPushButton:hover {"
        "background:#2c313c;"
        "border-color:#6ee7b7;"
        "}"
        );
    restartBtn->setEnabled(false);
    restartBtn->hide(); // Скрыто по умолчанию
    connect(restartBtn, &QPushButton::clicked, this, &Snake::restartGame);

    scoreLabel = new QLabel("0", this);
    scoreLabel->setAlignment(Qt::AlignCenter);
    scoreLabel->setFont(QFont("Segoe UI", 15, QFont::Bold));
    scoreLabel->setStyleSheet(
        "QLabel {"
        "background:#1f2229;"
        "color:#6ee7b7;"
        "border:2px solid #2c313c;"
        "border-radius:19px;"
        "}"
        );

    updateUIGeometry();

    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &Snake::gameLoop);

    renderTimer = new QTimer(this);
    connect(renderTimer, &QTimer::timeout, this, QOverload<>::of(&Snake::update));
    renderTimer->start(RENDER_INTERVAL);
}

Snake::~Snake()
{
    gameTimer->stop();
    renderTimer->stop();
}

void Snake::updateUIGeometry()
{
    int w = width(); // 500
    int gameY = getGameAreaY();

    constexpr int MARGIN = 14;
    constexpr int SPACING = 8;
    constexpr int BTN_SIZE = 38;
    constexpr int SCORE_W = 74;

    
    constexpr int COLOR_LABEL_W = 100;

    // ← Скорректировано до 185, чтобы уместиться между меткой и правыми кнопками
    constexpr int COLOR_COMBO_W = 185;

    constexpr int TOP_Y = 10;
    constexpr int ITEM_H = 40;

    // Счёт (слева)
    scoreLabel->setGeometry(MARGIN, TOP_Y, SCORE_W, ITEM_H);

    // Блок цвета (центр-лево)
    int colorX = MARGIN + SCORE_W + SPACING;
    colorLabel->setGeometry(colorX, TOP_Y, COLOR_LABEL_W, ITEM_H);
    colorCombo->setGeometry(colorX + COLOR_LABEL_W + 4, TOP_Y, COLOR_COMBO_W, ITEM_H);

    // Кнопки управления (справа)
    int rightX = w - MARGIN;
    restartBtn->setGeometry(rightX - BTN_SIZE, TOP_Y, BTN_SIZE, ITEM_H);
    pauseBtn->setGeometry(rightX - BTN_SIZE - SPACING - BTN_SIZE, TOP_Y, BTN_SIZE, ITEM_H);

    // Кнопка старта (центр игрового поля)
    int gameH = GRID_HEIGHT * CELL_SIZE;
    int btnW = 160;
    int btnH = 50;
    startBtn->setGeometry((w - btnW) / 2, gameY + (gameH - btnH) / 2, btnW, btnH);
}

void Snake::changeSnakeColor(int index)
{
    selectedColor = index;
    previewSmoothAnimation->setStartValue(0.0f);
    previewSmoothAnimation->setKeyValueAt(0.5, 1.0f);
    previewSmoothAnimation->setEndValue(0.0f);
    previewSmoothAnimation->start();
    update();
}

QColor Snake::getSnakeGradientStart(int index, bool isHead) const
{
    switch (index) {
    case 0: return isHead ? QColor(100, 200, 100) : QColor(80, 180, 80);
    case 1: return isHead ? QColor(80, 150, 255) : QColor(60, 130, 220);
    case 2: return isHead ? QColor(255, 80, 80) : QColor(220, 60, 60);
    case 3: return isHead ? QColor(255, 180, 80) : QColor(220, 150, 60);
    case 4: return isHead ? QColor(180, 120, 80) : QColor(150, 100, 60);
    default: return QColor(100, 200, 100);
    }
}

QColor Snake::getSnakeGradientEnd(int index, bool isHead) const
{
    switch (index) {
    case 0: return isHead ? QColor(50, 150, 50) : QColor(40, 130, 40);
    case 1: return isHead ? QColor(40, 100, 200) : QColor(30, 80, 180);
    case 2: return isHead ? QColor(200, 40, 40) : QColor(180, 40, 40);
    case 3: return isHead ? QColor(200, 130, 40) : QColor(180, 110, 30);
    case 4: return isHead ? QColor(130, 80, 40) : QColor(110, 70, 30);
    default: return QColor(50, 150, 50);
    }
}

void Snake::startGame()
{
    gameStarted = true;
    gameRunning = true;
    startBtn->hide();
    colorLabel->hide();
    colorCombo->hide();

    pauseBtn->setEnabled(true);
    pauseBtn->show();
    restartBtn->setEnabled(true);
    restartBtn->show();

    snake.clear();
    prevDirections.clear();
    segmentStartPositions.clear();
    segmentTargetPositions.clear();

    int startX = GRID_WIDTH / 2;
    int startY = GRID_HEIGHT / 2;

    for (int i = 0; i < 5; ++i) {
        SnakeSegment segment;
        segment.gridPos = QPoint(startX - i, startY);
        segment.renderPos = QPointF(segment.gridPos.x() * CELL_SIZE,
                                    segment.gridPos.y() * CELL_SIZE);
        segment.width = 1.0 - (i * 0.1);
        if (segment.width < 0.6) segment.width = 0.6;
        segment.direction = 3;
        snake.append(segment);

        prevDirections.append(3);
        segmentStartPositions.append(segment.renderPos);
        segmentTargetPositions.append(segment.renderPos);
    }

    direction = nextDirection = 3;
    score = 0;
    isPaused = false;
    pendingMove = false;
    pauseBtn->setText("⏸");
    scoreLabel->setText("0");
    animationProgress = 1.0;
    isMoving = false;

    spawnFood();
    gameTimer->start(GAME_UPDATE_INTERVAL);
    update();
}

void Snake::gameOver()
{
    gameRunning = false;
    isPaused = false;
    pendingMove = false;
    gameTimer->stop();

    colorLabel->show();
    colorCombo->show();
    pauseBtn->hide();

    QMessageBox::information(this, "Game Over",
                             QString("Your score: %1\n\nPress Restart to play again").arg(score));
}

void Snake::restartGame()
{
    gameTimer->stop();

    gameStarted = false;
    gameRunning = false;
    isPaused = false;
    pendingMove = false;

    startBtn->show();
    colorLabel->show();
    colorCombo->show();

    pauseBtn->setEnabled(false);
    pauseBtn->hide();

    snake.clear();
    score = 0;
    scoreLabel->setText("0");

    update();
}

void Snake::togglePause()
{
    if (!gameRunning || !gameStarted) return;

    isPaused = !isPaused;

    if (isPaused) {
        pauseBtn->setText("▶");
        gameTimer->stop();
    } else {
        pauseBtn->setText("⏸");
        gameTimer->start(GAME_UPDATE_INTERVAL);
        if (pendingMove && !isMoving) {
            applyDirection();
        }
    }
    update();
}

void Snake::spawnFood()
{
    if (snake.isEmpty()) return;

    bool validPosition;
    do {
        validPosition = true;
        foodGrid.setX(QRandomGenerator::global()->bounded(GRID_WIDTH));
        foodGrid.setY(QRandomGenerator::global()->bounded(GRID_HEIGHT));

        for (const auto& seg : snake) {
            if (seg.gridPos == foodGrid) {
                validPosition = false;
                break;
            }
        }
    } while (!validPosition);

    foodPos = QPointF(foodGrid.x() * CELL_SIZE, foodGrid.y() * CELL_SIZE);
}

void Snake::applyDirection()
{
    if (!gameRunning || isPaused || !gameStarted) return;
    if (isMoving) return;
    if (snake.isEmpty()) return;

    direction = nextDirection;
    pendingMove = false;

    // ===== ПРЕДСКАЗАНИЕ СЛЕДУЮЩЕЙ ПОЗИЦИИ =====
    QPoint head = snake[0].gridPos;
    QPoint nextHead = head;
    switch (direction) {
    case 0: nextHead.setY(head.y() - 1); break;
    case 1: nextHead.setY(head.y() + 1); break;
    case 2: nextHead.setX(head.x() - 1); break;
    case 3: nextHead.setX(head.x() + 1); break;
    }

    // ===== ПРОВЕРКА СТОЛКНОВЕНИЙ ДО ДВИЖЕНИЯ =====
    // Стены
    if (nextHead.x() < 0 || nextHead.x() >= GRID_WIDTH ||
        nextHead.y() < 0 || nextHead.y() >= GRID_HEIGHT) {
        gameOver();
        return;
    }
    // Тело змеи (начинаем с 1, т.к. 0 — это текущая голова)
    for (int i = 1; i < snake.size(); ++i) {
        if (nextHead == snake[i].gridPos) {
            gameOver();
            return;
        }
    }

    // ===== РАННЕЕ ОТКРЫТИЕ РОТА (за 1 шаг до яблока) =====
    if (!isEating && nextHead == foodGrid) {
        isEating = true;
        eatAnimProgress = 0.0f;
    }

    // ===== ПОДГОТОВКА К АНИМАЦИИ =====
    segmentStartPositions.clear();
    segmentTargetPositions.clear();
    prevDirections.clear();

    QVector<QPoint> oldGridPositions;
    for (const auto& s : snake) oldGridPositions.append(s.gridPos);

    // ===== ЛОГИЧЕСКОЕ ДВИЖЕНИЕ (теперь безопасно) =====
    move();
    checkFoodCollision();

    // ===== НАСТРОЙКА АНИМАЦИИ =====
    isMoving = true;
    animationProgress = 0.0;

    segmentStartPositions.reserve(snake.size());
    segmentTargetPositions.reserve(snake.size());

    for (int i = 0; i < snake.size(); ++i) {
        QPointF target(snake[i].gridPos.x() * CELL_SIZE,
                       snake[i].gridPos.y() * CELL_SIZE);
        segmentTargetPositions.append(target);

        if (i == 0) {
            segmentStartPositions.append(QPointF(oldGridPositions[0].x() * CELL_SIZE,
                                                 oldGridPositions[0].y() * CELL_SIZE));
        } else if (i - 1 < oldGridPositions.size()) {
            segmentStartPositions.append(QPointF(oldGridPositions[i-1].x() * CELL_SIZE,
                                                 oldGridPositions[i-1].y() * CELL_SIZE));
        } else {
            segmentStartPositions.append(target);
        }
        prevDirections.append(snake[i].direction);
    }

    update();
}

void Snake::keyPressEvent(QKeyEvent *event)
{
    if (!gameStarted) {
        if (event->key() == Qt::Key_Space || event->key() == Qt::Key_Return) {
            startGame();
        }
        return;
    }

    if (event->key() == Qt::Key_Space || event->key() == Qt::Key_P) {
        togglePause();
        return;
    }

    if (event->key() == Qt::Key_R) {
        restartGame();
        return;
    }

    if (!gameRunning || isPaused) return;

    if (pendingMove) return;

    int newDir = -1;

    switch (event->key()) {
    case Qt::Key_Up:    case Qt::Key_W: newDir = 0; break;
    case Qt::Key_Down:  case Qt::Key_S: newDir = 1; break;
    case Qt::Key_Left:  case Qt::Key_A: newDir = 2; break;
    case Qt::Key_Right: case Qt::Key_D: newDir = 3; break;
    default: return;
    }

    bool isOpposite = false;
    if (newDir == 0 && direction == 1) isOpposite = true;
    if (newDir == 1 && direction == 0) isOpposite = true;
    if (newDir == 2 && direction == 3) isOpposite = true;
    if (newDir == 3 && direction == 2) isOpposite = true;

    if (!isOpposite) {
        nextDirection = newDir;
        pendingMove = true;

        if (!isMoving) {
            applyDirection();
        }
    }
}

void Snake::gameLoop()
{
    if (!gameRunning || isPaused || !gameStarted) return;

    int dynamicInterval = qMax(80, GAME_UPDATE_INTERVAL - (score / 5) * 5);
    gameTimer->setInterval(dynamicInterval);


    if (!pendingMove) {
        nextDirection = direction;
        pendingMove = true;
    }

    if (!isMoving) {
        applyDirection();
    }
}

void Snake::move()
{
    if (snake.isEmpty()) return;

    QPoint head = snake[0].gridPos;
    QPoint newHead = head;

    switch (direction) {
    case 0: newHead.setY(head.y() - 1); break;
    case 1: newHead.setY(head.y() + 1); break;
    case 2: newHead.setX(head.x() - 1); break;
    case 3: newHead.setX(head.x() + 1); break;
    }

    SnakeSegment newSegment;
    newSegment.gridPos = newHead;
    newSegment.renderPos = QPointF(newHead.x() * CELL_SIZE, newHead.y() * CELL_SIZE);
    newSegment.direction = direction;
    newSegment.width = 1.0;

    snake.prepend(newSegment);

    for (int i = 0; i < snake.size(); ++i) {
        double width = 1.0 - (i * 0.08);
        if (width < 0.5) width = 0.5;
        snake[i].width = width;
    }
}

void Snake::updateRenderPositions()
{
    if (!isMoving || snake.isEmpty()) return;

    animationProgress += 0.12;

    if (animationProgress >= 1.0) {
        animationProgress = 1.0;
        isMoving = false;

        for (int i = 0; i < snake.size(); ++i) {
            snake[i].renderPos = QPointF(snake[i].gridPos.x() * CELL_SIZE,
                                         snake[i].gridPos.y() * CELL_SIZE);
        }

        segmentStartPositions.clear();
        segmentTargetPositions.clear();

    } else {
        double t = animationProgress;
        double easeT = t < 0.5 ? 4 * t * t * t : 1 - pow(-2 * t + 2, 3) / 2;

        for (int i = 0; i < snake.size(); ++i) {
            if (i < segmentStartPositions.size() && i < segmentTargetPositions.size()) {
                QPointF startPos = segmentStartPositions[i];
                QPointF targetPos = segmentTargetPositions[i];
                QPointF interpolatedPos = startPos + (targetPos - startPos) * easeT;
                snake[i].renderPos = interpolatedPos;
            } else {
                snake[i].renderPos = QPointF(snake[i].gridPos.x() * CELL_SIZE,
                                             snake[i].gridPos.y() * CELL_SIZE);
            }
        }
    }
}

void Snake::checkFoodCollision()
{
    if (snake.isEmpty()) return;

    if (snake[0].gridPos == foodGrid) {
        score += 1;
        scoreLabel->setText(QString::number(score));
        spawnFood();
        // ← Строки isEating/eatAnimProgress удалены (триггер теперь в applyDirection)
    } else {
        if (!snake.isEmpty()) {
            snake.removeLast();
        }
    }
}

void Snake::drawSnakePreview(QPainter &painter, int centerX, int centerY, double scale, int dir)
{
    int segmentCount = 6;
    double segmentSpacing = 28 * scale;
    double startX = centerX - (segmentCount - 1) * segmentSpacing / 2;

    for (int i = 0; i < segmentCount; ++i) {
        double x = startX + (segmentCount - 1 - i) * segmentSpacing;

        double amplitude = (1.0 - i * 0.15) * 5.0 * scale;
        if (amplitude < 1.5 * scale) amplitude = 1.5 * scale;
        double waveOffset = sin(previewPhase - i * 0.85) * amplitude;
        double y = centerY + waveOffset;

        double radius = 22 * scale;
        if (i == 0) radius = 28 * scale;
        else if (i == 1) radius = 26 * scale;
        else if (i == 2) radius = 24 * scale;
        else radius = 20 * scale;

        QRadialGradient gradient(x - 3 * scale, y - 3 * scale, radius);
        if (i == 0) {
            gradient.setColorAt(0, getSnakeGradientStart(selectedColor, true));
            gradient.setColorAt(1, getSnakeGradientEnd(selectedColor, true));
        } else {
            gradient.setColorAt(0, getSnakeGradientStart(selectedColor, false));
            gradient.setColorAt(1, getSnakeGradientEnd(selectedColor, false));
        }

        painter.setBrush(gradient);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QPointF(x, y), radius, radius);

        if (i == 0) {
            painter.setBrush(QColor(255, 255, 255));
            double eyeRadius = radius * 0.32;
            double eyeOffX = radius * 0.45;
            double eyeOffY = radius * 0.55;

            double pupilShift = sin(previewSmooth * 3.14159) * 2.0 * scale;

            painter.drawEllipse(QPointF(x + eyeOffX, y - eyeOffY), eyeRadius, eyeRadius);
            painter.drawEllipse(QPointF(x + eyeOffX, y + eyeOffY), eyeRadius, eyeRadius);

            painter.setBrush(QColor(0, 0, 0));
            painter.drawEllipse(QPointF(x + eyeOffX + 1.5 * scale + pupilShift, y - eyeOffY),
                                eyeRadius * 0.55, eyeRadius * 0.55);
            painter.drawEllipse(QPointF(x + eyeOffX + 1.5 * scale + pupilShift, y + eyeOffY),
                                eyeRadius * 0.55, eyeRadius * 0.55);

            painter.setBrush(QColor(255, 255, 255));
            painter.drawEllipse(QPointF(x + eyeOffX + 3.0 * scale + pupilShift, y - eyeOffY - 1.5 * scale),
                                eyeRadius * 0.25, eyeRadius * 0.25);
            painter.drawEllipse(QPointF(x + eyeOffX + 3.0 * scale + pupilShift, y + eyeOffY - 1.5 * scale),
                                eyeRadius * 0.25, eyeRadius * 0.25);

            painter.setPen(QPen(QColor(255, 60, 60), 2.5 * scale));
            painter.setBrush(Qt::NoBrush);
            QPainterPath tongueUpper;
            tongueUpper.moveTo(x + radius + 2 * scale, y);
            tongueUpper.quadTo(x + radius + 10 * scale, y - 5 * scale, x + radius + 8 * scale, y - 1.5 * scale);
            painter.drawPath(tongueUpper);

            QPainterPath tongueLower;
            tongueLower.moveTo(x + radius + 2 * scale, y);
            tongueLower.quadTo(x + radius + 10 * scale, y + 5 * scale, x + radius + 8 * scale, y + 1.5 * scale);
            painter.drawPath(tongueLower);
        }
    }
}

void Snake::paintEvent(QPaintEvent *)
{
    // ===== ОБНОВЛЕНИЕ АНИМАЦИЙ =====
    if (!gameStarted) {
        previewPhase += 0.05f;
        if (previewPhase > 6.28318f) previewPhase -= 6.28318f; // Защита от переполнения
    }
    if (isEating) {
        eatAnimProgress += 0.045f;
        if (eatAnimProgress >= 1.0f) {
            eatAnimProgress = 1.0f;
            isEating = false;
        }
    }

    updateRenderPositions();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int gameAreaY = getGameAreaY();
    int gameAreaHeight = GRID_HEIGHT * CELL_SIZE;

    // ===== ФОН =====
    QLinearGradient bg(0, 0, 0, height());
    bg.setColorAt(0, QColor(18, 20, 25));
    bg.setColorAt(1, QColor(10, 12, 16));
    painter.fillRect(rect(), bg);

    // ===== ВЕРХНЯЯ ПАНЕЛЬ И ИГРОВАЯ ОБЛАСТЬ =====
    painter.fillRect(0, 0, width(), BUTTON_PANEL_HEIGHT, QColor(22, 24, 29));
    painter.fillRect(0, gameAreaY, width(), gameAreaHeight, QColor(15, 17, 22));

    // ===== СЕТКА =====
    painter.setPen(QPen(QColor(35, 38, 45), 1));
    for (int x = 0; x <= GRID_WIDTH; ++x)
        painter.drawLine(x * CELL_SIZE, gameAreaY, x * CELL_SIZE, gameAreaY + gameAreaHeight);
    for (int y = 0; y <= GRID_HEIGHT; ++y)
        painter.drawLine(0, gameAreaY + y * CELL_SIZE,
                         static_cast<int>(GRID_WIDTH * CELL_SIZE), gameAreaY + y * CELL_SIZE);

    // ===== ТЕНЬ ПОД ПАНЕЛЬЮ =====
    painter.fillRect(0, gameAreaY, width(), 6, QColor(0, 0, 0, 80));

    // ===== ЯБЛОКО =====
    if (gameStarted) {
        double fx = foodPos.x() + CELL_SIZE / 2.0;
        double fy = foodPos.y() + gameAreaY + CELL_SIZE / 2.0;
        double r  = CELL_SIZE / 2.0 - 2;

        QRadialGradient appleGrad(fx - 3, fy - 3, r);
        appleGrad.setColorAt(0, QColor(255, 120, 120));
        appleGrad.setColorAt(1, QColor(220, 60, 60));
        painter.setBrush(appleGrad);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QPointF(fx, fy), r, r);

        painter.setBrush(QColor(255, 255, 255, 120));
        painter.drawEllipse(QPointF(fx - 3, fy - 3), r * 0.3, r * 0.25);

        painter.setBrush(QColor(80, 180, 80));
        painter.setPen(Qt::NoPen);
        QPainterPath leaf;
        leaf.moveTo(fx + 2, fy - r);
        leaf.quadTo(fx + 8, fy - r - 6, fx + 12, fy - r + 2);
        leaf.quadTo(fx + 6, fy - r - 2, fx + 2, fy - r);
        painter.drawPath(leaf);

        painter.setPen(QPen(QColor(100, 70, 30), 2));
        painter.drawLine(QPointF(fx + 4, fy - r + 2), QPointF(fx + 10, fy - r - 4));
    }

    // ===== ЗМЕЙКА В ИГРЕ =====
    if (!snake.isEmpty() && gameStarted) {
        for (int i = 0; i < snake.size(); ++i) {
            const SnakeSegment& seg = snake[i];
            double x = seg.renderPos.x();
            double y = seg.renderPos.y() + gameAreaY;
            double radius = (CELL_SIZE / 2.0) * seg.width;
            double cx = x + CELL_SIZE / 2.0;
            double cy = y + CELL_SIZE / 2.0;

            QRadialGradient gradient(cx - 2, cy - 2, radius);
            if (i == 0) {
                gradient.setColorAt(0, getSnakeGradientStart(selectedColor, true));
                gradient.setColorAt(1, getSnakeGradientEnd(selectedColor, true));
            } else {
                gradient.setColorAt(0, getSnakeGradientStart(selectedColor, false));
                gradient.setColorAt(1, getSnakeGradientEnd(selectedColor, false));
            }

            painter.setBrush(gradient);
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(QPointF(cx, cy), radius, radius);

            // Голова: глаза, зрачки, блики, рот
            if (i == 0) {
                painter.setBrush(QColor(255, 255, 255));
                double eyeR = radius * 0.28;
                double eyeOff = radius * 0.5;
                double ex = 0, ey = 0;
                if (direction == 3) ex = 1;
                else if (direction == 2) ex = -1;
                else if (direction == 0) ey = -1;
                else if (direction == 1) ey = 1;

                QPointF eye1, eye2;
                if (ex != 0) {
                    eye1 = QPointF(cx + eyeOff * ex, cy - eyeOff);
                    eye2 = QPointF(cx + eyeOff * ex, cy + eyeOff);
                } else {
                    eye1 = QPointF(cx - eyeOff, cy + eyeOff * ey);
                    eye2 = QPointF(cx + eyeOff, cy + eyeOff * ey);
                }

                painter.drawEllipse(eye1, eyeR, eyeR);
                painter.drawEllipse(eye2, eyeR, eyeR);

                painter.setBrush(QColor(0, 0, 0));
                double pShift = 1.5;
                painter.drawEllipse(eye1 + QPointF(ex * pShift, ey * pShift), eyeR * 0.6, eyeR * 0.6);
                painter.drawEllipse(eye2 + QPointF(ex * pShift, ey * pShift), eyeR * 0.6, eyeR * 0.6);

                painter.setBrush(QColor(255, 255, 255));
                painter.drawEllipse(eye1 + QPointF(ex * (pShift + 1), ey * pShift - 0.5), eyeR * 0.3, eyeR * 0.3);
                painter.drawEllipse(eye2 + QPointF(ex * (pShift + 1), ey * pShift - 0.5), eyeR * 0.3, eyeR * 0.3);

                // Рот (анимация поедания)
                if (isEating) {
                    double openAmount = sin(eatAnimProgress * 3.14159);
                    double mouthSize  = openAmount * radius * 1.15;

                    painter.setBrush(QColor(25, 10, 10));
                    painter.setPen(Qt::NoPen);

                    if (direction == 3) {
                        painter.drawEllipse(QPointF(cx + radius * 0.55, cy), mouthSize * 0.35, mouthSize);
                    } else if (direction == 2) {
                        painter.drawEllipse(QPointF(cx - radius * 0.55, cy), mouthSize * 0.35, mouthSize);
                    } else if (direction == 0) {
                        painter.drawEllipse(QPointF(cx, cy - radius * 0.55), mouthSize, mouthSize * 0.35);
                    } else if (direction == 1) {
                        painter.drawEllipse(QPointF(cx, cy + radius * 0.55), mouthSize, mouthSize * 0.35);
                    }
                }
            }
        }
    }

    // ===== ГЛАВНОЕ МЕНЮ (ПРЕВЬЮ) =====
    if (!gameStarted) {
        painter.setPen(QColor(230, 230, 235));
        painter.setFont(QFont("Segoe UI", 28, QFont::Bold));
        painter.drawText(QRect(0, gameAreaY + 130, width(), 50),
                         Qt::AlignCenter, QStringLiteral("S N A K E"));

        painter.setPen(QColor(120, 125, 140));
        painter.setFont(QFont("Segoe UI", 12));
        painter.drawText(QRect(0, gameAreaY + 190, width(), 30),
                         Qt::AlignCenter, QStringLiteral("Premium Edition"));

        drawSnakePreview(painter, width() / 2, gameAreaY + 320, 1.5, previewDirection);

        painter.setPen(QColor(90, 95, 110));
        painter.drawText(QRect(0, gameAreaY + 430, width(), 30),
                         Qt::AlignCenter, QStringLiteral("SPACE • START • ESCAPE THE GRID"));
    }

    // ===== ЭКРАН ПАУЗЫ =====
    if (isPaused && gameRunning && gameStarted) {
        painter.fillRect(0, gameAreaY, width(), gameAreaHeight, QColor(0, 0, 0, 140));
        painter.setPen(QColor(255, 255, 255));
        painter.setFont(QFont("Segoe UI", 28, QFont::Bold));
        painter.drawText(QRect(0, gameAreaY, width(), gameAreaHeight),
                         Qt::AlignCenter, QStringLiteral("PAUSED"));
    }
}
