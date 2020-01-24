#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <list>
#include <vector>
#include <algorithm>

class Player
{
  public:
    float x, y, dx, dy, w, h;
    FloatRect rect;
    bool onGround;
    Sprite sprite;
    float currentFrame;
    float offsetX, offsetY;
    bool isAlive = true;
    bool isDamaged = false;
    bool unv = false;
    bool isStand = false;
    bool isFinal = false;
    int health = 3;
    int powerlvl = 1;
    int cactus = 3;
    int rescue = 0;
    float temp = 0;
    float timecooldown;
    int form = 1;
    int score = 0;
    bool isAttack;
    int dir = 1;
    Clock t;

    Player(Texture &image)
    {
        sprite.setTexture(image);
        rect = FloatRect(150, 2 * 32, 48, 48);

        dx = dy = 0;
        currentFrame = 0;
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

    void update(float time)
    {
        if (isAttack)
        {
            timecooldown += 0.001 * time;
            if (timecooldown >= 3)
            {
                timecooldown = 0;
                isAttack = false;
            }
        }
        if (health <= 0)
            isAlive = false;

        if (isDamaged)
        {
            if (!unv)
            {
                t.restart();
                unv = true;
                health--;
            }
            else
            {
                float tt = t.getElapsedTime().asSeconds();
                temp += tt;
                if (temp <= 350)
                {
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

        rect.left += dx * time;
        Collision(0);

        if (!onGround)
            dy = dy + 0.0004 * time;
        rect.top += dy * time;
        onGround = false;
        Collision(1);

        if (form == 2)
        {
            if (isAlive)
                sprite.setTextureRect(IntRect(48 * int(curFrame(false, 8, time)), 240, 48, 48));
        }
        else
        {
            if (dx > 0)
            {
                sprite.setTextureRect(IntRect(48 * int(curFrame(1, 4, time)), 0, 48, 48));
                dir = 1;
            }
            if (dx < 0)
            {
                sprite.setTextureRect(IntRect(48 * int(curFrame(1, 4, time)) + 48, 0, -48, 48));
                dir = -1;
            }
            if ((isStand) && (isAlive) && (dir == 1))
                sprite.setTextureRect(IntRect(48 * int(curFrame(1, 2, time)), 48, 48, 48));
            if ((isStand) && (isAlive) && (dir == -1))
                sprite.setTextureRect(IntRect(48 * int(curFrame(1, 2, time)) + 48, 48, -48, 48));
            if ((dy != 0) && (dx >= 0))
                sprite.setTextureRect(IntRect(48 * int(curFrame(1, 4, time)), 96, 48, 48));
            if ((dy != 0) && (dx < 0))
                sprite.setTextureRect(IntRect(48 * int(curFrame(1, 4, time)) + 48, 96, -48, 48));
        }
        if (!isAlive)
        {
            sprite.setTextureRect(IntRect(48 * int(curFrame(false, 4, time)), 144, 48, 48));
        }

        sprite.setPosition(rect.left - offsetX, rect.top - offsetY);
    }

    void Collision(int dir)
    {
        for (int i = rect.top / 32; i < (rect.top + rect.height) / 32; i++)
            for (int j = rect.left / 32; j < (rect.left + rect.width) / 32; j++)
            {
                if ((TileMap[i][j] == 'B') || (TileMap[i][j] == 'R') || ((TileMap[i][j] == 'n') && (isFinal)))
                {
                    if ((dx > 0) && (dir == 0))
                        rect.left = j * 32 - rect.width;
                    if ((dx < 0) && (dir == 0))
                        rect.left = j * 32 + 32;
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

                if (TileMap[i][j] == '0')
                {
                    if (health < 3)
                        health++;
                    TileMap[i][j] = ' ';
                }
            }
    }
};