#pragma once
#include <SFML/Graphics.hpp>

class Player {
private: 
    sf::RectangleShape shape;
    float speed;

public:
    Player(float startX, float startY, float width, float height, float moveSpeed);
    
    void handleInput(float dt, float windowWidth);
    void draw(sf::RenderWindow* window) const;

    sf::Vector2f getPosition() const;
    sf::Vector2f getSize() const;
};
