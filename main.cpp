#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <list>
#include <vector>
#include <algorithm>
#include "src/map.h"
#include "src/player.h"
#include "src/boss.h"

using namespace sf;
using namespace std;

int getRandomNumber(int min, int max)
{
    unsigned value = unsigned(std::time(nullptr));
    do
    {
        value = (value * 73129 + 95121) % (max);
    } while (value < min);
    return value;
}

class PowerUps
{
  public:
    float baseX;
    int x, y;
    float dy;
    FloatRect rect;
    Sprite sprite;
    bool isAlive = true;

    void set(Texture &image, int x, int y)
    {
        sprite.setTexture(image);
        rect = FloatRect(x, y, 32, 32);
    }

    void Collision(int dir)
    {
        for (int i = rect.top / 32; i < (rect.top + rect.height) / 32; i++)
            for (int j = rect.left / 32; j < (rect.left + rect.width) / 32; j++)
            {
                if ((TileMap[i][j] == 'B') || (TileMap[i][j] == 'R'))
                {
                    if ((dy > 0) && (dir == 1))
                    {
                        rect.top = i * 32 - rect.height;
                        dy = 0;
                    }
                    if ((dy < 0) && (dir == 1))
                    {
                        rect.top = i * 32 + 32;
                        dy = 0;
                    }
                }
            }
    }

    void update(float time)
    {
        dy = 0.1;
        rect.top += dy * time;
        Collision(1);

        if (isAlive)
        {
            sprite.setTextureRect(IntRect(32 * 4, 0, 32, 32));
            sprite.setPosition(rect.left - offsetX, rect.top - offsetY);
        }
    }
};

class Enemy
{
  public:
    float dx, dy;
    float baseX;
    FloatRect rect;
    Sprite sprite;
    float currentFrame;
    bool isAlive = true;
    bool isDamaged = false;
    bool isTaunted = false;
    int type;
    bool cooldown = false;
    float timecooldown = 0;
    float deathtime;
    bool onGround;

    void set(Texture &image, int x, int y, int type)
    {
        sprite.setTexture(image);
        rect = FloatRect(x, y, 48, 48);
        dx = 0.05;
        dy = 0.1;
        currentFrame = 0;
        isAlive = true;
        baseX = x;
    }

    float curFrame(bool isAlive, int count, float time)
    {
        currentFrame += 0.0025 * time;
        if (isAlive)
        {
            if (currentFrame > count)
                currentFrame -= count;
        }
        else
        {
            if (currentFrame > count)
                currentFrame--;
        }
        return currentFrame;
    }

    void Collision(int dir)
    {
        for (int i = rect.top / 32; i < (rect.top + rect.height) / 32; i++)
            for (int j = rect.left / 32; j < (rect.left + rect.width) / 32; j++)
            {
                if ((TileMap[i][j] == 'B') || (TileMap[i][j] == 'R') || (TileMap[i][j] == 'n'))
                {
                    if ((dx > 0) && (dir == 0))
                    {
                        rect.left = j * 32 - rect.width;
                        baseX -= 200;
                        dx = -dx;
                    }
                    if ((dx < 0) && (dir == 0))
                    {
                        rect.left = j * 32 + 32;
                        dx = -dx;
                        baseX += 200;
                    }
                    if ((dy > 0) && (dir == 1))
                    {
                        rect.top = i * 32 - rect.height;
                        dy = 0;
                        onGround = true;
                    }
                }
            }
    }

    void update(float time, int type)
    {
        timecooldown += 0.001 * time;
        if (timecooldown >= 3)
        {
            timecooldown = 0;
            cooldown = true;
        }
        else
            cooldown = false;

        rect.left += dx * time;
        Collision(0);

        if (!onGround)
            dy += 0.001 * time;
        rect.top += dy * time;
        onGround = false;
        Collision(1);

        currentFrame += time * 0.005;
        if (!isDamaged)
        {
            if (currentFrame > 2)
                currentFrame -= 2;
            if (!isTaunted)
            {
                if (rect.left - 200 >= baseX)
                    dx = -0.05;
                if (rect.left + 200 <= baseX)
                    dx = 0.05;
                if (dx > 0)
                    sprite.setTextureRect(IntRect(48 * int(curFrame(1, 2, time)) + 48, 48 * (type), -48, 48));
                if (dx < 0)
                    sprite.setTextureRect(IntRect(48 * int(curFrame(1, 2, time)), 48 * (type), 48, 48));
            }
            else
            {
                sprite.setTextureRect(IntRect(48 * 2, 48 * (type), 48, 48));
                dx = 0;
            }
        }
        else
        {
            dx = 0;
            deathtime += time * 0.005;
            sprite.setTextureRect(IntRect(48 * int(curFrame(false, 2, time)), 48 * 4, 48, 48));
            if (deathtime > 1.7)
            {
                isAlive = false;
                deathtime = 0;
            }
        }
        sprite.setPosition(rect.left - offsetX, rect.top - offsetY);
    }
};

class Alien
{
  public:
    int x, y;
    float dy, currentFrame;
    FloatRect rect;
    Sprite sprite;
    bool isAlive = true;
    int count;

    void set(Texture &image, int x, int y)
    {
        sprite.setTexture(image);
        rect = FloatRect(x, y, 48, 60);
        dy = 0.15;
    }

    void Collision(int dir)
    {
        for (int i = rect.top / 32; i < (rect.top + rect.height) / 32; i++)
            for (int j = rect.left / 32; j < (rect.left + rect.width) / 32; j++)
            {
                if ((TileMap[i][j] == 'B') || (TileMap[i][j] == 'R'))
                {
                    if ((dy > 0) && (dir == 1))
                    {
                        rect.top = i * 32 - rect.height;
                        dy = 0;
                    }
                    if ((dy < 0) && (dir == 1))
                    {
                        rect.top = i * 32 + 32;
                        dy = 0;
                    }
                }
            }
    }

    void update(float time)
    {
        rect.top += dy * time;
        Collision(1);
        currentFrame += time * 0.005;
        if (currentFrame > 1.9)
            currentFrame = 0;

        if (isAlive)
        {
            sprite.setTextureRect(IntRect(int(currentFrame) * 48, 48 * 5, 48, 60));

            sprite.setPosition(rect.left - offsetX, rect.top - offsetY);
        }
        else
            sprite.setPosition(-200, -200);
    }
    void draw(RenderWindow &window, int x, int y)
    {
        sprite.setPosition(x, y);
        window.draw(sprite);
    }
};

class Bullet
{
  public:
    float x, y, dx, dy, w, h;
    FloatRect rect;
    Sprite sprite;
    bool isAlive = true;
    float angle = 0;

    void set(Texture &image, int x, int y, float angle)
    {
        sprite.setTexture(image);
        rect = FloatRect(x, y, 16, 16);
        dx = 0.15 * cos(angle);
        dy = 0.15 * sin(angle);
    }

    void update(float time)
    {
        rect.left += dx * time;
        rect.top += dy * time;
        sprite.setTextureRect(IntRect(0, 0, 16, 16));
        sprite.setPosition(rect.left - offsetX, rect.top - offsetY);
        int ltime = 0;
        ltime += time * 0.01;
        if (ltime >= 3)
        {
            isAlive = false;
        }
    }

    void draw(RenderWindow &window, int x, int y)
    {
        sprite.setPosition(x, y);
        window.draw(sprite);
    }
};

class Cactus
{
  public:
    float x, y, dx, w, h;
    FloatRect rect;
    Sprite sprite;
    bool isAlive = true;
    int dir;
    float currentFrame;

    void set(Texture &image, int x, int y, int dir)
    {
        sprite.setTexture(image);
        rect = FloatRect(x, y, 16, 16);
        dx = 0.15 * dir;
    }
    void update(float time)
    {
        currentFrame += time * 0.005;
        if (currentFrame > 2)
            currentFrame -= 2;
        sprite.setTextureRect(IntRect(16 * int(currentFrame) + 16, 0, 16, 16));
        rect.left += dx * time;
        sprite.setPosition(rect.left - offsetX, rect.top - offsetY);
    }

    void draw(RenderWindow &window, int x, int y)
    {
        sprite.setPosition(x, y);
        window.draw(sprite);
    }
};

class Interface
{
  public:
    float x, y, dx, dy, w, h;
    FloatRect HP, DB, HT;
    int hp;
    Sprite HPbar;
    Sprite powerbar;
    Sprite Hint;

    Interface(Texture &image)
    {
        HPbar.setTexture(image);
        HP = FloatRect(48, 48, 48, 48);
        HPbar.setPosition(20, 10);

        powerbar.setTexture(image);
        DB = FloatRect(48, 48, 48, 48);
        powerbar.setPosition(20, 70);

        Hint.setTexture(image);
        HT = FloatRect(48, 48, 48, 48);
        Hint.setPosition(60, 70);
    }

    void update(float time, int hp, int dc, bool isFinal)
    {
        int count = 0;
        HPbar.setTextureRect(IntRect(0, 192, 48 * hp, 48));
        powerbar.setTextureRect(IntRect(48 * (2 + dc), 192, 48, 48));
        if ((dc > 4) && (!isFinal))
            count = 1;
        Hint.setTextureRect(IntRect(48 * 8, 48 * (4 + count), 110, 48));
    }
};

bool startGame()
{

    RenderWindow window(VideoMode(800, 600), "Rescue alien from Area 51");
    int j, k, tm, l, n, z, timer = 0;
    bool gameLosing, gameWinning, warn;
    float gameoverDelay = 0;

    Texture tileset, q, bg, trump, enm, inter;
    tileset.loadFromFile("game/images/enviroment-sprite.png");
    trump.loadFromFile("game/images/boss-sprite.png");
    q.loadFromFile("game/images/naruto-sprite.png");
    bg.loadFromFile("game/images/lvl1.png");
    enm.loadFromFile("game/images/enemy-sprite.png");

    Boss Trump;
    HealthBarBoss TrumpBar(trump);

    Sprite background(bg);
    PowerUps pu1, pu2, pu3, pu4, pu5, pu6, pu7, pu8, pu9, pu10, pu11;
    Player p(q);
    Interface interface(q);

    Font font;                              //шрифт
    font.loadFromFile("game/src/font.ttf"); //передаем нашему шрифту файл шрифта
    Text text("", font, 20);
    Text rescue("", font, 20);
    Text warning("", font, 20);
    Text start("", font, 20);
    Text gameover("", font, 20);
    Text win("", font, 20);    //создаем объект текст. закидываем в объект текст строку, шрифт, размер шрифта(в пикселях);//сам объект текст (не строка)
    text.setColor(Color::Red); //покрасили текст в красный. если убрать эту строку, то по умолчанию он белый
    warning.setColor(Color::Red);
    rescue.setColor(Color::Red);
    start.setColor(Color::Red);
    gameover.setColor(Color::White);
    win.setColor(Color::White);

    Enemy e1, e2, e3, e4, e5, e6, e7, e8, e9, e10;

    std::vector<Enemy> enemies = {e1, e2, e3, e4, e5, e6, e7, e8, e9, e10};

    for (j = 0; j < enemies.size(); j++)
    {
        enemies[j].type = j % 3 + 1;
        enemies[j].set(enm, (j + 3) * getRandomNumber(240, 540), 50, enemies[j].type);
    }

    std::vector<PowerUps> powerups = {pu1, pu2, pu3, pu4, pu5, pu7, pu8, pu9, pu10};
    for (k = 0; k < powerups.size(); k++)
    {
        powerups[k].set(tileset, (k + 1) * 560, 80);
    }

    Bullet b;
    std::vector<Bullet> bullets = {};

    Cactus c;
    std::vector<Cactus> cactus = {};

    Alien a1, a2, a3, a4, a5, a6, a7, a8;
    std::vector<Alien> alien = {a1, a2, a3, a4, a5, a6, a7, a8};

    for (z = 0; z < alien.size(); z++)
    {
        alien[z].set(enm, (z + 1) * getRandomNumber(300, 340), 30);
    }

    Sprite tile(tileset);

    Clock clock, gameTime;

    RectangleShape rectangle(Vector2f(32, 32));

    while (window.isOpen())
    {
        float time = clock.getElapsedTime().asMicroseconds();
        clock.restart();
        float gT = gameTime.getElapsedTime().asMicroseconds();
        gameTime.restart();

        time = time / 500;

        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        if (p.health < 1)
            p.isAlive = false;
        ;

        if ((Keyboard::isKeyPressed(Keyboard::T)) && (p.powerlvl > 4))
        {
            p.form = 2;
        }
        if (Keyboard::isKeyPressed(Keyboard::Tab))
        {
            gameLosing = false;
            return true;
        } //если таб, то перезагружаем игру
        if (Keyboard::isKeyPressed(Keyboard::Escape))
        {
            return false;
        } //если эскейп, то выходим из игры

        if ((Keyboard::isKeyPressed(Keyboard::Left)) && (p.isAlive))
        {
            p.dx = -0.1 * p.form;
        }

        if ((Keyboard::isKeyPressed(Keyboard::Space)) && (p.isAlive) && (!p.isAttack))
        {
            if (p.cactus > 0)
            {
                p.isAttack = true;
                c.set(enm, p.rect.left + 10, p.rect.top + 20, p.dir);
                cactus.push_back(c);
                p.cactus--;
            }
            else
            {
                warn = true;
            }
        }

        if ((Keyboard::isKeyPressed(Keyboard::Right)) && (p.isAlive))
        {
            p.dx = 0.1 * p.form;
        }

        if (!p.isAlive)
            p.dx = 0;

        if ((Keyboard::isKeyPressed(Keyboard::Up)) && (p.isAlive))
        {
            if (p.onGround)
            {
                p.dy = -0.35;
                p.onGround = false;
            }
        }

        if (!((Keyboard::isKeyPressed(Keyboard::Up)) || (Keyboard::isKeyPressed(Keyboard::Left))

              || (Keyboard::isKeyPressed(Keyboard::Right))))
        {
            p.isStand = true;
            p.dx = 0;
        }
        else
            p.isStand = false;

        p.update(time);
        interface.update(time, p.health, p.powerlvl, p.isFinal);

        tm = int(gT) % 360;
        if ((p.rect.left > 140) && (p.rect.left < 5435) && (!p.isFinal))
        {
            offsetX = p.rect.left - 140;
        }
        if (p.rect.left > 5435)
            p.isFinal = true;

        offsetY = 50;

        p.offsetX = Trump.offsetX = offsetX;
        p.offsetY = Trump.offsetY = offsetY;

        if ((p.isFinal) && (!Trump.isSet))
        {
            Trump.set(trump, 5600, 150);
            p.form = 1;
            Trump.isSet = true;
        }

        background.setPosition(-offsetX, offsetY + -30);
        window.clear(Color::White);
        window.draw(background);

        for (int i = 0; i < H; i++)
            for (int j = 0; j < W; j++)
            {
                if (TileMap[i][j] == 'B')
                    tile.setTextureRect(IntRect(0, 0, 32, 32));

                if (TileMap[i][j] == '0')
                    tile.setTextureRect(IntRect(96, 0, 32, 32));

                if (TileMap[i][j] == ' ')
                    tile.setTextureRect(IntRect(256, 0, 32, 32));

                if (TileMap[i][j] == 'n')
                    tile.setTextureRect(IntRect(640, 0, 32, 32));

                if (TileMap[i][j] == 'R')
                    tile.setTextureRect(IntRect(288, 0, 32, 32));

                rectangle.setPosition(j * 32 - offsetX, i * 32 - offsetY);
                tile.setPosition(j * 32 - offsetX, i * 32 - offsetY);
                window.draw(tile);
            }

        for (j = 0; j < enemies.size(); j++)
        {
            if (enemies[j].isAlive)
                enemies[j].update(time, enemies[j].type);
            window.draw(enemies[j].sprite);
            if (!enemies[j].isAlive)
                enemies.erase(enemies.begin() + j);
            if ((p.rect.intersects(enemies[j].rect)) && (!p.unv))
            {
                p.isDamaged = true;
            }

            if (fabs(enemies[j].rect.left - p.rect.left) <= 250)
            {
                if (!enemies[j].isDamaged)
                {
                    float ang = atan2(-enemies[j].rect.top + p.rect.top, -enemies[j].rect.left + p.rect.left);
                    enemies[j].isTaunted = true;
                    if (enemies[j].cooldown)
                    {
                        b.set(enm, enemies[j].rect.left, enemies[j].rect.top + 20, ang);
                        bullets.push_back(b);
                    }
                }
            }
            else
            {
                enemies[j].isTaunted = false;
            }
        }

        for (z = 0; z < alien.size(); z++)
        {
            if (p.rect.intersects(alien[z].rect))
            {
                alien.erase(alien.begin() + z);
                p.rescue++;
            }
            else
            {
                alien[z].update(time);
                window.draw(alien[z].sprite);
            }
        }

        for (l = 0; l < bullets.size(); l++)
        {
            if ((p.rect.intersects(bullets[l].rect)) && (!p.unv))
            {
                p.isDamaged = true;
                bullets[l].isAlive = false;
            }
            bullets[l].update(time);
            window.draw(bullets[l].sprite);
            if (fabs(p.rect.left - bullets[l].rect.left) > 1000)
                bullets[l].isAlive = false;
            if (!bullets[l].isAlive)
                bullets.erase(bullets.begin() + l);
        }

        window.draw(p.sprite);
        window.draw(interface.HPbar);
        window.draw(interface.powerbar);
        window.draw(interface.Hint);

        for (k = 0; k < powerups.size(); k++)
        {
            if ((p.rect.intersects(powerups[k].rect)) && (powerups[k].isAlive))
            {
                p.cactus++;
                if (p.powerlvl < 5)
                    p.powerlvl++;
                powerups[k].isAlive = false;
            }
            powerups[k].update(time);
            window.draw(powerups[k].sprite);
            if (!powerups[k].isAlive)
                powerups.erase(powerups.begin() + k);
        }

        for (n = 0; n < cactus.size(); n++)
        {
            if (fabs(p.rect.left - cactus[n].rect.left) > 1500)
            {
                cactus.erase(cactus.begin() + n);
            }
            if ((Trump.rect.intersects(cactus[n].rect)) && (Trump.isAlive))
            {
                Trump.isDamaged = true;
                cactus.erase(cactus.begin() + n);
            }
            for (j = 0; j < enemies.size(); j++)
            {
                if ((enemies[j].rect.intersects(cactus[n].rect)) && (enemies[j].isAlive))
                {
                    enemies[j].isDamaged = true;
                    cactus.erase(cactus.begin() + n);
                }
            }
        }

        for (n = 0; n < cactus.size(); n++)
        {
            cactus[n].update(time);
            window.draw(cactus[n].sprite);
        }

        if ((Trump.rect.intersects(p.rect)) && (Trump.isAlive) && (p.isAlive))
        {
            p.isDamaged = true;
        }

        if (Trump.puFromSky)
        {
            pu6.set(tileset, getRandomNumber(int(p.rect.left - 350), int(p.rect.left + 350)), 20);
            powerups.push_back(pu6);
        }

        std::ostringstream playerScoreString, warningText, startText, gameoverText, winText, alienText; // объявили переменную
        playerScoreString << p.cactus;
        alienText << p.rescue;
        warningText << "Collect more cactus to attack";
        startText << "Level 1";
        gameoverText << "       Game Over\n\n Press Tab to restart";
        winText << "Level complete\n\n   Respect++\n\nAlien rescued: " + alienText.str();
        rescue.setString("Alien rescued: " + alienText.str());
        text.setString("Cactus count: " + playerScoreString.str());
        warning.setString(warningText.str());
        start.setString(startText.str());
        gameover.setString(gameoverText.str());
        win.setString(winText.str());
        text.setPosition(20, 120);
        rescue.setPosition(20, 150);
        warning.setPosition(240, 250);
        start.setPosition(360, 280);
        gameover.setPosition(270, 280);
        win.setPosition(300, 250);

        float delay, warndelay;
        delay += 0.015 * time;
        warndelay += 0.015 * time;
        if (delay < 2)
        {
            window.draw(start);
        }
        if (warn)
        {
            if (warndelay < 3)
            {
                window.draw(warning);
            }
            else
            {
                warndelay = 0;
                warn = false;
            }
        }

        if (Trump.isSet)
        {
            Trump.update(time);
            window.draw(Trump.sprite);
            if (Trump.isAlive)
            {
                TrumpBar.update(time, Trump.hp, Trump.hpmax, Trump.rect.left - offsetX + 10, Trump.rect.top - offsetY - 20);
                window.draw(TrumpBar.BossBarFull);
                window.draw(TrumpBar.BossBar);
            }
        }
        window.draw(text);
        window.draw(rescue);

        if (!p.isAlive)
            gameoverDelay += time * 0.005;
        if (gameoverDelay > 8)
        {
            gameLosing = true;
            window.clear(Color::Black);
            window.draw(gameover);
        }
        if ((!Trump.isAlive) && (Trump.isSet))
            gameoverDelay += time * 0.005;
        if (gameoverDelay > 8)
        {
            gameWinning = true;
            window.clear(Color::Black);
            window.draw(win);
        }
        window.display();
    }
}

void gameRunning()
{ //ф-ция перезагружает игру , если это необходимо
    if (startGame())
    {
        gameRunning();
    } ////если startGame() == true, то вызываем занова ф-цию isGameRunning, которая в свою очередь опять вызывает startGame()
}
int main()
{
    gameRunning(); //запускаем процесс игры
    return 0;
}