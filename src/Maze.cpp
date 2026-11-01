#include "Maze.hpp"
#include <algorithm>

Maze::Maze() {
    original_ = {
        "#########################",
        "#o........##...........o#",
        "#.###.###.##.###.###.##..#",
        "#.......................#",
        "#.###.##.######.##.###...#",
        "#.....##...##...##.......#",
        "#####.####.##.####.#######",
        "    #.##........##.#      ",
        "#####.##.###  ###.##.#####",
        "     ....# G  #....       ",
        "#####.##.######.##.##### #",
        "    #.##........##.#      ",
        "#####.##.######.##.#######",
        "#........##..............#",
        "#.###.##.######.##.###....#",
        "#o..#................#...o#",
        "###.#.###.##.###.##.#.#####",
        "#.....#...##...#..........#",
        "#.########.########.##.##..#",
        "#.........................#",
        "#########################"
    };
    // Keep all rows exactly the same width.
    for (auto& row : original_) {
        if (row.size() < Columns) row.append(Columns - row.size(), '#');
        if (row.size() > Columns) row.resize(Columns);
    }
    reset();
}

void Maze::reset() {
    grid_ = original_;
    pellets_.assign(Rows, std::vector<bool>(Columns, false));
    powerPellets_.assign(Rows, std::vector<bool>(Columns, false));
    for (int y = 0; y < Rows; ++y) {
        for (int x = 0; x < Columns; ++x) {
            if (grid_[y][x] == '.' || grid_[y][x] == 'o') {
                pellets_[y][x] = grid_[y][x] == '.';
                powerPellets_[y][x] = grid_[y][x] == 'o';
                grid_[y][x] = ' ';
            }
        }
    }
}

bool Maze::isWall(int x, int y) const {
    if (y < 0 || y >= Rows) return true;
    if (x < 0 || x >= Columns) return false; // horizontal tunnel
    return original_[y][x] == '#';
}

bool Maze::hasPellet(int x, int y) const {
    return y >= 0 && y < Rows && x >= 0 && x < Columns && pellets_[y][x];
}
bool Maze::hasPowerPellet(int x, int y) const {
    return y >= 0 && y < Rows && x >= 0 && x < Columns && powerPellets_[y][x];
}
bool Maze::eatPellet(int x, int y) {
    if (!hasPellet(x, y)) return false;
    pellets_[y][x] = false;
    return true;
}
bool Maze::eatPowerPellet(int x, int y) {
    if (!hasPowerPellet(x, y)) return false;
    powerPellets_[y][x] = false;
    return true;
}
int Maze::remainingPellets() const {
    int total = 0;
    for (int y = 0; y < Rows; ++y)
        for (int x = 0; x < Columns; ++x)
            total += pellets_[y][x] || powerPellets_[y][x] ? 1 : 0;
    return total;
}
sf::Vector2i Maze::playerSpawn() const { return {12, 15}; }
sf::Vector2i Maze::ghostSpawn(int index) const {
    static const sf::Vector2i spawns[] = {{12, 9}, {11, 9}, {13, 9}, {12, 10}};
    return spawns[index % 4];
}

void Maze::draw(sf::RenderWindow& window) const {
    sf::RectangleShape tile({static_cast<float>(TileSize - 1), static_cast<float>(TileSize - 1)});
    sf::CircleShape pellet(2.5f);
    pellet.setFillColor(sf::Color(255, 220, 170));
    sf::CircleShape power(6.f);
    power.setFillColor(sf::Color(255, 235, 180));
    for (int y = 0; y < Rows; ++y) {
        for (int x = 0; x < Columns; ++x) {
            const float px = static_cast<float>(x * TileSize);
            const float py = static_cast<float>(y * TileSize);
            if (original_[y][x] == '#') {
                tile.setPosition(px, py);
                tile.setFillColor(sf::Color(20, 40, 160));
                tile.setOutlineThickness(1.f);
                tile.setOutlineColor(sf::Color(55, 105, 255));
                window.draw(tile);
            } else if (pellets_[y][x]) {
                pellet.setPosition(px + TileSize / 2.f - 2.5f, py + TileSize / 2.f - 2.5f);
                window.draw(pellet);
            } else if (powerPellets_[y][x]) {
                power.setPosition(px + TileSize / 2.f - 6.f, py + TileSize / 2.f - 6.f);
                window.draw(power);
            }
        }
    }
}
