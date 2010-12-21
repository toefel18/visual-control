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

		using cognition::DetailedFaceDetector;

		facesLock.lock();
		cognition::Detector::RectVector facesDetected = faces; 
		DetailedFaceDetector::DetailedFaces detailedFacesDetected = detailedFaces;
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

			for(DetailedFaceDetector::DetailedFaces::iterator i = detailedFacesDetected.begin();
				i != detailedFacesDetected.end(); ++i)
			{
				//i->first == the face rect
				//i->second == a map (facepart id) -> (rect)

				//draw the face, stored in the first property of the map pair
				painter.setPen(QPen(Qt::red, 2));
				painter.drawRect((i->first).x, (i->first).y, (i->first).width, (i->first).height);
				
				//draw the eyes
				painter.setPen(QPen(Qt::blue, 2));
				cv::Rect &leftEye = (i->second)[DetailedFaceDetector::LEFT_EYE];
				painter.drawRect(leftEye.x, leftEye.y, leftEye.width, leftEye.height);

				cv::Rect &rightEye = (i->second)[DetailedFaceDetector::RIGHT_EYE];
				painter.drawRect(rightEye.x, rightEye.y, rightEye.width, rightEye.height);		
				
				//draw the nose
				painter.setPen(QPen(Qt::green, 2));
				cv::Rect &nose = (i->second)[DetailedFaceDetector::NOSE];
				painter.drawRect(nose.x, nose.y, nose.width, nose.height);

				//draw the mouth
				painter.setPen(QPen(Qt::yellow, 2));
				cv::Rect &mouth = (i->second)[DetailedFaceDetector::MOUTH];
				painter.drawRect(mouth.x, mouth.y, mouth.width, mouth.height);
			}

			//for(cognition::Detector::RectVectorItr i = facesDetected.begin();
			//	i != facesDetected.end(); ++i)
			//{
			//	painter.drawRect((*i).x, (*i).y, (*i).width, (*i).height); 
			//}
		}
	}

	
	void WebcamWidget::stateChanged(cognition::Detector *detector)
	{
		boost::lock_guard<boost::mutex>(this->facesLock);

		//maybe move this outside critical section
		updateFramerate();
		
		faces = detector->getAreas();
		detailedFaces = static_cast<cognition::DetailedFaceDetector *>(detector)->getDetailedFaceInfo();
	}

	inline void WebcamWidget::updateFramerate()
	{
		//http://www.boost.org/doc/libs/1_45_0/doc/html/date_time/posix_time.html

		boost::posix_time::ptime currentTime = boost::posix_time::microsec_clock::local_time();
		boost::posix_time::time_duration duration = currentTime.time_of_day() - previousTime.time_of_day();
		if(duration.total_milliseconds() != 0)
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