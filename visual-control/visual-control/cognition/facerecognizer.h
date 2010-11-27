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
		FaceRecognizer(const std::string& name = "face recognizer");
		virtual ~FaceRecognizer(void);

		//should only update next frame, taking concurrency into account
		virtual void receiveFrame(const cv::Mat &frame);

		void threadStart();
		void requestTreadStop();

	protected:
		cv::Mat currentFrame;
		cv::Mat nextFrame;
		boost::mutex objectLock;

		bool keepProcessing;

		//copies nextFrame to currentFrame, taking concurrency into account
		void loadNextFrameIntoCurrent();
	};
}

#endif //COGNITION_FACERECOGNIZER_H