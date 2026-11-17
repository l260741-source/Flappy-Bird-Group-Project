#include <SFML/Graphics.hpp>
#include <optional>
#include <cstdlib>
#include <ctime>
#include <string>

int main()
{
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Flappy Bird");

    window.setFramerateLimit(60);

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // BIRD BODY
    sf::CircleShape bird(22);
    bird.setFillColor(sf::Color(255, 220, 40));
    bird.setOutlineThickness(2);
    bird.setOutlineColor(sf::Color(210, 140, 20));
    bird.setPosition({150, 250});

    // BIRD WING
    sf::CircleShape wing(11);
    wing.setScale({1.3f, 0.7f});
    wing.setFillColor(sf::Color(255, 170, 20));

    // BIRD EYE
    sf::CircleShape eye(5);
    eye.setFillColor(sf::Color::White);

    sf::CircleShape pupil(2.5f);
    pupil.setFillColor(sf::Color::Black);

    // BIRD BEAK
    sf::ConvexShape beak(3);
    beak.setPoint(0, {0, 0});
    beak.setPoint(1, {18, 7});
    beak.setPoint(2, {0, 14});
    beak.setFillColor(sf::Color(255, 110, 20));

    // GRAVITY
    float velocity = 0;
    float gravity = 900;

    sf::Clock clock;

    // PIPES
    float pipeX = 850;
    float pipeWidth = 70;
    float gapSize = 180;
    float gapY = 300;
    float pipeSpeed = 220;

    // TOP PIPE
    sf::RectangleShape topPipe;
    sf::RectangleShape topCap;
    sf::RectangleShape topLight;

    // BOTTOM PIPE
    sf::RectangleShape bottomPipe;
    sf::RectangleShape bottomCap;
    sf::RectangleShape bottomLight;

    // PIPE COLORS
    sf::Color pipeGreen(25, 145, 55);
    sf::Color capGreen(50, 200, 75);
    sf::Color lightGreen(120, 235, 125);

    topPipe.setFillColor(pipeGreen);
    topCap.setFillColor(capGreen);
    topLight.setFillColor(lightGreen);

    bottomPipe.setFillColor(pipeGreen);
    bottomCap.setFillColor(capGreen);
    bottomLight.setFillColor(lightGreen);

    // GROUND
    float groundY = 570;

    sf::RectangleShape ground;
    ground.setSize({800, 30});
    ground.setPosition({0, groundY});
    ground.setFillColor(sf::Color(150, 100, 50));

    // SCORE
    int score = 0;
    bool scored = false;

    // GAME STATES
    bool gameOver = false;
    bool paused = false;

    // GAME LOOP
    while (window.isOpen())
    {
        float deltaTime = clock.restart().asSeconds();

        if (deltaTime > 0.1f)
        {
            deltaTime = 0.1f;
        }

        // EVENTS
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                // EXIT
                if (key->code == sf::Keyboard::Key::X)
                {
                    window.close();
                }

                // JUMP
                if (key->code == sf::Keyboard::Key::Space
                    && !gameOver && !paused)
                {
                    velocity = -350;
                }

                // PAUSE AND RESUME
                if (key->code == sf::Keyboard::Key::P && !gameOver)
                {
                    paused = !paused;
                }

                // RESTART
                if (key->code == sf::Keyboard::Key::R && gameOver)
                {
                    bird.setPosition({150, 250});

                    velocity = 0;

                    pipeX = 850;
                    gapY = 300;

                    score = 0;
                    scored = false;

                    gameOver = false;
                    paused = false;
                }
            }
        }

        // GAME MOVEMENT
        if (!gameOver && !paused)
        {
            // GRAVITY
            velocity += gravity * deltaTime;

            // BIRD MOVEMENT
            bird.move({0, velocity * deltaTime});

            // PIPE MOVEMENT
            pipeX -= pipeSpeed * deltaTime;

            // PIPE POSITIONS
            float topHeight = gapY - gapSize / 2;
            float bottomY = gapY + gapSize / 2;

            // TOP PIPE BODY
            topPipe.setSize({pipeWidth, topHeight - 24});
            topPipe.setPosition({pipeX, 0});

            // TOP PIPE CAP
            topCap.setSize({pipeWidth + 20, 24});
            topCap.setPosition({pipeX - 10, topHeight - 24});

            // TOP PIPE DETAIL
            topLight.setSize({8, topHeight - 24});
            topLight.setPosition({pipeX + 10, 0});

            // BOTTOM PIPE CAP
            bottomCap.setSize({pipeWidth + 20, 24});
            bottomCap.setPosition({pipeX - 10, bottomY});

            // BOTTOM PIPE BODY
            bottomPipe.setSize({pipeWidth, groundY - bottomY - 24});
            bottomPipe.setPosition({pipeX, bottomY + 24});

            // BOTTOM PIPE DETAIL
            bottomLight.setSize({8, groundY - bottomY - 24});
            bottomLight.setPosition({pipeX + 10, bottomY + 24});

            // SCORE
            if (pipeX + pipeWidth + 10 < bird.getPosition().x
                && !scored)
            {
                score++;
                scored = true;
            }

            // NEW RANDOM PIPE
            if (pipeX + pipeWidth + 20 < 0)
            {
                pipeX = 850;

                gapY = 170 + std::rand() % 220;

                scored = false;
            }

            // BIRD POSITION
            float birdLeft = bird.getPosition().x;
            float birdRight = birdLeft + 44;

            float birdTop = bird.getPosition().y;
            float birdBottom = birdTop + 44;

            // PIPE COLLISION
            bool horizontalHit =
                birdRight > pipeX - 10 &&
                birdLeft < pipeX + pipeWidth + 10;

            bool verticalHit =
                birdTop < topHeight ||
                birdBottom > bottomY;

            bool hitPipe = horizontalHit && verticalHit;

            // GROUND COLLISION
            bool hitGround = birdBottom >= groundY;

            // CEILING COLLISION
            bool hitCeiling = birdTop <= 0;

            // GAME OVER
            if (hitPipe || hitGround || hitCeiling)
            {
                gameOver = true;
            }
        }

        // BIRD PART POSITIONS
        float birdX = bird.getPosition().x;
        float birdY = bird.getPosition().y;

        wing.setPosition({birdX + 5, birdY + 20});
        eye.setPosition({birdX + 26, birdY + 8});
        pupil.setPosition({birdX + 29, birdY + 10});
        beak.setPosition({birdX + 36, birdY + 17});

        // WINDOW TITLE
        if (gameOver)
        {
            window.setTitle(
                "GAME OVER | Score: " +
                std::to_string(score) +
                " | Press R to Restart"
            );
        }
        else if (paused)
        {
            window.setTitle(
                "PAUSED | Score: " +
                std::to_string(score) +
                " | Press P to Resume"
            );
        }
        else
        {
            window.setTitle(
                "Flappy Bird | Score: " +
                std::to_string(score) +
                " | Press P to Pause"
            );
        }

        // DRAW BACKGROUND
        window.clear(sf::Color(100, 180, 255));

        // DRAW PIPES
        window.draw(topPipe);
        window.draw(topLight);
        window.draw(topCap);

        window.draw(bottomPipe);
        window.draw(bottomLight);
        window.draw(bottomCap);

        // DRAW GROUND
        window.draw(ground);

        // DRAW BIRD
        window.draw(bird);
        window.draw(wing);
        window.draw(eye);
        window.draw(pupil);
        window.draw(beak);

        window.display();
    }

    return 0;
}