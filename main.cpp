#include <ncurses.h>
#include <cstdlib>
#include <sstream>
#include <unistd.h>
#include <deque>
#include <ctime>

bool gameOver;
const int width = 30;
const int height = 10;
int point;
int direction;
std::deque<std::pair<int, int>> body;

int snakeX, snakeY, fruitX, fruitY;

void setUpGame()
{
    direction = KEY_DOWN;
    point = 0;
    gameOver = false;
    snakeX = width / 2;
    snakeY = height / 2;
    body.push_front({snakeX, snakeY});
    fruitX = std::rand() % width;
    fruitY = std::rand() % height;
}

void drawSnake(std::deque<std::pair<int, int>> &snakeBody)
{
    std::deque<std::pair<int, int>> copy = snakeBody;
    int count = 0;

    while (!copy.empty())
    {
        std::pair<int, int> position = copy.front();
        if (count == 0)
        {
            mvaddch(snakeY, snakeX, 'O');
        }
    }
}

void draw()
{
    erase(); // clear ncurses back buffer

    // draw border (use a 1-char margin around playfield)
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            if (j == 0)
            {
                mvaddch(j, i, '#');
            }
            else if (i == 0 || i == width - 1)
            {
                mvaddch(j, i, '#');
            }
            else if (j == height - 1)
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
    for (auto block : body)
    {
        int x = block.first;
        int y = block.second;
        if (count == 0)
        {
            mvaddch(y, x, 'O');
            count++;
        }
        else
        {
            mvaddch(y, x, 'o');
        }
    }

    std::stringstream ss; 

    ss << "snakeX: " << snakeX << " and snakeY: " << snakeY << "\n";
    ss << "fruitX: " << fruitX << " and fruitY: " << fruitY << "\n"; 
    mvprintw(height + 2, 0, "%s", ss.str().c_str()); // print it


    // std::stringstream ss;
    // ss << "Points: " << point; // build the label once

    // std::string line = ss.str();                 // extract the string
    // // mvprintw(height + 2, 0, "%s", line.c_str()); // print it

    refresh(); // flush to screen
}

void handleInput()
{
    int ch = getch();
    if (ch == ERR)
        return;
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

void keepInBounds()
{ // simple wrap; or set gameOver on hit
    if (snakeX < 0)
        snakeX = width - 2;
    if (snakeX > width - 2)
        snakeX = 1;
    if (snakeY < 0)
        snakeY = height - 2;
    if (snakeY > height - 2)
        snakeY = 1;
}

void move()
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

    body.push_front({snakeX, snakeY});
    body.pop_back();
}

void grow()
{
    body.push_front({snakeX, snakeY});
}

int main()
{
    // --- ncurses init ---
    initscr();
    noecho();
    cbreak();              // instant key delivery (no Enter)
    keypad(stdscr, TRUE);  // arrow keys
    nodelay(stdscr, TRUE); // non-blocking getch()
    curs_set(0);           // hide cursor

    std::srand(static_cast<unsigned>(std::time(nullptr))); // seed RNG once per run
    setUpGame();

    while (!gameOver)
    {
        handleInput();
        move();
        draw();

        if (snakeX == fruitX && snakeY == fruitY)
        {
            while (fruitX != snakeX || fruitY != snakeY)
            {
                fruitX = rand() % width;
                fruitY = rand() % height;
            }
        }

        sleep(1);
        napms(100); // ~10 FPS tick
    }

    endwin(); // restore terminal
    return 0;
}
