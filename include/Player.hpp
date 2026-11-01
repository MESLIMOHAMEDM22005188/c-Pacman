#pragma once
#include "Maze.hpp"
#include "Types.hpp"

class Player {
public:
    explicit Player(sf::Vector2i spawn = {12, 15});
    void update(float dt, const Maze& maze);
    void draw(sf::RenderWindow& window) const;
    void setDirection(Direction direction);
    sf::Vector2i tile() const;
    sf::Vector2f position() const { return position_; }
    Direction direction() const { return direction_; }
    void reset(sf::Vector2i spawn);
private:
    sf::Vector2f position_;
    Direction direction_ = Direction::None;
    Direction requested_ = Direction::None;
    float moveTimer_ = 0.f;
    float animation_ = 0.f;
    float speed_ = 7.0f;
    bool canMove(Direction direction, const Maze& maze) const;
};
