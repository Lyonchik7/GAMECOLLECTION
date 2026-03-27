#include "Snake.h"
#include <QPainter>
#include <QKeyEvent>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>

Snake::Snake(QWidget *parent)
    : QWidget(parent)
    , direction(3)
    , nextDirection(3)
    , score(0)
    , gameRunning(true)
{
    setWindowTitle("Змейка");
    setFixedSize(GRID_WIDTH * CELL_SIZE, GRID_HEIGHT * CELL_SIZE + 50);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    QWidget *topPanel = new QWidget();
    topPanel->setFixedHeight(50);
    QHBoxLayout *panelLayout = new QHBoxLayout(topPanel);

    scoreLabel = new QLabel("Счет: 0");
    scoreLabel->setFont(QFont("Arial", 14));
    panelLayout->addWidget(scoreLabel);

    QPushButton *restartBtn = new QPushButton("Новая игра");
    connect(restartBtn, &QPushButton::clicked, this, &Snake::restartGame);
    panelLayout->addWidget(restartBtn);
    panelLayout->addStretch();

    layout->addWidget(topPanel);

    setFocusPolicy(Qt::StrongFocus);
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Snake::gameLoop);

    restartGame();
}

Snake::~Snake()
{
    timer->stop();
}

void Snake::restartGame()
{
    snake.clear();
    snake.append(QPoint(5, 5));
    snake.append(QPoint(4, 5));
    snake.append(QPoint(3, 5));

    direction = nextDirection = 3;
    score = 0;
    gameRunning = true;
    scoreLabel->setText("Счет: 0");

    spawnFood();
    timer->start(TIMER_INTERVAL);
    update();
}

void Snake::spawnFood()
{
    bool validPosition;
    do {
        validPosition = true;
        food.setX(QRandomGenerator::global()->bounded(GRID_WIDTH));
        food.setY(QRandomGenerator::global()->bounded(GRID_HEIGHT));

        for (const QPoint &p : snake) {
            if (p == food) {
                validPosition = false;
                break;
            }
        }
    } while (!validPosition);
}

void Snake::keyPressEvent(QKeyEvent *event)
{
    if (!gameRunning) return;

    switch (event->key()) {
    case Qt::Key_Up:
        if (direction != 1) nextDirection = 0;
        break;
    case Qt::Key_Down:
        if (direction != 0) nextDirection = 1;
        break;
    case Qt::Key_Left:
        if (direction != 3) nextDirection = 2;
        break;
    case Qt::Key_Right:
        if (direction != 2) nextDirection = 3;
        break;
    }
}

void Snake::gameLoop()
{
    if (!gameRunning) return;

    direction = nextDirection;
    move();
    checkFoodCollision();

    if (checkCollision()) {
        gameOver();
        return;
    }

    update();
}

void Snake::move()
{
    QPoint head = snake.first();
    QPoint newHead = head;

    switch (direction) {
    case 0: newHead.setY(head.y() - 1); break;
    case 1: newHead.setY(head.y() + 1); break;
    case 2: newHead.setX(head.x() - 1); break;
    case 3: newHead.setX(head.x() + 1); break;
    }

    snake.prepend(newHead);
}

void Snake::checkFoodCollision()
{
    if (snake.first() == food) {
        score += 10;
        scoreLabel->setText(QString("Счет: %1").arg(score));
        spawnFood();
    } else {
        snake.removeLast();
    }
}

bool Snake::checkCollision()
{
    QPoint head = snake.first();

    if (head.x() < 0 || head.x() >= GRID_WIDTH ||
        head.y() < 0 || head.y() >= GRID_HEIGHT) {
        return true;
    }

    for (int i = 1; i < snake.size(); ++i) {
        if (head == snake[i]) {
            return true;
        }
    }

    return false;
}

void Snake::gameOver()
{
    gameRunning = false;
    timer->stop();
    QMessageBox::information(this, "Игра окончена",
                             QString("Ваш счет: %1").arg(score));
}

void Snake::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(0, 50, width(), height() - 50, QColor(0, 0, 0));

    for (int i = 0; i < snake.size(); ++i) {
        QPoint p = snake[i];
        QColor color = (i == 0) ? QColor(144, 238, 144) : QColor(0, 255, 0);
        painter.fillRect(p.x() * CELL_SIZE, 50 + p.y() * CELL_SIZE,
                         CELL_SIZE - 1, CELL_SIZE - 1, color);
    }

    painter.setBrush(Qt::red);
    painter.setPen(Qt::darkRed);
    painter.drawEllipse(food.x() * CELL_SIZE, 50 + food.y() * CELL_SIZE,
                        CELL_SIZE - 2, CELL_SIZE - 2);
}
