#pragma once
class GameObject {
public:
	GameObject();
	virtual ~GameObject();

	virtual void Initialize();
	virtual void Update(float fp_TimeSinceLastFrame);
	virtual void ConstantUpdate(float fp_TimeSinceLastFrame);
	//virtual void HalfConstantUpdate(float fp_TimeSinceLastFrame);
	virtual void OnSceneTreeExit();

};