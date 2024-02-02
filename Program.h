#ifndef _PROGRAM_H
#define _PROGRAM_H
#include <SFML/Graphics.hpp>
#include <random>
#include <string>
#include <math.h>

const std::string VERSION = "1.0";

struct Object {
    Object(float x, float y, float mass, float radius = 10.f, sf::Color color = sf::Color::White) {
        pos = sf::Vector2f(x, y);
        this->mass = mass;
        //this->radius = std::max(radius, std::logf(mass) * 500);
        this->radius = radius;
        this->color = color;

        std::string text = std::to_string(x + y * mass);
        unsigned int hash = 0;
        for (int i = 0; i < text.length(); i++) {
            hash = ((hash << 5) + hash) + (unsigned int)text.at(i);
        }
        id = std::to_string(hash);
    }

    sf::Vector2f pos;
    sf::Vector2f velocity;
    float mass;
    float radius;
    sf::Color color;

    std::string id;
};

class Program {
public:
    Program(sf::View* camera);

    void update();
    void draw(sf::RenderTexture& surface);
    void drawUI(sf::RenderTexture& surface);

    float deltaTime = 10.f; //1440.f for 1 day/s
    float zoom = 0.f;

private:
    sf::View* camera;

    sf::Font font;

    std::vector<Object> objects;
};

#endif 

