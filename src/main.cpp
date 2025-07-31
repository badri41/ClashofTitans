#include <SFML/Graphics.hpp>
#include <SFML/Audio/Music.hpp>
#include <iostream>

int main() {
    const unsigned int width = 1280;
    const unsigned int height = 720;
    sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode({ width, height }), "Clash of Titans", sf::Style::Titlebar | sf::Style::Close);
    window->setFramerateLimit(60);

    sf::Texture bgtexture;
    if (!bgtexture.loadFromFile("/Users/apple/Coding/Projects/ClashofTitans/assets/bg.jpg")) {
        std::cerr << "Error Loading BgTexture " << std::endl;
        return -1;
    }
    sf::Sprite bgsprite(bgtexture);
    
    sf::Music bgm("/Users/apple/Coding/Projects/ClashofTitans/assets/bgm.mp3");
    bgm.setLooping(true);
    bgm.play();
        
    while (window->isOpen()) {
        while (const std::optional<sf::Event> event = window->pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window->close();
            } else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                    window->close();
                }
            }
        }

        window->clear();

        window->draw(bgsprite);
        
        window->display();
    }

    delete window;
    return 0;
}
