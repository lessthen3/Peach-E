#pragma once

#include <string>
#include "../Unsorted/PeachObject.h"

namespace PeachCore {

	class PeachNode: public PeachObject {
	public:
		PeachNode();
		virtual ~PeachNode();

		virtual void Initialize();
		virtual void Update(float fp_TimeSinceLastFrame);
		virtual void ConstantUpdate(float fp_TimeSinceLastFrame);
		//virtual void HalfConstantUpdate(float fp_TimeSinceLastFrame);
		virtual void OnSceneTreeExit();

		bool IsPausable = true;

	};

}