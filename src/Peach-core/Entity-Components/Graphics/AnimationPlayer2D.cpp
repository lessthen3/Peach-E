//#include "AnimationPlayer2D.h"
//
//AnimationPlayer2D::AnimationPlayer2D(sf::Sprite& fp_Target) : m_Target(fp_Target)
//{
//    m_Progress = m_TotalLength = 0.0;
//}
//
//AnimationPlayer2D::~AnimationPlayer2D() {}
//
//void AnimationPlayer2D::AddFrame(Frame&& frame) {
//    m_TotalLength += frame.duration;
//    frames.push_back(std::move(frame));
//}
//
//void AnimationPlayer2D::StepFrameForward(float fp_TimeSinceLastFrame)
//{
//    m_Progress += fp_TimeSinceLastFrame;
//    float f_Counter = m_Progress;
//
//    for (size_t i = 0; i < frames.size(); i++) {
//        f_Counter -= frames[i].duration;
//
//        // if we have progressed OR if we're on the last frame, apply and stop.
//        if (f_Counter <= 0.0 || &(frames[i]) == &frames.back())
//        {
//            m_Target.setTextureRect(frames[i].rect);
//            break; // we found our frame
//        }
//    }
//}