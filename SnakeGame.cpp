#include "SnakeGame.hpp"

#include <ncurses.h>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <string>
#include <unistd.h>

SnakeGame::SnakeGame(int widthValue, int heightValue)
    : gameOver(false),
      width(widthValue),
      height(heightValue),
      points(0),
      direction(KEY_DOWN),
      body(),
      snakeX(widthValue / 2),
      snakeY(heightValue / 2),
      fruitX(0),
      fruitY(0),
      screenInitialized(false)
{
}

SnakeGame::~SnakeGame()
{
    shutdownScreen();
}

void SnakeGame::run()
{
    initScreen();
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    resetState();

    while (!gameOver)
    {
        handleInput();
        if (gameOver)
        {
            break;
        }

        updateSnakePosition();

        if (snakeX == fruitX && snakeY == fruitY)
        {
            grow();
            points++;
            placeFruit();
        }

        draw();
        usleep(100000);
        napms(100);
    }

    nodelay(stdscr, FALSE);
    if (gameOver)
    {
        showGameOverPrompt();
    }

    shutdownScreen();
}

void SnakeGame::initScreen()
{
    if (screenInitialized)
    {
        return;
    }

    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);
    screenInitialized = true;
}

void SnakeGame::shutdownScreen()
{
    if (!screenInitialized)
    {
        return;
    }

    curs_set(1);
    endwin();
    screenInitialized = false;
}

void SnakeGame::resetState()
{
    direction = KEY_DOWN;
    points = 0;
    gameOver = false;
    snakeX = width / 2;
    snakeY = height / 2;
    body.clear();
    body.push_front({snakeX, snakeY});
    placeFruit();
}

void SnakeGame::placeFruit()
{
    while (true)
    {
        int x = std::rand() % (width - 2) + 1;
        int y = std::rand() % (height - 2) + 1;

        bool collides = false;
        for (auto &segment : body)
        {
            if (segment.first == x && segment.second == y)
            {
                collides = true;
                break;
            }
        }

        if (!collides)
        {
            fruitX = x;
            fruitY = y;
            return;
        }
    }
}

void SnakeGame::handleInput()
{
    int ch = getch();
    if (ch == 'q')
    {
        gameOver = true;
        return;
    }

    switch (ch)
    {
    case KEY_UP:
        if (direction != KEY_DOWN)
        {
            direction = ch;
        }
        break;
    case KEY_DOWN:
        if (direction != KEY_UP)
        {
            direction = ch;
        }
        break;
    case KEY_LEFT:
        if (direction != KEY_RIGHT)
        {
            direction = ch;
        }
        break;
    case KEY_RIGHT:
        if (direction != KEY_LEFT)
        {
            direction = ch;
        }
        break;
    default:
        break;
    }
}

void SnakeGame::updateSnakePosition()
{
    switch (direction)
    {
    case KEY_UP:
        snakeY--;
        break;
    case KEY_DOWN:
        snakeY++;
        break;
    case KEY_LEFT:
        snakeX--;
        break;
    case KEY_RIGHT:
        snakeX++;
        break;
    default:
        break;
    }

    keepInBounds();

    if (isSelfCollision())
    {
        gameOver = true;
    }

    body.push_front({snakeX, snakeY});
    body.pop_back();
}

void SnakeGame::keepInBounds()
{
    if (snakeX < 1)
    {
        snakeX = width - 2;
    }

    if (snakeX > width - 2)
    {
        snakeX = 1;
    }

    if (snakeY < 1)
    {
        snakeY = height - 2;
    }

    if (snakeY > height - 2)
    {
        snakeY = 1;
    }
}

bool SnakeGame::isSelfCollision() const
{
    for (const auto &segment : body)
    {
        if (segment == body.front())
        {
            continue;
        }

        if (snakeX == segment.first && snakeY == segment.second)
        {
            return true;
        }
    }
    return false;
}

void SnakeGame::grow()
{
    body.push_back(body.back());
}

void SnakeGame::draw() const
{
    erase();

    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            if (j == 0 || j == height - 1 || i == 0 || i == width - 1)
            {
                mvaddch(j, i, '#');
            }
            else
            {
                mvaddch(j, i, ' ');
            }
        }
    }

    mvaddch(fruitY, fruitX, '*');

    int count = 0;
    for (const auto &block : body)
    {
        if (count == 0)
        {
            mvaddch(block.second, block.first, 'O');
            count++;
        }
        else
        {
            mvaddch(block.second, block.first, 'o');
        }
    }

    std::stringstream ss;
    ss << "Points: " << points;

    const std::string line = ss.str();
    mvprintw(height + 2, 0, "%s", line.c_str());

    refresh();
}

void SnakeGame::showGameOverPrompt()
{
    const std::string gameOverText = "GAME OVER - Press q to quit";
    mvprintw(height / 2, width / 2, "%s", gameOverText.c_str());
    refresh();

    nodelay(stdscr, FALSE);
    int input = 0;
    while ((input = getch()) != 'q')
    {
    }
}
