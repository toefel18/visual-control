#ifndef VISUALCONTROL_H
#define VISUALCONTROL_H

#include <QtGui/QMainWindow>
#include <highgui.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include "cognition/framecapture.h"
#include "cognition/detector/detailedfacedetector.h"
#include "cognition/util.h"
#include "cognition/recognizer/eigenfacerecognizer.h"

class QListWidget;
class QPushButton;

namespace gui
{
	class WebcamWidget;

	/*! \brief Main application window
	 * 
	 * \author Christophe hesters
	 */
	class VisualControl : public QMainWindow
	{
		Q_OBJECT

	public:
		VisualControl(QWidget *parent = 0, Qt::WFlags flags = 0);
		~VisualControl();

	public slots:
		void captureTrainingImage();
		void trainRecognizer();
		void recognizeFaces();

	protected:

		static const cv::Size testImageSize;

		WebcamWidget *webcamWidget;
		QListWidget *logWidget;

		QPushButton *captureTrainingImageButton;
		QPushButton *trainRecognizerButton;
		QPushButton *recognizeFaceButton;

		boost::thread frameCaptureThread;
		boost::thread faceDetectorThread;
		
		//auto_ptr would suffice here too, for our purposes! 
		boost::shared_ptr<cv::VideoCapture> videoCapture;
		boost::shared_ptr<cognition::FrameCapture> frameCapture;
		boost::shared_ptr<cognition::DetailedFaceDetector> faceDetector;
		boost::shared_ptr<cognition::EigenfaceRecognizer> recognizer;

		void closeEvent(QCloseEvent *event);

		void setupFramework();
		void setupGUI();

		void captureFrameAndFaces(cognition::Detector::RectVector &rects, cv::Mat &frame);

	};

}
#endif // VISUALCONTROL_H
