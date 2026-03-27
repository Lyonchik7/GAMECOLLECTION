#include "SimpleMario.h"
#include <QPainter>
#include <QKeyEvent>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>

const float SimpleMario::GRAVITY = 0.8f;
const float SimpleMario::JUMP_POWER = -15.0f;
const float SimpleMario::MOVE_SPEED = 7.0f;

SimpleMario::SimpleMario(QWidget *parent)
    : QWidget(parent)
    , velocityX(0)
    , velocityY(0)
    , onGround(false)
    , enemyDirection(1)
    , gameOver_(false)
    , score(0)
{
    setWindowTitle("Супер Марио (упрощенная версия)");
    setFixedSize(CANVAS_WIDTH, CANVAS_HEIGHT + 50);
    setFocusPolicy(Qt::StrongFocus);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *topLayout = new QHBoxLayout();

    scoreLabel = new QLabel("Счет: 0");
    scoreLabel->setFont(QFont("Arial", 14));
    topLayout->addWidget(scoreLabel);

    QPushButton *restartBtn = new QPushButton("Новая игра");
    connect(restartBtn, &QPushButton::clicked, this, &SimpleMario::restartGame);
    topLayout->addWidget(restartBtn);
    topLayout->addStretch();

    mainLayout->addLayout(topLayout);

    QLabel *instruction = new QLabel("← → движение, Пробел - прыжок. Собери монету, избегай врага!");
    instruction->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(instruction);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &SimpleMario::gameLoop);

    restartGame();
}

SimpleMario::~SimpleMario()
{
    timer->stop();
}

void SimpleMario::restartGame()
{
    playerRect = QRectF(100, GROUND_LEVEL - PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE);
    velocityX = 0;
    velocityY = 0;
    onGround = false;

    ground = QRectF(0, GROUND_LEVEL, CANVAS_WIDTH, CANVAS_HEIGHT - GROUND_LEVEL);

    platforms.clear();
    platforms.append(QRectF(300, 400, 150, 20));
    platforms.append(QRectF(500, 350, 150, 20));
    platforms.append(QRectF(200, 300, 100, 20));

    coin = QRectF(600, 300, 40, 40);
    enemy = QRectF(400, 410, 40, 40);
    enemyDirection = 1;

    pressedKeys.clear();
    gameOver_ = false;
    score = 0;
    scoreLabel->setText("Счет: 0");

    timer->start(30);
    update();
}

void SimpleMario::keyPressEvent(QKeyEvent *event)
{
    if (gameOver_) return;
    pressedKeys.insert(event->key());
}

void SimpleMario::keyReleaseEvent(QKeyEvent *event)
{
    pressedKeys.remove(event->key());
}

void SimpleMario::gameLoop()
{
    if (gameOver_) return;

    updatePlayer();
    checkEnemyCollision();
    checkCoinCollision();

    update();
}

void SimpleMario::updatePlayer()
{
    velocityX = 0;
    if (pressedKeys.contains(Qt::Key_Left)) velocityX = -MOVE_SPEED;
    if (pressedKeys.contains(Qt::Key_Right)) velocityX = MOVE_SPEED;

    if (pressedKeys.contains(Qt::Key_Space) && onGround) {
        velocityY = JUMP_POWER;
        onGround = false;
    }

    velocityY += GRAVITY;

    playerRect.translate(velocityX, 0);
    checkCollisions();

    playerRect.translate(0, velocityY);
    checkCollisions();

    if (playerRect.left() < 0) playerRect.moveLeft(0);
    if (playerRect.right() > CANVAS_WIDTH) playerRect.moveRight(CANVAS_WIDTH);

    if (playerRect.top() > CANVAS_HEIGHT + 100) {
        gameOver();
    }

    enemy.translate(enemyDirection * 2, 0);
    if (enemy.left() < 200 || enemy.right() > 700) {
        enemyDirection *= -1;
    }
}

void SimpleMario::checkCollisions()
{
    onGround = false;

    if (playerRect.intersects(ground)) {
        if (velocityY >= 0 && playerRect.bottom() > ground.top()) {
            playerRect.moveBottom(ground.top());
            velocityY = 0;
            onGround = true;
        }
    }

    for (const QRectF &platform : platforms) {
        if (playerRect.intersects(platform)) {
            if (velocityY >= 0 && playerRect.bottom() > platform.top() &&
                playerRect.top() < platform.top()) {
                playerRect.moveBottom(platform.top());
                velocityY = 0;
                onGround = true;
            }
            else if (velocityY < 0 && playerRect.top() < platform.bottom() &&
                     playerRect.bottom() > platform.bottom()) {
                playerRect.moveTop(platform.bottom());
                velocityY = 0;
            }
            else if (velocityX > 0) {
                playerRect.moveRight(platform.left());
            } else if (velocityX < 0) {
                playerRect.moveLeft(platform.right());
            }
        }
    }
}

void SimpleMario::checkCoinCollision()
{
    if (playerRect.intersects(coin)) {
        score += 10;
        scoreLabel->setText(QString("Счет: %1").arg(score));
        coin.moveTo(QRandomGenerator::global()->bounded(200, 700), 300);
        update();
    }
}

void SimpleMario::checkEnemyCollision()
{
    if (playerRect.intersects(enemy)) {
        gameOver();
    }
}

void SimpleMario::gameOver()
{
    gameOver_ = true;
    timer->stop();
    QMessageBox::information(this, "Игра окончена",
                             QString("Ваш счет: %1").arg(score));
}

void SimpleMario::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(0, 50, CANVAS_WIDTH, CANVAS_HEIGHT - 50, QColor(135, 206, 235));

    painter.fillRect(ground.translated(0, 50), QColor(139, 69, 19));

    painter.setBrush(Qt::green);
    for (const QRectF &platform : platforms) {
        painter.fillRect(platform.translated(0, 50), Qt::green);
    }

    painter.setBrush(Qt::red);
    painter.fillRect(playerRect.translated(0, 50), Qt::red);

    painter.setBrush(Qt::yellow);
    painter.setPen(Qt::darkYellow);
    painter.drawEllipse(coin.translated(0, 50));

    painter.setBrush(QColorConstants::Svg::purple);
    painter.fillRect(enemy.translated(0, 50), QColorConstants::Svg::purple);
}
