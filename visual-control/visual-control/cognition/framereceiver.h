#ifndef FRAMERECEIVER_H
#define FRAMERECEIVER_H

//forward declaration to speedup compile time
namespace cv {class Mat;}

namespace cognition
{
	/*!
	 * \brief Interfaces for classes that receive frames
	 *
	 * \author Christophe Hesters
	 */
	class FrameReceiver
	{
	public:
		/*!
		 * \brief implementors receive frames trough this method from FrameCapture
		 *
		 * \param frame the new frame
		 */
		virtual void receiveFrame(const cv::Mat &frame) = 0;
	};
}
#endif //FRAMERECEIVER_H