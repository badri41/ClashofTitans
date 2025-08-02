#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <chrono>

enum class Level {
    Level1,
    Level2,
    Level3
};

class HUD {
private:
    sf::Font font;
    sf::Text levelText, timeText, scoreText, bulletsText; 

    Level currentLevel = Level::Level1; // Lets HardCode Level1
    int score = 0;
    int bullets = 50;

    std::chrono::steady_clock::time_point levelStartTime;

    std::string levelToString(Level lvl);

public:
    HUD();

    void update(); // Time Counter
    void draw(sf::RenderWindow* window);
    void setScore(int newScore); // Show the Score here
    void setBullets(int newBullets); // Show how many Bullets Remaining 
    void setLevel(Level newLevel); // Show Which Level we are currently on
};
