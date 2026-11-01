#include "Game.hpp"
#include <algorithm>
#include <cmath>

namespace {
constexpr unsigned WindowWidth = Maze::Columns * Maze::TileSize;
constexpr unsigned WindowHeight = Maze::Rows * Maze::TileSize + 58;
const sf::Color playerColors[] = {
    sf::Color(255,220,0), sf::Color(255,95,175), sf::Color(100,245,130),
    sf::Color(80,220,255), sf::Color(255,145,55), sf::Color(205,155,255)
};
const char* colorNames[] = {"SUN YELLOW","PINK","MINT","CYAN","ORANGE","LAVENDER"};
const char* themeNames[] = {"CLASSIC","NEON","FIRE","ICE","FOREST","MONO"};
const char* mapNames[] = {"CLASSIC MAZE","MIRROR MAZE","REVERSE MAZE","DOUBLE MIRROR","POWER MAZE","BONUS MAZE"};
}

Game::Game()
    : window_(sf::VideoMode(WindowWidth, WindowHeight), "PAC-MAN | C++ Arcade", sf::Style::Titlebar | sf::Style::Close),
      player_(maze_.playerSpawn(), playerColors[0]),
      player2_(maze_.playerSpawn(1), sf::Color(80,220,255)) {
    window_.setFramerateLimit(60);
    fontLoaded_ = font_.loadFromFile("assets/DejaVuSans.ttf");
    if (!fontLoaded_) fontLoaded_ = font_.loadFromFile("C:/Windows/Fonts/arial.ttf");
    ghosts_.emplace_back(maze_.ghostSpawn(0), sf::Color(235,55,75), 0);
    ghosts_.emplace_back(maze_.ghostSpawn(1), sf::Color(255,145,205), 1);
    ghosts_.emplace_back(maze_.ghostSpawn(2), sf::Color(55,220,235), 2);
    ghosts_.emplace_back(maze_.ghostSpawn(3), sf::Color(255,160,45), 3);
    maze_.setTheme(selectedTheme_);
}
sf::Text Game::makeText(const std::string& value, unsigned size, sf::Color color, float x, float y) const {
    sf::Text text;
    if (fontLoaded_) text.setFont(font_);
    text.setString(value); text.setCharacterSize(size); text.setFillColor(color); text.setPosition(x,y);
    return text;
}
void Game::run() {
    sf::Clock clock;
    while(window_.isOpen()) {
        const float dt=std::min(clock.restart().asSeconds(),0.05f);
        handleEvents(); update(dt); render();
    }
}
void Game::changeMenuValue(int direction) {
    switch(menuSelection_) {
        case 0: gameMode_ = (gameMode_ + direction + 2) % 2; break;
        case 1: selectedMap_ = (selectedMap_ + direction + 6) % 6; break;
        case 2: selectedTheme_ = (selectedTheme_ + direction + 6) % 6; maze_.setTheme(selectedTheme_); break;
        case 3: selectedColor_ = (selectedColor_ + direction + 6) % 6; player_.setColor(playerColors[selectedColor_]); break;
    }
}
void Game::handleEvents() {
    sf::Event event{};
    while(window_.pollEvent(event)) {
        if(event.type==sf::Event::Closed) window_.close();
        if(event.type!=sf::Event::KeyPressed) continue;
        const auto key=event.key.code;
        if(key==sf::Keyboard::Q) { window_.close(); continue; }

        if(state_==State::Menu) {
            if(key==sf::Keyboard::Up) menuSelection_=(menuSelection_+3)%4;
            if(key==sf::Keyboard::Down) menuSelection_=(menuSelection_+1)%4;
            if(key==sf::Keyboard::Left) changeMenuValue(-1);
            if(key==sf::Keyboard::Right) changeMenuValue(1);
            if(key==sf::Keyboard::Enter || key==sf::Keyboard::Space) startGame();
            continue;
        }
        if(state_==State::GameOver || state_==State::Won) {
            if(key==sf::Keyboard::Enter) state_=State::Menu;
            continue;
        }
        if(key==sf::Keyboard::P || key==sf::Keyboard::Escape) {
            state_=state_==State::Playing ? State::Paused : State::Playing;
            continue;
        }
        if(state_!=State::Playing) continue;

        // Player 1: WASD. Player 2 (or solo alternative): arrow keys.
        if(key==sf::Keyboard::W) player_.setDirection(Direction::Up);
        if(key==sf::Keyboard::S) player_.setDirection(Direction::Down);
        if(key==sf::Keyboard::A) player_.setDirection(Direction::Left);
        if(key==sf::Keyboard::D) player_.setDirection(Direction::Right);
        if(key==sf::Keyboard::Up) player2_.setDirection(Direction::Up);
        if(key==sf::Keyboard::Down) player2_.setDirection(Direction::Down);
        if(key==sf::Keyboard::Left) player2_.setDirection(Direction::Left);
        if(key==sf::Keyboard::Right) player2_.setDirection(Direction::Right);
    }
}
void Game::startGame() {
    score_=0; lives_=3; level_=1;
    maze_.setMap(selectedMap_); maze_.setTheme(selectedTheme_); maze_.reset();
    player_.setColor(playerColors[selectedColor_]);
    resetRound(); state_=State::Playing;
}
void Game::resetRound() {
    player_.reset(maze_.playerSpawn());
    player2_.reset(maze_.playerSpawn(1));
    for(std::size_t i=0;i<ghosts_.size();++i) ghosts_[i].reset(maze_.ghostSpawn(static_cast<int>(i)));
    frightenedTimer_=0.f; modeTimer_=0.f; scatter_=false;
}
void Game::loseLife() {
    --lives_;
    if(lives_<=0) state_=State::GameOver;
    else resetRound();
}
void Game::collectItems(const Player& player) {
    const auto p=player.tile();
    if(maze_.eatPellet(p.x,p.y)) score_+=10;
    if(maze_.eatPowerPellet(p.x,p.y)) { score_+=50; frightenedTimer_=7.f; }
}
void Game::checkCollisions() {
    auto checkPlayer=[this](const Player& player) {
        const auto p=player.tile();
        for(auto& ghost:ghosts_) {
            const auto g=ghost.tile();
            if(std::abs(p.x-g.x)+std::abs(p.y-g.y)>0) continue;
            if(ghost.isFrightened()) { ghost.sendHome(); score_+=200; }
            else if(!ghost.isReturning()) { loseLife(); return false; }
        }
        return true;
    };
    if(!checkPlayer(player_)) return;
    if(gameMode_==1) checkPlayer(player2_);
}
void Game::updateGhostModes(float dt) {
    modeTimer_+=dt;
    if(modeTimer_>(scatter_?5.f:std::max(12.f,22.f-level_))) { scatter_=!scatter_; modeTimer_=0.f; }
}
void Game::update(float dt) {
    if(state_!=State::Playing) return;
    player_.update(dt,maze_);
    if(gameMode_==1) player2_.update(dt,maze_);
    collectItems(player_);
    if(gameMode_==1) collectItems(player2_);
    if(maze_.remainingPellets()==0) {
        if(level_>=6) state_=State::Won;
        else {
            ++level_;
            maze_.setMap((selectedMap_+level_-1)%6);
            maze_.setTheme(selectedTheme_);
            maze_.reset(); resetRound();
        }
        return;
    }
    if(frightenedTimer_>0.f) frightenedTimer_=std::max(0.f,frightenedTimer_-dt);
    updateGhostModes(dt);
    const Player& target=(gameMode_==1 &&
        std::abs(ghosts_[0].tile().x-player2_.tile().x)+std::abs(ghosts_[0].tile().y-player2_.tile().y) <
        std::abs(ghosts_[0].tile().x-player_.tile().x)+std::abs(ghosts_[0].tile().y-player_.tile().y)) ? player2_ : player_;
    for(auto& ghost:ghosts_) ghost.update(dt,maze_,target.tile(),target.direction(),frightenedTimer_>0.f);
    checkCollisions();
}
void Game::drawHud() {
    sf::RectangleShape panel({static_cast<float>(WindowWidth),58.f});
    panel.setPosition(0.f,Maze::Rows*Maze::TileSize); panel.setFillColor(sf::Color(8,8,24)); window_.draw(panel);
    window_.draw(makeText("SCORE "+std::to_string(score_),17,sf::Color::White,12.f, Maze::Rows*Maze::TileSize+8.f));
    window_.draw(makeText("LEVEL "+std::to_string(level_)+"/6",17,sf::Color(255,220,30),220.f,Maze::Rows*Maze::TileSize+8.f));
    window_.draw(makeText("LIVES "+std::to_string(lives_),17,sf::Color(255,100,110),390.f,Maze::Rows*Maze::TileSize+8.f));
    if(gameMode_==1) window_.draw(makeText("2 PLAYERS",12,sf::Color(80,220,255),510.f,Maze::Rows*Maze::TileSize+12.f));
}
void Game::drawMenu() {
    sf::RectangleShape shade({static_cast<float>(WindowWidth),static_cast<float>(WindowHeight)});
    shade.setFillColor(sf::Color(3,5,20)); window_.draw(shade);
    auto title=makeText("PAC-MAN",48,sf::Color(255,220,0),0,45);
    if(fontLoaded_) title.setOrigin(title.getLocalBounds().width/2.f,0);
    title.setPosition(WindowWidth/2.f,45); window_.draw(title);
    window_.draw(makeText("ARCADE EDITION  |  C++ / SFML",13,sf::Color(130,170,255),165,108));
    const std::string labels[]={"GAME MODE","MAP SELECT","VISUAL THEME","PAC-MAN COLOR"};
    const std::string values[]={
        gameMode_==0?"1 PLAYER":"2 PLAYERS LOCAL",
        mapNames[selectedMap_], themeNames[selectedTheme_], colorNames[selectedColor_]
    };
    for(int i=0;i<4;++i) {
        const float y=155.f+i*55.f;
        const bool active=i==menuSelection_;
        sf::RectangleShape row({390.f,42.f});
        row.setPosition(55.f,y-5.f);
        row.setFillColor(active?sf::Color(35,48,95):sf::Color(15,20,45));
        row.setOutlineThickness(active?2.f:1.f);
        row.setOutlineColor(active?sf::Color(255,220,0):sf::Color(55,65,105));
        window_.draw(row);
        window_.draw(makeText(labels[i],12,sf::Color(155,175,220),68,y));
        window_.draw(makeText(values[i],16,active?sf::Color(255,220,0):sf::Color::White,235,y+14));
    }
    sf::CircleShape preview(12.f); preview.setFillColor(playerColors[selectedColor_]); preview.setPosition(470,320); window_.draw(preview);
    window_.draw(makeText("UP / DOWN : SELECT",12,sf::Color(170,185,220),90,405));
    window_.draw(makeText("LEFT / RIGHT : CHANGE",12,sf::Color(170,185,220),90,425));
    window_.draw(makeText("ENTER : PLAY     Q : QUIT",14,sf::Color(255,220,0),90,465));
    window_.draw(makeText("P1 WASD   |   P2 ARROW KEYS",12,sf::Color(100,220,255),90,495));
}
void Game::drawOverlay(const std::string& title,const std::string& subtitle) {
    sf::RectangleShape shade({static_cast<float>(WindowWidth),static_cast<float>(WindowHeight)});
    shade.setFillColor(sf::Color(0,0,0,190)); window_.draw(shade);
    auto heading=makeText(title,36,sf::Color(255,220,0),0,185);
    auto hint=makeText(subtitle,17,sf::Color::White,0,250);
    if(fontLoaded_) { heading.setOrigin(heading.getLocalBounds().width/2.f,0); hint.setOrigin(hint.getLocalBounds().width/2.f,0); }
    heading.setPosition(WindowWidth/2.f,185); hint.setPosition(WindowWidth/2.f,250);
    window_.draw(heading); window_.draw(hint);
}
void Game::render() {
    window_.clear(sf::Color(3,3,18));
    if(state_==State::Menu) drawMenu();
    else {
        maze_.draw(window_);
        player_.draw(window_);
        if(gameMode_==1) player2_.draw(window_);
        for(const auto& ghost:ghosts_) ghost.draw(window_);
        drawHud();
        if(state_==State::Paused) drawOverlay("PAUSED","Press P or ESC to continue");
        if(state_==State::GameOver) drawOverlay("GAME OVER","Press ENTER to return to menu");
        if(state_==State::Won) drawOverlay("YOU WIN!","Six levels completed - press ENTER for menu");
    }
    window_.display();
}
