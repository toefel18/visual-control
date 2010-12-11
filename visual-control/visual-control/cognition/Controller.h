#ifndef COGNITION_CONTROLLER_H
#define COGNITION_CONTROLLER_H

namespace cognition
{
	class Detector;

	// interface for controllers
	// 
	// @author Christophe hesters
	class Controller
	{
	public:
		//warning about using shared ptr here!
		//if someone is not using shared pointers to store
	    //their recognizers, (and there is no refcount!)
	    //supplying a shared ptr will cauze it to delete
		//the recognizer!
		virtual void stateChanged(Detector *recognizer) = 0;
	};

}

#endif //COGNITION_CONTROLLER_H