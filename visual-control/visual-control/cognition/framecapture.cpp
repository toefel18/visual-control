#include "framecapture.h"

namespace cognition
{

	FrameCapture::FrameCapture(float framerate)
		:framerate(framerate)
	{
		captureDevice = shared_ptr<CaptureDevice> (new CaptureDevice(0));
	}

	FrameCapture::FrameCapture(shared_ptr<CaptureDevice> existingCaptureDevice, float framerate)
		:captureDevice(existingCaptureDevice), framerate(framerate)
	{
		if(captureDevice == 0) // if the user did not provide an empty pointer, construct a capturing device
			captureDevice = shared_ptr<CaptureDevice> (new CaptureDevice(0));
	}

	FrameCapture::~FrameCapture()
	{
		//shared pointer takes care of memory management
	}

	bool FrameCapture::addFrameReceiver(FrameReceiver *receiver)
	{
		return receivers.insert(receiver).second; 
	}

	void FrameCapture::removeFrameReceiver(FrameReceiver *receiver)
	{
		receivers.erase(receiver);
	}
}