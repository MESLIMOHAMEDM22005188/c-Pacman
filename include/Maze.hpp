#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

class Maze {
public:
    static constexpr int TileSize = 24;
    static constexpr int Columns = 25;
    static constexpr int Rows = 21;
    Maze();
    void setMap(int mapIndex);
    void setTheme(int themeIndex);
    void draw(sf::RenderWindow& window) const;
    bool isWall(int x, int y) const;
    bool eatPellet(int x, int y);
    bool eatPowerPellet(int x, int y);
    void reset();
    int remainingPellets() const;
    sf::Vector2i playerSpawn(int playerIndex = 0) const;
    sf::Vector2i ghostSpawn(int index) const;
private:
    std::vector<std::string> base_;
    std::vector<std::string> original_;
    std::vector<std::vector<bool>> pellets_;
    std::vector<std::vector<bool>> powerPellets_;
    int mapIndex_ = 0;
    int themeIndex_ = 0;
    sf::Color wallColor_ = sf::Color(20, 40, 160);
    sf::Color wallOutline_ = sf::Color(55, 105, 255);
};