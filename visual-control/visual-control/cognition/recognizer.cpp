#include "recognizer.h"
#include "controller.h"

namespace cognition
{
	Recognizer::Recognizer(const std::string& name)
		:name(name)
	{
	}

	Recognizer::~Recognizer(void)
	{
	}


	bool Recognizer::addController(Controller* controller)
	{
		return controllers.insert(controller).second;
	}

	void Recognizer::removeController(Controller* controller)
	{
		controllers.erase(controller);
	}

	void Recognizer::notifyControllers()
	{
		for(ControllerSetItr i = controllers.begin(); i != controllers.end(); ++i)
			(*i)->stateChanged(this);
	}

}