#include "TicTacToe.h"
#include <QGridLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QColorDialog>

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
    , colorX(QColor(41, 128, 185))
    , colorO(QColor(231, 76, 60))
{
    setWindowTitle("Крестики-нолики");
    setFixedSize(500, 650);
    setFocusPolicy(Qt::StrongFocus);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    QHBoxLayout *topPanel = new QHBoxLayout();
    topPanel->setSpacing(15);

    themeButton = new QPushButton("🌙 Тёмная тема");
    themeButton->setFixedSize(130, 40);
    themeButton->setFont(QFont("Arial", 11, QFont::Bold));
    themeButton->setCursor(Qt::PointingHandCursor);
    themeButton->setStyleSheet(
        "QPushButton {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "      stop:0 #8e44ad, stop:1 #9b59b6);"
        "  color: white;"
        "  border: none;"
        "  border-radius: 8px;"
        "}"
        "QPushButton:hover {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "      stop:0 #9b59b6, stop:1 #a569bd);"
        "}"
        );
    connect(themeButton, &QPushButton::clicked, this, &TicTacToe::toggleTheme);

    modeButton = new QPushButton("🎮 Обычный");
    modeButton->setFixedSize(130, 40);
    modeButton->setFont(QFont("Arial", 11, QFont::Bold));
    modeButton->setCursor(Qt::PointingHandCursor);
    modeButton->setStyleSheet(
        "QPushButton {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "      stop:0 #16a085, stop:1 #1abc9c);"
        "  color: white;"
        "  border: none;"
        "  border-radius: 8px;"
        "}"
        "QPushButton:hover {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "      stop:0 #1abc9c, stop:1 #1e8449);"
        "}"
        );
    connect(modeButton, &QPushButton::clicked, this, &TicTacToe::switchMode);

    topPanel->addWidget(themeButton);
    topPanel->addStretch();
    topPanel->addWidget(modeButton);
    mainLayout->addLayout(topPanel);

    QHBoxLayout *colorPanel = new QHBoxLayout();
    colorPanel->setSpacing(15);

    colorXBtn = new QPushButton("🎨 Цвет X");
    colorXBtn->setFixedSize(120, 35);
    colorXBtn->setFont(QFont("Arial", 10, QFont::Bold));
    colorXBtn->setCursor(Qt::PointingHandCursor);
    colorXBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #34495e;"
        "  color: white;"
        "  border: none;"
        "  border-radius: 6px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #3d566e;"
        "}"
        );
    connect(colorXBtn, &QPushButton::clicked, this, &TicTacToe::chooseColorX);

    colorOBtn = new QPushButton("🎨 Цвет O");
    colorOBtn->setFixedSize(120, 35);
    colorOBtn->setFont(QFont("Arial", 10, QFont::Bold));
    colorOBtn->setCursor(Qt::PointingHandCursor);
    colorOBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #34495e;"
        "  color: white;"
        "  border: none;"
        "  border-radius: 6px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #3d566e;"
        "}"
        );
    connect(colorOBtn, &QPushButton::clicked, this, &TicTacToe::chooseColorO);

    colorPanel->addStretch();
    colorPanel->addWidget(colorXBtn);
    colorPanel->addWidget(colorOBtn);
    colorPanel->addStretch();
    mainLayout->addLayout(colorPanel);

    scoreLabel = new QLabel("⭐ X: 0  |  O: 0 ⭐");
    scoreLabel->setAlignment(Qt::AlignCenter);
    scoreLabel->setFont(QFont("Arial", 14, QFont::Bold));
    mainLayout->addWidget(scoreLabel);

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(10);

    buttons.resize(3);
    for (int i = 0; i < 3; ++i) {
        buttons[i].resize(3);
        for (int j = 0; j < 3; ++j) {
            QPushButton *btn = new QPushButton("");
            btn->setFixedSize(100, 100);
            btn->setFont(QFont("Arial", 40, QFont::Bold));
            btn->setCursor(Qt::PointingHandCursor);
            connect(btn, &QPushButton::clicked, this, &TicTacToe::onButtonClick);
            gridLayout->addWidget(btn, i, j);
            buttons[i][j] = btn;
        }
    }

    mainLayout->addLayout(gridLayout);

    timerLabel = new QLabel("⏱ Режим: обычный");
    timerLabel->setAlignment(Qt::AlignCenter);
    timerLabel->setFont(QFont("Arial", 12));
    mainLayout->addWidget(timerLabel);

    restartBtn = new QPushButton("🔄 Новая игра");
    restartBtn->setFixedSize(180, 45);
    restartBtn->setFont(QFont("Arial", 12, QFont::Bold));
    restartBtn->setCursor(Qt::PointingHandCursor);
    restartBtn->setStyleSheet(
        "QPushButton {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "      stop:0 #e74c3c, stop:1 #c0392b);"
        "  color: white;"
        "  border: none;"
        "  border-radius: 10px;"
        "}"
        "QPushButton:hover {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "      stop:0 #c0392b, stop:1 #a93226);"
        "}"
        );
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
    updatePlayerColors();
}

void TicTacToe::updatePlayerColors()
{
    colorXBtn->setStyleSheet(QString(
                                 "QPushButton {"
                                 "  background-color: #34495e;"
                                 "  color: white;"
                                 "  border: 2px solid %1;"
                                 "  border-radius: 6px;"
                                 "}"
                                 "QPushButton:hover {"
                                 "  background-color: #3d566e;"
                                 "}"
                                 ).arg(colorX.name()));

    colorOBtn->setStyleSheet(QString(
                                 "QPushButton {"
                                 "  background-color: #34495e;"
                                 "  color: white;"
                                 "  border: 2px solid %1;"
                                 "  border-radius: 6px;"
                                 "}"
                                 "QPushButton:hover {"
                                 "  background-color: #3d566e;"
                                 "}"
                                 ).arg(colorO.name()));
}

void TicTacToe::chooseColorX()
{
    QColor newColor = QColorDialog::getColor(colorX, this, "Выберите цвет для X");
    if (newColor.isValid()) {
        colorX = newColor;
        updatePlayerColors();
        updateButtonStyles();
    }
}

void TicTacToe::chooseColorO()
{
    QColor newColor = QColorDialog::getColor(colorO, this, "Выберите цвет для O");
    if (newColor.isValid()) {
        colorO = newColor;
        updatePlayerColors();
        updateButtonStyles();
    }
}

void TicTacToe::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QLinearGradient gradient(0, 0, width(), height());
    if (isDarkTheme) {
        gradient.setColorAt(0, QColor(25, 35, 50));
        gradient.setColorAt(0.5, QColor(35, 45, 65));
        gradient.setColorAt(1, QColor(20, 30, 45));
    } else {
        gradient.setColorAt(0, QColor(255, 255, 255));
        gradient.setColorAt(0.5, QColor(245, 245, 250));
        gradient.setColorAt(1, QColor(235, 235, 240));
    }
    painter.fillRect(rect(), gradient);

    if (gameEnded && winningRow1 != -1) {
        painter.setPen(QPen(Qt::yellow, 6, Qt::SolidLine));
        QPushButton *firstBtn = buttons[winningRow1][winningCol1];
        QPushButton *lastBtn = buttons[winningRow2][winningCol2];
        QPoint start = firstBtn->geometry().center();
        QPoint end = lastBtn->geometry().center();
        painter.drawLine(start, end);
    }
}

void TicTacToe::updateButtonStyles()
{
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            QPushButton *btn = buttons[i][j];
            if (btn->text().isEmpty()) {
                if (isDarkTheme) {
                    btn->setStyleSheet(
                        "QPushButton {"
                        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
                        "      stop:0 #f0f0f0, stop:1 #e0e0e0);"
                        "  border-radius: 15px;"
                        "  border: 2px solid #5a6e8a;"
                        "}"
                        "QPushButton:hover {"
                        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
                        "      stop:0 #ffffff, stop:1 #f5f5f5);"
                        "  border: 2px solid #7a9ecc;"
                        "}"
                        );
                } else {
                    btn->setStyleSheet(
                        "QPushButton {"
                        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
                        "      stop:0 #ecf0f1, stop:1 #bdc3c7);"
                        "  border-radius: 15px;"
                        "  border: 2px solid #2c3e50;"
                        "}"
                        "QPushButton:hover {"
                        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
                        "      stop:0 #ffffff, stop:1 #d5dbdb);"
                        "  border: 2px solid #1a2632;"
                        "}"
                        );
                }
            } else if (btn->text() == "X") {
                btn->setStyleSheet(QString(
                                       "QPushButton {"
                                       "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
                                       "      stop:0 #e8f4ff, stop:1 #d0e8ff);"
                                       "  border-radius: 15px;"
                                       "  border: 2px solid %1;"
                                       "  color: %1;"
                                       "  font-weight: bold;"
                                       "}"
                                       ).arg(colorX.name()));
            } else if (btn->text() == "O") {
                btn->setStyleSheet(QString(
                                       "QPushButton {"
                                       "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
                                       "      stop:0 #ffe8e8, stop:1 #ffd0d0);"
                                       "  border-radius: 15px;"
                                       "  border: 2px solid %1;"
                                       "  color: %1;"
                                       "  font-weight: bold;"
                                       "}"
                                       ).arg(colorO.name()));
            }
        }
    }
}

void TicTacToe::onButtonClick()
{
    QPushButton *btn = qobject_cast<QPushButton *>(sender());
    if (!btn || gameEnded || !btn->text().isEmpty())
        return;

    stopTimer();

    if (currentPlayer == "X") {
        btn->setText("X");
        btn->setStyleSheet(QString(
                               "QPushButton {"
                               "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
                               "      stop:0 #e8f4ff, stop:1 #d0e8ff);"
                               "  border-radius: 15px;"
                               "  border: 2px solid %1;"
                               "  color: %1;"
                               "  font-weight: bold;"
                               "}"
                               ).arg(colorX.name()));
    } else {
        btn->setText("O");
        btn->setStyleSheet(QString(
                               "QPushButton {"
                               "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
                               "      stop:0 #ffe8e8, stop:1 #ffd0d0);"
                               "  border-radius: 15px;"
                               "  border: 2px solid %1;"
                               "  color: %1;"
                               "  font-weight: bold;"
                               "}"
                               ).arg(colorO.name()));
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
            if (buttons[i][0]->text() == "X") scoreX++;
            else scoreO++;
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
            if (buttons[0][i]->text() == "X") scoreX++;
            else scoreO++;
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
        if (buttons[0][0]->text() == "X") scoreX++;
        else scoreO++;
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
        if (buttons[0][2]->text() == "X") scoreX++;
        else scoreO++;
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
            "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
            "      stop:0 #2ecc71, stop:1 #27ae60);"
            "  border-radius: 15px;"
            "  border: 3px solid #f1c40f;"
            "  color: white;"
            "  font-weight: bold;"
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
    msgBox.setText(QString("%1 победили! 🎉").arg(winner));
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
        }
    }
    updateButtonStyles();
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

void TicTacToe::updateScoreDisplay()
{
    scoreLabel->setText(QString("⭐ X: %1  |  O: %2 ⭐").arg(scoreX).arg(scoreO));
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
        timerLabel->setText(QString("⏱ Осталось %1 сек (ход %2)").arg(remainingTime).arg(currentPlayer == "X" ? "X" : "O"));
    } else {
        timerLabel->setText("⏱ Режим: обычный");
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
        themeButton->setText("☀️ Светлая тема");
        scoreLabel->setStyleSheet("color: white; background: transparent;");
        timerLabel->setStyleSheet("color: #cccccc; background: transparent;");
        colorXBtn->setStyleSheet(QString(
                                     "QPushButton {"
                                     "  background-color: #34495e;"
                                     "  color: white;"
                                     "  border: 2px solid %1;"
                                     "  border-radius: 6px;"
                                     "}"
                                     "QPushButton:hover {"
                                     "  background-color: #3d566e;"
                                     "}"
                                     ).arg(colorX.name()));
        colorOBtn->setStyleSheet(QString(
                                     "QPushButton {"
                                     "  background-color: #34495e;"
                                     "  color: white;"
                                     "  border: 2px solid %1;"
                                     "  border-radius: 6px;"
                                     "}"
                                     "QPushButton:hover {"
                                     "  background-color: #3d566e;"
                                     "}"
                                     ).arg(colorO.name()));
    } else {
        themeButton->setText("🌙 Тёмная тема");
        scoreLabel->setStyleSheet("color: #2c3e50; background: transparent;");
        timerLabel->setStyleSheet("color: #34495e; background: transparent;");
        colorXBtn->setStyleSheet(QString(
                                     "QPushButton {"
                                     "  background-color: #bdc3c7;"
                                     "  color: #2c3e50;"
                                     "  border: 2px solid %1;"
                                     "  border-radius: 6px;"
                                     "}"
                                     "QPushButton:hover {"
                                     "  background-color: #95a5a6;"
                                     "}"
                                     ).arg(colorX.name()));
        colorOBtn->setStyleSheet(QString(
                                     "QPushButton {"
                                     "  background-color: #bdc3c7;"
                                     "  color: #2c3e50;"
                                     "  border: 2px solid %1;"
                                     "  border-radius: 6px;"
                                     "}"
                                     "QPushButton:hover {"
                                     "  background-color: #95a5a6;"
                                     "}"
                                     ).arg(colorO.name()));
    }

    updateButtonStyles();
    update();
}

void TicTacToe::switchMode()
{
    isTurboMode = !isTurboMode;

    if (isTurboMode) {
        modeButton->setText("⚡ Турбо-режим");
        timerLabel->setText(QString("⏱ Осталось %1 сек (ход %2)").arg(timeLimit).arg(currentPlayer == "X" ? "X" : "O"));
        if (!gameEnded) {
            startTimerForCurrentPlayer();
        }
    } else {
        modeButton->setText("🎮 Обычный");
        timerLabel->setText("⏱ Режим: обычный");
        stopTimer();
    }
}
