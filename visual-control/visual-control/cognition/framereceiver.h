#ifndef FRAMERECEIVER_H
#define FRAMERECEIVER_H

#include <cv.h>

namespace cognition
{
	class FrameReceiver
	{
	public:
		virtual void receiveFrame(const cv::Mat &frame) = 0;
		//check if virtual destructor is needed?
	};
}
#endif //FRAMERECEIVER_H