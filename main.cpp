#include <iostream>
#include "map/map.hpp"

float Euklid(sf::Vector2u left, sf::Vector2u right) {
    const sf::Vector2f vec = sf::Vector2f(left) - sf::Vector2f(right);
    return std::sqrt(vec.x*vec.x + vec.y*vec.y);
}
float PoiskVShirinu(sf::Vector2u, sf::Vector2u) {
    static float factor = 1.0f;
    factor++;
    return std::numeric_limits<float>::epsilon()*factor;
}
float PoiskVGlubinu(sf::Vector2u, sf::Vector2u) {
    static float counter = static_cast<float>(1000000);
    counter -= 5.0f;
    return counter;
}

int main() {
    const auto euristicFunc = Euklid;

    sf::RenderWindow window(sf::VideoMode(800,800), "Count1");
    window.setFramerateLimit(144);

    // init
    bool pathIsFound = false;
    size_t size = 0, numBarriers = 0;
    std::cin >> size >> numBarriers;
    std::vector<sf::Vector2u> barriers;
    for (size_t i = 0; i < numBarriers; i++) {
        size_t x, y;
        std::cin >> x >> y;
        barriers.emplace_back(x, y);
    }
    size_t start_x, start_y;
    std::cin >> start_x >> start_y;
    sf::Vector2u start(start_x, start_y);

    size_t finish_x, finish_y;
    std::cin >> finish_x >> finish_y;
    sf::Vector2u finish(finish_x, finish_y);

    krv::Map map(size, barriers, start, finish);
    // init end

    window.setView(sf::View(sf::FloatRect(0.0f, 0.0f, size, size)));

    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && !pathIsFound) {
                if (event.key.code == sf::Keyboard::Enter) {
                    map.pathFind(euristicFunc, window);
                    pathIsFound = true;
                }
            }
        }

        window.clear();
        window.draw(map);
        window.display();
    }

    return 0;
}