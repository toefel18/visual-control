#include "webcamwidget.h"

WebcamWidget::WebcamWidget(QWidget *parent, CaptureDevicePtr existingCaptureDevice)
	: QWidget(parent), captureDevice(existingCaptureDevice)
{
	if(captureDevice != 0)
		captureDevice->addFrameReceiver(this);
}

WebcamWidget::~WebcamWidget()
{
	if(captureDevice != 0)
		captureDevice->removeFrameReceiver(this);
}

void WebcamWidget::receiveFrame(const cv::Mat &frame)
{
	//add image to queue and return, so that captureDevice can continue work!
	//other thread should show the images in the queue.
}