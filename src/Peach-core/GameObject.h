#pragma once
class GameObject {
public:
	GameObject();
	virtual ~GameObject();

	void Initialize();
	void Update(float timeSinceLastFrame);
	void ConstantUpdate(float timeSinceLastFrame);
	void OnSceneTreeExit();

};