#ifndef TICTACTOE_H
#define TICTACTOE_H

#include <QWidget>
#include <QVector>

class QPushButton;

class TicTacToe : public QWidget
{
    Q_OBJECT

public:
    explicit TicTacToe(QWidget *parent = nullptr);

private slots:
    void onButtonClick();
    void restartGame();

private:
    void checkGameState();
    void disableAllButtons();
    void enableAllButtons();

    QVector<QVector<QPushButton*>> buttons;
    QString currentPlayer;
    bool gameEnded;
    int moveCount;
};

#endif // TICTACTOE_H
