#ifndef WEBCAMWIDGET_H
#define WEBCAMWIDGET_H

#include <QWidget>
#include <boost/shared_ptr.hpp>
#include "cognition/framecapture.h"
#include "cognition/framereceiver.h"

class WebcamWidget : public QWidget, public cognition::FrameReceiver
{
	Q_OBJECT

public:
	typedef boost::shared_ptr<cognition::FrameCapture> CaptureDevicePtr;

	//enum ViewConfiguration{
	//	VIEW_FRAMERATE, //depends on the framerate of FrameCapture
	//	VIEW_REALTIME   //uses interal VideoCapture to get ~24fps, maybe not safe!
	//};	 

	WebcamWidget(QWidget *parent, CaptureDevicePtr existingCaptureDevice);
	virtual ~WebcamWidget();

	virtual void receiveFrame(const cv::Mat &frame);

	//void cvMathToQtImage();

	////defines the strategy for querying frames
	//void setViewConfig(ViewConfiguration config) {viewConfig = config;}
	//ViewConfiguration setViewConfig(ViewConfiguration config) {return viewConfig;}

private:
	CaptureDevicePtr captureDevice;
	//ViewConfiguration viewConfig;
};

#endif // WEBCAMWIDGET_H
