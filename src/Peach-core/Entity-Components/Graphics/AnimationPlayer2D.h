//#pragma once
//
//#include <SFML/Graphics.hpp>
//#include <vector>
//#include <string>
//
//struct Frame {
//	sf::IntRect rect;
//	float duration;
//};
//
//class AnimationPlayer2D
//{
//	std::vector<Frame> frames;
//	float m_TotalLength;
//	float m_Progress;
//	sf::Sprite& m_Target;
//
//public:
//	AnimationPlayer2D(sf::Sprite& fp_Target);
//	virtual ~AnimationPlayer2D();
//	void AddFrame(Frame&& fp_Frame);
//	void StepFrameForward(float fp_TimeSinceLastFrame);
//	const float GetLength() const { return m_TotalLength; }
//
//};
//
