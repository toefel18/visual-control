#include "recognizer.h"
#include "controller.h"
#include <cassert>

namespace cognition
{
	Recognizer::Recognizer(const std::string& name, shared_ptr<FrameCapture> captureDevice)
		:name(name), captureDevice(captureDevice)
	{
		if(captureDevice != 0)
			captureDevice->addFrameReceiver(this);
	}

	Recognizer::~Recognizer(void)
	{
		if(captureDevice != 0)
			captureDevice->removeFrameReceiver(this);
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

	void Recognizer::threadStart()
	{
		keepProcessing = true;

		while(keepProcessing)
		{
			processFrame();
			//maybe there should be a wait if there is no new frame loaded! 
			//we can use condition variables
		}
	}

	void Recognizer::requestTreadStop()
	{
		keepProcessing = false;
	}
}