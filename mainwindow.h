#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>

class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void openTicTacToe();
    void openSnake();
    void openMinesweeper();
    void open2048();
    void openMario();

private:
    void setupUI();
    void centerWindow();
    void createGameButton(const QString &text, const QString &toolTip,
                          const QString &color, const char *slot);

    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QWidget *buttonsWidget;
    QVBoxLayout *buttonsLayout;
    QLabel *statusLabel;
};

#endif // MAINWINDOW_H
