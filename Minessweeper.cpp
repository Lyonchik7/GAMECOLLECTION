#include "Minessweeper.h"
#include <QGridLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRandomGenerator>
#include <QMouseEvent>

Minessweeper::Minessweeper(QWidget *parent)
    : QWidget(parent)
    , flagsPlaced(0)
    , gameOver(false)
    , firstClick(true)
{
    setWindowTitle("Сапёр");
    setFixedSize(320, 400);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QHBoxLayout *topLayout = new QHBoxLayout();

    QLabel *minesLabel = new QLabel(QString("💣 %1").arg(MINES_COUNT));
    minesLabel->setFont(QFont("Arial", 12));
    topLayout->addWidget(minesLabel);

    QPushButton *restartBtn = new QPushButton("Новая игра");
    connect(restartBtn, &QPushButton::clicked, this, &Minessweeper::restartGame);
    topLayout->addWidget(restartBtn);

    flagsLabel = new QLabel("🚩 0");
    flagsLabel->setFont(QFont("Arial", 12));
    topLayout->addWidget(flagsLabel);
    topLayout->addStretch();

    mainLayout->addLayout(topLayout);

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(1);

    buttons.resize(ROWS);
    for (int i = 0; i < ROWS; ++i) {
        buttons[i].resize(COLS);
        for (int j = 0; j < COLS; ++j) {
            QPushButton *btn = new QPushButton();
            btn->setFixedSize(30, 30);
            btn->setFont(QFont("Arial", 10, QFont::Bold));
            btn->setContextMenuPolicy(Qt::PreventContextMenu);

            connect(btn, &QPushButton::clicked, this, &Minessweeper::onLeftClick);

            gridLayout->addWidget(btn, i, j);
            buttons[i][j] = btn;
        }
    }

    mainLayout->addLayout(gridLayout);
    restartGame();
}

void Minessweeper::restartGame()
{
    mines.resize(ROWS);
    numbers.resize(ROWS);
    opened.resize(ROWS);
    flagged.resize(ROWS);

    for (int i = 0; i < ROWS; ++i) {
        mines[i].resize(COLS, false);
        numbers[i].resize(COLS, 0);
        opened[i].resize(COLS, false);
        flagged[i].resize(COLS, false);
    }

    flagsPlaced = 0;
    gameOver = false;
    firstClick = true;

    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            buttons[i][j]->setText("");
            buttons[i][j]->setEnabled(true);
            buttons[i][j]->setStyleSheet("");
        }
    }

    updateFlagsLabel();
}

void Minessweeper::placeMines(int firstRow, int firstCol)
{
    int minesPlaced = 0;
    while (minesPlaced < MINES_COUNT) {
        int r = QRandomGenerator::global()->bounded(ROWS);
        int c = QRandomGenerator::global()->bounded(COLS);

        if (!mines[r][c] && !(r == firstRow && c == firstCol)) {
            mines[r][c] = true;
            minesPlaced++;
        }
    }

    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            if (mines[i][j]) {
                numbers[i][j] = -1;
            } else {
                numbers[i][j] = countAdjacentMines(i, j);
            }
        }
    }
}

int Minessweeper::countAdjacentMines(int row, int col)
{
    int count = 0;
    for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
            if (dr == 0 && dc == 0) continue;
            int nr = row + dr, nc = col + dc;
            if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS && mines[nr][nc])
                count++;
        }
    }
    return count;
}

void Minessweeper::onLeftClick()
{
    if (gameOver) return;

    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    int row = -1, col = -1;
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            if (buttons[i][j] == btn) {
                row = i; col = j;
                break;
            }
        }
        if (row != -1) break;
    }

    if (flagged[row][col]) return;

    if (firstClick) {
        placeMines(row, col);
        firstClick = false;
    }

    if (mines[row][col]) {
        gameOver = true;
        btn->setText("💣");
        btn->setStyleSheet("background-color: red;");

        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                if (mines[i][j]) {
                    buttons[i][j]->setText("💣");
                }
            }
        }

        QMessageBox::information(this, "БУМ!", "Вы наступили на мину!");
    } else {
        openCell(row, col);
        checkWin();
    }
}

void Minessweeper::onRightClick()
{
    if (gameOver) return;

    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    int row = -1, col = -1;
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            if (buttons[i][j] == btn) {
                row = i; col = j;
                break;
            }
        }
        if (row != -1) break;
    }

    if (opened[row][col]) return;

    if (!flagged[row][col]) {
        if (flagsPlaced < MINES_COUNT) {
            btn->setText("🚩");
            btn->setStyleSheet("color: red;");
            flagged[row][col] = true;
            flagsPlaced++;
        }
    } else {
        btn->setText("");
        btn->setStyleSheet("");
        flagged[row][col] = false;
        flagsPlaced--;
    }

    updateFlagsLabel();
    checkWin();
}

void Minessweeper::openCell(int row, int col)
{
    if (row < 0 || row >= ROWS || col < 0 || col >= COLS ||
        opened[row][col] || flagged[row][col]) return;

    opened[row][col] = true;
    buttons[row][col]->setEnabled(false);
    buttons[row][col]->setStyleSheet("background-color: lightgray;");

    if (numbers[row][col] > 0) {
        QColor color;
        switch (numbers[row][col]) {
        case 1: color = Qt::blue; break;
        case 2: color = Qt::green; break;
        case 3: color = Qt::red; break;
        case 4: color = QColor(0, 0, 128); break;
        case 5: color = QColor(128, 0, 0); break;
        default: color = Qt::black;
        }
        buttons[row][col]->setText(QString::number(numbers[row][col]));
        buttons[row][col]->setStyleSheet(QString("color: %1; background-color: lightgray;")
                                             .arg(color.name()));
    } else if (numbers[row][col] == 0) {
        buttons[row][col]->setText("");
        for (int dr = -1; dr <= 1; ++dr)
            for (int dc = -1; dc <= 1; ++dc)
                if (dr != 0 || dc != 0)
                    openCell(row + dr, col + dc);
    }
}

void Minessweeper::checkWin()
{
    bool allMinesFlagged = true;
    bool allSafeOpened = true;

    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            if (mines[i][j]) {
                if (!flagged[i][j]) allMinesFlagged = false;
            } else {
                if (!opened[i][j]) allSafeOpened = false;
            }
        }
    }

    if (allMinesFlagged || allSafeOpened) {
        gameOver = true;
        QMessageBox::information(this, "Победа!", "Вы обезвредили все мины!");
    }
}

void Minessweeper::updateFlagsLabel()
{
    flagsLabel->setText(QString("🚩 %1").arg(flagsPlaced));
}
