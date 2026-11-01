#pragma once
#include "Ghost.hpp"
#include "Maze.hpp"
#include "Player.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

class Game {
public:
    Game();
    void run();
private:
    enum class State { Menu, Playing, Paused, GameOver, Won };
    sf::RenderWindow window_;
    sf::Font font_;
    bool fontLoaded_ = false;
    Maze maze_;
    Player player_;
    std::vector<Ghost> ghosts_;
    State state_ = State::Menu;
    int score_ = 0;
    int lives_ = 3;
    int level_ = 1;
    float frightenedTimer_ = 0.f;
    float modeTimer_ = 0.f;
    bool scatter_ = false;

    void handleEvents();
    void update(float dt);
    void render();
    void drawHud();
    void drawOverlay(const std::string& title, const std::string& subtitle);
    void startGame();
    void resetRound();
    void loseLife();
    void nextLevel();
    void collectItems();
    void checkCollisions();
    void updateGhostModes(float dt);
    sf::Text makeText(const std::string& value, unsigned size, sf::Color color, float x, float y) const;
};
