#include "simulation.h"
#include <iostream>
#include <SFML/Graphics.hpp>

int main()
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Disease propagation", sf::Style::Close | sf::Style::Titlebar, settings);

	sf::Image icon;
	icon.loadFromFile("dependencies/resources/icon.png");
	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

	std::vector<Variable> variables;
	variables.push_back(Variable("Taille des pixels", Int, 5, 1, 20));
	variables.push_back(Variable("Pourcentage de vaccinés", Double, 50., 0., 100., 1, 1));
	variables.push_back(Variable("Nombre de malades", Int, 1, 1, 1000));
	variables.push_back(Variable("Mortalité de la maladie", Double, 50., 0., 100., 1, 1));
	variables.push_back(Variable("Contagiosité de la maladie", Double, 3., 0., 100., 1, 1));
	variables.push_back(Variable("Temps avant la mort", Int, 50, 1, 1000));
	variables.push_back(Variable("Temps avant la guérison", Int, 80, 1, 1000));
	variables.push_back(Variable("FPS (0 = pas de limite)", Int, 0, 0, 1000));
	
	Menu menu = Menu(variables, sf::Color(20, 20, 200));
	menu.init_names(50, 750, sf::Color::White, window, 150);
	menu.init_lines(700, 15, 1200, sf::Color(75, 75, 255));
	menu.init_circles(20, sf::Color::White);
	menu.init_values(40, 50, sf::Color::White);
	menu.init_boxes(45, 600, sf::Color::White);
	menu.init_start(150, window.getSize().x - 150, window.getSize().y - 150, sf::Color::White);

	Restart restart_button = Restart(150, window.getSize().x - 150, window.getSize().y - 150, sf::Color::White);

	bool end = false;
	Cursor_type cursor_type = arrow;
	sf::Vector2i mouse_position;
	sf::Vector2i previous_mouse_position;

	sf::Mouse mouse;
	sf::Cursor cursor;
	sf::Event sf_event;
	sf::Clock clock;

	cursor.loadFromSystem(sf::Cursor::Arrow);
	window.setMouseCursor(cursor);

	Simulation simulation(menu);

	while (window.isOpen())
	{
		while (!menu.end)
		{
			while (window.pollEvent(sf_event))
			{
				switch (sf_event.type)
				{

				case sf::Event::Closed:

					window.close();
					break;
				}
			}

			previous_mouse_position = mouse_position;
			mouse_position = mouse.getPosition(window);

			menu.update(previous_mouse_position, mouse_position, mouse.isButtonPressed(mouse.Left), cursor, cursor_type, window);
			menu.draw(window);
		}

		cursor.loadFromSystem(sf::Cursor::Wait);
		window.setMouseCursor(cursor);

		menu.end = false;
		menu.grabbed = false;
		menu.grab_forbiden = false;

		simulation.restart(menu);

		if (restart_button.is_in(sf::Vector2f(mouse_position)))
			cursor.loadFromSystem(sf::Cursor::Hand);

		else
			cursor.loadFromSystem(sf::Cursor::Arrow);

		window.setMouseCursor(cursor);

		simulation.draw(window);
		window.display();
		sf::sleep(sf::seconds(5));

		while (!end)
		{
			while (window.pollEvent(sf_event))
			{
				switch (sf_event.type)
				{

				case sf::Event::Closed:

					window.close();
					break;
				}
			}

			window.clear(sf::Color::Black);

			if (menu.variables[7].int_value != 0)
			{
				if (clock.getElapsedTime() >= sf::seconds(1. / (double)menu.variables[7].int_value))
				{
					simulation.update();
					clock.restart();
				}
			}

			else
				simulation.update();

			simulation.draw(window);

			previous_mouse_position = mouse_position;
			mouse_position = mouse.getPosition(window);
			end = restart_button.update(mouse_position, mouse.isButtonPressed(mouse.Left), cursor, cursor_type, window);
			//restart_button.draw(window);

			window.display();
		}

		end = false;
		restart_button.grabbed = false;
		restart_button.grab_forbiden = false;
	}

	return 0;
}