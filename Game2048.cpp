#include "Game2048.h"

#include <QPainter>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRandomGenerator>
#include <QSettings>
#include <QDialog>
#include <QListWidget>
#include <algorithm>

Game2048::Game2048(QWidget *parent)
    : QWidget(parent),
    score(0),
    bestScore(0),
    gameOver(false),
    win(false)
{
    setWindowTitle("2048");

    setFixedSize(520, 680);

    setFocusPolicy(Qt::StrongFocus);

    setStyleSheet("background-color: #faf8ef;");

    loadData();

    tileColors[0] = QColor("#cdc1b4");

    tileColors[2] = QColor("#ffadad");
    tileColors[4] = QColor("#ffd6a5");

    tileColors[8] = QColor("#fdffb6");
    tileColors[16] = QColor("#ffe66d");

    tileColors[32] = QColor("#caffbf");
    tileColors[64] = QColor("#70e000");

    tileColors[128] = QColor("#9bf6ff");
    tileColors[256] = QColor("#48cae4");

    tileColors[512] = QColor("#4ea8de");
    tileColors[1024] = QColor("#4361ee");

    tileColors[2048] = QColor("#9d4edd");
    tileColors[4096] = QColor("#c77dff");

    tileColors[8192] = QColor("#ff006e");
    tileColors[16384] = QColor("#fb5607");

    tileColors[32768] = QColor("#3a86ff");
    tileColors[65536] = QColor("#06d6a0");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    mainLayout->setContentsMargins(20, 20, 20, 20);

    mainLayout->setSpacing(15);

    QHBoxLayout *topLayout = new QHBoxLayout();

    QLabel *titleLabel = new QLabel("2048");

    titleLabel->setFont(QFont("Segoe UI", 44, QFont::Bold));

    titleLabel->setStyleSheet("color: #776e65;");

    topLayout->addWidget(titleLabel);

    topLayout->addStretch();

    QWidget *scorePanel = new QWidget();

    scorePanel->setFixedSize(230, 80);

    scorePanel->setStyleSheet(
        "background-color: #bbada0;"
        "border-radius: 12px;"
        );

    QHBoxLayout *scoreLayout = new QHBoxLayout(scorePanel);

    QVBoxLayout *scoreBox = new QVBoxLayout();

    QLabel *scoreText = new QLabel("SCORE");

    scoreText->setAlignment(Qt::AlignCenter);

    scoreText->setStyleSheet(
        "color: #eee4da;"
        "font-weight: bold;"
        "font-size: 14px;"
        );

    scoreLabel = new QLabel("0");

    scoreLabel->setAlignment(Qt::AlignCenter);

    scoreLabel->setFont(QFont("Segoe UI", 22, QFont::Bold));

    scoreLabel->setStyleSheet("color: white;");

    scoreBox->addWidget(scoreText);

    scoreBox->addWidget(scoreLabel);

    QVBoxLayout *bestBox = new QVBoxLayout();

    QLabel *bestText = new QLabel("BEST");

    bestText->setAlignment(Qt::AlignCenter);

    bestText->setStyleSheet(
        "color: #eee4da;"
        "font-weight: bold;"
        "font-size: 14px;"
        );

    bestScoreLabel = new QLabel(QString::number(bestScore));

    bestScoreLabel->setAlignment(Qt::AlignCenter);

    bestScoreLabel->setFont(QFont("Segoe UI", 22, QFont::Bold));

    bestScoreLabel->setStyleSheet("color: white;");

    bestBox->addWidget(bestText);

    bestBox->addWidget(bestScoreLabel);

    scoreLayout->addLayout(scoreBox);

    scoreLayout->addLayout(bestBox);

    topLayout->addWidget(scorePanel);

    mainLayout->addLayout(topLayout);

    statusLabel = new QLabel();

    statusLabel->setAlignment(Qt::AlignCenter);

    statusLabel->setFont(QFont("Segoe UI", 16, QFont::Bold));

    statusLabel->hide();

    mainLayout->addWidget(statusLabel);

    mainLayout->addSpacing(420);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();

    buttonsLayout->setSpacing(20);

    restartButton = new QPushButton("Новая игра");

    restartButton->setFixedSize(180, 55);

    recordsButton = new QPushButton("Таблица рекордов");

    recordsButton->setFixedSize(220, 55);

    QString buttonStyle =
        "QPushButton {"
        "background-color: #8f7a66;"
        "color: white;"
        "border: none;"
        "border-radius: 12px;"
        "font-size: 16px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background-color: #a1887f;"
        "}";

    restartButton->setStyleSheet(buttonStyle);

    recordsButton->setStyleSheet(buttonStyle);

    connect(restartButton,
            &QPushButton::clicked,
            this,
            &Game2048::restartGame);

    connect(recordsButton,
            &QPushButton::clicked,
            this,
            &Game2048::showHighScores);

    buttonsLayout->addStretch();

    buttonsLayout->addWidget(restartButton);

    buttonsLayout->addWidget(recordsButton);

    buttonsLayout->addStretch();

    mainLayout->addLayout(buttonsLayout);

    restartGame();
}

void Game2048::restartGame()
{
    for (int i = 0; i < SIZE; ++i)
    {
        for (int j = 0; j < SIZE; ++j)
        {
            board[i][j] = 0;
        }
    }

    score = 0;

    gameOver = false;

    win = false;

    scoreLabel->setText("0");

    statusLabel->hide();

    addNewTile();

    addNewTile();

    update();
}

void Game2048::addNewTile()
{
    QVector<QPair<int, int>> emptyCells;

    for (int i = 0; i < SIZE; ++i)
    {
        for (int j = 0; j < SIZE; ++j)
        {
            if (board[i][j] == 0)
            {
                emptyCells.append(qMakePair(i, j));
            }
        }
    }

    if (emptyCells.isEmpty())
        return;

    int index =
        QRandomGenerator::global()->bounded(emptyCells.size());

    int row = emptyCells[index].first;

    int col = emptyCells[index].second;

    board[row][col] =
        (QRandomGenerator::global()->bounded(10) < 9) ? 2 : 4;
}

void Game2048::updateScore(int points)
{
    score += points;

    scoreLabel->setText(QString::number(score));

    if (score > bestScore)
    {
        bestScore = score;

        bestScoreLabel->setText(QString::number(bestScore));
    }
}

void Game2048::saveData()
{
    QSettings settings("StudentProject", "2048");

    settings.setValue("BestScore", bestScore);

    if (!highScores.contains(score))
    {
        highScores.append(score);

        std::sort(highScores.begin(),
                  highScores.end(),
                  std::greater<int>());

        while (highScores.size() > 10)
        {
            highScores.removeLast();
        }
    }

    QStringList scores;

    for (int s : highScores)
    {
        scores.append(QString::number(s));
    }

    settings.setValue("HighScores", scores);
}

void Game2048::loadData()
{
    QSettings settings("StudentProject", "2048");

    bestScore =
        settings.value("BestScore", 0).toInt();

    QStringList scores =
        settings.value("HighScores").toStringList();

    highScores.clear();

    for (const QString &s : scores)
    {
        highScores.append(s.toInt());
    }
}

void Game2048::showHighScores()
{
    QDialog dialog(this);

    dialog.setWindowTitle("Таблица рекордов");

    dialog.setFixedSize(320, 420);

    dialog.setStyleSheet(
        "background-color: #faf8ef;"
        );

    QVBoxLayout *layout =
        new QVBoxLayout(&dialog);

    QLabel *title =
        new QLabel("🏆 ТОП РЕКОРДОВ 🏆");

    title->setAlignment(Qt::AlignCenter);

    title->setFont(QFont("Segoe UI", 16, QFont::Bold));

    title->setStyleSheet("color: #776e65;");

    layout->addWidget(title);

    QListWidget *list = new QListWidget();

    list->setStyleSheet(
        "QListWidget {"
        "background-color: white;"
        "border-radius: 10px;"
        "padding: 10px;"
        "font-size: 16px;"
        "}"
        );

    if (highScores.isEmpty())
    {
        list->addItem("Нет рекордов");
    }
    else
    {
        for (int i = 0; i < highScores.size(); ++i)
        {
            list->addItem(
                QString("%1 место — %2 очков")
                    .arg(i + 1)
                    .arg(highScores[i]));
        }
    }

    layout->addWidget(list);

    QPushButton *closeBtn =
        new QPushButton("Закрыть");

    closeBtn->setFixedSize(140, 45);

    closeBtn->setStyleSheet(
        "QPushButton {"
        "background-color: #8f7a66;"
        "color: white;"
        "border-radius: 10px;"
        "font-weight: bold;"
        "}"
        );

    connect(closeBtn,
            &QPushButton::clicked,
            &dialog,
            &QDialog::accept);

    layout->addWidget(closeBtn,
                      0,
                      Qt::AlignCenter);

    dialog.exec();
}

void Game2048::keyPressEvent(QKeyEvent *event)
{
    if (gameOver)
        return;

    int oldBoard[SIZE][SIZE];

    for (int i = 0; i < SIZE; ++i)
    {
        for (int j = 0; j < SIZE; ++j)
        {
            oldBoard[i][j] = board[i][j];
        }
    }

    switch (event->key())
    {
    case Qt::Key_Left:
        moveLeft();
        break;

    case Qt::Key_Right:
        moveRight();
        break;

    case Qt::Key_Up:
        moveUp();
        break;

    case Qt::Key_Down:
        moveDown();
        break;

    default:
        return;
    }

    bool moved = false;

    for (int i = 0; i < SIZE; ++i)
    {
        for (int j = 0; j < SIZE; ++j)
        {
            if (oldBoard[i][j] != board[i][j])
            {
                moved = true;
            }
        }
    }

    if (moved)
    {
        addNewTile();

        checkGameState();

        update();
    }
}

void Game2048::moveLeft()
{
    for (int i = 0; i < SIZE; ++i)
    {
        QVector<int> row;

        for (int j = 0; j < SIZE; ++j)
        {
            if (board[i][j] != 0)
            {
                row.append(board[i][j]);
            }
        }

        for (int j = 0; j < row.size() - 1; ++j)
        {
            if (row[j] == row[j + 1])
            {
                row[j] *= 2;

                updateScore(row[j]);

                row.remove(j + 1);
            }
        }

        while (row.size() < SIZE)
        {
            row.append(0);
        }

        for (int j = 0; j < SIZE; ++j)
        {
            board[i][j] = row[j];
        }
    }
}

void Game2048::moveRight()
{
    for (int i = 0; i < SIZE; ++i)
    {
        QVector<int> row;

        for (int j = SIZE - 1; j >= 0; --j)
        {
            if (board[i][j] != 0)
            {
                row.append(board[i][j]);
            }
        }

        for (int j = 0; j < row.size() - 1; ++j)
        {
            if (row[j] == row[j + 1])
            {
                row[j] *= 2;

                updateScore(row[j]);

                row.remove(j + 1);
            }
        }

        while (row.size() < SIZE)
        {
            row.append(0);
        }

        for (int j = 0; j < SIZE; ++j)
        {
            board[i][SIZE - 1 - j] = row[j];
        }
    }
}

void Game2048::moveUp()
{
    for (int j = 0; j < SIZE; ++j)
    {
        QVector<int> col;

        for (int i = 0; i < SIZE; ++i)
        {
            if (board[i][j] != 0)
            {
                col.append(board[i][j]);
            }
        }

        for (int i = 0; i < col.size() - 1; ++i)
        {
            if (col[i] == col[i + 1])
            {
                col[i] *= 2;

                updateScore(col[i]);

                col.remove(i + 1);
            }
        }

        while (col.size() < SIZE)
        {
            col.append(0);
        }

        for (int i = 0; i < SIZE; ++i)
        {
            board[i][j] = col[i];
        }
    }
}

void Game2048::moveDown()
{
    for (int j = 0; j < SIZE; ++j)
    {
        QVector<int> col;

        for (int i = SIZE - 1; i >= 0; --i)
        {
            if (board[i][j] != 0)
            {
                col.append(board[i][j]);
            }
        }

        for (int i = 0; i < col.size() - 1; ++i)
        {
            if (col[i] == col[i + 1])
            {
                col[i] *= 2;

                updateScore(col[i]);

                col.remove(i + 1);
            }
        }

        while (col.size() < SIZE)
        {
            col.append(0);
        }

        for (int i = 0; i < SIZE; ++i)
        {
            board[SIZE - 1 - i][j] = col[i];
        }
    }
}

bool Game2048::canMove()
{
    for (int i = 0; i < SIZE; ++i)
    {
        for (int j = 0; j < SIZE; ++j)
        {
            if (board[i][j] == 0)
            {
                return true;
            }
        }
    }

    for (int i = 0; i < SIZE; ++i)
    {
        for (int j = 0; j < SIZE - 1; ++j)
        {
            if (board[i][j] == board[i][j + 1])
            {
                return true;
            }
        }
    }

    for (int i = 0; i < SIZE - 1; ++i)
    {
        for (int j = 0; j < SIZE; ++j)
        {
            if (board[i][j] == board[i + 1][j])
            {
                return true;
            }
        }
    }

    return false;
}

void Game2048::checkGameState()
{
    for (int i = 0; i < SIZE; ++i)
    {
        for (int j = 0; j < SIZE; ++j)
        {
            if (board[i][j] == 2048 && !win)
            {
                win = true;

                statusLabel->setText("🎉 YOU WIN!");

                statusLabel->setStyleSheet(
                    "color: #00c853;"
                    "font-size: 18px;"
                    "font-weight: bold;"
                    );

                statusLabel->show();
            }
        }
    }

    if (!canMove())
    {
        gameOver = true;

        statusLabel->setText("💀 GAME OVER");

        statusLabel->setStyleSheet(
            "color: red;"
            "font-size: 18px;"
            "font-weight: bold;"
            );

        statusLabel->show();

        saveData();
    }
}

void Game2048::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    int boardX = 35;

    int boardY = 140;

    int cellSize = 95;

    int spacing = 10;

    painter.setPen(Qt::NoPen);

    painter.setBrush(QColor("#bbada0"));

    painter.drawRoundedRect(
        boardX - 5,
        boardY - 5,
        430,
        430,
        12,
        12);

    for (int i = 0; i < SIZE; ++i)
    {
        for (int j = 0; j < SIZE; ++j)
        {
            int value = board[i][j];

            int x =
                boardX + j * (cellSize + spacing);

            int y =
                boardY + i * (cellSize + spacing);

            QColor tileColor =
                tileColors.value(value,
                                 QColor("#3c3a32"));

            painter.setBrush(tileColor);

            painter.setPen(
                QPen(QColor(255,255,255,70), 2));

            painter.drawRoundedRect(
                x,
                y,
                cellSize,
                cellSize,
                12,
                12);

            if (value != 0)
            {
                painter.setPen(Qt::black);

                int fontSize = 28;

                if (value >= 1024)
                    fontSize = 22;

                if (value >= 16384)
                    fontSize = 18;

                painter.setFont(
                    QFont("Segoe UI",
                          fontSize,
                          QFont::Bold));

                painter.drawText(
                    QRect(x,
                          y,
                          cellSize,
                          cellSize),
                    Qt::AlignCenter,
                    QString::number(value));
            }
        }
    }

    if (gameOver)
    {
        painter.fillRect(
            boardX - 5,
            boardY - 5,
            430,
            430,
            QColor(0, 0, 0, 170));
    }
}
