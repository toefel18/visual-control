#ifndef RECOGNIZER_H
#define RECOGNIZER_H
#include <string>
#include <set>

#include "framereceiver.h"
#include "util.h"

namespace cognition
{
	class Controller;

	class Recognizer : public FrameReceiver
	{
	public:
		Recognizer(const std::string& name);
		virtual ~Recognizer(void);

		std::string getName(){ return name; }
		Rect getArea(){ return lastDetect;}

		bool addController(Controller* controller);
		void removeController(Controller* controller);
		void notifyControllers();

	protected:
		std::string name;
		Rect lastDetect;
		
		typedef std::set<Controller*> ControllerSet;
		typedef ControllerSet::iterator ControllerSetItr;

		ControllerSet controllers;
	};
}

#endif //RECOGNIZER_H