#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <tuple>

float guy_x = 400.0;
float guy_y = 225.0;
float guy_vx = 0.0;
float guy_vy = 0.0;

float speed = 500.0;

float camera_vx = 0.0;
float camera_vy = 0.0;

std::tuple<float,float> getMove(float x, float y, float dt, float x_min_bound, float x_max_bound, float y_min_bound, float y_max_bound)
{	
	float vx = 0.0;
	float vy = 0.0;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) && y > y_min_bound)
	{
		vy = -dt * speed;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) && x > x_min_bound)
	{
		vx = -dt * speed;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) && y < y_max_bound)
	{
		vy = dt * speed;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) && x < x_max_bound)
	{
		vx = dt * speed;
	}

	if (vx != 0.0)
	{
		vy *= 0.7f;
	}

	if (vy != 0.0)
	{
		vx *= 0.7f;
	}

	return {vx, vy};
}





class TileMap : public sf::Drawable, public sf::Transformable
{
public:
	bool load(const std::filesystem::path& tileset, sf::Vector2u tileSize, const int* tiles, unsigned int width, unsigned int height)
	{
		// load the tileset texture
		if (!m_tileset.loadFromFile(tileset))
			return false;

		// resize the vertex array to fit the level size
		m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
		m_vertices.resize(width * height * 6);

		// populate the vertex array, with two triangles per tile
		for (unsigned int i = 0; i < width; ++i)
		{
			for (unsigned int j = 0; j < height; ++j)
			{
				// get the current tile number
				const int tileNumber = tiles[i + j * width];

				// find its position in the tileset texture
				const int tu = tileNumber % (m_tileset.getSize().x / tileSize.x);
				const int tv = tileNumber / (m_tileset.getSize().x / tileSize.x);

				// get a pointer to the triangles' vertices of the current tile
				sf::Vertex* triangles = &m_vertices[(i + j * width) * 6];

				// define the 6 corners of the two triangles
				triangles[0].position = sf::Vector2f(i * tileSize.x, j * tileSize.y);
				triangles[1].position = sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
				triangles[2].position = sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);
				triangles[3].position = sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);
				triangles[4].position = sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
				triangles[5].position = sf::Vector2f((i + 1) * tileSize.x, (j + 1) * tileSize.y);

				// define the 6 matching texture coordinates
				triangles[0].texCoords = sf::Vector2f(tu * tileSize.x, tv * tileSize.y);
				triangles[1].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
				triangles[2].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);
				triangles[3].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);
				triangles[4].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
				triangles[5].texCoords = sf::Vector2f((tu + 1) * tileSize.x, (tv + 1) * tileSize.y);
			}
		}

		return true;
	}

private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		// apply the transform
		states.transform *= getTransform();

		// apply the tileset texture
		states.texture = &m_tileset;

		// draw the vertex array
		target.draw(m_vertices, states);
	}

	sf::VertexArray m_vertices;
	sf::Texture     m_tileset;
};





int main()
{
	sf::Clock clock;
	
	sf::RenderWindow window(sf::VideoMode({800, 450}), "Raiders");

	constexpr std::array level = {
		0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 2, 0, 0, 0, 0,
		1, 1, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3,
		0, 1, 0, 0, 2, 0, 3, 3, 3, 0, 1, 1, 1, 0, 0, 0,
		0, 1, 1, 0, 3, 3, 3, 0, 0, 0, 1, 1, 1, 2, 0, 0,
		0, 0, 1, 0, 3, 0, 2, 2, 0, 0, 1, 1, 1, 1, 2, 0,
		2, 0, 1, 0, 3, 0, 2, 2, 2, 0, 1, 1, 1, 1, 1, 1,
		0, 0, 1, 0, 3, 2, 2, 2, 0, 0, 0, 0, 1, 1, 1, 1,
	};
	TileMap map;
	if (!map.load("tileset.png", { 32, 32 }, level.data(), 16, 8))
		return -1;

	sf::View view({400.f, 225.f}, {800.f, 450.f});
	sf::Texture texture("assets/guy.png", false, sf::IntRect({0, 0}, {128, 128}));
	sf::Sprite sprite(texture);
	//sprite.setColor(sf::Color::Green);
	//sprite.setPosition({10.f, 50.f});

	sf::RectangleShape shape(sf::Vector2f(100.f, 50.f));
	shape.setFillColor(sf::Color::Green);
	
	while (window.isOpen())
	{
		float dt = clock.restart().asSeconds();

		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				window.close();
		}

		std::tie(guy_vx, guy_vy) = getMove(sprite.getPosition().x, sprite.getPosition().y, dt, 0.0, 1472.0, 0.0, 772.0);

		sprite.move({guy_vx, guy_vy});
		if (sprite.getPosition().x > 336.0f && sprite.getPosition().x < 1136.0f)
		{
			view.setCenter({sprite.getPosition().x + 64.0f, view.getCenter().y});
		}
		if (sprite.getPosition().y > 161.0f && sprite.getPosition().y < 611.0f)
		{
			view.setCenter({view.getCenter().x, sprite.getPosition().y + 64.0f});
		}

		window.clear();
		window.setView(view);
		window.draw(map);
		window.draw(shape);
		window.draw(sprite);
		window.display();
	}
}