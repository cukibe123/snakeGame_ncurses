#ifndef SNAKE_GAME_HPP
#define SNAKE_GAME_HPP

#include <deque>
#include <utility>

class SnakeGame
{
public:
    SnakeGame(int width, int height);
    ~SnakeGame();

    void run();

private:
    void initScreen();
    void shutdownScreen();
    void resetState();
    void placeFruit();
    void handleInput();
    void updateSnakePosition();
    void keepInBounds();
    bool isSelfCollision() const;
    void grow();
    void draw() const;
    void showGameOverPrompt();

    bool gameOver;
    const int width;
    const int height;
    int points;
    int direction;
    std::deque<std::pair<int, int>> body;
    int snakeX;
    int snakeY;
    int fruitX;
    int fruitY;
    bool screenInitialized;
};

#endif
