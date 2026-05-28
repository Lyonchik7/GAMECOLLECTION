#ifndef MINESSWEEPER_H
#define MINESSWEEPER_H

#include <QWidget>
#include <QPushButton>
#include <QVector>
#include <QLabel>
#include <QTimer>
#include <QComboBox>
#include <QStackedWidget>

class Minessweeper : public QWidget
{
public:
    explicit Minessweeper(QWidget *parent = nullptr);

private:
    int ROWS = 0;
    int COLS = 0;
    int MINES = 0;

    QVector<QVector<QPushButton*>> buttons;

    QVector<QVector<bool>> mines;
    QVector<QVector<int>> numbers;
    QVector<QVector<bool>> opened;
    QVector<QVector<bool>> flagged;

    QLabel *flagsLabel = nullptr;
    QLabel *timerLabel = nullptr;
    QLabel *hintLabel = nullptr;

    QLabel *difficultyDescription = nullptr;
    QLabel *themeDescription = nullptr;

    QComboBox *difficultyBox = nullptr;
    QComboBox *themeBox = nullptr;

    QStackedWidget *stack = nullptr;
    QWidget *menuWidget = nullptr;
    QWidget *gameWidget = nullptr;

    int flagsPlaced = 0;
    bool gameOver = false;
    bool firstClick = true;

    QTimer *timer = nullptr;
    int seconds = 0;

    QString backgroundColor;

    void createMenu();
    void startGame();
    void applyTheme();
    void applyMenuTheme();
    void restartGame();
    void placeMines(int firstRow, int firstCol);
    int countAdjacent(int row, int col);
    void openCell(int row, int col);
    void checkWin();
    void updateFlags();
    void explodeAnimation(int row, int col);
    void shakeWindow();
    void updateDescriptions();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif
