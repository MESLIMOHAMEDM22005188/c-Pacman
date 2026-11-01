#include "Maze.hpp"
#include <algorithm>

Maze::Maze() {
    base_ = {
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
    for (auto& row : base_) {
        if (row.size() < Columns) row.append(Columns - row.size(), '#');
        if (row.size() > Columns) row.resize(Columns);
    }
    setMap(0);
}
void Maze::setMap(int mapIndex) {
    mapIndex_ = ((mapIndex % 6) + 6) % 6;
    original_ = base_;
    for (int y = 0; y < Rows; ++y) {
        for (int x = 0; x < Columns; ++x) {
            const int sx = (mapIndex_ & 1) ? Columns - 1 - x : x;
            const int sy = (mapIndex_ & 2) ? Rows - 1 - y : y;
            char c = base_[sy][sx];
            if (mapIndex_ == 4) {
                if (c == '.') c = ((x + y) % 5 == 0) ? 'o' : '.';
            } else if (mapIndex_ == 5) {
                if (c == '.' && (x * 3 + y) % 7 == 0) c = 'o';
            }
            original_[y][x] = c;
        }
    }
    reset();
}
void Maze::setTheme(int themeIndex) {
    themeIndex_ = ((themeIndex % 6) + 6) % 6;
    static const sf::Color walls[] = {
        sf::Color(20,40,160), sf::Color(105,35,180), sf::Color(170,55,25),
        sf::Color(20,125,175), sf::Color(25,115,65), sf::Color(90,95,110)
    };
    static const sf::Color outlines[] = {
        sf::Color(55,105,255), sf::Color(210,100,255), sf::Color(255,135,45),
        sf::Color(100,225,255), sf::Color(100,230,135), sf::Color(190,195,210)
    };
    wallColor_ = walls[themeIndex_];
    wallOutline_ = outlines[themeIndex_];
}
void Maze::reset() {
    pellets_.assign(Rows, std::vector<bool>(Columns, false));
    powerPellets_.assign(Rows, std::vector<bool>(Columns, false));
    for (int y = 0; y < Rows; ++y)
        for (int x = 0; x < Columns; ++x) {
            pellets_[y][x] = original_[y][x] == '.';
            powerPellets_[y][x] = original_[y][x] == 'o';
        }
}
bool Maze::isWall(int x, int y) const {
    if (y < 0 || y >= Rows) return true;
    if (x < 0 || x >= Columns) return false;
    return original_[y][x] == '#';
}
bool Maze::eatPellet(int x, int y) {
    if (y < 0 || y >= Rows || x < 0 || x >= Columns || !pellets_[y][x]) return false;
    pellets_[y][x] = false; return true;
}
bool Maze::eatPowerPellet(int x, int y) {
    if (y < 0 || y >= Rows || x < 0 || x >= Columns || !powerPellets_[y][x]) return false;
    powerPellets_[y][x] = false; return true;
}
int Maze::remainingPellets() const {
    int total = 0;
    for (int y = 0; y < Rows; ++y)
        for (int x = 0; x < Columns; ++x)
            total += (pellets_[y][x] || powerPellets_[y][x]) ? 1 : 0;
    return total;
}
sf::Vector2i Maze::playerSpawn(int playerIndex) const {
    const int x = (mapIndex_ & 1) ? 12 : 12;
    const int y = playerIndex == 0 ? 15 : 17;
    return {x, y};
}
sf::Vector2i Maze::ghostSpawn(int index) const {
    static const sf::Vector2i spawns[] = {{12,9},{11,9},{13,9},{12,10}};
    auto p = spawns[index % 4];
    if (mapIndex_ & 1) p.x = Columns - 1 - p.x;
    if (mapIndex_ & 2) p.y = Rows - 1 - p.y;
    return p;
}
void Maze::draw(sf::RenderWindow& window) const {
    sf::RectangleShape tile({static_cast<float>(TileSize - 1), static_cast<float>(TileSize - 1)});
    sf::CircleShape pellet(2.5f), power(6.f);
    pellet.setFillColor(sf::Color(255,220,170));
    power.setFillColor(sf::Color(255,235,180));
    for (int y = 0; y < Rows; ++y)
        for (int x = 0; x < Columns; ++x) {
            const float px = static_cast<float>(x * TileSize), py = static_cast<float>(y * TileSize);
            if (original_[y][x] == '#') {
                tile.setPosition(px,py); tile.setFillColor(wallColor_);
                tile.setOutlineThickness(1.f); tile.setOutlineColor(wallOutline_); window.draw(tile);
            } else if (pellets_[y][x]) {
                pellet.setPosition(px + TileSize/2.f - 2.5f, py + TileSize/2.f - 2.5f); window.draw(pellet);
            } else if (powerPellets_[y][x]) {
                power.setPosition(px + TileSize/2.f - 6.f, py + TileSize/2.f - 6.f); window.draw(power);
            }
        }
}