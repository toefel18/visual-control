#include "webcamwidget.h"
#include <QImage>
#include <QPainter>
#include <QPen>
#include <cv.h>
#include <boost/thread/locks.hpp>
#include "cognition/framecapture.h"

namespace gui
{
	using boost::shared_ptr;

	WebcamWidget::WebcamWidget(QWidget *parent, FrameCapturePtr existingCaptureDevice)
		: QWidget(parent)
	{
		if(existingCaptureDevice != 0)
			existingCaptureDevice->addFrameReceiver(this);
		//frameForwarder = FrameForwarderPtr(
		//	new cognition::FrameForwarder<WebcamWidget>(this, existingCaptureDevice, true));
	}

	WebcamWidget::~WebcamWidget()
	{
		//frameForwarder->unregister();
	}

	QImage WebcamWidget::convertToQImage(const cv::Mat& mat) const 
	{
		IplImage cvimage = (IplImage)mat;
		return QImage((const uchar *)cvimage.imageData, cvimage.width, cvimage.height, QImage::Format_RGB888).rgbSwapped();
	}

	void WebcamWidget::paintEvent(QPaintEvent* event)
	{
		loadNextFrameIntoCurrent();

		facesLock.lock();
		cognition::Recognizer::RectVector facesDetected = faces; 
		facesLock.unlock();

		if(!currentFrame.empty())
		{
			QImage img = convertToQImage(currentFrame);

			QPainter painter(this);
			painter.drawImage(QPoint(0,0), img);

			painter.setPen(QPen(Qt::red, 3));

			for(cognition::Recognizer::RectVectorItr i = facesDetected.begin();
				i != facesDetected.end(); ++i)
			{
				painter.drawRect((*i).x, (*i).y, (*i).width, (*i).height); 
			}
		}
	}

	void WebcamWidget::stateChanged(cognition::Recognizer *recognizer)
	{
		boost::lock_guard<boost::mutex>(this->facesLock);

		faces = recognizer->getAreas();
	}

	void WebcamWidget::receiveFrame(const cv::Mat &frame)
	{
		frameLoadLock.lock();

		nextFrame = frame.clone();
		
		frameLoadLock.unlock();

		//update causes a paintEvent, which calls loadNextFrameIntoCurrent
		//this could cause deadlock
		update();
	}

	void WebcamWidget::loadNextFrameIntoCurrent()
	{
		boost::lock_guard<boost::mutex>(this->frameLoadLock);

		currentFrame = nextFrame;
	}

}