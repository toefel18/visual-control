#ifndef FRAMECAPTURE_H
#define FRAMECAPTURE_H

#include "framereceiver.h"

//OpenCV video capture device
#include <cv.h>
#include <highgui.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <set>

namespace cognition
{

	/*! \brief Captures frames from a capture device (cv::VideoCapture) on a certain framerate
	 *		  and sends the frames to all attached frame receivers
	 *
	 * \author Christophe hesters
	 */
	class FrameCapture
	{
		typedef cv::VideoCapture CaptureDevice;
		//typedef boost::shared_ptr<CaptureDevice> CaptureDevicePtr;
		typedef CaptureDevice* CaptureDevicePtr;

	public:

		//typedef boost::shared_ptr<FrameReceiver> FrameReceiverPtr;
		typedef FrameReceiver* FrameReceiverPtr;

		/*! 
		 * \brief Constructs the object, automatically creates a captureDevice
		 * 
		 * \param framerate	the framerate at which it sends updates to the receivers
		 */
		FrameCapture(float framerate = 24.0f);

		/*! 
		 * \brief Constructs the object with an existing captureDevices
		 * 
		 * \param existingCaptureDevice	a valid CaptureDevice (warning: destructor will release the feed!)
		 * \param framerate				the framerate at which it sends updates to the receivers
		 */
		FrameCapture(CaptureDevicePtr existingCaptureDevice, float framerate = 24.0f);

		/*! 
		 * \brief Closes the captureDevice, even if it the device already existed!
		 */
		~FrameCapture();

		/*! 
		 * \brief Returns a pointer to the internal capture device
		 * \returns pointer to the interal capture devices
		 */
		CaptureDevicePtr getCaptureDevice() { return captureDevice; }

		/*! 
		 * \brief Starts capturing frames. This will be a loop that captures
		 *        frames and sends them to frame receivers. This method should
		 *		  be called by a thread that is free and designed for this purpose.
		 */
		void startCapturing();

		/*! 
		 * \brief Signals the capturing loop to stop in a clean way.
		 */
		void stopCapturing();

		/*! 
		 * \brief Adds an object that wants to receive frames (thread-safe)
		 * 
		 * \param receiver	an object that will get frames from this object
		 *
		 * \returns true if it was added. false if it already was registered or if it failed
		 */
		bool addFrameReceiver(FrameReceiverPtr receiver);

		/*! 
		 * \brief Removes an object that received frames (thread-safe)
		 * 
		 * \param receiver	the object to remove
		 */
		void removeFrameReceiver(FrameReceiverPtr receiver);
		
		/*! 
		 * \brief Returns the number of frames per second this object sends to its receivers
		 * 
		 * \returns number of frames per second
		 */
		float getFramerate(){return framerateWaitMs * 1000.0f;}

		/*! 
		 * \brief Sets the framerate at which this object sends frames to its receivers
		 * 
		 * \param framerate the number of frames per second
		 */
		void setFramerate(float framerate){
			this->framerateWaitMs = static_cast<unsigned long>(1000.0f / framerate);
		}
	protected:
		CaptureDevicePtr captureDevice;
		unsigned long framerateWaitMs;
		bool keepCapturing;

		typedef std::set<FrameReceiverPtr > ReceiverSet;
		typedef ReceiverSet::iterator ReceiverSetItr;

		ReceiverSet receivers;
		boost::mutex receiversLock;

		/*! 
		 * \brief Notifies the frame receivers with the new frame (thread-safe)
		 * 
		 * \param frame the frame to send to the frame receivers
		 */
		void notifyReceivers(cv::Mat &frame);

		/*! 
		 * \brief Captures frames and notifies receivers.
		 */
		void captureLoop();
	};
}
#endif //FRAMECAPTURE_H