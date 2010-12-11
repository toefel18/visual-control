#include "webcamwidget.h"
#include <QImage>
#include <QPainter>
#include <QPen>
#include <qdebug.h>
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

		previousTime = boost::posix_time::microsec_clock::local_time();
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
		cognition::Detector::RectVector facesDetected = faces; 
		facesLock.unlock();

		if(!currentFrame.empty())
		{
			QImage img = convertToQImage(currentFrame);

			QPainter painter(this);
			painter.drawImage(QPoint(0,0), img);
			
			QString fps;
			fps.setNum(detectionFramerate) += " fps";

			//y coordinate is baseline of font!
			painter.drawText(QPoint(3,15), fps);

			painter.setPen(QPen(Qt::red, 3));

			for(cognition::Detector::RectVectorItr i = facesDetected.begin();
				i != facesDetected.end(); ++i)
			{
				painter.drawRect((*i).x, (*i).y, (*i).width, (*i).height); 
			}
		}
	}

	
	void WebcamWidget::stateChanged(cognition::Detector *recognizer)
	{
		boost::lock_guard<boost::mutex>(this->facesLock);

		//maybe move this outside critical section
		updateFramerate();
		
		faces = recognizer->getAreas();
	}

	inline void WebcamWidget::updateFramerate()
	{
		//http://www.boost.org/doc/libs/1_45_0/doc/html/date_time/posix_time.html

		boost::posix_time::ptime currentTime = boost::posix_time::microsec_clock::local_time();
		boost::posix_time::time_duration duration = currentTime.time_of_day() - previousTime.time_of_day();
		detectionFramerate = (1000 / duration.total_milliseconds());
		previousTime = currentTime;
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