#include "Game.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace {
constexpr unsigned WindowWidth = Maze::Columns * Maze::TileSize;
constexpr unsigned WindowHeight = Maze::Rows * Maze::TileSize + 58;
}

Game::Game()
    : window_(sf::VideoMode(WindowWidth, WindowHeight), "PAC-MAN | C++ Arcade", sf::Style::Titlebar | sf::Style::Close),
      player_(maze_.playerSpawn()) {
    window_.setFramerateLimit(60);
    fontLoaded_ = font_.loadFromFile("assets/DejaVuSans.ttf");
    if (!fontLoaded_) fontLoaded_ = font_.loadFromFile("C:/Windows/Fonts/arial.ttf");
    ghosts_.emplace_back(maze_.ghostSpawn(0), sf::Color(235, 55, 75), 0);
    ghosts_.emplace_back(maze_.ghostSpawn(1), sf::Color(255, 145, 205), 1);
    ghosts_.emplace_back(maze_.ghostSpawn(2), sf::Color(55, 220, 235), 2);
    ghosts_.emplace_back(maze_.ghostSpawn(3), sf::Color(255, 160, 45), 3);
}

sf::Text Game::makeText(const std::string& value, unsigned size, sf::Color color, float x, float y) const {
    sf::Text text;
    if (fontLoaded_) text.setFont(font_);
    text.setString(value);
    text.setCharacterSize(size);
    text.setFillColor(color);
    text.setPosition(x, y);
    return text;
}

void Game::run() {
    sf::Clock clock;
    while (window_.isOpen()) {
        const float dt = std::min(clock.restart().asSeconds(), 0.05f);
        handleEvents();
        update(dt);
        render();
    }
}

void Game::handleEvents() {
    sf::Event event{};
    while (window_.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window_.close();
        if (event.type != sf::Event::KeyPressed) continue;
        const auto key = event.key.code;
        if (key == sf::Keyboard::Q) { window_.close(); continue; }
        if (state_ == State::Menu || state_ == State::GameOver || state_ == State::Won) {
            if (key == sf::Keyboard::Enter || key == sf::Keyboard::Space) startGame();
            continue;
        }
        if (key == sf::Keyboard::P || key == sf::Keyboard::Escape) {
            state_ = state_ == State::Playing ? State::Paused : State::Playing;
            continue;
        }
        if (state_ != State::Playing) continue;
        if (key == sf::Keyboard::Up || key == sf::Keyboard::W) player_.setDirection(Direction::Up);
        if (key == sf::Keyboard::Down || key == sf::Keyboard::S) player_.setDirection(Direction::Down);
        if (key == sf::Keyboard::Left || key == sf::Keyboard::A) player_.setDirection(Direction::Left);
        if (key == sf::Keyboard::Right || key == sf::Keyboard::D) player_.setDirection(Direction::Right);
    }
}

void Game::startGame() {
    if (state_ != State::Paused) {
        score_ = 0;
        lives_ = 3;
        level_ = 1;
        maze_.reset();
        resetRound();
    }
    state_ = State::Playing;
}
void Game::resetRound() {
    player_.reset(maze_.playerSpawn());
    for (std::size_t i = 0; i < ghosts_.size(); ++i) ghosts_[i].reset(maze_.ghostSpawn(static_cast<int>(i)));
    frightenedTimer_ = 0.f;
    modeTimer_ = 0.f;
    scatter_ = false;
}
void Game::loseLife() {
    --lives_;
    if (lives_ <= 0) state_ = State::GameOver;
    else resetRound();
}
void Game::nextLevel() {
    ++level_;
    maze_.reset();
    resetRound();
}
void Game::collectItems() {
    const auto p = player_.tile();
    if (maze_.eatPellet(p.x, p.y)) score_ += 10;
    if (maze_.eatPowerPellet(p.x, p.y)) {
        score_ += 50;
        frightenedTimer_ = 7.f;
    }
    if (maze_.remainingPellets() == 0) state_ = State::Won;
}
void Game::checkCollisions() {
    const auto p = player_.tile();
    for (auto& ghost : ghosts_) {
        const auto g = ghost.tile();
        if (std::abs(p.x - g.x) + std::abs(p.y - g.y) > 0) continue;
        if (ghost.isFrightened()) {
            ghost.sendHome();
            score_ += 200;
        } else if (!ghost.isReturning()) {
            loseLife();
            return;
        }
    }
}
void Game::updateGhostModes(float dt) {
    modeTimer_ += dt;
    if (modeTimer_ > (scatter_ ? 5.f : std::max(12.f, 22.f - level_))) {
        scatter_ = !scatter_;
        modeTimer_ = 0.f;
    }
}
void Game::update(float dt) {
    if (state_ != State::Playing) return;
    player_.update(dt, maze_);
    collectItems();
    if (state_ != State::Playing) return;
    if (frightenedTimer_ > 0.f) frightenedTimer_ = std::max(0.f, frightenedTimer_ - dt);
    updateGhostModes(dt);
    for (auto& ghost : ghosts_) ghost.update(dt, maze_, player_.tile(), player_.direction(), frightenedTimer_ > 0.f);
    checkCollisions();
}

void Game::drawHud() {
    sf::RectangleShape panel({static_cast<float>(WindowWidth), 58.f});
    panel.setPosition(0.f, Maze::Rows * Maze::TileSize);
    panel.setFillColor(sf::Color(8, 8, 24));
    window_.draw(panel);
    window_.draw(makeText("SCORE  " + std::to_string(score_), 18, sf::Color::White, 12.f, 8.f + Maze::Rows * Maze::TileSize));
    window_.draw(makeText("LEVEL  " + std::to_string(level_), 18, sf::Color(255, 220, 30), 245.f, 8.f + Maze::Rows * Maze::TileSize));
    window_.draw(makeText("LIVES  " + std::to_string(lives_), 18, sf::Color(255, 100, 110), 440.f, 8.f + Maze::Rows * Maze::TileSize));
}
void Game::drawOverlay(const std::string& title, const std::string& subtitle) {
    sf::RectangleShape shade({static_cast<float>(WindowWidth), static_cast<float>(WindowHeight)});
    shade.setFillColor(sf::Color(0, 0, 0, 190));
    window_.draw(shade);
    auto heading = makeText(title, 36, sf::Color(255, 220, 0), 0.f, 185.f);
    auto hint = makeText(subtitle, 17, sf::Color::White, 0.f, 250.f);
    auto controls = makeText("ARROWS / WASD : MOVE     P : PAUSE     Q : QUIT", 12, sf::Color(170, 190, 230), 0.f, 300.f);
    if (fontLoaded_) {
        heading.setOrigin(heading.getLocalBounds().width / 2.f, 0.f);
        hint.setOrigin(hint.getLocalBounds().width / 2.f, 0.f);
        controls.setOrigin(controls.getLocalBounds().width / 2.f, 0.f);
    }
    heading.setPosition(WindowWidth / 2.f, 185.f);
    hint.setPosition(WindowWidth / 2.f, 250.f);
    controls.setPosition(WindowWidth / 2.f, 300.f);
    window_.draw(heading);
    window_.draw(hint);
    window_.draw(controls);
}
void Game::render() {
    window_.clear(sf::Color(3, 3, 18));
    maze_.draw(window_);
    if (state_ != State::Menu) {
        player_.draw(window_);
        for (const auto& ghost : ghosts_) ghost.draw(window_);
    }
    drawHud();
    if (state_ == State::Menu) drawOverlay("PAC-MAN", "Press ENTER to start your adventure");
    if (state_ == State::Paused) drawOverlay("PAUSED", "Press P or ESC to continue");
    if (state_ == State::GameOver) drawOverlay("GAME OVER", "Press ENTER to try again");
    if (state_ == State::Won) drawOverlay("MAZE CLEARED!", "Press ENTER to play again");
    window_.display();
}
