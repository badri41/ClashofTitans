#include "HUD.hpp"
#include <sstream>
#include <iomanip>
#include <iostream>

HUD::HUD(): levelText(font),
      timeText(font),
      scoreText(font),
      bulletsText(font) {

    levelText.setCharacterSize(18);
    timeText.setCharacterSize(18);
    scoreText.setCharacterSize(18);
    bulletsText.setCharacterSize(18);

    if(!font.openFromFile("../assets/Arial Unicode.ttf")) {
        std::cerr << "error in opening the font: " << std::endl;
    }

    levelText.setPosition({ 20, 10 });
    timeText.setPosition({ 20, 40 });
    scoreText.setPosition({ 20, 70 });
    bulletsText.setPosition({ 20, 100 });

    levelText.setFillColor(sf::Color::Black);
    timeText.setFillColor(sf::Color::Black);
    scoreText.setFillColor(sf::Color::Black);
    bulletsText.setFillColor(sf::Color::Black);

    levelStartTime = std::chrono::steady_clock::now();
}

void HUD::update() {
    // Time elapsed in MM:SS
    auto now = std::chrono::steady_clock::now();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now - levelStartTime).count();

    int minutes = seconds / 60;
    int remainingSeconds = seconds % 60;

    std::ostringstream timeStream;
    timeStream << std::setfill('0') << std::setw(2) << minutes << ":"
               << std::setfill('0') << std::setw(2) << remainingSeconds;

    timeText.setString("Time: " + timeStream.str());
    levelText.setString("Level: " + levelToString(currentLevel));
    scoreText.setString("Score: " + std::to_string(score));
    bulletsText.setString("Bullets: " + std::to_string(bullets));
}

void HUD::draw(sf::RenderWindow* window) {
    window->draw(levelText);
    window->draw(timeText);
    window->draw(scoreText);
    window->draw(bulletsText);
}

void HUD::setScore(int newScore) {
    score = newScore;
}

void HUD::setBullets(int newBullets) {
    bullets = newBullets;
}

void HUD::setLevel(Level newLevel) {
    currentLevel = newLevel;
    levelStartTime = std::chrono::steady_clock::now(); // reset timer for a new level
}

std::string HUD::levelToString(Level lvl) {
    switch (lvl) {
        case Level::Level1: return "1";
        case Level::Level2: return "2";
        case Level::Level3: return "3";
        default: return "Unknown";
    }
}
