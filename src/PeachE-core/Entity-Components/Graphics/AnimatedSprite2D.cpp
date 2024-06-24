#include "AnimatedSprite2D.h"

AnimatedSprite2D::AnimatedSprite2D(sf::Sprite& target) : target(target) 
{
    progress = totalLength = 0.0;
}

AnimatedSprite2D::~AnimatedSprite2D() {}

void AnimatedSprite2D::AddFrame(Frame&& frame) {
    totalLength += frame.duration;
    frames.push_back(std::move(frame));
}

void AnimatedSprite2D::Update(float timeSinceLastFrame)
{
    progress += timeSinceLastFrame;
    float p = progress;

    for (size_t i = 0; i < frames.size(); i++) {
        p -= frames[i].duration;

        // if we have progressed OR if we're on the last frame, apply and stop.
        if (p <= 0.0 || &(frames[i]) == &frames.back())
        {
            target.setTextureRect(frames[i].rect);
            break; // we found our frame
        }
    }
}