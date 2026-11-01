#include "Player.hpp"
#include <cmath>

Player::Player(sf::Vector2i spawn, sf::Color color) : color_(color) { reset(spawn); }
void Player::reset(sf::Vector2i spawn) {
    position_ = {spawn.x * Maze::TileSize + Maze::TileSize / 2.f,
                 spawn.y * Maze::TileSize + Maze::TileSize / 2.f};
    direction_ = Direction::None; requested_ = Direction::None; moveTimer_ = 0.f; animation_ = 0.f;
}
sf::Vector2i Player::tile() const {
    return {static_cast<int>(std::round((position_.x - Maze::TileSize/2.f)/Maze::TileSize)),
            static_cast<int>(std::round((position_.y - Maze::TileSize/2.f)/Maze::TileSize))};
}
bool Player::canMove(Direction direction, const Maze& maze) const {
    const auto d = directionVector(direction); const auto t = tile();
    return !maze.isWall(t.x+d.x,t.y+d.y);
}
void Player::setDirection(Direction direction) { requested_ = direction; }
void Player::update(float dt, const Maze& maze) {
    animation_ += dt*12.f; moveTimer_ += dt;
    const float interval = 1.f/speed_;
    while (moveTimer_ >= interval) {
        moveTimer_ -= interval;
        if (requested_ != Direction::None && canMove(requested_,maze)) direction_ = requested_;
        if (direction_ != Direction::None && canMove(direction_,maze)) {
            const auto d = directionVector(direction_);
            position_.x += d.x*Maze::TileSize; position_.y += d.y*Maze::TileSize;
            if (position_.x < 0) position_.x = (Maze::Columns-1)*Maze::TileSize+Maze::TileSize/2.f;
            if (position_.x >= Maze::Columns*Maze::TileSize) position_.x = Maze::TileSize/2.f;
        } else direction_ = Direction::None;
    }
}
void Player::draw(sf::RenderWindow& window) const {
    const float radius = 9.5f;
    sf::CircleShape body(radius,30); body.setOrigin(radius,radius); body.setPosition(position_); body.setFillColor(color_);
    if (direction_ == Direction::None) { window.draw(body); return; }
    const float mouth = (static_cast<int>(animation_)%2==0) ? 0.18f : 0.06f;
    sf::ConvexShape pac(3);
    pac.setPoint(0,{0.f,0.f}); pac.setPoint(1,{19.f,radius*(1.f-mouth)}); pac.setPoint(2,{19.f,radius*(1.f+mouth)});
    pac.setOrigin(radius,radius); pac.setPosition(position_);
    float rotation=0.f;
    if(direction_==Direction::Down) rotation=90.f;
    if(direction_==Direction::Left) rotation=180.f;
    if(direction_==Direction::Up) rotation=270.f;
    pac.setRotation(rotation); pac.setFillColor(color_); window.draw(pac);
}