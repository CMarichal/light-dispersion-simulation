#pragma once
#include "stdafx.h"
#include <SFML/Graphics.hpp>
#include "IDrawingManager.h"
#include <memory>

class SFML_Manager : public IDrawingManager
{
private:
	sf::RenderWindow* window;
	sf::Texture* texture;
	sf::Sprite* sprite;
	sf::Image* image;
	sf::Event event;
	int width;
	int height;

public:
	SFML_Manager(int width, int height):
		width(width), height(height)
	{
		window = new sf::RenderWindow(sf::VideoMode(width, height), "Display window");
		
		texture = new sf::Texture();
		texture->create(static_cast<sf::Uint8>(width), static_cast<sf::Uint8>(height));
		
		image = new sf::Image();
		image->create(width, height, sf::Color::Black);
		
		sprite = new sf::Sprite(*texture);	
	}

	bool closedWindowEventHandler() override
	{
		bool isWindowClosed = window->pollEvent(event) && event.type == sf::Event::Closed;
		if (isWindowClosed)
		{
			window->close();
		}
		return isWindowClosed;
	}

	void display() override
	{
		texture->update(*image);
		sprite->setTexture(*texture);
		window->draw(*sprite);
		window->display();
	}

	// clear the window with black color
	void cleanWindow() override
	{
		window->clear(sf::Color::Black);
	}

	void drawPixel(int x, int y, const glm::vec3& color) override
	{
		putPixel(x, y, convertColor(color));
	}

	void saveToFile(char*& filename) override
	{
		image->saveToFile(filename);
	}

private:
	void putPixel(int x, int y,const sf::Color& color)
	{
		image->setPixel(x, y, color);
	}

	sf::Color convertColor(const glm::vec3& color) {
		uint8_t r = (color.r > 1) ? 255u : static_cast<sf::Uint8>(color.r * 255);
		uint8_t g = (color.g > 1) ? 255u : static_cast<sf::Uint8>(color.g * 255);
		uint8_t b = (color.b > 1) ? 255u : static_cast<sf::Uint8>(color.b * 255);

		return sf::Color{ r, g, b, 255u };
	}

};