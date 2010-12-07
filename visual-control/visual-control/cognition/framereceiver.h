#ifndef FRAMERECEIVER_H
#define FRAMERECEIVER_H

//forward declaration to speedup compile time
namespace cv {class Mat;}

namespace cognition
{
	// Interfaces for classes that receive frames!
	// 
	// @author Christophe hesters
	class FrameReceiver
	{
	public:
		virtual void receiveFrame(const cv::Mat &frame) = 0;
		//check if virtual destructor is needed?
	};
}
#endif //FRAMERECEIVER_H