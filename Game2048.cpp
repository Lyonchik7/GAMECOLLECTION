#include "Game2048.h"
#include <QPainter>
#include <QKeyEvent>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QTimer>
#include <QFontDatabase>

Game2048::Game2048(QWidget *parent)
    : QWidget(parent)
    , score(0)
    , bestScore(0)
    , gameOver(false)
    , win(false)
{
    setWindowTitle("2048");
    setFixedSize(SIZE * CELL_SIZE + PADDING * 2, SIZE * CELL_SIZE + PADDING * 2 + 120);
    setFocusPolicy(Qt::StrongFocus);
    
    // Load custom font
    QFontDatabase::addApplicationFont(":/fonts/Roboto-Bold.ttf");
    
    // Modern color scheme
    tileColors[0] = QColor(205, 193, 180);
    tileColors[2] = QColor(238, 228, 218);
    tileColors[4] = QColor(237, 224, 200);
    tileColors[8] = QColor(242, 177, 121);
    tileColors[16] = QColor(245, 149, 99);
    tileColors[32] = QColor(246, 124, 95);
    tileColors[64] = QColor(246, 94, 59);
    tileColors[128] = QColor(237, 207, 114);
    tileColors[256] = QColor(237, 204, 97);
    tileColors[512] = QColor(237, 200, 80);
    tileColors[1024] = QColor(237, 197, 63);
    tileColors[2048] = QColor(237, 194, 46);
    tileColors[4096] = QColor(242, 177, 121);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);
    
    // Header with score and best score
    QHBoxLayout *headerLayout = new QHBoxLayout();
    
    QWidget *titleWidget = new QWidget();
    titleWidget->setFixedSize(120, 80);
    QLabel *titleLabel = new QLabel("2048", titleWidget);
    titleLabel->setFont(QFont("Arial", 32, QFont::Bold));
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: #776e65;");
    titleLabel->setGeometry(0, 0, 120, 80);
    
    headerLayout->addWidget(titleWidget);
    headerLayout->addStretch();
    
    // Score panel
    QWidget *scorePanel = new QWidget();
    scorePanel->setStyleSheet("background-color: #bbada0; border-radius: 10px;");
    QHBoxLayout *scoreLayout = new QHBoxLayout(scorePanel);
    
    QWidget *scoreWidget = new QWidget();
    QVBoxLayout *scoreWidgetLayout = new QVBoxLayout(scoreWidget);
    QLabel *scoreTitle = new QLabel("SCORE");
    scoreTitle->setAlignment(Qt::AlignCenter);
    scoreTitle->setStyleSheet("color: #eee4da; font-size: 13px; font-weight: bold;");
    scoreLabel = new QLabel("0");
    scoreLabel->setAlignment(Qt::AlignCenter);
    scoreLabel->setFont(QFont("Arial", 24, QFont::Bold));
    scoreLabel->setStyleSheet("color: white;");
    scoreWidgetLayout->addWidget(scoreTitle);
    scoreWidgetLayout->addWidget(scoreLabel);
    
    QWidget *bestWidget = new QWidget();
    QVBoxLayout *bestWidgetLayout = new QVBoxLayout(bestWidget);
    QLabel *bestTitle = new QLabel("BEST");
    bestTitle->setAlignment(Qt::AlignCenter);
    bestTitle->setStyleSheet("color: #eee4da; font-size: 13px; font-weight: bold;");
    bestScoreLabel = new QLabel("0");
    bestScoreLabel->setAlignment(Qt::AlignCenter);
    bestScoreLabel->setFont(QFont("Arial", 24, QFont::Bold));
    bestScoreLabel->setStyleSheet("color: white;");
    bestWidgetLayout->addWidget(bestTitle);
    bestWidgetLayout->addWidget(bestScoreLabel);
    
    scoreLayout->addWidget(scoreWidget);
    scoreLayout->addWidget(bestWidget);
    headerLayout->addWidget(scorePanel);
    
    mainLayout->addLayout(headerLayout);
    
    // Game status label
    gameStatusLabel = new QLabel();
    gameStatusLabel->setAlignment(Qt::AlignCenter);
    gameStatusLabel->setFont(QFont("Arial", 16, QFont::Bold));
    gameStatusLabel->setStyleSheet("color: #776e65;");
    gameStatusLabel->hide();
    mainLayout->addWidget(gameStatusLabel);
    
    // New game button
    QPushButton *restartBtn = new QPushButton("Новая игра");
    restartBtn->setFont(QFont("Arial", 12, QFont::Bold));
    restartBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #8f7a66;"
        "  color: white;"
        "  border: none;"
        "  border-radius: 5px;"
        "  padding: 10px 20px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #9f8a76;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #7f6a56;"
        "}"
    );
    connect(restartBtn, &QPushButton::clicked, this, &Game2048::restartGame);
    mainLayout->addWidget(restartBtn, 0, Qt::AlignCenter);
    
    animationGroup = new QParallelAnimationGroup(this);
    
    // Load best score from settings
    bestScore = 0; // You can load from QSettings here
    
    restartGame();
}

void Game2048::restartGame()
{
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            board[i][j] = 0;
    
    score = 0;
    gameOver = false;
    win = false;
    scoreLabel->setText("0");
    gameStatusLabel->hide();
    
    addNewTile();
    addNewTile();
    update();
}

void Game2048::addNewTile()
{
    QVector<QPair<int, int>> emptyCells;
    
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            if (board[i][j] == 0)
                emptyCells.append(qMakePair(i, j));
    
    if (emptyCells.isEmpty()) return;
    
    int index = QRandomGenerator::global()->bounded(emptyCells.size());
    int row = emptyCells[index].first;
    int col = emptyCells[index].second;
    
    board[row][col] = (QRandomGenerator::global()->bounded(10) < 9) ? 2 : 4;
    
    // Animate new tile
    animateTileAppearance(row, col);
}

void Game2048::animateTileAppearance(int x, int y)
{
    // Visual feedback for new tile (scale animation would be ideal, but we'll repaint)
    QTimer::singleShot(50, this, [this]() { update(); });
}

void Game2048::createScoreAnimation(int x, int y, int points)
{
    // Floating score animation
    QLabel *floatingScore = new QLabel(QString("+%1").arg(points), this);
    floatingScore->setFont(QFont("Arial", 18, QFont::Bold));
    floatingScore->setStyleSheet("color: #f9f6f2; background: transparent;");
    floatingScore->setAlignment(Qt::AlignCenter);
    
    int globalX = y * CELL_SIZE + PADDING + CELL_SIZE/2 - 20;
    int globalY = 120 + x * CELL_SIZE + PADDING + CELL_SIZE/2 - 20;
    floatingScore->setGeometry(globalX, globalY, 40, 40);
    floatingScore->show();
    
    QPropertyAnimation *animation = new QPropertyAnimation(floatingScore, "geometry");
    animation->setDuration(800);
    animation->setStartValue(QRect(globalX, globalY, 40, 40));
    animation->setEndValue(QRect(globalX, globalY - 50, 40, 40));
    
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect();
    floatingScore->setGraphicsEffect(effect);
    QPropertyAnimation *fadeOut = new QPropertyAnimation(effect, "opacity");
    fadeOut->setDuration(800);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.0);
    
    QParallelAnimationGroup *group = new QParallelAnimationGroup();
    group->addAnimation(animation);
    group->addAnimation(fadeOut);
    
    connect(group, &QParallelAnimationGroup::finished, floatingScore, &QLabel::deleteLater);
    group->start();
}

void Game2048::updateScore(int points)
{
    score += points;
    scoreLabel->setText(QString::number(score));
    
    if (score > bestScore) {
        bestScore = score;
        bestScoreLabel->setText(QString::number(bestScore));
    }
}

void Game2048::saveBoardState()
{
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            savedBoard[i][j] = board[i][j];
}

bool Game2048::compareBoardState()
{
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            if (savedBoard[i][j] != board[i][j])
                return false;
    return true;
}

void Game2048::keyPressEvent(QKeyEvent *event)
{
    if (gameOver) return;
    
    saveBoardState();
    
    switch (event->key()) {
    case Qt::Key_Left:  moveLeft(); break;
    case Qt::Key_Right: moveRight(); break;
    case Qt::Key_Up:    moveUp(); break;
    case Qt::Key_Down:  moveDown(); break;
    default: return;
    }
    
    if (!compareBoardState()) {
        addNewTile();
        update();
        checkGameOver();
    }
}

void Game2048::moveLeft()
{
    for (int i = 0; i < SIZE; ++i) {
        QVector<int> newRow;
        for (int j = 0; j < SIZE; ++j) {
            if (board[i][j] != 0)
                newRow.append(board[i][j]);
        }
        
        for (int j = 0; j < newRow.size() - 1; ++j) {
            if (newRow[j] == newRow[j + 1]) {
                newRow[j] *= 2;
                updateScore(newRow[j]);
                createScoreAnimation(i, j, newRow[j]);
                newRow.remove(j + 1);
            }
        }
        
        while (newRow.size() < SIZE)
            newRow.append(0);
        
        for (int j = 0; j < SIZE; ++j)
            board[i][j] = newRow[j];
    }
}

void Game2048::moveRight()
{
    for (int i = 0; i < SIZE; ++i) {
        QVector<int> newRow;
        for (int j = SIZE - 1; j >= 0; --j) {
            if (board[i][j] != 0)
                newRow.append(board[i][j]);
        }
        
        for (int j = 0; j < newRow.size() - 1; ++j) {
            if (newRow[j] == newRow[j + 1]) {
                newRow[j] *= 2;
                updateScore(newRow[j]);
                createScoreAnimation(i, SIZE - 1 - j, newRow[j]);
                newRow.remove(j + 1);
            }
        }
        
        while (newRow.size() < SIZE)
            newRow.append(0);
        
        for (int j = 0; j < SIZE; ++j)
            board[i][SIZE - 1 - j] = newRow[j];
    }
}

void Game2048::moveUp()
{
    for (int j = 0; j < SIZE; ++j) {
        QVector<int> newCol;
        for (int i = 0; i < SIZE; ++i) {
            if (board[i][j] != 0)
                newCol.append(board[i][j]);
        }
        
        for (int i = 0; i < newCol.size() - 1; ++i) {
            if (newCol[i] == newCol[i + 1]) {
                newCol[i] *= 2;
                updateScore(newCol[i]);
                createScoreAnimation(i, j, newCol[i]);
                newCol.remove(i + 1);
            }
        }
        
        while (newCol.size() < SIZE)
            newCol.append(0);
        
        for (int i = 0; i < SIZE; ++i)
            board[i][j] = newCol[i];
    }
}

void Game2048::moveDown()
{
    for (int j = 0; j < SIZE; ++j) {
        QVector<int> newCol;
        for (int i = SIZE - 1; i >= 0; --i) {
            if (board[i][j] != 0)
                newCol.append(board[i][j]);
        }
        
        for (int i = 0; i < newCol.size() - 1; ++i) {
            if (newCol[i] == newCol[i + 1]) {
                newCol[i] *= 2;
                updateScore(newCol[i]);
                createScoreAnimation(SIZE - 1 - i, j, newCol[i]);
                newCol.remove(i + 1);
            }
        }
        
        while (newCol.size() < SIZE)
            newCol.append(0);
        
        for (int i = 0; i < SIZE; ++i)
            board[SIZE - 1 - i][j] = newCol[i];
    }
}

bool Game2048::canMove()
{
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            if (board[i][j] == 0) return true;
    
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE - 1; ++j)
            if (board[i][j] == board[i][j + 1]) return true;
    
    for (int i = 0; i < SIZE - 1; ++i)
        for (int j = 0; j < SIZE; ++j)
            if (board[i][j] == board[i + 1][j]) return true;
    
    return false;
}

void Game2048::checkGameOver()
{
    if (!canMove() && !win) {
        gameOver = true;
        gameStatusLabel->setText("GAME OVER!");
        gameStatusLabel->setStyleSheet("color: #ed1c24; font-size: 20px; font-weight: bold;");
        gameStatusLabel->show();
        update();
    }
    
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            if (board[i][j] == 2048 && !win) {
                win = true;
                gameStatusLabel->setText("YOU WIN!");
                gameStatusLabel->setStyleSheet("color: #4caf50; font-size: 20px; font-weight: bold;");
                gameStatusLabel->show();
                return;
            }
        }
    }
}

void Game2048::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Background
    painter.fillRect(0, 0, width(), height(), QColor(250, 248, 239));
    
    // Game board
    int boardX = PADDING;
    int boardY = 120;
    int boardSize = SIZE * CELL_SIZE;
    painter.fillRect(boardX, boardY, boardSize, boardSize, QColor(187, 173, 160));
    
    // Draw grid and tiles
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            int value = board[i][j];
            QColor color = tileColors.value(value, QColor(60, 58, 50));
            
            int x = j * CELL_SIZE + PADDING;
            int y = boardY + i * CELL_SIZE;
            
            // Rounded corners effect
            painter.setPen(Qt::NoPen);
            painter.setBrush(color);
            painter.drawRoundedRect(x, y, CELL_SIZE - PADDING, CELL_SIZE - PADDING, 8, 8);
            
            if (value != 0) {
                // Text color based on tile value
                QColor textColor = (value <= 4) ? QColor(119, 110, 101) : QColor(249, 246, 242);
                painter.setPen(textColor);
                
                // Dynamic font size
                int fontSize;
                if (value < 100) fontSize = 40;
                else if (value < 1000) fontSize = 32;
                else if (value < 10000) fontSize = 24;
                else fontSize = 20;
                
                QFont font("Arial", fontSize, QFont::Bold);
                painter.setFont(font);
                
                // Draw shadow for better readability
                painter.setPen(QColor(0, 0, 0, 30));
                painter.drawText(x + 2, y + 2, CELL_SIZE - PADDING, CELL_SIZE - PADDING,
                                 Qt::AlignCenter, QString::number(value));
                
                painter.setPen(textColor);
                painter.drawText(x, y, CELL_SIZE - PADDING, CELL_SIZE - PADDING,
                                 Qt::AlignCenter, QString::number(value));
            }
        }
    }
    
    // Draw game over overlay
    if (gameOver) {
        painter.fillRect(boardX, boardY, boardSize, boardSize, QColor(0, 0, 0, 180));
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 28, QFont::Bold));
        painter.drawText(boardX, boardY, boardSize, boardSize, Qt::AlignCenter, "GAME OVER");
    } else if (win && !gameOver) {
        painter.fillRect(boardX, boardY, boardSize, boardSize, QColor(0, 0, 0, 180));
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 28, QFont::Bold));
        painter.drawText(boardX, boardY, boardSize, boardSize, Qt::AlignCenter, "YOU WIN!");
    }
}

void Game2048::animateTile(int fromX, int fromY, int toX, int toY)
{
    // Placeholder for future animation implementation
    update();
}
