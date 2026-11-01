#include "Ghost.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

Ghost::Ghost(sf::Vector2i spawn, sf::Color color, int personality)
    : spawn_(spawn), color_(color), personality_(personality) { reset(spawn); }

void Ghost::reset(sf::Vector2i spawn) {
    spawn_ = spawn;
    position_ = {spawn.x * Maze::TileSize + Maze::TileSize / 2.f,
                 spawn.y * Maze::TileSize + Maze::TileSize / 2.f};
    direction_ = Direction::Left;
    mode_ = Mode::Chase;
    moveTimer_ = 0.f;
}

sf::Vector2i Ghost::tile() const {
    return {static_cast<int>(std::round((position_.x - Maze::TileSize / 2.f) / Maze::TileSize)),
            static_cast<int>(std::round((position_.y - Maze::TileSize / 2.f) / Maze::TileSize))};
}
void Ghost::sendHome() { mode_ = Mode::Returning; }

Direction Ghost::chooseDirection(const Maze& maze, sf::Vector2i playerTile, Direction playerDirection) {
    const auto current = tile();
    std::vector<Direction> options;
    for (Direction d : {Direction::Up, Direction::Left, Direction::Down, Direction::Right}) {
        if (d == opposite(direction_)) continue;
        const auto v = directionVector(d);
        if (!maze.isWall(current.x + v.x, current.y + v.y)) options.push_back(d);
    }
    if (options.empty()) {
        const Direction reverse = opposite(direction_);
        const auto v = directionVector(reverse);
        return maze.isWall(current.x + v.x, current.y + v.y) ? Direction::None : reverse;
    }

    sf::Vector2i target = playerTile;
    if (mode_ == Mode::Returning) target = spawn_;
    else if (mode_ == Mode::Scatter) {
        static const sf::Vector2i corners[] = {{23, 1}, {1, 1}, {23, 19}, {1, 19}};
        target = corners[personality_ % 4];
    } else if (personality_ == 1) {
        const auto v = directionVector(playerDirection);
        target = {playerTile.x + v.x * 4, playerTile.y + v.y * 4};
    } else if (personality_ == 2) {
        target = {playerTile.x + (playerTile.x - current.x), playerTile.y + (playerTile.y - current.y)};
    } else if (personality_ == 3 && std::abs(playerTile.x - current.x) + std::abs(playerTile.y - current.y) < 8) {
        target = {1, 19};
    }

    if (mode_ == Mode::Frightened) {
        return options[static_cast<std::size_t>((current.x + current.y + personality_) % options.size())];
    }

    Direction best = options.front();
    float bestDistance = std::numeric_limits<float>::max();
    for (Direction d : options) {
        const auto v = directionVector(d);
        const float dx = static_cast<float>(current.x + v.x - target.x);
        const float dy = static_cast<float>(current.y + v.y - target.y);
        const float distance = dx * dx + dy * dy;
        if (distance < bestDistance) { bestDistance = distance; best = d; }
    }
    return best;
}

void Ghost::update(float dt, const Maze& maze, sf::Vector2i playerTile, Direction playerDirection, bool frightened) {
    animation_ += dt * 8.f;
    if (mode_ != Mode::Returning) mode_ = frightened ? Mode::Frightened : Mode::Chase;
    moveTimer_ += dt;
    const float speed = mode_ == Mode::Frightened ? 3.8f : (mode_ == Mode::Returning ? 12.f : 5.f);
    const float interval = 1.f / speed;
    while (moveTimer_ >= interval) {
        moveTimer_ -= interval;
        const auto t = tile();
        const auto d = directionVector(direction_);
        const bool atCenter = true;
        if (atCenter || maze.isWall(t.x + d.x, t.y + d.y))
            direction_ = chooseDirection(maze, playerTile, playerDirection);
        if (direction_ == Direction::None) continue;
        const auto v = directionVector(direction_);
        position_.x += v.x * Maze::TileSize;
        position_.y += v.y * Maze::TileSize;
        if (position_.x < 0) position_.x = (Maze::Columns - 1) * Maze::TileSize + Maze::TileSize / 2.f;
        if (position_.x >= Maze::Columns * Maze::TileSize) position_.x = Maze::TileSize / 2.f;
        if (mode_ == Mode::Returning && tile() == spawn_) mode_ = Mode::Chase;
    }
}

void Ghost::draw(sf::RenderWindow& window) const {
    const bool frightened = mode_ == Mode::Frightened;
    const sf::Color bodyColor = frightened ? sf::Color(35, 60, 220) :
                                (mode_ == Mode::Returning ? sf::Color(170, 170, 190) : color_);
    sf::CircleShape head(9.5f, 24);
    head.setPosition(position_.x - 9.5f, position_.y - 9.5f);
    head.setFillColor(bodyColor);
    window.draw(head);
    sf::RectangleShape skirt({19.f, 9.f});
    skirt.setPosition(position_.x - 9.5f, position_.y);
    skirt.setFillColor(bodyColor);
    window.draw(skirt);
    for (int i = 0; i < 3; ++i) {
        sf::CircleShape foot(3.2f);
        foot.setPosition(position_.x - 9.5f + i * 6.3f, position_.y + 5.f);
        foot.setFillColor(bodyColor);
        window.draw(foot);
    }
    if (mode_ == Mode::Returning) return;
    sf::CircleShape eye(3.1f);
    eye.setFillColor(sf::Color::White);
    eye.setPosition(position_.x - 5.f, position_.y - 3.f);
    window.draw(eye);
    eye.setPosition(position_.x + 2.f, position_.y - 3.f);
    window.draw(eye);
    sf::CircleShape pupil(1.5f);
    pupil.setFillColor(sf::Color(20, 30, 90));
    pupil.setPosition(position_.x - 4.f, position_.y - 2.f);
    window.draw(pupil);
    pupil.setPosition(position_.x + 3.f, position_.y - 2.f);
    window.draw(pupil);
}
