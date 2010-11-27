#ifndef COGNITION_CONTROLLER_H
#define COGNITION_CONTROLLER_H

#include "recognizer.h"

namespace cognition
{
	class Controller
	{
	public:
		virtual void stateChanged(Recognizer *recognizer) = 0;
	};

}

#endif //COGNITION_CONTROLLER_H