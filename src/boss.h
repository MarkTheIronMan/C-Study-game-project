#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <list>
#include <vector>
#include <algorithm>

class Boss
{
  public:
    float dx, dy;
    FloatRect rect;
    Sprite sprite;
    float currentFrame;
    float offsetX, offsetY;
    float temp = 0;
    bool isAlive;
    bool isDamaged = false;
    bool onAttack;
    int hpmax = 5;
    int hp;
    bool unv = false;
    bool onGround;
    bool isSet = false;
    bool puFromSky = false;
    float timecooldown = 0;
    float powerupdown = 0;
    bool cooldown = false;
    Clock t;

    void set(Texture &image, int x, int y)
    {
        hp = hpmax;
        sprite.setTexture(image);
        rect = FloatRect(x, y, 64, 64);
        dx = 0.1;
        currentFrame = 0;
        isAlive = true;
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
                        dx = -dx;
                    }
                    else if ((dx < 0) && (dir == 0))
                    {
                        rect.left = j * 32 + 32;
                        dx = -dx;
                    }
                    if ((dy > 0) && (dir == 1))
                    {
                        rect.top = i * 32 - rect.height;
                        dy = 0;
                        onGround = true;
                    }
                    if ((dy < 0) && (dir == 1))
                    {
                        rect.top = i * 32 + 32;
                        dy = 0;
                    }
                }
            }
    }

    float curFrame(bool onAttack, int count, float time)
    {
        currentFrame += 0.0015 * time;
        if (!onAttack)
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

    void update(float time)
    {
        timecooldown += 0.001 * time;
        powerupdown += 0.001 * time;
        puFromSky = false;
        if (powerupdown >= 15)
        {
            powerupdown = 0;
            puFromSky = true;
        }
        if (timecooldown >= 4)
        {
            cooldown = true;
            if (timecooldown >= 4.8)
                timecooldown = 0;
        }
        else
            cooldown = false;
        if (isDamaged)
        {
            if (!unv)
            {
                t.restart();
                unv = true;
                hp--;
            }
            else
            {
                float tt = t.getElapsedTime().asSeconds();
                temp += tt;
                if (temp <= 350)
                {
                    if (hp > 0)
                        sprite.setColor(Color::Red);
                }
                else
                {
                    sprite.setColor(Color::White);
                    unv = false;
                    isDamaged = false;
                    temp = 0;
                }
            }
        }
        if (hp <= 0)
            isAlive = false;
        if (isAlive)
        {
            rect.left += dx * time;
            Collision(0);

            if (!onGround)
                dy += 0.00012 * time;
            rect.top += dy * time;
            onGround = false;
            Collision(1);
            if (cooldown)
            {
                int direction;
                if (dx >= 0)
                {
                    direction = 1;
                    sprite.setTextureRect(IntRect(64 * int(curFrame(true, 3, time)), 128, 64 * direction, 64));
                }
                else
                {
                    direction = -1;
                    sprite.setTextureRect(IntRect(64 * int(curFrame(true, 3, time)) - 64 * direction, 128, 64 * direction, 64));
                }
                rect.left += dx * time;
                dy = -0.12;
            }
            if ((dx > 0) && (!cooldown))
            {
                sprite.setTextureRect(IntRect(64 * int(curFrame(false, 2, time)), 0, 64, 64));
            }
            if ((dx < 0) && (!cooldown))
            {
                sprite.setTextureRect(IntRect(64 * int(curFrame(false, 2, time)) + 64, 0, -64, 64));
            }
            sprite.setPosition(rect.left - offsetX, rect.top - offsetY);
        }
        else
        {
            if (dx >= 0)
            {
                sprite.setTextureRect(IntRect(0, 128, 64, -64));
            }
            else
            {
                sprite.setTextureRect(IntRect(64, 128, -64, -64));
            }
            sprite.setPosition(rect.left - offsetX, rect.top - offsetY);
            rect.top += 0.15 * time;
        }
    }
};

class HealthBarBoss
{
  public:
    float x, y;
    FloatRect BB, BBF;
    int hp;
    int hpmax;
    Sprite BossBarFull;
    Sprite BossBar;

    HealthBarBoss(Texture &image)
    {
        BossBarFull.setTexture(image);
        BBF = FloatRect(x, y, 42, 10);
        BossBar.setTexture(image);
        BB = FloatRect(x, y, 40, 10);
    }

    void update(float time, int hp, int hpmax, float x, float y)
    {
        BossBarFull.setTextureRect(IntRect(0, 192, 40, 12));
        BossBar.setTextureRect(IntRect(42, 192, 40 * hp / hpmax, 10));
        BossBarFull.setPosition(x, y);
        BossBar.setPosition(x + 2, y);
    }
};