#pragma once
#include "Maze.hpp"
#include "Types.hpp"
#include <string>

class Ghost {
public:
    enum class Mode { Chase, Scatter, Frightened, Returning };
    Ghost(sf::Vector2i spawn, sf::Color color, int personality);
    void update(float dt, const Maze& maze, sf::Vector2i playerTile, Direction playerDirection, bool frightened);
    void draw(sf::RenderWindow& window) const;
    sf::Vector2i tile() const;
    void reset(sf::Vector2i spawn);
    bool isFrightened() const { return mode_ == Mode::Frightened; }
    bool isReturning() const { return mode_ == Mode::Returning; }
    void sendHome();
    sf::Color color() const { return color_; }
private:
    sf::Vector2f position_;
    sf::Vector2i spawn_;
    sf::Color color_;
    int personality_;
    Direction direction_ = Direction::Left;
    Mode mode_ = Mode::Chase;
    float moveTimer_ = 0.f;
    float animation_ = 0.f;
    Direction chooseDirection(const Maze& maze, sf::Vector2i playerTile, Direction playerDirection);
};
