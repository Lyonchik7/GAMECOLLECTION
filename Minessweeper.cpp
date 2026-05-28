#include "Minessweeper.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QScreen>
#include <QGuiApplication>
#include <QSizePolicy>
#include <QEvent>
#include <cmath>

Minessweeper::Minessweeper(QWidget *parent)
    : QWidget(parent), timer(new QTimer(this))
{
    setWindowTitle("💣 САПЁР");

    resize(900, 760);
    setMinimumSize(700, 650);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    stack = new QStackedWidget(this);
    mainLayout->addWidget(stack);

    createMenu();
    stack->addWidget(menuWidget);
    stack->setCurrentWidget(menuWidget);

    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    move((screenGeometry.width() - width()) / 2,
         (screenGeometry.height() - height()) / 2);
}

void Minessweeper::createMenu()
{
    menuWidget = new QWidget();
    QVBoxLayout *menuLayout = new QVBoxLayout(menuWidget);
    menuLayout->setContentsMargins(40, 20, 40, 20);

    QLabel *title = new QLabel("💣 САПЁР");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(R"(
        font-size: 36px;
        font-weight: bold;
        color: white;
        margin-top: 20px;
    )");

    QLabel *subtitle = new QLabel("Выберите настройки игры");
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setStyleSheet(R"(
        font-size: 18px;
        color: #DDDDDD;
        margin-bottom: 20px;
    )");

    difficultyBox = new QComboBox();
    difficultyBox->addItem("🟢 Лёгкий");
    difficultyBox->addItem("🟡 Средний");
    difficultyBox->addItem("🔴 Сложный");
    difficultyBox->setStyleSheet(R"(
        QComboBox {
            background: white;
            padding: 12px;
            border-radius: 10px;
            font-size: 18px;
        }
    )");

    difficultyDescription = new QLabel();
    difficultyDescription->setWordWrap(true);
    difficultyDescription->setStyleSheet(R"(
        color: white;
        font-size: 15px;
        padding: 10px;
    )");

    themeBox = new QComboBox();
    themeBox->addItem("🌞 День");
    themeBox->addItem("🌙 Ночь");
    themeBox->setStyleSheet(R"(
        QComboBox {
            background: white;
            padding: 12px;
            border-radius: 10px;
            font-size: 18px;
        }
    )");

    themeDescription = new QLabel();
    themeDescription->setWordWrap(true);
    themeDescription->setStyleSheet(R"(
        color: white;
        font-size: 15px;
        padding: 10px;
    )");

    QPushButton *startBtn = new QPushButton("▶ НАЧАТЬ ИГРУ");
    startBtn->setFixedHeight(60);
    startBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #4CAF50;
            color: white;
            border-radius: 14px;
            font-size: 22px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #66BB6A;
        }
    )");

    connect(startBtn, &QPushButton::clicked, this, &Minessweeper::startGame);
    connect(difficultyBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Minessweeper::updateDescriptions);
    connect(themeBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Minessweeper::updateDescriptions);

    hintLabel = new QLabel(
        "💡 ЛКМ — открыть клетку\n"
        "💡 ПКМ — поставить флаг\n"
        "💡 Первый клик безопасен");

    hintLabel->setAlignment(Qt::AlignCenter);
    hintLabel->setWordWrap(true);
    hintLabel->setStyleSheet(R"(
        color: white;
        font-size: 16px;
        margin-top: 20px;
    )");

    menuLayout->addWidget(title);
    menuLayout->addWidget(subtitle);
    menuLayout->addWidget(difficultyBox);
    menuLayout->addWidget(difficultyDescription);
    menuLayout->addSpacing(10);
    menuLayout->addWidget(themeBox);
    menuLayout->addWidget(themeDescription);
    menuLayout->addSpacing(20);
    menuLayout->addWidget(startBtn);
    menuLayout->addStretch();
    menuLayout->addWidget(hintLabel);

    updateDescriptions();
    applyMenuTheme();
}

void Minessweeper::applyMenuTheme()
{
    if (themeBox->currentIndex() == 0)
    {
        menuWidget->setStyleSheet(R"(
            QWidget {
                background-color: #87CEEB;
                color: white;
                font-family: Segoe UI;
            }

            QLabel {
                font-size: 18px;
                font-weight: bold;
            }

            QComboBox {
                background: white;
                padding: 12px;
                border-radius: 10px;
                font-size: 18px;
                color: black;
            }
        )");
    }
    else
    {
        menuWidget->setStyleSheet(R"(
            QWidget {
                background-color: #111111;
                color: white;
                font-family: Segoe UI;
            }

            QLabel {
                font-size: 18px;
                font-weight: bold;
            }

            QComboBox {
                background: #2B2B2B;
                padding: 12px;
                border-radius: 10px;
                font-size: 18px;
                color: white;
            }
        )");
    }
}

void Minessweeper::updateDescriptions()
{
    switch (difficultyBox->currentIndex())
    {
    case 0:
        difficultyDescription->setText(
            "🟢 Лёгкий режим:\n"
            "Поле 10x10 и 15 мин.\n"
            "Подходит новичкам.");
        break;
    case 1:
        difficultyDescription->setText(
            "🟡 Средний режим:\n"
            "Поле 12x12 и 25 мин.\n"
            "Требует внимательности.");
        break;
    case 2:
        difficultyDescription->setText(
            "🔴 Сложный режим:\n"
            "Поле 16x16 и 45 мин.\n"
            "Настоящее испытание.");
        break;
    }

    switch (themeBox->currentIndex())
    {
    case 0:
        themeDescription->setText(
            "🌞 Светлая дневная тема.\n"
            "Яркий интерфейс.");
        break;
    case 1:
        themeDescription->setText(
            "🌙 Тёмная ночная тема.\n"
            "Комфортна для глаз.");
        break;
    }

    applyMenuTheme();
}

void Minessweeper::startGame()
{
    if (gameWidget)
    {
        stack->removeWidget(gameWidget);
        gameWidget->deleteLater();
        gameWidget = nullptr;
    }

    if (difficultyBox->currentIndex() == 0)
    {
        ROWS = 10;
        COLS = 10;
        MINES = 15;
    }
    else if (difficultyBox->currentIndex() == 1)
    {
        ROWS = 12;
        COLS = 12;
        MINES = 25;
    }
    else
    {
        ROWS = 16;
        COLS = 16;
        MINES = 45;
    }

    applyTheme();

    gameWidget = new QWidget();
    gameWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *mainLayout = new QVBoxLayout(gameWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    QHBoxLayout *top = new QHBoxLayout();

    QLabel *mineText = new QLabel(QString("💣 %1").arg(MINES));
    flagsLabel = new QLabel("🚩 0");
    timerLabel = new QLabel("⏱ 0");

    QPushButton *restart = new QPushButton("🔄 Новая игра");
    restart->setFixedHeight(45);
    restart->setStyleSheet(R"(
        QPushButton {
            background-color: #2196F3;
            color: white;
            border-radius: 10px;
            font-size: 16px;
            font-weight: bold;
            padding: 8px 15px;
        }
        QPushButton:hover {
            background-color: #42A5F5;
        }
    )");

    QPushButton *backBtn = new QPushButton("⬅ Назад");
    backBtn->setFixedHeight(45);
    backBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #FF7043;
            color: white;
            border-radius: 10px;
            font-size: 16px;
            font-weight: bold;
            padding: 8px 15px;
        }
        QPushButton:hover {
            background-color: #FF8A65;
        }
    )");

    connect(restart, &QPushButton::clicked, this, &Minessweeper::restartGame);

    connect(backBtn, &QPushButton::clicked, this, [this]() {
        if (timer)
            timer->stop();

        if (gameWidget)
        {
            stack->removeWidget(gameWidget);
            gameWidget->deleteLater();
            gameWidget = nullptr;
        }

        stack->setCurrentWidget(menuWidget);
        applyMenuTheme();
    });

    top->addWidget(mineText);
    top->addSpacing(10);
    top->addWidget(flagsLabel);
    top->addSpacing(10);
    top->addWidget(timerLabel);
    top->addStretch();
    top->addWidget(restart);
    top->addWidget(backBtn);

    mainLayout->addLayout(top);

    QGridLayout *grid = new QGridLayout();
    grid->setSpacing(2);
    grid->setContentsMargins(5, 5, 5, 5);

    buttons.clear();
    buttons.resize(ROWS);

    for (int i = 0; i < ROWS; i++)
    {
        buttons[i].resize(COLS);

        for (int j = 0; j < COLS; j++)
        {
            QPushButton *btn = new QPushButton();

            btn->setMinimumSize(28, 28);
            btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            btn->setProperty("row", i);
            btn->setProperty("col", j);
            btn->installEventFilter(this);
            btn->setStyleSheet(R"(
                QPushButton {
                    background-color: #43A047;
                    border: 2px solid #2E7D32;
                    border-radius: 8px;
                    font-size: 16px;
                    font-weight: bold;
                    color: white;
                }
                QPushButton:hover {
                    background-color: #66BB6A;
                }
            )");

            grid->addWidget(btn, i, j);
            buttons[i][j] = btn;
        }
    }

    mainLayout->addLayout(grid);

    stack->addWidget(gameWidget);
    stack->setCurrentWidget(gameWidget);

    timer->stop();
    seconds = 0;
    timerLabel->setText("⏱ 0");

    restartGame();
}

void Minessweeper::applyTheme()
{
    if (themeBox->currentIndex() == 0)
        backgroundColor = "#87CEEB";
    else
        backgroundColor = "#111111";

    setStyleSheet(QString(R"(
        QWidget {
            background-color: %1;
            color: white;
            font-family: Segoe UI;
        }

        QLabel {
            font-size: 18px;
            font-weight: bold;
        }
    )").arg(backgroundColor));
}

void Minessweeper::restartGame()
{
    if (!gameWidget)
        return;

    mines = QVector<QVector<bool>>(ROWS, QVector<bool>(COLS, false));
    numbers = QVector<QVector<int>>(ROWS, QVector<int>(COLS, 0));
    opened = QVector<QVector<bool>>(ROWS, QVector<bool>(COLS, false));
    flagged = QVector<QVector<bool>>(ROWS, QVector<bool>(COLS, false));

    flagsPlaced = 0;
    gameOver = false;
    firstClick = true;
    seconds = 0;

    if (timer)
        timer->stop();

    timerLabel->setText("⏱ 0");

    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            buttons[i][j]->setEnabled(true);
            buttons[i][j]->setText("");
            buttons[i][j]->setStyleSheet(R"(
                QPushButton {
                    background-color: #43A047;
                    border: 2px solid #2E7D32;
                    border-radius: 8px;
                    font-size: 16px;
                    font-weight: bold;
                    color: white;
                }
                QPushButton:hover {
                    background-color: #66BB6A;
                }
            )");
        }
    }

    updateFlags();
}

void Minessweeper::placeMines(int firstRow, int firstCol)
{
    int placed = 0;

    while (placed < MINES)
    {
        int r = QRandomGenerator::global()->bounded(ROWS);
        int c = QRandomGenerator::global()->bounded(COLS);

        bool safe = std::abs(r - firstRow) <= 1 && std::abs(c - firstCol) <= 1;

        if (!mines[r][c] && !safe)
        {
            mines[r][c] = true;
            placed++;
        }
    }

    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            if (mines[i][j])
                numbers[i][j] = -1;
            else
                numbers[i][j] = countAdjacent(i, j);
        }
    }
}

int Minessweeper::countAdjacent(int row, int col)
{
    int count = 0;

    for (int dr = -1; dr <= 1; dr++)
    {
        for (int dc = -1; dc <= 1; dc++)
        {
            int nr = row + dr;
            int nc = col + dc;

            if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS && mines[nr][nc])
                count++;
        }
    }

    return count;
}

bool Minessweeper::eventFilter(QObject *obj, QEvent *event)
{
    QPushButton *btn = qobject_cast<QPushButton*>(obj);
    if (!btn)
        return false;

    int row = btn->property("row").toInt();
    int col = btn->property("col").toInt();

    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *mouse = static_cast<QMouseEvent*>(event);

        if (mouse->button() == Qt::RightButton)
        {
            if (!opened[row][col] && !gameOver)
            {
                flagged[row][col] = !flagged[row][col];

                if (flagged[row][col])
                {
                    btn->setText("🚩");
                    flagsPlaced++;
                }
                else
                {
                    btn->setText("");
                    flagsPlaced--;
                }

                updateFlags();
            }

            return true;
        }

        if (mouse->button() == Qt::LeftButton)
        {
            if (flagged[row][col] || gameOver)
                return true;

            if (firstClick)
            {
                placeMines(row, col);
                firstClick = false;
                timer->start(1000);
            }

            if (mines[row][col])
            {
                gameOver = true;
                timer->stop();

                explodeAnimation(row, col);
                shakeWindow();

                for (int i = 0; i < ROWS; i++)
                {
                    for (int j = 0; j < COLS; j++)
                    {
                        if (mines[i][j])
                        {
                            buttons[i][j]->setText("💣");
                            buttons[i][j]->setEnabled(false);
                            buttons[i][j]->setStyleSheet(R"(
                                QPushButton {
                                    background-color: #2C2C2C;
                                    border: 3px solid #FF3333;
                                    border-radius: 8px;
                                    font-size: 18px;
                                    color: white;
                                }
                            )");
                        }
                    }
                }

                buttons[row][col]->setText("💥");
                buttons[row][col]->setStyleSheet(R"(
                    QPushButton {
                        background-color: #FF0000;
                        border: 4px solid #FFD700;
                        border-radius: 8px;
                        font-size: 22px;
                        color: white;
                    }
                )");

                QMessageBox::critical(this, "💀 GAME OVER", "Вы подорвались на мине!");
                return true;
            }

            openCell(row, col);
            checkWin();
            return true;
        }
    }

    return false;
}

void Minessweeper::openCell(int row, int col)
{
    if (row < 0 || row >= ROWS || col < 0 || col >= COLS)
        return;

    if (opened[row][col] || flagged[row][col])
        return;

    opened[row][col] = true;

    QPushButton *btn = buttons[row][col];
    btn->setEnabled(false);
    btn->setStyleSheet(R"(
        background-color: white;
        border-radius: 8px;
        color: black;
        font-size: 18px;
        font-weight: bold;
    )");

    if (numbers[row][col] > 0)
    {
        btn->setText(QString::number(numbers[row][col]));
    }
    else
    {
        for (int dr = -1; dr <= 1; dr++)
        {
            for (int dc = -1; dc <= 1; dc++)
            {
                if (dr != 0 || dc != 0)
                    openCell(row + dr, col + dc);
            }
        }
    }
}

void Minessweeper::checkWin()
{
    int safe = 0;

    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            if (!mines[i][j] && opened[i][j])
                safe++;
        }
    }

    if (safe == ROWS * COLS - MINES)
    {
        timer->stop();
        QMessageBox::information(this, "🏆 ПОБЕДА", "Вы нашли все мины!");
    }
}

void Minessweeper::updateFlags()
{
    if (flagsLabel)
        flagsLabel->setText(QString("🚩 %1").arg(flagsPlaced));
}

void Minessweeper::explodeAnimation(int row, int col)
{
    QPushButton *btn = buttons[row][col];
    QPropertyAnimation *anim = new QPropertyAnimation(btn, "geometry");
    QRect start = btn->geometry();

    anim->setDuration(300);
    anim->setKeyValueAt(0, start);
    anim->setKeyValueAt(0.5, QRect(start.x() - 8, start.y() - 8, start.width() + 16, start.height() + 16));
    anim->setKeyValueAt(1, start);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void Minessweeper::shakeWindow()
{
    QPoint original = pos();
    QPropertyAnimation *anim = new QPropertyAnimation(this, "pos");

    anim->setDuration(400);
    anim->setKeyValueAt(0.0, original);
    anim->setKeyValueAt(0.1, original + QPoint(-10, 0));
    anim->setKeyValueAt(0.2, original + QPoint(10, 0));
    anim->setKeyValueAt(0.3, original + QPoint(-10, 0));
    anim->setKeyValueAt(0.4, original + QPoint(10, 0));
    anim->setKeyValueAt(1.0, original);

    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
