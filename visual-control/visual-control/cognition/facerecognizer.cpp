#include "facerecognizer.h"
#include <boost/thread/locks.hpp>

namespace cognition
{
	FaceRecognizer::FaceRecognizer(const std::string& name, 
			shared_ptr<FrameCapture> captureDevice)
		:Recognizer(name, captureDevice)
	{
	}

	FaceRecognizer::~FaceRecognizer(void)
	{
	}

	void FaceRecognizer::receiveFrame(const cv::Mat &frame)
	{
		//locks on entry, destructor unlocks!
		boost::lock_guard<boost::mutex>(this->objectLock);

		nextFrame = frame;
	}

	void FaceRecognizer::loadNextFrameIntoCurrent()
	{
		//locks on entry, destructor unlocks!
		boost::lock_guard<boost::mutex>(this->objectLock);

		currentFrame = nextFrame;
	}

	void FaceRecognizer::processFrame()
	{
		//do face detection, store result in rect


	}
}