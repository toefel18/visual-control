#include "framecapture.h"
#include <boost/thread/locks.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

namespace cognition
{

	FrameCapture::FrameCapture(float framerate)
	{
		captureDevice = new CaptureDevice(0);
		setFramerate(framerate);
	}

	FrameCapture::FrameCapture(CaptureDevicePtr existingCaptureDevice, float framerate)
		:captureDevice(existingCaptureDevice)
	{
		if(captureDevice == 0) // if the user did not provide an empty pointer, construct a capturing device
			captureDevice = new CaptureDevice(0);

		setFramerate(framerate);
	}

	FrameCapture::~FrameCapture()
	{
		
	}

	void FrameCapture::startCapturing(bool createNewThread)
	{
		keepCapturing = true;

		if(createNewThread)
			boost::thread(boost::bind(&FrameCapture::captureLoop, this));
		else
			captureLoop();
	}

	void FrameCapture::stopCapturing()
	{
		keepCapturing = false;
	}

	void FrameCapture::captureLoop()
	{
		while(keepCapturing)
		{
			cv::Mat frame;
			*captureDevice >> frame; 
			cv::flip( frame, frame, 1); //1 is flip horizontal
			notifyReceivers(frame);
			
			boost::this_thread::interruptible_wait(framerateWaitMs);
		}
	}

	void FrameCapture::notifyReceivers(cv::Mat &frame)
	{
		boost::lock_guard<boost::mutex>(this->receiversLock);

		for(ReceiverSetItr itr = receivers.begin(); itr != receivers.end(); ++itr)
			(*itr)->receiveFrame(frame);
	}

	bool FrameCapture::addFrameReceiver(FrameReceiverPtr receiver)
	{
		boost::lock_guard<boost::mutex>(this->receiversLock);

		return receivers.insert(receiver).second; 
	}

	void FrameCapture::removeFrameReceiver(FrameReceiverPtr receiver)
	{
		boost::lock_guard<boost::mutex>(this->receiversLock);

		receivers.erase(receiver);
	}
}