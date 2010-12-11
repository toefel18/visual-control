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

	// Captures frames from a capture device (cv::VideoCapture) on a certain framerate
	// and sends the frames to all attached frame receivers
	//
	// @author Christophe hesters
	class FrameCapture
	{
		typedef cv::VideoCapture CaptureDevice;
		//typedef boost::shared_ptr<CaptureDevice> CaptureDevicePtr;
		typedef CaptureDevice* CaptureDevicePtr;

	public:

		//typedef boost::shared_ptr<FrameReceiver> FrameReceiverPtr;
		typedef FrameReceiver* FrameReceiverPtr;

		FrameCapture(float framerate = 24.0f);
		FrameCapture(CaptureDevicePtr existingCaptureDevice, float framerate = 24.0f);
		~FrameCapture();

		CaptureDevicePtr getCaptureDevice() { return captureDevice; }

		void startCapturing();
		void stopCapturing();

		bool addFrameReceiver(FrameReceiverPtr receiver);
		void removeFrameReceiver(FrameReceiverPtr receiver);
		
		//returns frames per second
		float getFramerate(){return framerateWaitMs * 1000.0f;}

		//input should be frames per sec
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

		void notifyReceivers(cv::Mat &frame);

		void captureLoop();
	};
}
#endif //FRAMECAPTURE_H