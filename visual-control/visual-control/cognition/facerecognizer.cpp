#include "facerecognizer.h"
#include <boost/thread/locks.hpp>

namespace cognition
{
	FaceRecognizer::FaceRecognizer(const std::string& name)
		:Recognizer(name)
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

	void FaceRecognizer::threadStart()
	{
		keepProcessing = true;

		while(keepProcessing)
		{
			//processing work!
		}
	}

	void FaceRecognizer::requestTreadStop()
	{
		keepProcessing = false;
	}
}