#pragma once

#include <string>

namespace PeachCore {

	struct PeachComponent {
	public:
		PeachComponent();
		virtual ~PeachComponent();

		virtual void Initialize();
		virtual void Update(float fp_TimeSinceLastFrame);
		virtual void ConstantUpdate(float fp_TimeSinceLastFrame);
		//virtual void HalfConstantUpdate(float fp_TimeSinceLastFrame);
		virtual void OnSceneTreeExit();

	};

}