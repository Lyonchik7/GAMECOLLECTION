#ifndef MINESSWEEPER_H
#define MINESSWEEPER_H

#include <QWidget>
#include <QVector>

class QPushButton;
class QLabel;

class Minessweeper : public QWidget
{
    Q_OBJECT

public:
    explicit Minessweeper(QWidget *parent = nullptr);

private slots:
    void onLeftClick();
    void onRightClick();
    void restartGame();

private:
    void placeMines(int firstRow, int firstCol);
    int countAdjacentMines(int row, int col);
    void openCell(int row, int col);
    void checkWin();
    void updateFlagsLabel();

    static const int ROWS = 9;
    static const int COLS = 9;
    static const int MINES_COUNT = 10;

    QVector<QVector<QPushButton*>> buttons;
    QVector<QVector<bool>> mines;
    QVector<QVector<int>> numbers;
    QVector<QVector<bool>> opened;
    QVector<QVector<bool>> flagged;

    int flagsPlaced;
    bool gameOver;
    bool firstClick;

    QLabel *flagsLabel;
};

#endif // MINESSWEEPER_H
