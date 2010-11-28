#ifndef COGNITION_FACERECOGNIZER_H
#define COGNITION_FACERECOGNIZER_H
#include "recognizer.h"

#include <boost/thread/mutex.hpp>

//#include <cv.h> //not needed, included in base interfaces

namespace cognition
{
	class FaceRecognizer : public Recognizer
	{
	public:
		FaceRecognizer(const std::string& name = "face recognizer", 
			shared_ptr<FrameCapture> captureDevice = shared_ptr<FrameCapture>());
		virtual ~FaceRecognizer(void);

		//should only update next frame, taking concurrency into account
		virtual void receiveFrame(const cv::Mat &frame);

		//this will be called by the processing loop! or by the client if the threading system is not used!
		virtual void processFrame();

	protected:
		cv::Mat currentFrame;
		cv::Mat nextFrame;

		boost::mutex objectLock;

		//copies nextFrame to currentFrame, taking concurrency into account
		void loadNextFrameIntoCurrent();
	};
}

#endif //COGNITION_FACERECOGNIZER_H