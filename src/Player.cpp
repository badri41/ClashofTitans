#include "Player.hpp"

Player::Player(float startX, float startY, float width, float height, float moveSpeed) {
    shape.setSize({ width, height });
    shape.setFillColor(sf::Color::Black);
    shape.setPosition({ startX, startY });
    speed = moveSpeed;
}

void Player::handleInput(float dt, float windowWidth) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A)) {
        shape.move({ -speed * dt, 0.f });
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D)) {
        shape.move({ speed * dt, 0.f });
    }
    
    // Prevent moving out of bounds
    sf::Vector2f pos = shape.getPosition();
    float width = shape.getSize().x;

    if (pos.x < 0)
        shape.setPosition({ 0.f, pos.y });
    if (pos.x + width > windowWidth)
        shape.setPosition({ windowWidth - width, pos.y });
}

void Player::draw(sf::RenderWindow* window) const {
    window->draw(shape);
}

sf::Vector2f Player::getPosition() const {
    return shape.getPosition();
}

sf::Vector2f Player::getSize() const {
    return shape.getSize();
}
