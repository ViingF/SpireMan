#ifndef SPIRELIKE_FAILURETOAST_HPP
#define SPIRELIKE_FAILURETOAST_HPP

#include <SFML/Graphics.hpp>
#include <string>

class FailureToast {
public:
    void show(const std::string& message);
    void update(float dt);
    void draw(sf::RenderWindow& window, const sf::Font& font) const;

private:
    std::string message_;
    float remainSeconds_ = 0.0f;

    static constexpr float kDurationSeconds = 1.0f;
};

#endif
