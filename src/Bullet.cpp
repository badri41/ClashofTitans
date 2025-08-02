#include "Bullet.hpp"

Bullet::Bullet(float startX, float startY, float width, float height, float spd) {
    shape.setSize({ width, height });
    shape.setFillColor(sf::Color::Red);
    shape.setPosition({ startX, startY });
    speed = spd;
}

void Bullet::update(float dt) {
    shape.move({ 0.f, -speed * dt });
}

void Bullet::draw(sf::RenderWindow* window) const {
    window->draw(shape);
}

bool Bullet::isOffScreen() const {
    return shape.getPosition().y + shape.getSize().y < 0;
}

sf::FloatRect Bullet::getBounds() const {
    return shape.getGlobalBounds();
}
