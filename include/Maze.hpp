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
    void draw(sf::RenderWindow& window) const;
    bool isWall(int x, int y) const;
    bool eatPellet(int x, int y);
    bool eatPowerPellet(int x, int y);
    void reset();
    int remainingPellets() const;
    sf::Vector2i playerSpawn() const;
    sf::Vector2i ghostSpawn(int index) const;

private:
    std::vector<std::string> original_;
    std::vector<std::vector<bool>> pellets_;
    std::vector<std::vector<bool>> powerPellets_;
};
