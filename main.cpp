#include "Program.h"

int main() {
    int WIDTH = sf::VideoMode::getDesktopMode().width / 1.5;
    int HEIGHT = sf::VideoMode::getDesktopMode().height / 1.5;

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "");
    window.setFramerateLimit(60);

    sf::View camera(sf::Vector2f(0, 0), sf::Vector2f(WIDTH, HEIGHT));

    sf::Event event;

    sf::RenderTexture mainSurface;
    mainSurface.create(WIDTH, HEIGHT);
    const sf::Texture& mainSurfaceTexture = mainSurface.getTexture();

    sf::Sprite mainSurfaceSprite;
    mainSurfaceSprite.setTexture(mainSurfaceTexture);

    sf::RenderTexture uiSurface;
    uiSurface.create(WIDTH, HEIGHT);
    const sf::Texture& uiSurfaceTextrure = uiSurface.getTexture();

    sf::Sprite uiSurfaceSprite;
    uiSurfaceSprite.setTexture(uiSurfaceTextrure);

    Program program(&camera);

    program.zoom = 780625.f;
    camera.zoom(program.zoom);

    bool paused = false;
    float previousDeltaTime = program.deltaTime;

    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::KeyReleased:
                    switch (event.key.code) {
                        case sf::Keyboard::Equal:
                            camera.zoom(0.75f);
                            program.zoom *= 0.75f;
                            break;
                        case sf::Keyboard::Hyphen:
                            camera.zoom(1.25f);
                            program.zoom *= 1.25f;
                            break;
                        case sf::Keyboard::Slash:
                            program.deltaTime = 0.5f;
                            break;
                        case sf::Keyboard::Space:
                            if (paused) {
                                program.deltaTime = previousDeltaTime;
                                paused = false;
                            } else {
                                previousDeltaTime = program.deltaTime;
                                program.deltaTime = 0.f;
                                paused = true;
                            }
                            break;
                    }
                    break;
                case sf::Event::KeyPressed:
                    const float speed = 100000;
                    switch (event.key.code) {
                        case sf::Keyboard::W:
                            camera.move(0, -speed);
                            break;
                        case sf::Keyboard::S:
                            camera.move(0, speed);
                            break;
                        case sf::Keyboard::A:
                            camera.move(-speed, 0);
                            break;
                        case sf::Keyboard::D:
                            camera.move(speed, 0);
                            break;
                        case sf::Keyboard::Comma:
                            program.deltaTime -= 0.5f;
                            if (program.deltaTime < 0.5f) program.deltaTime = 0.f;
                            break;
                        case sf::Keyboard::Period:
                            program.deltaTime += 0.5f;
                            break;
                    }
            }
        }

        mainSurface.setView(camera);

        program.update();

        mainSurface.clear(sf::Color::Black);
        program.draw(mainSurface);
        mainSurface.display();

        uiSurface.clear(sf::Color::Transparent);
        program.drawUI(uiSurface);
        uiSurface.display();

        window.clear();
        window.draw(mainSurfaceSprite);
        window.draw(uiSurfaceSprite);
        window.display();
    }

    return 0;
}