#ifndef FRAMECAPTURE_H
#define FRAMECAPTURE_H

#include "framereceiver.h"

//OpenCV video capture device
#include <highgui.h>

//for shared pointer
#include <boost/shared_ptr.hpp>

#include <set>

namespace cognition
{
	using boost::shared_ptr;


	class FrameCapture
	{
		typedef cv::VideoCapture CaptureDevice;

	public:
		FrameCapture(float framerate = 24.0f);
		FrameCapture(shared_ptr<CaptureDevice> existingCaptureDevice, float framerate = 24.0f);
		~FrameCapture();

		shared_ptr<CaptureDevice> getCaptureDevice() { return captureDevice; }

		bool addFrameReceiver(FrameReceiver *receiver);
		void removeFrameReceiver(FrameReceiver *receiver);
		
		float getFramerate(){return framerate;}
		void setFramerate(float framerate){this->framerate = framerate;}
	protected:
		shared_ptr<CaptureDevice> captureDevice;
		float framerate;
		std::set<FrameReceiver *> receivers;
	};
}
#endif //FRAMECAPTURE_H