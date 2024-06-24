#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

struct Frame {
	sf::IntRect rect;
	double duration;
};

class AnimatedSprite2D
{
	std::vector<Frame> frames;
	double totalLength;
	double progress;
	sf::Sprite& target;

public:
	AnimatedSprite2D(sf::Sprite& target);
	virtual ~AnimatedSprite2D();
	void AddFrame(Frame&& frame);
	void Update(float timeSinceLastFrame);
	const double GetLength() const { return totalLength; }

};

