#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

int main()
{
	sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "My window");
	sf::Texture texture("assets/stick_figure.png", false, sf::IntRect({10, 10}, {64, 64}));
	sf::Sprite sprite(texture);
	sprite.setColor(sf::Color::Green);

	sf::RectangleShape shape(sf::Vector2f(100.f, 50.f));
	shape.setFillColor(sf::Color::Green);

	while (window.isOpen())
	{
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				window.close();
		}

		window.clear();
		window.draw(shape);
		window.draw(sprite);
		window.display();
	}
}