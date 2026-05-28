#include "SimpleMario.h"
#include <QPainter>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QTimer>
#include <QGraphicsDropShadowEffect>
#include <QRandomGenerator>

const float SimpleMario::GRAVITY = 0.8f;
const float SimpleMario::JUMP_POWER = -14.0f;
const float SimpleMario::MOVE_SPEED = 6.0f;

// ---------- Конструктор, деструктор ----------
SimpleMario::SimpleMario(int startLevel, QWidget *parent)
    : QWidget(parent)
    , currentLevel(startLevel)
    , lives(3)
    , score(0)
    , coinsCollected(0)
    , gameActive(false)
    , levelComplete(false)
    , victoryShown(false)
    , respawnInvincibility(0)
    , starInvincibility(0)
    , cloudOffset(0)
    , coinAnimFrame(0)
    , starTwinkle(0)
    , flagWave(0)
    , showLevelNotify(true)
    , notifyAlpha(255)
    , characterColor(0)
    , walkFrame(0)
{
    setFixedSize(CANVAS_WIDTH, CANVAS_HEIGHT + 80);
    setFocusPolicy(Qt::StrongFocus);
    setStyleSheet("background-color: #0a1a2a;");

    topPanel = new QFrame(this);
    topPanel->setGeometry(0, 0, CANVAS_WIDTH, 60);
    topPanel->setStyleSheet(
        "QFrame { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #2c3e50, stop:1 #1a252f);"
        "border-bottom: 2px solid #f39c12; }"
        "QLabel { color: white; font-size: 18px; font-weight: bold; padding: 5px; }"
        );
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
    shadow->setBlurRadius(15);
    shadow->setOffset(0, 3);
    topPanel->setGraphicsEffect(shadow);

    QHBoxLayout *panelLayout = new QHBoxLayout(topPanel);
    panelLayout->setContentsMargins(20, 0, 20, 0);

    scoreLabel = new QLabel("⭐ Счёт: 0");
    livesLabel = new QLabel("❤️ Жизни: 3");
    levelLabel = new QLabel("🏰 Уровень: 1");
    scoreLabel->setStyleSheet("color: #f1c40f; background: transparent;");
    livesLabel->setStyleSheet("color: #e74c3c; background: transparent;");
    levelLabel->setStyleSheet("color: #2ecc71; background: transparent;");

    panelLayout->addWidget(scoreLabel);
    panelLayout->addWidget(livesLabel);
    panelLayout->addWidget(levelLabel);
    panelLayout->addStretch();

    QPushButton *menuBtn = new QPushButton("🏠 Меню");
    menuBtn->setStyleSheet(
        "QPushButton { background-color: #e67e22; border: none; border-radius: 5px; "
        "padding: 5px 15px; color: white; font-weight: bold; }"
        "QPushButton:hover { background-color: #f39c12; }"
        );
    connect(menuBtn, &QPushButton::clicked, this, &SimpleMario::showPauseMenu);
    panelLayout->addWidget(menuBtn);

    // Меню паузы
    pauseOverlay = new QWidget(this);
    pauseOverlay->setGeometry(0, 60, CANVAS_WIDTH, CANVAS_HEIGHT);
    pauseOverlay->setStyleSheet("background-color: rgba(0,0,0,220);");
    QVBoxLayout *pauseLayout = new QVBoxLayout(pauseOverlay);
    pauseLayout->setAlignment(Qt::AlignCenter);
    pauseLayout->setSpacing(15);

    resumeBtn = new QPushButton("▶ Продолжить");
    restartLevelBtn = new QPushButton("🔄 Заново уровень");
    restartGameBtn = new QPushButton("🏁 Начать сначала");
    quitBtn = new QPushButton("🏠 Выход в главное меню");

    QString btnStyle = "QPushButton { font-size: 18px; padding: 10px 30px; border-radius: 10px; background-color: #34495e; color: white; }"
                       "QPushButton:hover { background-color: #e67e22; }";
    resumeBtn->setStyleSheet(btnStyle);
    restartLevelBtn->setStyleSheet(btnStyle);
    restartGameBtn->setStyleSheet(btnStyle);
    quitBtn->setStyleSheet(btnStyle);

    pauseLayout->addWidget(resumeBtn);
    pauseLayout->addWidget(restartLevelBtn);
    pauseLayout->addWidget(restartGameBtn);
    pauseLayout->addWidget(quitBtn);

    connect(resumeBtn, &QPushButton::clicked, this, &SimpleMario::hidePauseMenu);
    connect(restartLevelBtn, &QPushButton::clicked, this, &SimpleMario::restartLevel);
    connect(restartGameBtn, &QPushButton::clicked, this, &SimpleMario::restartGame);
    connect(quitBtn, &QPushButton::clicked, this, &SimpleMario::quitToMainMenu);

    // Меню смерти
    deathOverlay = new QWidget(this);
    deathOverlay->setGeometry(0, 60, CANVAS_WIDTH, CANVAS_HEIGHT);
    deathOverlay->setStyleSheet("background-color: rgba(0,0,0,230);");
    QVBoxLayout *deathLayout = new QVBoxLayout(deathOverlay);
    deathLayout->setAlignment(Qt::AlignCenter);
    deathLayout->setSpacing(15);
    QLabel *deathTitle = new QLabel("💀 ВЫ УМЕРЛИ 💀");
    deathTitle->setStyleSheet("color: #e74c3c; font-size: 32px; font-weight: bold;");
    deathLayout->addWidget(deathTitle);
    deathRestartLevelBtn = new QPushButton("🔄 Заново уровень");
    deathRestartGameBtn = new QPushButton("🏁 Начать сначала");
    deathQuitBtn = new QPushButton("🏠 Выход в главное меню");
    deathRestartLevelBtn->setStyleSheet(btnStyle);
    deathRestartGameBtn->setStyleSheet(btnStyle);
    deathQuitBtn->setStyleSheet(btnStyle);
    deathLayout->addWidget(deathRestartLevelBtn);
    deathLayout->addWidget(deathRestartGameBtn);
    deathLayout->addWidget(deathQuitBtn);
    connect(deathRestartLevelBtn, &QPushButton::clicked, this, &SimpleMario::restartLevel);
    connect(deathRestartGameBtn, &QPushButton::clicked, this, &SimpleMario::restartGame);
    connect(deathQuitBtn, &QPushButton::clicked, this, &SimpleMario::quitToMainMenu);

    // Экран победы
    victoryOverlay = new QWidget(this);
    victoryOverlay->setGeometry(0, 60, CANVAS_WIDTH, CANVAS_HEIGHT);
    victoryOverlay->setStyleSheet("background-color: rgba(0,0,0,240);");
    QVBoxLayout *victoryLayout = new QVBoxLayout(victoryOverlay);
    victoryLayout->setAlignment(Qt::AlignCenter);
    victoryLayout->setSpacing(20);
    QLabel *victoryTitle = new QLabel("🏆 ВЫ ПОБЕДИЛИ! 🏆");
    victoryTitle->setStyleSheet("color: #f1c40f; font-size: 36px; font-weight: bold;");
    victoryLayout->addWidget(victoryTitle);
    QLabel *victoryScore = new QLabel();
    victoryScore->setStyleSheet("color: white; font-size: 24px;");
    victoryScore->setObjectName("victoryScore");
    victoryLayout->addWidget(victoryScore);
    victoryRestartBtn = new QPushButton("🎮 Играть снова");
    victoryQuitBtn = new QPushButton("🏠 Выход в главное меню");
    victoryRestartBtn->setStyleSheet(btnStyle);
    victoryQuitBtn->setStyleSheet(btnStyle);
    victoryLayout->addWidget(victoryRestartBtn);
    victoryLayout->addWidget(victoryQuitBtn);
    connect(victoryRestartBtn, &QPushButton::clicked, this, &SimpleMario::restartGame);
    connect(victoryQuitBtn, &QPushButton::clicked, this, &SimpleMario::quitToMainMenu);

    pauseOverlay->hide();
    deathOverlay->hide();
    victoryOverlay->hide();

    ground = QRectF(0, GROUND_Y, CANVAS_WIDTH, CANVAS_HEIGHT - GROUND_Y);
    lavaZone = QRectF(0,0,0,0);

    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &SimpleMario::gameLoop);
    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &SimpleMario::updateAnimation);
    animationTimer->start(120);
    cloudTimer = new QTimer(this);
    connect(cloudTimer, &QTimer::timeout, [this]() { cloudOffset = (cloudOffset + 1) % 200; update(); });
    cloudTimer->start(50);
    coinAnimTimer = new QTimer(this);
    connect(coinAnimTimer, &QTimer::timeout, [this]() { coinAnimFrame = (coinAnimFrame + 1) % 8; update(); });
    coinAnimTimer->start(100);
    starTimer = new QTimer(this);
    connect(starTimer, &QTimer::timeout, [this]() { starTwinkle = (starTwinkle + 1) % 4; update(); });
    starTimer->start(200);
    flagTimer = new QTimer(this);
    connect(flagTimer, &QTimer::timeout, [this]() { flagWave = (flagWave + 0.2f); update(); });
    flagTimer->start(50);
    notifyTimer = new QTimer(this);
    connect(notifyTimer, &QTimer::timeout, [this]() { notifyAlpha -= 15; if (notifyAlpha <= 0) { notifyTimer->stop(); showLevelNotify = false; } update(); });
    starInvTimer = new QTimer(this);
    connect(starInvTimer, &QTimer::timeout, [this]() { starInvincibility -= 0.05f; if (starInvincibility <= 0) { starInvincibility = 0; starInvTimer->stop(); } update(); });

    loadLevel(currentLevel);
}

SimpleMario::~SimpleMario()
{
    gameTimer->stop();
    animationTimer->stop();
    cloudTimer->stop();
    coinAnimTimer->stop();
    starTimer->stop();
    flagTimer->stop();
    notifyTimer->stop();
    starInvTimer->stop();
}

void SimpleMario::startGame()
{
    gameActive = true;
    levelComplete = false;
    victoryShown = false;
    gameTimer->start(16);
    setFocus();
    update();
}

void SimpleMario::stopGame() { gameTimer->stop(); }

void SimpleMario::showPauseMenu()
{
    if (!gameActive || levelComplete || victoryShown) return;
    gameActive = false;
    pauseOverlay->show();
    setFocus();
}
void SimpleMario::hidePauseMenu() { pauseOverlay->hide(); gameActive = true; setFocus(); }
void SimpleMario::showDeathMenu() { gameActive = false; deathOverlay->show(); setFocus(); }
void SimpleMario::showVictoryMenu()
{
    gameActive = false;
    victoryShown = true;
    QLabel *scoreLbl = victoryOverlay->findChild<QLabel*>("victoryScore");
    if (scoreLbl) scoreLbl->setText(QString("🏆 Ваш счёт: %1 🏆").arg(score));
    victoryOverlay->show();
    setFocus();
}
void SimpleMario::restartLevel()
{
    pauseOverlay->hide(); deathOverlay->hide(); victoryOverlay->hide();
    victoryShown = false;
    loadLevel(currentLevel);
    gameActive = true; levelComplete = false; setFocus();
}
void SimpleMario::restartGame()
{
    currentLevel = 1; lives = 3; score = 0; coinsCollected = 0; characterColor = 0;
    pauseOverlay->hide(); deathOverlay->hide(); victoryOverlay->hide();
    victoryShown = false;
    loadLevel(currentLevel);
    gameActive = true; levelComplete = false; setFocus();
}
void SimpleMario::quitToMainMenu()
{
    gameActive = false; gameTimer->stop();
    emit gameFinished(); close();
}
void SimpleMario::changeCharacter() { characterColor = 1 - characterColor; }

void SimpleMario::addDust(float x, float y)
{
    Particle p; p.x = x; p.y = y;
    p.vx = (QRandomGenerator::global()->generateDouble() - 0.5) * 2;
    p.vy = -2 - QRandomGenerator::global()->generateDouble() * 3;
    p.life = 1.0f; p.type = 0; particles.append(p);
}
void SimpleMario::addExplosion(float x, float y)
{
    for (int i = 0; i < 12; ++i) {
        Particle p; p.x = x; p.y = y;
        p.vx = (QRandomGenerator::global()->generateDouble() - 0.5) * 7;
        p.vy = (QRandomGenerator::global()->generateDouble() - 0.5) * 7 - 2;
        p.life = 1.0f; p.type = 1; particles.append(p);
    }
}
void SimpleMario::addCoinSparkle(float x, float y)
{
    for (int i = 0; i < 8; ++i) {
        Particle p; p.x = x; p.y = y;
        p.vx = (QRandomGenerator::global()->generateDouble() - 0.5) * 4;
        p.vy = (QRandomGenerator::global()->generateDouble() - 0.5) * 4 - 2;
        p.life = 0.8f; p.type = 2; particles.append(p);
    }
}

// ---------- Обработка событий ----------
void SimpleMario::keyPressEvent(QKeyEvent *event)
{
    if (pauseOverlay->isVisible() || deathOverlay->isVisible() || victoryOverlay->isVisible()) {
        if (event->key() == Qt::Key_Escape) {
            if (pauseOverlay->isVisible()) hidePauseMenu();
        }
        return;
    }
    if (gameActive && !levelComplete && !victoryShown) {
        pressedKeys.insert(event->key());
    }
    if (event->key() == Qt::Key_Escape && gameActive && !levelComplete && !victoryShown)
        showPauseMenu();
    if (event->key() == Qt::Key_C || event->key() == Qt::Key_V) {
        changeCharacter();
        update();
    }
}
void SimpleMario::keyReleaseEvent(QKeyEvent *event) { pressedKeys.remove(event->key()); }
void SimpleMario::resizeEvent(QResizeEvent *)
{
    topPanel->setGeometry(0, 0, width(), 60);
    pauseOverlay->setGeometry(0, 60, width(), height()-60);
    deathOverlay->setGeometry(0, 60, width(), height()-60);
    victoryOverlay->setGeometry(0, 60, width(), height()-60);
}

// ---------- Загрузка уровней (трубы удалены полностью) ----------
void SimpleMario::loadLevel(int level)
{
    platforms.clear(); coins.clear(); enemies.clear(); enemyDirs.clear(); enemyType.clear();
    pipes.clear(); torches.clear(); movingPlatforms.clear(); movingPlatformDirs.clear();
    lavaZone = QRectF(0,0,0,0); particles.clear();
    platforms.append(QRectF(0, GROUND_Y, CANVAS_WIDTH, 20));

    switch(level) {
    case 1:
        platforms.append(QRectF(200,400,100,20)); platforms.append(QRectF(500,370,120,20)); platforms.append(QRectF(300,320,80,20));
        for(int i=0;i<3;i++) coins.append(QRectF(220+i*150,375,25,25));
        enemies.append(QRectF(450,410,35,35)); enemyDirs.append(1); enemyType.append(0);
        flagPole = QRectF(750, GROUND_Y-70, 15,70);
        break;
    case 2:
        platforms.append(QRectF(150,420,120,20)); platforms.append(QRectF(400,380,100,20));
        platforms.append(QRectF(600,340,140,20)); platforms.append(QRectF(300,300,90,20));
        for(int i=0;i<4;i++) coins.append(QRectF(170+i*150,395,25,25));
        enemies.append(QRectF(250,400,35,35)); enemyDirs.append(-1); enemyType.append(0);
        enemies.append(QRectF(550,360,35,35)); enemyDirs.append(1); enemyType.append(0);
        flagPole = QRectF(750, GROUND_Y-70, 15,70);
        break;
    case 3:
        platforms.append(QRectF(100,430,100,20)); platforms.append(QRectF(300,390,80,20));
        platforms.append(QRectF(500,350,120,20)); platforms.append(QRectF(680,400,80,20));
        platforms.append(QRectF(250,310,100,20)); platforms.append(QRectF(550,270,100,20));
        for(int i=0;i<10;i++) coins.append(QRectF(120+i*40,405,20,20));
        enemies.append(QRectF(350,370,35,35)); enemyDirs.append(1); enemyType.append(0);
        enemies.append(QRectF(600,330,35,35)); enemyDirs.append(-1); enemyType.append(0);
        enemies.append(QRectF(700,380,35,35)); enemyDirs.append(1); enemyType.append(0);
        for(int i=0;i<5;i++) torches.append(QPointF(100+i*150, GROUND_Y-30));
        flagPole = QRectF(750, GROUND_Y-70, 15,70);
        break;
    case 4:
        platforms.append(QRectF(50,440,150,20)); platforms.append(QRectF(300,390,180,20));
        platforms.append(QRectF(600,420,100,20)); platforms.append(QRectF(200,340,100,20));
        platforms.append(QRectF(500,290,120,20)); platforms.append(QRectF(700,350,70,20));
        for(int i=0;i<6;i++) coins.append(QRectF(80+i*100,415,25,25));
        enemies.append(QRectF(400,370,35,35)); enemyDirs.append(1); enemyType.append(0);
        enemies.append(QRectF(650,400,35,35)); enemyDirs.append(-1); enemyType.append(0);
        enemies.append(QRectF(250,320,35,35)); enemyDirs.append(1); enemyType.append(0);
        for(int i=0;i<6;i++) torches.append(QPointF(50+i*130, GROUND_Y-30));
        flagPole = QRectF(750, GROUND_Y-70, 15,70);
        break;
    case 5:
        platforms.append(QRectF(0,450,200,20)); platforms.append(QRectF(250,400,100,20));
        platforms.append(QRectF(450,370,80,20)); platforms.append(QRectF(600,320,100,20));
        platforms.append(QRectF(150,330,100,20)); platforms.append(QRectF(350,270,120,20));
        platforms.append(QRectF(680,420,80,20));
        for(int i=0;i<8;i++) coins.append(QRectF(30+i*35,425,20,20));
        coins.append(QRectF(270,375,25,25)); coins.append(QRectF(470,345,25,25));
        coins.append(QRectF(620,295,25,25)); coins.append(QRectF(170,305,25,25));
        coins.append(QRectF(370,245,25,25));
        enemies.append(QRectF(300,380,35,35)); enemyDirs.append(1); enemyType.append(0);
        enemies.append(QRectF(500,350,35,35)); enemyDirs.append(-1); enemyType.append(0);
        enemies.append(QRectF(650,300,35,35)); enemyDirs.append(1); enemyType.append(0);
        enemies.append(QRectF(150,310,35,35)); enemyDirs.append(-1); enemyType.append(0);
        flagPole = QRectF(750, GROUND_Y-70, 15,70);
        break;
    case 6:
        platforms.append(QRectF(100,440,100,20)); platforms.append(QRectF(350,390,120,20));
        platforms.append(QRectF(550,350,100,20)); platforms.append(QRectF(200,310,80,20));
        platforms.append(QRectF(650,400,80,20));
        for(int i=0;i<10;i++) coins.append(QRectF(120+i*60,405,20,20));
        enemies.append(QRectF(300,370,40,30)); enemyDirs.append(1); enemyType.append(1);
        enemies.append(QRectF(500,330,40,30)); enemyDirs.append(-1); enemyType.append(1);
        enemies.append(QRectF(650,380,35,35)); enemyDirs.append(1); enemyType.append(0);
        flagPole = QRectF(750, GROUND_Y-70, 15,70);
        break;
    case 7:
        for(int i=0;i<5;i++) movingPlatforms.append(QRectF(100+i*150, 350+ (i%2)*50, 80,20));
        for(int i=0;i<5;i++) movingPlatformDirs.append(1);
        for(int i=0;i<5;i++) coins.append(QRectF(500+i*40,300,25,25));
        enemies.append(QRectF(400,380,35,35)); enemyDirs.append(1); enemyType.append(0);
        enemies.append(QRectF(650,420,35,35)); enemyDirs.append(-1); enemyType.append(0);
        flagPole = QRectF(750, GROUND_Y-70, 15,70);
        break;
    case 8:
        platforms.append(QRectF(80,430,120,20)); platforms.append(QRectF(350,400,100,20));
        platforms.append(QRectF(550,370,140,20)); platforms.append(QRectF(200,330,90,20));
        for(int i=0;i<12;i++) coins.append(QRectF(50+i*60,410,20,20));
        enemies.append(QRectF(280,410,35,35)); enemyDirs.append(1); enemyType.append(0);
        enemies.append(QRectF(480,380,35,35)); enemyDirs.append(-1); enemyType.append(0);
        enemies.append(QRectF(680,350,35,35)); enemyDirs.append(1); enemyType.append(0);
        flagPole = QRectF(750, GROUND_Y-70, 15,70);
        break;
    case 9:
        platforms.append(QRectF(200,420,100,20)); platforms.append(QRectF(450,380,120,20)); platforms.append(QRectF(650,340,100,20));
        for(int i=0;i<15;i++) coins.append(QRectF(100+i*45,400,20,20));
        enemies.append(QRectF(350,400,40,30)); enemyDirs.append(1); enemyType.append(1);
        enemies.append(QRectF(550,360,40,30)); enemyDirs.append(-1); enemyType.append(1);
        flagPole = QRectF(750, GROUND_Y-70, 15,70);
        break;
    case 10:
        platforms.append(QRectF(0,450,300,20)); platforms.append(QRectF(350,400,150,20));
        platforms.append(QRectF(600,350,150,20)); platforms.append(QRectF(200,310,100,20));
        platforms.append(QRectF(500,270,120,20));
        for(int i=0;i<20;i++) coins.append(QRectF(30+i*35,430,20,20));
        coins.append(QRectF(400,370,25,25)); coins.append(QRectF(650,320,25,25));
        enemies.append(QRectF(300,390,35,35)); enemyDirs.append(1); enemyType.append(0);
        enemies.append(QRectF(450,380,35,35)); enemyDirs.append(-1); enemyType.append(0);
        enemies.append(QRectF(600,335,35,35)); enemyDirs.append(1); enemyType.append(0);
        enemies.append(QRectF(700,430,35,35)); enemyDirs.append(-1); enemyType.append(0);
        enemies.append(QRectF(250,340,35,35)); enemyDirs.append(1); enemyType.append(0);
        enemies.append(QRectF(550,290,35,35)); enemyDirs.append(-1); enemyType.append(0);
        lavaZone = QRectF(0, GROUND_Y + 30, CANVAS_WIDTH, 20);
        flagPole = QRectF(750, GROUND_Y-70, 15,70);
        break;
    default:
        gameActive = false; gameTimer->stop(); emit gameFinished(); return;
    }

    for (const QRectF &mp : movingPlatforms) platforms.append(mp);

    playerRect = QRectF(50, GROUND_Y - PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE);
    velocityX = velocityY = 0; onGround = true; levelComplete = false;
    respawnInvincibility = 0; pressedKeys.clear();

    showLevelNotify = true; notifyAlpha = 255;
    if (notifyTimer->isActive()) notifyTimer->stop();
    notifyTimer->start(30);

    levelLabel->setText(QString("🏰 Уровень: %1").arg(level));
    scoreLabel->setText(QString("⭐ Счёт: %1").arg(score));
    livesLabel->setText(QString("❤️ Жизни: %1").arg(lives));
    update();
}

void SimpleMario::nextLevel()
{
    if(levelComplete) return;
    levelComplete = true;
    currentLevel++;
    if(currentLevel > 10) { showVictoryMenu(); return; }
    loadLevel(currentLevel);
    gameActive = true;
}

void SimpleMario::killPlayer()
{
    if(respawnInvincibility > 0 || starInvincibility > 0) return;
    lives--;
    livesLabel->setText(QString("❤️ Жизни: %1").arg(lives));
    if(lives <= 0) showDeathMenu();
    else {
        playerRect.moveTo(50, GROUND_Y - PLAYER_SIZE);
        velocityX = velocityY = 0; onGround = true;
        respawnInvincibility = 60;
    }
}

void SimpleMario::stompEnemy(int enemyIndex)
{
    if(enemyIndex >= 0 && enemyIndex < enemies.size()) {
        QRectF enemyRect = enemies[enemyIndex];
        int addScore = (enemyType[enemyIndex] == 0) ? 100 : 200;
        enemies.removeAt(enemyIndex); enemyDirs.removeAt(enemyIndex); enemyType.removeAt(enemyIndex);
        score += addScore;
        scoreLabel->setText(QString("⭐ Счёт: %1").arg(score));
        addExplosion(enemyRect.center().x(), enemyRect.center().y());
        velocityY = JUMP_POWER * 0.6f; onGround = false;
        addDust(playerRect.center().x(), playerRect.bottom());
    }
}

// ---------- Обновление игрока и коллизии ----------
void SimpleMario::updatePlayer()
{
    velocityX = 0;
    if(pressedKeys.contains(Qt::Key_Left)) velocityX = -MOVE_SPEED;
    if(pressedKeys.contains(Qt::Key_Right)) velocityX = MOVE_SPEED;
    if(pressedKeys.contains(Qt::Key_Space) && onGround && !levelComplete && !victoryShown) {
        velocityY = JUMP_POWER; onGround = false;
        addDust(playerRect.center().x(), playerRect.bottom());
    }
    velocityY += GRAVITY;

    playerRect.translate(velocityX, 0); checkHorizontalCollisions();
    playerRect.translate(0, velocityY); checkVerticalCollisions();

    if(playerRect.left() < 0) playerRect.moveLeft(0);
    if(playerRect.right() > CANVAS_WIDTH) playerRect.moveRight(CANVAS_WIDTH);
    if(playerRect.top() > CANVAS_HEIGHT + 50) killPlayer();
    if(lavaZone.height() > 0 && playerRect.intersects(lavaZone)) killPlayer();

    for(int i = 0; i < enemies.size(); ++i) {
        enemies[i].translate(enemyDirs[i] * 1.8f, 0);
        if(enemies[i].left() < 50 || enemies[i].right() > CANVAS_WIDTH - 50) enemyDirs[i] *= -1;
    }
    for(int i = 0; i < movingPlatforms.size(); ++i) {
        QRectF &mp = movingPlatforms[i];
        mp.translate(2 * movingPlatformDirs[i], 0);
        if(mp.left() < 100 || mp.right() > CANVAS_WIDTH - 100) movingPlatformDirs[i] *= -1;
        for(int j = 0; j < platforms.size(); ++j) {
            if(platforms[j].x() == mp.x() && platforms[j].y() == mp.y()) platforms[j] = mp;
        }
    }

    if(respawnInvincibility > 0) respawnInvincibility--;
    for(int i = 0; i < particles.size(); ++i) {
        particles[i].x += particles[i].vx; particles[i].y += particles[i].vy;
        particles[i].vy += 0.2f; particles[i].life -= 0.03f;
        if(particles[i].life <= 0 || particles[i].y > CANVAS_HEIGHT) { particles.removeAt(i); --i; }
    }
}

void SimpleMario::checkHorizontalCollisions()
{
    if(playerRect.intersects(ground) && velocityX != 0) {
        if(velocityX > 0 && playerRect.right() > ground.left()) playerRect.moveRight(ground.left());
        else if(velocityX < 0 && playerRect.left() < ground.right()) playerRect.moveLeft(ground.right());
    }
    for(const QRectF &p : platforms) {
        if(playerRect.intersects(p)) {
            if(velocityX > 0 && playerRect.right() > p.left()) playerRect.moveRight(p.left());
            else if(velocityX < 0 && playerRect.left() < p.right()) playerRect.moveLeft(p.right());
        }
    }
    for(const QRectF &pipe : pipes) {
        if(playerRect.intersects(pipe)) {
            if(velocityX > 0 && playerRect.right() > pipe.left()) playerRect.moveRight(pipe.left());
            else if(velocityX < 0 && playerRect.left() < pipe.right()) playerRect.moveLeft(pipe.right());
        }
    }
}

void SimpleMario::checkVerticalCollisions()
{
    onGround = false;
    if(playerRect.intersects(ground) && velocityY >= 0) {
        playerRect.moveBottom(ground.top()); velocityY = 0; onGround = true;
        addDust(playerRect.center().x(), playerRect.bottom());
    }
    for(const QRectF &p : platforms) {
        if(playerRect.intersects(p)) {
            if(velocityY >= 0 && playerRect.bottom() > p.top() && playerRect.top() < p.top()) {
                playerRect.moveBottom(p.top()); velocityY = 0; onGround = true;
                addDust(playerRect.center().x(), playerRect.bottom());
            } else if(velocityY < 0 && playerRect.top() < p.bottom() && playerRect.bottom() > p.bottom()) {
                playerRect.moveTop(p.bottom()); velocityY = 0;
            }
        }
    }
    for(const QRectF &pipe : pipes) {
        if(playerRect.intersects(pipe)) {
            if(velocityY >= 0 && playerRect.bottom() > pipe.top() && playerRect.top() < pipe.top()) {
                playerRect.moveBottom(pipe.top()); velocityY = 0; onGround = true;
                addDust(playerRect.center().x(), playerRect.bottom());
            } else if(velocityY < 0 && playerRect.top() < pipe.bottom() && playerRect.bottom() > pipe.bottom()) {
                playerRect.moveTop(pipe.bottom()); velocityY = 0;
            }
        }
    }
}

void SimpleMario::checkCoinCollision()
{
    for(int i = 0; i < coins.size(); ++i) {
        if(playerRect.intersects(coins[i])) {
            addCoinSparkle(coins[i].center().x(), coins[i].center().y());
            coins.removeAt(i); score += 10; coinsCollected++;
            scoreLabel->setText(QString("⭐ Счёт: %1").arg(score));
            if (coinsCollected % 100 == 0) {
                starInvincibility = 8.0f;
                starInvTimer->start(50);
            }
            --i;
        }
    }
}

void SimpleMario::checkEnemyCollision()
{
    if(respawnInvincibility > 0 || starInvincibility > 0) return;
    for(int i = 0; i < enemies.size(); ++i) {
        if(playerRect.intersects(enemies[i])) {
            if(velocityY > 0 && playerRect.bottom() <= enemies[i].top() + 15) { stompEnemy(i); return; }
            else { killPlayer(); return; }
        }
    }
}

void SimpleMario::checkFlagCollision()
{
    if(!levelComplete && playerRect.intersects(flagPole)) nextLevel();
}

void SimpleMario::gameLoop()
{
    if(!gameActive || levelComplete || pauseOverlay->isVisible() || deathOverlay->isVisible() || victoryOverlay->isVisible()) return;
    updatePlayer();
    checkCoinCollision();
    checkEnemyCollision();
    checkFlagCollision();
    update();
}

void SimpleMario::updateAnimation()
{
    if(!gameActive || levelComplete) return;
    if(onGround && qAbs(velocityX) > 0.1f && !pressedKeys.isEmpty()) walkFrame = 1 - walkFrame;
    else walkFrame = 0;
}

// ---------- Отрисовка (улучшенная) ----------
void SimpleMario::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    int theme = (currentLevel <= 2) ? 0 : (currentLevel <= 4 ? 1 : (currentLevel <= 7 ? 2 : (currentLevel <= 9 ? 3 : 4)));
    QLinearGradient skyGrad(0, 0, 0, GROUND_Y);
    switch(theme) {
    case 0: skyGrad.setColorAt(0, QColor(10,40,90)); skyGrad.setColorAt(0.4, QColor(60,120,200)); skyGrad.setColorAt(1, QColor(200,230,255)); break;
    case 1: skyGrad.setColorAt(0, QColor(20,20,40)); skyGrad.setColorAt(1, QColor(60,60,90)); break;
    case 2: skyGrad.setColorAt(0, QColor(60,20,20)); skyGrad.setColorAt(1, QColor(140,60,40)); break;
    case 3: skyGrad.setColorAt(0, QColor(100,200,255)); skyGrad.setColorAt(1, QColor(200,240,255)); break;
    case 4: skyGrad.setColorAt(0, QColor(10,10,30)); skyGrad.setColorAt(1, QColor(40,20,60)); break;
    }
    painter.fillRect(0, topPanel->height(), CANVAS_WIDTH, GROUND_Y - topPanel->height(), skyGrad);

    if(theme == 0 || theme == 4) drawStars(painter);
    if(theme != 1) drawClouds(painter);

    // Тени от облаков
    painter.setBrush(QColor(0,0,0,40));
    int shift = cloudOffset;
    painter.drawEllipse(100+shift, GROUND_Y-20, 60, 20);
    painter.drawEllipse(140+shift, GROUND_Y-18, 70, 22);
    painter.drawEllipse(180+shift, GROUND_Y-20, 50, 18);
    painter.drawEllipse(550-shift/2, GROUND_Y-22, 70, 20);
    painter.drawEllipse(600-shift/2, GROUND_Y-20, 80, 22);
    painter.drawEllipse(660-shift/2, GROUND_Y-22, 60, 18);

    drawHills(painter);

    QColor dirtColor = (theme == 1) ? QColor(70,50,40) : QColor(88,57,39);
    painter.fillRect(ground, dirtColor);
    QColor grassColor = (theme == 1) ? QColor(50,70,40) : QColor(70,130,30);
    painter.fillRect(0, GROUND_Y, CANVAS_WIDTH, 8, grassColor);
    painter.setPen(Qt::darkGreen);
    for(int i=0;i<20;i++) { painter.drawLine(i*40, GROUND_Y, i*40+12, GROUND_Y-10); painter.drawLine(i*40+20, GROUND_Y, i*40+32, GROUND_Y-8); }
    drawBushes(painter);

    for(const QRectF &p : platforms) if(p.y() < GROUND_Y && p.height() == 20) drawWoodenPlatform(painter, p);
    for(const QRectF &pipe : pipes) drawPipe(painter, pipe);
    for(const QPointF &pos : torches) drawTorch(painter, pos);
    if(lavaZone.height() > 0) drawLava(painter, lavaZone);

    for(const QRectF &c : coins) drawCoin(painter, c, coinAnimFrame);
    for(int i=0; i<enemies.size(); ++i) {
        if(enemyType[i]==0) drawGoomba(painter, enemies[i]);
        else drawSnail(painter, enemies[i]);
    }
    drawFlag(painter, flagPole);

    for(const Particle &p : particles) {
        if(p.type == 0) painter.setBrush(QColor(180,180,160, int(120 * p.life)));
        else if(p.type == 1) {
            QRadialGradient grad(p.x, p.y, 8 * p.life);
            grad.setColorAt(0, QColor(255,120,0, int(200 * p.life)));
            grad.setColorAt(1, QColor(255,50,0,0));
            painter.setBrush(grad);
            painter.drawEllipse(QPointF(p.x, p.y), 8 * p.life, 8 * p.life);
            continue;
        } else painter.setBrush(QColor(255,255,100, int(200 * p.life)));
        painter.drawEllipse(QPointF(p.x, p.y), 5 * p.life, 3 * p.life);
    }

    if(respawnInvincibility > 0 && (static_cast<int>(respawnInvincibility)/5)%2==0) painter.setOpacity(0.5);
    if(starInvincibility > 0) { painter.setBrush(QColor(255,255,0,100)); painter.drawEllipse(playerRect.center(), 25, 25); }
    drawMario(painter, playerRect);
    painter.setOpacity(1.0);

    if(showLevelNotify && notifyAlpha > 0) {
        painter.setOpacity(notifyAlpha / 255.0);
        QFont font("Arial", 32, QFont::Bold);
        painter.setFont(font);
        painter.setPen(Qt::white);
        painter.setBrush(QColor(0,0,0,150));
        QString text = QString("УРОВЕНЬ %1").arg(currentLevel);
        QRect textRect = painter.fontMetrics().boundingRect(text);
        textRect.moveCenter(QPoint(CANVAS_WIDTH/2, CANVAS_HEIGHT/2 - 50));
        painter.fillRect(textRect.adjusted(-20,-10,20,10), QColor(0,0,0,180));
        painter.setPen(QColor(255,215,0));
        painter.drawText(textRect, Qt::AlignCenter, text);
        painter.setOpacity(1.0);
    }
}

void SimpleMario::drawStars(QPainter &painter)
{
    painter.setPen(Qt::NoPen);
    static const QPointF stars[] = {{50,30},{130,55},{210,25},{320,70},{430,40},{560,60},{690,35},{760,50},{100,80},{490,85}};
    int alpha = 180 + (starTwinkle * 20);
    painter.setBrush(QColor(255,255,200,alpha));
    for(int i=0;i<10;i++) painter.drawEllipse(stars[i], 2.5,2.5);
}

void SimpleMario::drawClouds(QPainter &painter)
{
    painter.setBrush(QColor(255,255,245,200)); painter.setPen(Qt::NoPen);
    int shift = cloudOffset;
    painter.drawEllipse(100+shift,70,60,40); painter.drawEllipse(140+shift,50,70,50); painter.drawEllipse(180+shift,70,50,40);
    painter.drawEllipse(550-shift/2,90,70,45); painter.drawEllipse(600-shift/2,70,80,55); painter.drawEllipse(660-shift/2,90,60,40);
}

void SimpleMario::drawHills(QPainter &painter)
{
    painter.setBrush(QColor(50,90,50)); painter.setPen(Qt::NoPen);
    painter.drawEllipse(20, GROUND_Y-70,160,100); painter.drawEllipse(260, GROUND_Y-90,220,120); painter.drawEllipse(620, GROUND_Y-60,150,90);
}

void SimpleMario::drawBushes(QPainter &painter)
{
    painter.setBrush(QColor(30,70,25));
    painter.drawEllipse(80,GROUND_Y-18,55,30); painter.drawEllipse(115,GROUND_Y-25,45,35);
    painter.drawEllipse(400,GROUND_Y-15,65,28); painter.drawEllipse(435,GROUND_Y-22,50,32);
    painter.drawEllipse(650,GROUND_Y-18,60,30); painter.drawEllipse(685,GROUND_Y-25,45,35);
}

void SimpleMario::drawWoodenPlatform(QPainter &painter, const QRectF &rect)
{
    painter.fillRect(rect.x()+3, rect.y()+4, rect.width(), rect.height(), QColor(0,0,0,80));
    painter.fillRect(rect, QColor(150,95,55)); painter.setPen(Qt::black); painter.drawRect(rect);
    painter.setPen(QColor(100,60,30));
    for(int x = rect.x()+5; x < rect.x()+rect.width(); x+=20) painter.drawLine(x, rect.y(), x, rect.y()+rect.height());
    painter.setPen(Qt::darkGray); painter.drawLine(rect.x(), rect.y()+rect.height()/2, rect.x()+rect.width(), rect.y()+rect.height()/2);
    painter.setBrush(Qt::black);
    for(int x = rect.x()+10; x < rect.x()+rect.width(); x+=25) painter.drawEllipse(QPointF(x, rect.y()+rect.height()/2), 2,2);
}

void SimpleMario::drawPipe(QPainter &painter, const QRectF &rect)
{
    painter.fillRect(rect.x()+3, rect.y()+3, rect.width(), rect.height(), QColor(0,0,0,60));
    painter.fillRect(rect, QColor(50,150,50)); painter.setPen(Qt::black); painter.drawRect(rect);
    painter.fillRect(rect.x()-5, rect.y()-5, rect.width()+10,5, QColor(50,150,50));
    painter.fillRect(rect.x()-5, rect.y()-10, rect.width()+10,5, QColor(30,120,30));
    painter.drawRect(rect.x()-5, rect.y()-10, rect.width()+10,5);
    painter.drawRect(rect.x()-5, rect.y()-5, rect.width()+10,5);
    painter.setBrush(QColor(100,200,100,80)); painter.drawRect(rect.x()+5, rect.y()+5, rect.width()/3, rect.height()-10);
}

void SimpleMario::drawTorch(QPainter &painter, const QPointF &pos)
{
    painter.fillRect(pos.x(), pos.y(),5,30, QColor(100,70,40));
    painter.setBrush(QColor(255,100,0)); painter.drawEllipse(pos.x()-5, pos.y()-10,15,15);
    painter.setBrush(QColor(255,200,0)); painter.drawEllipse(pos.x()-3, pos.y()-12,10,10);
    painter.setBrush(QColor(255,255,200)); painter.drawEllipse(pos.x()-1, pos.y()-14,5,5);
}

void SimpleMario::drawLava(QPainter &painter, const QRectF &rect)
{
    painter.fillRect(rect, QColor(200,50,0));
    painter.setBrush(QColor(255,100,0,150));
    for(int i=0;i<10;i++) painter.drawEllipse(rect.x()+i*80+(coinAnimFrame%10), rect.y(),30,10);
    painter.setBrush(QColor(255,200,0,100));
    for(int i=0;i<10;i++) painter.drawEllipse(rect.x()+i*70+(coinAnimFrame%7), rect.y()-3,20,8);
}

void SimpleMario::drawMario(QPainter &painter, const QRectF &rect)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setBrush(QColor(0,0,0,100)); painter.drawEllipse(rect.center().x()-18, rect.bottom()-6,36,12);

    QColor shirtColor = (characterColor==0) ? QColor(200,40,40) : QColor(40,100,200);
    QColor hatColor = (characterColor==0) ? QColor(180,30,30) : QColor(30,80,180);

    painter.fillRect(rect.x(), rect.y()+rect.height()*0.4, rect.width(), rect.height()*0.6, QColor(25,50,180));
    painter.fillRect(rect.x(), rect.y(), rect.width(), rect.height()*0.45, shirtColor);
    painter.fillRect(rect.x()+rect.width()*0.2, rect.y()+rect.height()*0.15, rect.width()*0.6, rect.height()*0.3, QColor(255,210,160));
    painter.fillRect(rect.x()+rect.width()*0.1, rect.y()-rect.height()*0.15, rect.width()*0.8, rect.height()*0.25, hatColor);
    painter.fillRect(rect.x()+rect.width()*0.2, rect.y()-rect.height()*0.05, rect.width()*0.6, rect.height()*0.15, hatColor.lighter(120));

    painter.setBrush(Qt::white);
    painter.drawEllipse(QPointF(rect.x()+rect.width()*0.35, rect.y()+rect.height()*0.3),5,5);
    painter.drawEllipse(QPointF(rect.x()+rect.width()*0.65, rect.y()+rect.height()*0.3),5,5);
    painter.setBrush(Qt::black);
    painter.drawEllipse(QPointF(rect.x()+rect.width()*0.35+(walkFrame?2:0), rect.y()+rect.height()*0.3),2.5,2.5);
    painter.drawEllipse(QPointF(rect.x()+rect.width()*0.65+(walkFrame?2:0), rect.y()+rect.height()*0.3),2.5,2.5);

    painter.drawLine(rect.x()+rect.width()*0.3, rect.y()+rect.height()*0.4,
                     rect.x()+rect.width()*0.5, rect.y()+rect.height()*0.38);
    painter.drawLine(rect.x()+rect.width()*0.7, rect.y()+rect.height()*0.4,
                     rect.x()+rect.width()*0.5, rect.y()+rect.height()*0.38);

    painter.setBrush(Qt::yellow);
    painter.drawEllipse(QPointF(rect.x()+rect.width()*0.5, rect.y()+rect.height()*0.55),2.5,2.5);
    painter.drawEllipse(QPointF(rect.x()+rect.width()*0.5, rect.y()+rect.height()*0.7),2.5,2.5);

    int arm = (walkFrame==0) ? 6 : -4;
    painter.drawLine(rect.x()+rect.width()*0.2, rect.y()+rect.height()*0.5,
                     rect.x()+rect.width()*0.1+arm, rect.y()+rect.height()*0.65);
    painter.drawLine(rect.x()+rect.width()*0.8, rect.y()+rect.height()*0.5,
                     rect.x()+rect.width()*0.9-arm, rect.y()+rect.height()*0.65);
    painter.restore();
}

void SimpleMario::drawGoomba(QPainter &painter, const QRectF &rect)
{
    painter.setBrush(QColor(101,67,33)); painter.setPen(Qt::black); painter.drawEllipse(rect);
    painter.setBrush(QColor(70,40,20)); painter.drawRect(rect.x()+rect.width()*0.2, rect.y()+rect.height()*0.5, rect.width()*0.6, rect.height()*0.3);
    painter.setBrush(Qt::white);
    painter.drawEllipse(QPointF(rect.x()+rect.width()*0.35, rect.y()+rect.height()*0.35),4,4);
    painter.drawEllipse(QPointF(rect.x()+rect.width()*0.65, rect.y()+rect.height()*0.35),4,4);
    painter.setBrush(Qt::black);
    painter.drawEllipse(QPointF(rect.x()+rect.width()*0.35, rect.y()+rect.height()*0.35),2,2);
    painter.drawEllipse(QPointF(rect.x()+rect.width()*0.65, rect.y()+rect.height()*0.35),2,2);
    painter.drawLine(rect.x()+rect.width()*0.3, rect.y()+rect.height()*0.55, rect.x()+rect.width()*0.7, rect.y()+rect.height()*0.55);
    painter.drawLine(rect.x()+rect.width()*0.3, rect.y()+rect.height()*0.28, rect.x()+rect.width()*0.45, rect.y()+rect.height()*0.3);
    painter.drawLine(rect.x()+rect.width()*0.7, rect.y()+rect.height()*0.28, rect.x()+rect.width()*0.55, rect.y()+rect.height()*0.3);
}

void SimpleMario::drawSnail(QPainter &painter, const QRectF &rect)
{
    painter.setBrush(QColor(120,80,40));
    painter.drawEllipse(rect);
    painter.setBrush(QColor(80,50,20));
    painter.drawEllipse(rect.x()+5, rect.y()+5, rect.width()-10, rect.height()-10);
    painter.setBrush(QColor(255,200,150));
    painter.drawRect(rect.x(), rect.y()+rect.height()*0.5, rect.width(), rect.height()*0.4);
    painter.setBrush(Qt::black);
    painter.drawEllipse(QPointF(rect.x()+rect.width()*0.7, rect.y()+rect.height()*0.35), 3,3);
    painter.drawEllipse(QPointF(rect.x()+rect.width()*0.85, rect.y()+rect.height()*0.35), 3,3);
}

void SimpleMario::drawFlag(QPainter &painter, const QRectF &rect)
{
    painter.fillRect(rect.x(), rect.y(),5, rect.height(), Qt::darkGray);
    painter.setBrush(Qt::red);
    float offset = sin(flagWave)*4;
    QPolygonF flag; flag << QPointF(rect.x()+5, rect.y()) << QPointF(rect.x()+rect.width()-5+offset, rect.y())
         << QPointF(rect.x()+rect.width()-5+offset*0.7, rect.y()+rect.height()*0.2)
         << QPointF(rect.x()+rect.width()-5, rect.y()+rect.height()*0.4);
    painter.drawPolygon(flag);
    painter.setBrush(Qt::yellow); painter.drawEllipse(QPointF(rect.x()+2.5, rect.y()),6,6);
    painter.setBrush(Qt::white); painter.drawEllipse(QPointF(rect.x()+8, rect.y()+5),2,2);
}

void SimpleMario::drawCoin(QPainter &painter, const QRectF &rect, int frame)
{
    painter.setBrush(Qt::yellow); painter.setPen(Qt::darkYellow); painter.drawEllipse(rect);
    int shine = 3 + (frame%4);
    painter.setBrush(Qt::white); painter.drawEllipse(rect.center(), shine, shine);
    painter.setPen(Qt::darkYellow);
    painter.drawLine(rect.center().x()-3, rect.center().y(), rect.center().x()+3, rect.center().y());
    painter.drawLine(rect.center().x(), rect.center().y()-3, rect.center().x(), rect.center().y()+3);
}
