#include <SFML/Window.hpp>

int main()
{
	sf::Window window(sf::VideoMode({ 800, 600 }), "My window");
	/*sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);*/

	while (window.isOpen())
	{
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				window.close();
		}

		/*window.clear();
		window.draw(shape);
		window.display();*/
	}
}