#pragma once
#include "stdafx.h"
#include <SFML/Graphics.hpp>
#include "IDrawingManager.h"
#include "IInputManager.h"
#include <memory>

class SFML_Manager : public IDrawingManager, public IInputManager
{
private:
	sf::RenderWindow* window;
	sf::Texture* texture;
	sf::Sprite* sprite;
	sf::Image* image;
	int width;
	int height;

public:
	SFML_Manager(int width, int height):
		width(width), height(height)
	{
		window = new sf::RenderWindow(sf::VideoMode(width, height), "Display window");
		
		texture = new sf::Texture();
		texture->create(width, height);
		
		image = new sf::Image();
		image->create(width, height, sf::Color::Black);
		
		sprite = new sf::Sprite(*texture);	
	}

	bool closedWindowEventHandler() override
	{
		sf::Event event;
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

	void saveToFile(std::string filename) override
	{
		image->saveToFile(filename);
	}

	bool isKeyPressed(Key key)
	{
		switch (key)
		{
		case Key::Q:
			return sf::Keyboard::isKeyPressed(sf::Keyboard::Q);
		case Key::W:
			return sf::Keyboard::isKeyPressed(sf::Keyboard::W);
		case Key::E:
			return sf::Keyboard::isKeyPressed(sf::Keyboard::E);
		case Key::A:
			return sf::Keyboard::isKeyPressed(sf::Keyboard::A);
		case Key::S:
			return sf::Keyboard::isKeyPressed(sf::Keyboard::S);		
		case Key::D:
			return sf::Keyboard::isKeyPressed(sf::Keyboard::D);
		case Key::R:
			return sf::Keyboard::isKeyPressed(sf::Keyboard::R);
		case Key::F:
			return sf::Keyboard::isKeyPressed(sf::Keyboard::F);
		case Key::T:
			return sf::Keyboard::isKeyPressed(sf::Keyboard::T);
		case Key::Y:
			return sf::Keyboard::isKeyPressed(sf::Keyboard::Y);
		case Key::LEFT_ARROW:
			return sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
		case Key::RIGHT_ARROW:
			return sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
		case Key::UP_ARROW:
			return sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
		case Key::DOWN_ARROW:
			return sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
		default:
			return false;
			break;
		}
	}

private:
	void putPixel(int x, int y,const sf::Color& color)
	{
		image->setPixel(x, y, color);
	}

	sf::Color convertColor(const glm::vec3& color) {
		sf::Uint8 r = (color.r >= 1) ? 255u : static_cast<sf::Uint8>(color.r * 255);
		sf::Uint8 g = (color.g >= 1) ? 255u : static_cast<sf::Uint8>(color.g * 255);
		sf::Uint8 b = (color.b >= 1) ? 255u : static_cast<sf::Uint8>(color.b * 255);

		return sf::Color{ r, g, b, 255u };
	}

};