#include <SFML/Graphics.hpp>
#include <SFML/Audio/Music.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include "../include/Player.hpp"
#include "../include/Bullet.hpp"
#include "../include/Jets.hpp"
#include "../include/HUD.hpp"

int main() {
    const unsigned int width = 1280;
    const unsigned int height = 720;
    sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode({ width, height }), "Clash of Titans", sf::Style::Titlebar | sf::Style::Close);
    window->setFramerateLimit(60);

    HUD hud;

    sf::Texture bgtexture("../assets/bg.jpg");
    sf::Sprite bgsprite(bgtexture);
    sf::Music bgm("../assets/bgm.mp3");
    bgm.setLooping(true);
    bgm.play();

    const unsigned int Pwidth = 80.f;  //Default Player width
    const unsigned int Pheight = 20.f; //Default Player height
    const unsigned int Pspeed = 300.f; //Default Player Speed

    Player player(width / 2.f, height - 50.f, Pwidth, Pheight, 300.f); //Player Sprite

    const unsigned int Bwidth = 5.f;  //Default Bullet width
    const unsigned int Bheight = 10.f; //Default Bullet height
    const unsigned int Bspeed = 500.f; //Default Bullet Speed

    std::vector<Bullet> bullets;
    sf::Clock clock;

    while (window->isOpen()) {
        while (const std::optional<sf::Event> event = window->pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window->close();
            } else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                    window->close();
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::Space || keyPressed->scancode == sf::Keyboard::Scancode::W || keyPressed->scancode == sf::Keyboard::Scancode::Up) {
                    sf::Vector2f playerPos = player.getPosition();
                    sf::Vector2f playerSize = player.getSize();

                    float bulletX = playerPos.x + Pwidth / 2.f; // center bullet
                    float bulletY = playerPos.y;

                    bullets.emplace_back(bulletX, bulletY, Bwidth, Bheight, Bspeed);
                }
            }
        }

        float dt = clock.restart().asSeconds();
        player.handleInput(dt, width);
        
        window->clear();
        
        window->draw(bgsprite);
        player.draw(window);
        
        hud.update();
        hud.draw(window);

        for (Bullet& b : bullets) 
            b.update(dt);


        bullets.erase(
            std::remove_if(bullets.begin(), bullets.end(),
                        [&](const Bullet& b) {
                            return b.isOffScreen();
                        }),
            bullets.end()
        );

        for (const Bullet& b : bullets)
            b.draw(window);

        window->display();
    }

    delete window;
    return 0;
}
