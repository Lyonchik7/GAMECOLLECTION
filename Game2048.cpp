#include "Game2048.h"
#include <QPainter>
#include <QKeyEvent>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

Game2048::Game2048(QWidget *parent)
    : QWidget(parent)
    , score(0)
    , gameOver(false)
{
    setWindowTitle("2048");
    setFixedSize(SIZE * CELL_SIZE + PADDING, SIZE * CELL_SIZE + PADDING + 50);
    setFocusPolicy(Qt::StrongFocus);

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

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QHBoxLayout *topLayout = new QHBoxLayout();

    scoreLabel = new QLabel("Счет: 0");
    scoreLabel->setFont(QFont("Arial", 14));
    topLayout->addWidget(scoreLabel);

    QPushButton *restartBtn = new QPushButton("Новая игра");
    connect(restartBtn, &QPushButton::clicked, this, &Game2048::restartGame);
    topLayout->addWidget(restartBtn);
    topLayout->addStretch();

    mainLayout->addLayout(topLayout);
    mainLayout->addSpacing(10);

    restartGame();
}

void Game2048::restartGame()
{
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            board[i][j] = 0;

    score = 0;
    gameOver = false;
    scoreLabel->setText("Счет: 0");

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
}

void Game2048::keyPressEvent(QKeyEvent *event)
{
    if (gameOver) return;

    int oldBoard[SIZE][SIZE];
    memcpy(oldBoard, board, sizeof(board));

    switch (event->key()) {
    case Qt::Key_Left:  moveLeft(); break;
    case Qt::Key_Right: moveRight(); break;
    case Qt::Key_Up:    moveUp(); break;
    case Qt::Key_Down:  moveDown(); break;
    default: return;
    }

    bool changed = false;
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            if (oldBoard[i][j] != board[i][j])
                changed = true;

    if (changed) {
        addNewTile();
        update();
        checkGameOver();
    }
}

void Game2048::moveLeft()
{
    for (int i = 0; i < SIZE; ++i) {
        int writePos = 0;
        for (int j = 0; j < SIZE; ++j) {
            if (board[i][j] != 0) {
                if (writePos != j) {
                    board[i][writePos] = board[i][j];
                    board[i][j] = 0;
                }
                writePos++;
            }
        }

        for (int j = 0; j < SIZE - 1; ++j) {
            if (board[i][j] != 0 && board[i][j] == board[i][j + 1]) {
                board[i][j] *= 2;
                score += board[i][j];
                board[i][j + 1] = 0;
                for (int k = j + 1; k < SIZE - 1; ++k) {
                    board[i][k] = board[i][k + 1];
                }
                board[i][SIZE - 1] = 0;
            }
        }
    }
    scoreLabel->setText(QString("Счет: %1").arg(score));
}

void Game2048::moveRight()
{
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE / 2; ++j) {
            std::swap(board[i][j], board[i][SIZE - 1 - j]);
        }
    }

    moveLeft();

    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE / 2; ++j) {
            std::swap(board[i][j], board[i][SIZE - 1 - j]);
        }
    }
}

void Game2048::moveUp()
{
    int temp[SIZE][SIZE];
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            temp[i][j] = board[j][i];

    memcpy(board, temp, sizeof(board));
    moveLeft();

    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            temp[i][j] = board[j][i];

    memcpy(board, temp, sizeof(board));
}

void Game2048::moveDown()
{
    int temp[SIZE][SIZE];
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            temp[i][j] = board[j][i];

    memcpy(board, temp, sizeof(board));
    moveRight();

    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            temp[i][j] = board[j][i];

    memcpy(board, temp, sizeof(board));
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
    if (!canMove()) {
        gameOver = true;
        QMessageBox::information(this, "Игра окончена",
                                 QString("Ваш счет: %1").arg(score));
    }

    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            if (board[i][j] == 2048) {
                QMessageBox::information(this, "Победа!", "Вы собрали 2048!");
                return;
            }
}

void Game2048::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(0, 50, width(), height() - 50, QColor(187, 173, 160));

    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            int value = board[i][j];
            QColor color = tileColors.value(value, QColor(60, 58, 50));

            int x = j * CELL_SIZE + PADDING;
            int y = 50 + i * CELL_SIZE + PADDING;

            painter.fillRect(x, y, CELL_SIZE - PADDING, CELL_SIZE - PADDING, color);

            if (value != 0) {
                QColor textColor = (value <= 4) ? QColor(119, 110, 101) : QColor(249, 246, 242);
                painter.setPen(textColor);

                int fontSize = (value < 100) ? 36 : (value < 1000) ? 24 : 18;
                QFont font("Arial", fontSize, QFont::Bold);
                painter.setFont(font);

                painter.drawText(x, y, CELL_SIZE - PADDING, CELL_SIZE - PADDING,
                                 Qt::AlignCenter, QString::number(value));
            }
        }
    }
}
