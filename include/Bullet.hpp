#pragma once
#include <SFML/Graphics.hpp>

class Bullet {
private:
    sf::RectangleShape shape;
    float speed;

public:
    Bullet(float startX, float startY, float width, float height, float speed);

    void update(float dt);
    void draw(sf::RenderWindow* window) const;
    bool isOffScreen() const;

    sf::FloatRect getBounds() const;
};
