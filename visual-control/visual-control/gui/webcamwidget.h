#ifndef WEBCAMWIDGET_H
#define WEBCAMWIDGET_H

#include <QWidget>
//#include <boost/shared_ptr.hpp>
//#include <boost/weak_ptr.hpp>
#include "cognition/framereceiver.h"
#include "cognition/controller.h"
//#include "cognition/util.h"
#include <boost/thread/mutex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "cognition/detector.h"
#include "cognition/detector/detailedfacedetector.h"

//forward declaration
namespace cognition{class FrameCapture;}

namespace gui
{

	/*! \brief	Widget that displays the webcam feed, provided by a capture devices
	 *			it also draws the rectangles it recieves from detectors!
	 * 
	 * \author Christophe hesters
	 */
	class WebcamWidget : public QWidget, 
		public cognition::FrameReceiver,
		public cognition::Controller  
	{
		Q_OBJECT

	public:
		//typedef boost::shared_ptr<cognition::FrameCapture> FrameCapturePtr;
		typedef cognition::FrameCapture* FrameCapturePtr;

		WebcamWidget(QWidget *parent, FrameCapturePtr existingCaptureDevice);
		virtual ~WebcamWidget();

		QImage convertToQImage(const cv::Mat& mat) const;

		void receiveFrame(const cv::Mat &frame);
		
		//refactor this in another class, this is just for testing!
		void stateChanged(cognition::Detector *detector);

		
		//just for convenience!
		cv::Mat getCurrentFrame();
		cognition::Detector::RectVector getCurrentFaces();

	protected:
		cognition::Detector::RectVector faces;
		cognition::DetailedFaceDetector::DetailedFaces detailedFaces;
		boost::mutex facesLock;

		cv::Mat currentFrame;
		cv::Mat nextFrame;

		//lock for loading new frames
		boost::mutex frameLoadLock;

		//copies nextFrame to currentFrame, taking concurrency into account
		virtual void loadNextFrameIntoCurrent();
		void paintEvent(QPaintEvent* event);

		boost::posix_time::ptime previousTime;
		//std::time_t previousTime;
		volatile int detectionFramerate;

		void updateFramerate();
	};

}

#endif // WEBCAMWIDGET_H
