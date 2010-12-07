#include "visualcontrol.h"
#include "webcamwidget.h"
#include <boost/bind.hpp>
#include <QDebug>
#include <QCloseEvent>

namespace gui
{
	VisualControl::VisualControl(QWidget *parent, Qt::WFlags flags)
		: QMainWindow(parent, flags)
	{
		setupFramework();
		setupGUI();

		setMinimumSize(QSize(640,480));

		frameCaptureThread = boost::thread(
			boost::bind(&cognition::FrameCapture::startCapturing, frameCapture, false));

		faceRecognizerThread = boost::thread(
			boost::bind(&cognition::Recognizer::threadStart, faceRecognizer));
	}

	VisualControl::~VisualControl()
	{
	}

	void VisualControl::setupFramework()
	{
		using boost::shared_ptr;
		//videoCapture = shared_ptr<VideoCapture>( new VideoCapture(0) );
		frameCapture = shared_ptr<cognition::FrameCapture>( new cognition::FrameCapture(32) );

		//update the path of the classifier to the local path of your openCV installation!
		faceRecognizer = shared_ptr<cognition::FaceRecognizer>( 
			new cognition::FaceRecognizer("C:/OpenCV2.1/data/haarcascades/haarcascade_frontalface_alt.xml",
			frameCapture.get(), false, 1.16));
		frameCapture->addFrameReceiver(faceRecognizer.get());
	}

	void VisualControl::closeEvent(QCloseEvent *event)
	{
		//webcamController->unregister();
		faceRecognizer->removeController(webcamWidget);

		//qDebug() << "controller count = " << faceRecognizer->getControllerCount();

		frameCapture->stopCapturing();

		faceRecognizer->requestTreadStop();

		frameCaptureThread.interrupt();

		faceRecognizerThread.interrupt();

	//	qDebug() << "Close event called, joining";
		frameCaptureThread.join();

		faceRecognizerThread.join();

		//event->accept();
	
		//QMainWindow::closeEvent(event);
	}

	void VisualControl::setupGUI()
	{
		webcamWidget = new WebcamWidget(this, frameCapture.get());

		faceRecognizer->addController(webcamWidget); 
		
		setCentralWidget(webcamWidget);
	}
}