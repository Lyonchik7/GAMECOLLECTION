#include "TicTacToe.h"
#include <QGridLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QVBoxLayout>

TicTacToe::TicTacToe(QWidget *parent)
    : QWidget(parent)
    , currentPlayer("X")
    , gameEnded(false)
    , moveCount(0)
{
    setWindowTitle("Крестики-нолики");
    setFixedSize(300, 350);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QGridLayout *gridLayout = new QGridLayout();

    buttons.resize(3);
    for (int i = 0; i < 3; ++i) {
        buttons[i].resize(3);
        for (int j = 0; j < 3; ++j) {
            QPushButton *btn = new QPushButton("");
            btn->setFixedSize(80, 80);
            btn->setFont(QFont("Arial", 24, QFont::Bold));
            connect(btn, &QPushButton::clicked, this, &TicTacToe::onButtonClick);
            gridLayout->addWidget(btn, i, j);
            buttons[i][j] = btn;
        }
    }

    mainLayout->addLayout(gridLayout);

    QPushButton *restartBtn = new QPushButton("Начать заново");
    restartBtn->setFixedSize(280, 30);
    connect(restartBtn, &QPushButton::clicked, this, &TicTacToe::restartGame);
    mainLayout->addWidget(restartBtn);

    setLayout(mainLayout);
}

void TicTacToe::onButtonClick()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn || gameEnded || !btn->text().isEmpty())
        return;

    btn->setText(currentPlayer);
    moveCount++;

    checkGameState();

    currentPlayer = (currentPlayer == "X") ? "O" : "X";
}

void TicTacToe::checkGameState()
{
    for (int i = 0; i < 3; ++i) {
        if (!buttons[i][0]->text().isEmpty() &&
            buttons[i][0]->text() == buttons[i][1]->text() &&
            buttons[i][1]->text() == buttons[i][2]->text()) {
            gameEnded = true;
            QMessageBox::information(this, "Игра окончена",
                                     QString("Игрок %1 победил!").arg(buttons[i][0]->text()));
            disableAllButtons();
            return;
        }
    }

    for (int i = 0; i < 3; ++i) {
        if (!buttons[0][i]->text().isEmpty() &&
            buttons[0][i]->text() == buttons[1][i]->text() &&
            buttons[1][i]->text() == buttons[2][i]->text()) {
            gameEnded = true;
            QMessageBox::information(this, "Игра окончена",
                                     QString("Игрок %1 победил!").arg(buttons[0][i]->text()));
            disableAllButtons();
            return;
        }
    }

    if (!buttons[0][0]->text().isEmpty() &&
        buttons[0][0]->text() == buttons[1][1]->text() &&
        buttons[1][1]->text() == buttons[2][2]->text()) {
        gameEnded = true;
        QMessageBox::information(this, "Игра окончена",
                                 QString("Игрок %1 победил!").arg(buttons[0][0]->text()));
        disableAllButtons();
        return;
    }

    if (!buttons[0][2]->text().isEmpty() &&
        buttons[0][2]->text() == buttons[1][1]->text() &&
        buttons[1][1]->text() == buttons[2][0]->text()) {
        gameEnded = true;
        QMessageBox::information(this, "Игра окончена",
                                 QString("Игрок %1 победил!").arg(buttons[0][2]->text()));
        disableAllButtons();
        return;
    }

    if (moveCount == 9) {
        gameEnded = true;
        QMessageBox::information(this, "Игра окончена", "Ничья!");
        disableAllButtons();
    }
}

void TicTacToe::disableAllButtons()
{
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            buttons[i][j]->setEnabled(false);
}

void TicTacToe::enableAllButtons()
{
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j) {
            buttons[i][j]->setEnabled(true);
            buttons[i][j]->setText("");
        }
}

void TicTacToe::restartGame()
{
    enableAllButtons();
    currentPlayer = "X";
    gameEnded = false;
    moveCount = 0;
}
