#ifndef TICTACTOE_H
#define TICTACTOE_H

#include <QVector>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QTimer>

class TicTacToe : public QWidget
{
    Q_OBJECT

public:
    explicit TicTacToe(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onButtonClick();
    void restartGame();
    void toggleTheme();
    void switchMode();
    void updateTimerDisplay();
    void timeOut();

private:
    void checkGameState();
    void disableAllButtons();
    void enableAllButtons();
    void highlightWinningButtons(const QVector<QPushButton*> &winningBtns);
    void showWinnerMessage(const QString &winnerText);
    void updateScoreDisplay();
    void stopTimer();
    void startTimerForCurrentPlayer();

    QVector<QVector<QPushButton*>> buttons;

    QString currentPlayer;
    bool gameEnded;
    int moveCount;
    int winningRow1, winningCol1, winningRow2, winningCol2;

    int scoreX;
    int scoreO;
    QLabel *scoreLabel;
    QLabel *timerLabel;
    QPushButton *themeButton;
    QPushButton *modeButton;
    QPushButton *restartBtn;

    bool isDarkTheme;
    bool isTurboMode;
    int timeLimit;
    QTimer *gameTimer;
    int remainingTime;
};

#endif // TICTACTOE_H
