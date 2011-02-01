#ifndef COGNITION_DETECTOR_H
#define COGNITION_DETECTOR_H
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

	/*!
	 * \brief Base class for detectors. Provides threading system and thread safe 
	 *		  methods for getting the most recent frame, notifying controllers
	 *		  and getting the detection result.
	 * 
	 * \author Christophe Hesters
	 */
	class Detector : public FrameReceiver
	{
	public:
		
		typedef std::vector<cv::Rect> RectVector;
		typedef RectVector::iterator RectVectorItr;

		//typedef boost::shared_ptr<FrameCapture> FrameCapturePtr;
		typedef FrameCapture* FrameCapturePtr;

		//typedef boost::shared_ptr<Controller> ControllerPtr;
		typedef Controller* ControllerPtr;

		/*!
		 * \brief Constructs this object with a name and the selected capture device
		 * 
		 * \param name			Description of parameter name.
		 * \param captureDevice	Description of parameter captureDevice.
		 */
		Detector(const std::string& name, FrameCapturePtr captureDevice =0);
		virtual ~Detector(void);

		/*!
		 * \brief Returns the name of the detector
		 * 
		 * \returns the name
		 */
		std::string getName() const { return name; }
		
		/*!
		 * \brief Returns the latest detection result (thread-safe)
		 * 
		 * \returns the rectangles wherin the detected objects reside
		 */
		RectVector getAreas();
		
		/*!
		 * \brief Adds the controller to the internal set. It will receive updated status notifications (thread-safe)
		 *
		 * \param controller pointer to the controller to receive notifications of this object
		 * \returns true if added, false on failure or if the controller was already in the set
		 */
		bool addController(ControllerPtr controller);

		/*!
		 * \brief Removes the controller to the internal set. (thread-safe)
		 *
		 * \param controller pointer to the controller that needs to be removed
		 */
		void removeController(ControllerPtr controller);

		/*!
		 * \brief Gets the internal controller count
		 *
		 * \returns the number of controllers registered
		 */
		unsigned getControllerCount(){return controllers.size();}

		/*!
		 * \brief Starts the internal processing thread. Make sure this method is called
		 *		  from a new thread as it will be trapped inside a loop until requestThreadStop
		 *		  is called or an internal error causes it to stop. The internal loop calls
		 *		  processFrame!
		 */
		void threadStart();

		/*!
		 * \brief Signals the internal processing thread to stop. It sets the internal
		 *		  keepProcessing flag to false, which should terminate the internal 
		 *		  processing loop in a clean way.
		 */
		void requestTreadStop();

		/*!
		 * \brief Updates the internal latest frame for processing.(thread-safe)
		 *
		 * \param frame the latest frame captured.
		 */
		virtual void receiveFrame(const cv::Mat &frame);

		/*!
		 * \brief Template method for implementors. They should call getMostRecentFrame
		 *		  , check if the returned frame it is not empty and process it. 
		 */
		virtual void processFrame() = 0;

		/*!
		 * \brief Returns true if there are updates.
		 *
		 * \returns true if the detector had updates
		 */
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

		/*!
		 * \brief Retrieves the most recent frame
		 *
		 * \returns a copy of the most recent frame
		 */
		cv::Mat getMostRecentFrame();

		/*!
		 * \brief Notifies all the attached controllers about updates
		 */
		void notifyControllers();

		/*!
		 * \brief Updates the internal detection areas, and checks if they are really different
		 * 
		 * \param areas		the new rectangle vector where objects are detected
		 *
		 * \returns true if areas was indeed different than the previous state, false if there wasn't a real change
		 */
		bool setAreas(const RectVector &areas);

		/*!
		 * \brief Checks if the detector automatically notifies on setAreas
		 *
		 * \returns true if it automatically notifies on setAreas, false otherwise
		 */
		bool autoNotifying(){ return autoNotify; }

		/*!
		 * \brief Sets an internal flag that causes setAreas to send
		 *		  attached controllers notifications automatically.
		 *
		 * \param	doAutoNotify true if it needs to send updates automatically, 
		 *			false if it wants to this automatically
		 */
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

#endif //COGNITION_DETECTOR_H