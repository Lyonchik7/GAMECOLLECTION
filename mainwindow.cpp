#include "mainwindow.h"
#include "TicTacToe.h"
#include "Snake.h"
#include "Minessweeper.h"
#include "Game2048.h"
#include "SimpleMario.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QScreen>
#include <QEvent>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    centerWindow();

    setStyleSheet(R"(
        QMainWindow { background-color: #2C3E50; }
        QLabel#titleLabel {
            color: #ECF0F1;
            font-size: 28px;
            font-weight: bold;
            padding: 20px;
        }
        QLabel#subtitleLabel {
            color: #BDC3C7;
            font-size: 14px;
        }
        QLabel#statusLabel {
            background-color: #34495E;
            color: #ECF0F1;
            padding: 8px;
            font-size: 12px;
            border-radius: 5px;
        }
        QPushButton {
            border: none;
            border-radius: 8px;
            font-weight: bold;
            padding: 15px;
            color: white;
            font-size: 12px;
            min-width: 200px;
        }
        QPushButton:hover {
            opacity: 0.9;
            transform: scale(1.02);
        }
        QPushButton:pressed {
            opacity: 0.8;
            padding-top: 16px;
            padding-bottom: 14px;
        }
    )");
}

MainWindow::~MainWindow() {}

void MainWindow::centerWindow()
{
    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        int x = (screenGeometry.width() - width()) / 2;
        int y = (screenGeometry.height() - height()) / 2;
        move(x, y);
    }
}

void MainWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(30, 20, 30, 20);

    QLabel *titleLabel = new QLabel("🎮 КОЛЛЕКЦИЯ ИГР 🎮");
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    QLabel *subtitleLabel = new QLabel("Выбери игру и начинай!");
    subtitleLabel->setObjectName("subtitleLabel");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(subtitleLabel);

    mainLayout->addSpacing(30);

    buttonsWidget = new QWidget();
    buttonsLayout = new QVBoxLayout(buttonsWidget);
    buttonsLayout->setSpacing(15);

    mainLayout->addWidget(buttonsWidget);

    createGameButton("❌ КРЕСТИКИ-НОЛИКИ ⭕",
                     "Игра в крестики-нолики против друга",
                     "#E74C3C", SLOT(openTicTacToe()));

    createGameButton("🐍 ЗМЕЙКА 🐍",
                     "Классическая змейка - собирай еду и расти",
                     "#27AE60", SLOT(openSnake()));

    createGameButton("💣 САПЁР 💣",
                     "Найди все мины, не подорвись!",
                     "#F39C12", SLOT(openMinesweeper()));

    createGameButton("🔢 2048 🔢",
                     "Соединяй числа, чтобы получить 2048",
                     "#3498DB", SLOT(open2048()));

    createGameButton("👨 СУПЕР МАРИО (упрощенно) 👨",
                     "Упрощенный платформер в стиле Марио",
                     "#9B59B6", SLOT(openMario()));

    mainLayout->addStretch();

    statusLabel = new QLabel("👆 Наведи на кнопку для подсказки");
    statusLabel->setObjectName("statusLabel");
    statusLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(statusLabel);

    setFixedSize(650, 550);
}

void MainWindow::createGameButton(const QString &text, const QString &toolTip,
                                  const QString &color, const char *slot)
{
    QPushButton *btn = new QPushButton(text);
    btn->setStyleSheet(QString("QPushButton { background-color: %1; }").arg(color));
    btn->setToolTip(toolTip);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setProperty("gameTip", toolTip);
    btn->installEventFilter(this);

    connect(btn, SIGNAL(clicked()), this, slot);

    if (buttonsLayout) {
        buttonsLayout->addWidget(btn);
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Enter) {
        QPushButton *btn = qobject_cast<QPushButton*>(obj);
        if (btn && statusLabel) {
            QString tip = btn->property("gameTip").toString();
            if (!tip.isEmpty()) {
                statusLabel->setText("👉 " + tip);
            }
        }
    }
    else if (event->type() == QEvent::Leave) {
        QPushButton *btn = qobject_cast<QPushButton*>(obj);
        if (btn && statusLabel) {
            statusLabel->setText("👆 Наведи на кнопку для подсказки");
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::openTicTacToe()
{
    TicTacToe *game = new TicTacToe();
    game->setAttribute(Qt::WA_DeleteOnClose);
    game->show();
    statusLabel->setText("✅ Игра 'Крестики-нолики' запущена");
}

void MainWindow::openSnake()
{
    Snake *game = new Snake();
    game->setAttribute(Qt::WA_DeleteOnClose);
    game->show();
    statusLabel->setText("✅ Игра 'Змейка' запущена");
}

void MainWindow::openMinesweeper()
{
    Minessweeper *game = new Minessweeper();
    game->setAttribute(Qt::WA_DeleteOnClose);
    game->show();
    statusLabel->setText("✅ Игра 'Сапёр' запущена");
}

void MainWindow::open2048()
{
    Game2048 *game = new Game2048();
    game->setAttribute(Qt::WA_DeleteOnClose);
    game->show();
    statusLabel->setText("✅ Игра '2048' запущена");
}

void MainWindow::openMario()
{
    SimpleMario *game = new SimpleMario(1);
    game->setAttribute(Qt::WA_DeleteOnClose);
    game->show();
    game->startGame();
    statusLabel->setText("✅ Игра 'Марио' запущена");
}
