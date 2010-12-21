#ifndef RECOGNIZER_H
#define RECOGNIZER_H
#include <string>
#include <set>
#include <vector>
#include <cv.h>

//#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include "framereceiver.h"

namespace cognition
{
	class Controller;
	class FrameCapture;

	// Base class for detectors
	// specialize this class and implement processFrame to create a detector
	// 
	// @author Christophe hesters
	class Detector : public FrameReceiver
	{
	public:
		
		typedef std::vector<cv::Rect> RectVector;
		typedef RectVector::iterator RectVectorItr;

		//typedef boost::shared_ptr<FrameCapture> FrameCapturePtr;
		typedef FrameCapture* FrameCapturePtr;

		//typedef boost::shared_ptr<Controller> ControllerPtr;
		typedef Controller* ControllerPtr;

		Detector(const std::string& name, FrameCapturePtr captureDevice =0);
		virtual ~Detector(void);

		//detector can be given a name for convenience
		//names should not change during program execution
		std::string getName() const { return name; }
		
		//the rect where the object is last detected!
		RectVector getAreas();
		
		//create mutex, now contrllers cannot be added when thread is running!
		//methods for controllers that get notifications about state changes
		bool addController(ControllerPtr controller);
		void removeController(ControllerPtr controller);
		unsigned getControllerCount(){return controllers.size();}

		//threadstart method if this threading facility is used. 
		//do not call this method from a normal thread, it will be trapped in a processing loop!
		void threadStart();

		//sets a flag that causes the thread loop to stop, thread can be joined afterwards
		void requestTreadStop();

		//should only update next frame, taking concurrency into account
		//subclasses who want different behaviour can re-implement this!
		virtual void receiveFrame(const cv::Mat &frame);

		//this will be called by the internal thread loop, if used!
		virtual void processFrame() = 0;

		//true if the last process frame 
		bool getHasUpdates(){ return hasUpdates; }

	protected:
		std::string name;

		//if false, the thread stops
		bool keepProcessing;

		//true if the last processFrame had any updates, false otherwise
		volatile bool hasUpdates;

		//the set of controllers who want to be explicity notified of each status change
		typedef std::set< ControllerPtr > ControllerSet;
		typedef ControllerSet::iterator ControllerSetItr;
	
		ControllerSet controllers;
		boost::mutex controllersLock;

		//cv::Mat currentFrame;
		cv::Mat nextFrame;
		boost::mutex frameLoadLock;

		//copies nextFrame to currentFrame, taking concurrency into account
		cv::Mat getMostRecentFrame();

		//notify all attached controllers about updated areas
		void notifyControllers();

		//update the internal areas
		bool setAreas(const RectVector &areas);

		//checks if the object automatically notifies the controllers if new area's are set!
		bool autoNotifying(){ return autoNotify; }

		//if doAutoNotify == true, controllers are notified on 
		//tAreas, otherwise this has to be done manually
		void setAutoNotify(bool doAutoNotify){ autoNotify = doAutoNotify; }
		 
	private:
		bool autoNotify;
		//derived types should no be able to mess with this!
		//lock used for area variable which can be read/written concurrently
		boost::mutex areaLock;
		
		//should be private for correct copy and locking behaviour 
		//the areas where the classifier detected objects
		RectVector areas;
	};
}

#endif //RECOGNIZER_H