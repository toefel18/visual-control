#ifndef RECOGNIZER_H
#define RECOGNIZER_H
#include <string>
#include <set>
#include <boost/shared_ptr.hpp>

#include "framecapture.h"
#include "framereceiver.h"
#include "util.h"

namespace cognition
{
	class Controller;

	using boost::shared_ptr;

	class Recognizer : public FrameReceiver
	{
	public:
		Recognizer(const std::string& name, shared_ptr<FrameCapture> captureDevice = shared_ptr<FrameCapture>());
		virtual ~Recognizer(void);

		//recognizers can be given a name for convenience
		std::string getName() const { return name; }
		
		//the rect where the object is last detected!
		Rect getArea() const { return lastDetect;}

		//convert to smart pointers..
		//methods for controllers that get notifications about state changes
		bool addController(Controller* controller);
		void removeController(Controller* controller);
		void notifyControllers();

		//threadstart method if this threading facility is used. 
		//do not call this method from a normal thread, it will be trapped in a processing loop!
		void threadStart();

		//sets a flag that causes the thread loop to stop, thread can be joined afterwards
		void requestTreadStop();

		//this will be called by the internal thread loop
		virtual void processFrame() = 0;

	protected:
		std::string name;
		shared_ptr<FrameCapture> captureDevice; 
		cognition::Rect lastDetect;

		bool keepProcessing;

		typedef std::set<Controller*> ControllerSet;
		typedef ControllerSet::iterator ControllerSetItr;

		ControllerSet controllers;
	};
}

#endif //RECOGNIZER_H