#include "Program.h"
#include <iostream>
#include <iomanip>
#include <sstream>

Program::Program(sf::RenderWindow* window, sf::View* camera) {
    this->window = window;
    this->camera = camera;

    if (!font.loadFromFile("font.ttf")) {
        std::cout << "Failed to load font!" << std::endl;
    }

    Object earth(0, 0, 5.9722e24f, 6378000.f);
    earth.id = "earth";
    earth.color = sf::Color::Blue;
    earth.velocity.y = -29780.f;
    objects.push_back(earth);

    Object moon(0, 384467e3, 7.34767309e22, 1.74e6f);
    moon.id = "moon";
    moon.velocity.x = 1023.f;
    moon.velocity.y = earth.velocity.y;
    objects.push_back(moon);

    Object iss(0, -6.7981e6, 4.5e5, 109.f * 100.f);
    iss.id = "iss";
    iss.velocity.x = -7990.f; //about 7660 m/s irl
    iss.velocity.y = earth.velocity.y;
    objects.push_back(iss);

    Object sun(-1.49598023e11, 0, 1.989e30f, 6.957e8f);
    sun.id = "sun";
    sun.color = sf::Color::Yellow;
    objects.push_back(sun);
}

void Program::update() {
    const float G = 6.6743015e-11;
    for (auto& object : objects) {
        sf::Vector2f acceleration;

        for (auto& otherObject : objects) {
            if (object.id == otherObject.id) continue;
            
            sf::Vector2f dist = otherObject.pos - object.pos;
            float eucDist = std::sqrt((float)std::pow(dist.x, 2) + (float)std::pow(dist.y, 2));

            float force = (G * otherObject.mass) / (float)std::pow(eucDist, 2);
            float angle = std::atan2(dist.y, dist.x);

            sf::Vector2f accelerationDelta(force * std::cos(angle), force * std::sin(angle));
            acceleration += accelerationDelta;
        }

        object.velocity += deltaTime * acceleration;
        object.pos += deltaTime * object.velocity;
    }
}

void Program::draw(sf::RenderTexture& surface) {
    for (auto& object : objects) {
        sf::CircleShape circle;
        circle.setFillColor(object.color);
        circle.setPosition(object.pos);
        circle.setRadius(object.radius);
        circle.setOrigin(object.radius, object.radius);

        surface.draw(circle);
    }

    if (cameraFocusObject != nullptr) camera->setCenter(cameraFocusObject->pos);
}

void Program::drawUI(sf::RenderTexture& surface) {
    sf::Text timestepLabel;
    timestepLabel.setFont(font);
    timestepLabel.setFillColor(sf::Color::White);
    timestepLabel.setCharacterSize(20);
    timestepLabel.setPosition(12, surface.getSize().y - 24);

    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << deltaTime;
    std::string dTimeString = stream.str();

    stream.str("");
    stream.clear();

    std::string timeScale = "second";
    float timeStepSeconds = (deltaTime * 60.f);
    if (timeStepSeconds >= 60.f * 60.f * 24.f * 365.f) {
        timeStepSeconds /= 60.f * 60.f * 24.f * 365.f;
        timeScale = "year";
    } else if (timeStepSeconds >= 60.f * 60.f * 24.f) {
        timeStepSeconds /= 60.f * 60.f * 24.f;
        timeScale = "day";
    } else if (timeStepSeconds >= 60.f * 60.f) {
        timeStepSeconds /= 60.f * 60.f;
        timeScale = "hour";
    } else if (timeStepSeconds >= 60.f) {
        timeStepSeconds /= 60.f;
        timeScale = "minute";
    }

    stream << std::fixed << std::setprecision(2) << timeStepSeconds;
    std::string dTimeConvertedString = stream.str();

    timestepLabel.setString("Timestep: " + dTimeString + " (" + dTimeConvertedString + " " + timeScale + (timeStepSeconds != 1.f ? "s" : "") + " per second)");

    surface.draw(timestepLabel);


    sf::Text zoomLabel;
    zoomLabel.setFont(font);
    zoomLabel.setFillColor(sf::Color::White);
    zoomLabel.setCharacterSize(20);
    zoomLabel.setPosition(12, surface.getSize().y - 48);

    stream.str("");
    stream.clear();

    stream << std::fixed << std::setprecision(2) << zoom;
    std::string zoomString = stream.str();

    stream.str("");
    stream.clear();

    std::string zoomScale = "meter";
    float zoomScaled = zoom;
    if (zoom >= 1000.f) {
        zoomScale = "kilometer";
        zoomScaled /= 1000.f;
    }

    stream << std::fixed << std::setprecision(2) << zoomScaled;
    std::string zoomScaledString = stream.str();

    zoomLabel.setString("Zoom: " + zoomString + " (" + zoomScaledString + " " + zoomScale + (zoomScaled != 1.f ? "s" : "") + " per pixel)");

    surface.draw(zoomLabel);

    sf::Text versionLabel;
    versionLabel.setString("v" + VERSION);
    versionLabel.setFont(font);
    versionLabel.setFillColor(sf::Color::White);
    versionLabel.setCharacterSize(16);
    surface.draw(versionLabel);


    sf::Vector2f mousePos = window->mapPixelToCoords(sf::Mouse::getPosition(*window), *camera);
    sf::Vector2f windowMousePos = sf::Vector2f(sf::Mouse::getPosition(*window).x, sf::Mouse::getPosition(*window).y);
    for (auto& object : objects) {
        sf::FloatRect objectBounds(object.pos - sf::Vector2f(object.radius, object.radius), sf::Vector2f(object.radius * 2, object.radius * 2));
        if (objectBounds.contains(mousePos)) {
            sf::Text toolTipText;
            toolTipText.setString(object.id);
            toolTipText.setFont(font);
            toolTipText.setCharacterSize(24);
            toolTipText.setFillColor(sf::Color::White);
            toolTipText.setPosition(windowMousePos - sf::Vector2f(0, 35));

            sf::RectangleShape toolTipBackground;
            float padding = 2.f;
            toolTipBackground.setPosition(windowMousePos - sf::Vector2f(0, 30) - sf::Vector2f(padding, padding));
            toolTipBackground.setSize(sf::Vector2f(toolTipText.getGlobalBounds().width + padding * 4, toolTipText.getGlobalBounds().height + padding * 4));
            toolTipBackground.setFillColor(sf::Color(0x555555FF));
            
            surface.draw(toolTipBackground);
            surface.draw(toolTipText);            
            break;
        }
    }
}

void Program::mouseButtonReleased(sf::Event::MouseButtonEvent mouseButtonEvent) {
    switch (mouseButtonEvent.button) {
        case sf::Mouse::Right:
            sf::Vector2f mousePos = window->mapPixelToCoords(sf::Vector2i(mouseButtonEvent.x, mouseButtonEvent.y), *camera);

            for (auto& object : objects) {
                sf::FloatRect objectBounds(object.pos - sf::Vector2f(object.radius, object.radius), sf::Vector2f(object.radius * 2, object.radius * 2));
                if (objectBounds.contains(mousePos)) {
                    cameraFocusObject = &object;
                    break;
                }
            }
            break;
    }
}
