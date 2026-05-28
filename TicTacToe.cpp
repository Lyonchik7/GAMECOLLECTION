#include "TicTacToe.h"
#include <QGridLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>

TicTacToe::TicTacToe(QWidget *parent)
    : QWidget(parent)
    , currentPlayer("X")
    , gameEnded(false)
    , moveCount(0)
    , winningRow1(-1), winningCol1(-1), winningRow2(-1), winningCol2(-1)
    , scoreX(0)
    , scoreO(0)
    , isDarkTheme(true)
    , isTurboMode(false)
    , timeLimit(5)
    , gameTimer(nullptr)
    , remainingTime(5)
{
    setWindowTitle("Крестики-нолики");
    setFixedSize(400, 520);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QHBoxLayout *topPanel = new QHBoxLayout();
    themeButton = new QPushButton("Тёмная тема");
    themeButton->setFixedSize(100, 30);
    connect(themeButton, &QPushButton::clicked, this, &TicTacToe::toggleTheme);

    modeButton = new QPushButton("Обычный режим");
    modeButton->setFixedSize(100, 30);
    connect(modeButton, &QPushButton::clicked, this, &TicTacToe::switchMode);

    topPanel->addWidget(themeButton);
    topPanel->addStretch();
    topPanel->addWidget(modeButton);
    mainLayout->addLayout(topPanel);

    scoreLabel = new QLabel("X: 0  |  O: 0");
    scoreLabel->setAlignment(Qt::AlignCenter);
    scoreLabel->setFont(QFont("Arial", 12, QFont::Bold));
    mainLayout->addWidget(scoreLabel);

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(5);

    buttons.resize(3);
    for (int i = 0; i < 3; ++i) {
        buttons[i].resize(3);
        for (int j = 0; j < 3; ++j) {
            QPushButton *btn = new QPushButton("");
            btn->setFixedSize(90, 90);
            btn->setFont(QFont("Arial", 32, QFont::Bold));
            connect(btn, &QPushButton::clicked, this, &TicTacToe::onButtonClick);
            gridLayout->addWidget(btn, i, j);
            buttons[i][j] = btn;
        }
    }

    mainLayout->addLayout(gridLayout);

    timerLabel = new QLabel("Режим: обычный");
    timerLabel->setAlignment(Qt::AlignCenter);
    timerLabel->setFont(QFont("Arial", 11));
    mainLayout->addWidget(timerLabel);

    restartBtn = new QPushButton("Начать заново");
    restartBtn->setFixedSize(150, 35);
    restartBtn->setFont(QFont("Arial", 11));
    connect(restartBtn, &QPushButton::clicked, this, &TicTacToe::restartGame);

    QHBoxLayout *bottomPanel = new QHBoxLayout();
    bottomPanel->addStretch();
    bottomPanel->addWidget(restartBtn);
    bottomPanel->addStretch();
    mainLayout->addLayout(bottomPanel);

    setLayout(mainLayout);

    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &TicTacToe::timeOut);

    toggleTheme();
}

void TicTacToe::onButtonClick()
{
    QPushButton *btn = qobject_cast<QPushButton *>(sender());
    if (!btn || gameEnded || !btn->text().isEmpty())
        return;

    stopTimer();

    if (currentPlayer == "X") {
        btn->setText("X");
        btn->setStyleSheet(
            "QPushButton {"
            "    background-color: white;"
            "    border-radius: 10px;"
            "    border: 2px solid #3498db;"
            "    color: #2980b9;"
            "    font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "    background-color: #ecf0f1;"
            "}"
            );
    } else {
        btn->setText("O");
        btn->setStyleSheet(
            "QPushButton {"
            "    background-color: white;"
            "    border-radius: 10px;"
            "    border: 2px solid #3498db;"
            "    color: #e74c3c;"
            "    font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "    background-color: #ecf0f1;"
            "}"
            );
    }
    moveCount++;

    checkGameState();

    if (!gameEnded) {
        currentPlayer = (currentPlayer == "X") ? "O" : "X";
        if (isTurboMode && !gameEnded) {
            startTimerForCurrentPlayer();
        }
    }
}

void TicTacToe::checkGameState()
{
    for (int i = 0; i < 3; ++i) {
        if (!buttons[i][0]->text().isEmpty() &&
            buttons[i][0]->text() == buttons[i][1]->text() &&
            buttons[i][1]->text() == buttons[i][2]->text()) {
            gameEnded = true;
            winningRow1 = i; winningCol1 = 0;
            winningRow2 = i; winningCol2 = 2;
            highlightWinningButtons({buttons[i][0], buttons[i][1], buttons[i][2]});
            if (buttons[i][0]->text() == "X") {
                scoreX++;
            } else {
                scoreO++;
            }
            updateScoreDisplay();
            showWinnerMessage(buttons[i][0]->text());
            stopTimer();
            return;
        }
    }

    for (int i = 0; i < 3; ++i) {
        if (!buttons[0][i]->text().isEmpty() &&
            buttons[0][i]->text() == buttons[1][i]->text() &&
            buttons[1][i]->text() == buttons[2][i]->text()) {
            gameEnded = true;
            winningRow1 = 0; winningCol1 = i;
            winningRow2 = 2; winningCol2 = i;
            highlightWinningButtons({buttons[0][i], buttons[1][i], buttons[2][i]});
            if (buttons[0][i]->text() == "X") {
                scoreX++;
            } else {
                scoreO++;
            }
            updateScoreDisplay();
            showWinnerMessage(buttons[0][i]->text());
            stopTimer();
            return;
        }
    }

    if (!buttons[0][0]->text().isEmpty() &&
        buttons[0][0]->text() == buttons[1][1]->text() &&
        buttons[1][1]->text() == buttons[2][2]->text()) {
        gameEnded = true;
        winningRow1 = 0; winningCol1 = 0;
        winningRow2 = 2; winningCol2 = 2;
        highlightWinningButtons({buttons[0][0], buttons[1][1], buttons[2][2]});
        if (buttons[0][0]->text() == "X") {
            scoreX++;
        } else {
            scoreO++;
        }
        updateScoreDisplay();
        showWinnerMessage(buttons[0][0]->text());
        stopTimer();
        return;
    }

    if (!buttons[0][2]->text().isEmpty() &&
        buttons[0][2]->text() == buttons[1][1]->text() &&
        buttons[1][1]->text() == buttons[2][0]->text()) {
        gameEnded = true;
        winningRow1 = 0; winningCol1 = 2;
        winningRow2 = 2; winningCol2 = 0;
        highlightWinningButtons({buttons[0][2], buttons[1][1], buttons[2][0]});
        if (buttons[0][2]->text() == "X") {
            scoreX++;
        } else {
            scoreO++;
        }
        updateScoreDisplay();
        showWinnerMessage(buttons[0][2]->text());
        stopTimer();
        return;
    }

    if (moveCount == 9) {
        gameEnded = true;
        QMessageBox::information(this, "Игра окончена", "Ничья!");
        disableAllButtons();
        stopTimer();
    }
}

void TicTacToe::highlightWinningButtons(const QVector<QPushButton*> &winningBtns)
{
    for (QPushButton *btn : winningBtns) {
        btn->setStyleSheet(
            "QPushButton {"
            "    background-color: #27ae60;"
            "    border-radius: 10px;"
            "    border: 3px solid #f1c40f;"
            "    color: black;"
            "    font-weight: bold;"
            "}"
            );
    }
}

void TicTacToe::showWinnerMessage(const QString &winnerText)
{
    QString winner;
    if (winnerText == "X") {
        winner = "Крестики (X)";
    } else {
        winner = "Нолики (O)";
    }

    QMessageBox msgBox;
    msgBox.setWindowTitle("Победа!");
    msgBox.setText(QString("%1 победили!").arg(winner));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();
}

void TicTacToe::disableAllButtons()
{
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            buttons[i][j]->setEnabled(false);
}

void TicTacToe::enableAllButtons()
{
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            buttons[i][j]->setEnabled(true);
            buttons[i][j]->setText("");
            buttons[i][j]->setStyleSheet(
                "QPushButton {"
                "    background-color: white;"
                "    border-radius: 10px;"
                "    border: 2px solid #3498db;"
                "}"
                "QPushButton:hover {"
                "    background-color: #ecf0f1;"
                "}"
                );
        }
    }
}

void TicTacToe::restartGame()
{
    enableAllButtons();
    currentPlayer = "X";
    gameEnded = false;
    moveCount = 0;
    winningRow1 = -1; winningCol1 = -1;
    winningRow2 = -1; winningCol2 = -1;
    stopTimer();
    remainingTime = timeLimit;
    updateTimerDisplay();
    if (isTurboMode && !gameEnded) {
        startTimerForCurrentPlayer();
    }
    update();
}

void TicTacToe::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    if (gameEnded && winningRow1 != -1) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QPen(Qt::yellow, 6, Qt::SolidLine));

        QPushButton *firstBtn = buttons[winningRow1][winningCol1];
        QPushButton *lastBtn = buttons[winningRow2][winningCol2];

        QPoint start = firstBtn->geometry().center();
        QPoint end = lastBtn->geometry().center();

        painter.drawLine(start, end);
    }
}

void TicTacToe::updateScoreDisplay()
{
    scoreLabel->setText(QString("X (синие): %1  |  O (красные): %2").arg(scoreX).arg(scoreO));
}

void TicTacToe::stopTimer()
{
    if (gameTimer && gameTimer->isActive()) {
        gameTimer->stop();
    }
}

void TicTacToe::startTimerForCurrentPlayer()
{
    if (!isTurboMode || gameEnded) return;

    remainingTime = timeLimit;
    updateTimerDisplay();

    if (gameTimer->isActive()) {
        gameTimer->stop();
    }
    gameTimer->start(1000);
}

void TicTacToe::updateTimerDisplay()
{
    if (isTurboMode && !gameEnded) {
        timerLabel->setText(QString("Осталось %1 сек (ход %2)").arg(remainingTime).arg(currentPlayer == "X" ? "X" : "O"));
    } else {
        timerLabel->setText("Режим: обычный");
    }
}

void TicTacToe::timeOut()
{
    if (!isTurboMode || gameEnded) return;

    remainingTime--;
    updateTimerDisplay();

    if (remainingTime <= 0) {
        gameTimer->stop();
        QMessageBox::warning(this, "Время вышло!", QString("Игрок %1 не успел сходить! Ход переходит к сопернику.").arg(currentPlayer == "X" ? "X" : "O"));
        currentPlayer = (currentPlayer == "X") ? "O" : "X";
        remainingTime = timeLimit;
        updateTimerDisplay();
        startTimerForCurrentPlayer();
    }
}

void TicTacToe::toggleTheme()
{
    isDarkTheme = !isDarkTheme;

    if (isDarkTheme) {
        setStyleSheet("background-color: #2c3e50;");
        scoreLabel->setStyleSheet("color: white;");
        timerLabel->setStyleSheet("color: white;");
        themeButton->setText("Светлая тема");
        themeButton->setStyleSheet("background-color: #34495e; color: white; border-radius: 5px;");
        modeButton->setStyleSheet("background-color: #34495e; color: white; border-radius: 5px;");
        restartBtn->setStyleSheet("background-color: #e74c3c; color: white; border-radius: 5px;");
    } else {
        setStyleSheet("background-color: #ecf0f1;");
        scoreLabel->setStyleSheet("color: black;");
        timerLabel->setStyleSheet("color: black;");
        themeButton->setText("Тёмная тема");
        themeButton->setStyleSheet("background-color: #bdc3c7; color: black; border-radius: 5px;");
        modeButton->setStyleSheet("background-color: #bdc3c7; color: black; border-radius: 5px;");
        restartBtn->setStyleSheet("background-color: #e74c3c; color: white; border-radius: 5px;");
    }
}

void TicTacToe::switchMode()
{
    isTurboMode = !isTurboMode;

    if (isTurboMode) {
        modeButton->setText("Турбо-режим");
        timerLabel->setText(QString("Осталось %1 сек (ход %2)").arg(timeLimit).arg(currentPlayer == "X" ? "X" : "O"));
        if (!gameEnded) {
            startTimerForCurrentPlayer();
        }
    } else {
        modeButton->setText("Обычный режим");
        timerLabel->setText("Режим: обычный");
        stopTimer();
    }
}
