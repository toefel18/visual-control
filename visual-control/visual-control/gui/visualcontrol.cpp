#include "visualcontrol.h"
#include "webcamwidget.h"
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>

#include <QDebug>
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QMessageBox>
#include <QInputDialog>
#include <logger.h>

#include "cognition/detector/facedetector.h"
#include "cognition/trainerimage.h"

namespace gui
{

	const cv::Size VisualControl::testImageSize(150,200);

	VisualControl::VisualControl(QWidget *parent, Qt::WFlags flags)
		: QMainWindow(parent, flags)
	{
		setupFramework();
		setupGUI();

		setMinimumSize(QSize(640,480));

		frameCaptureThread = boost::thread(
			boost::bind(&cognition::FrameCapture::startCapturing, frameCapture));

		faceDetectorThread = boost::thread(
			boost::bind(&cognition::Detector::threadStart, faceDetector));

		Logger::getInstance().log("Ready...");
	}

	VisualControl::~VisualControl()
	{
	}

	void VisualControl::setupFramework()
	{
		using boost::shared_ptr;
		using cognition::DetailedFaceDetector;
		using cognition::EigenfaceRecognizer;

		//videoCapture = shared_ptr<VideoCapture>( new VideoCapture(0) );
		frameCapture = shared_ptr<cognition::FrameCapture>( new cognition::FrameCapture(32) );

		//update the path of the classifier to the local path of your openCV installation!
		faceDetector = shared_ptr<DetailedFaceDetector>( 
			new DetailedFaceDetector(DetailedFaceDetector::ALL, 
			"C:/OpenCV2.1/data/haarcascades/haarcascade_frontalface_alt.xml",
			frameCapture.get(), false, 1.16));

		faceDetector->loadCascade(DetailedFaceDetector::EYES, "C:/OpenCV2.1/data/haarcascades/haarcascade_eye.xml");
		faceDetector->loadCascade(DetailedFaceDetector::NOSE, "C:/OpenCV2.1/data/haarcascades/haarcascade_mcs_nose.xml");
		faceDetector->loadCascade(DetailedFaceDetector::MOUTH, "C:/OpenCV2.1/data/haarcascades/haarcascade_mcs_mouth.xml");

		frameCapture->addFrameReceiver(faceDetector.get());

		recognizer = shared_ptr<EigenfaceRecognizer>( new cognition::EigenfaceRecognizer );
	}

	void VisualControl::closeEvent(QCloseEvent *event)
	{
		//webcamController->unregister();
		faceDetector->removeController(webcamWidget);

		//qDebug() << "controller count = " << faceDetector->getControllerCount();

		frameCapture->stopCapturing();

		faceDetector->requestTreadStop();

		frameCaptureThread.interrupt();

		faceDetectorThread.interrupt();

	//	qDebug() << "Close event called, joining";
		frameCaptureThread.join();

		faceDetectorThread.join();

		//event->accept();
	
		//QMainWindow::closeEvent(event);
	}

	void VisualControl::setupGUI()
	{
		webcamWidget = new WebcamWidget(this, frameCapture.get());

		faceDetector->addController(webcamWidget); 

		logWidget = new QListWidget;

		captureTrainingImageButton = new QPushButton(tr("Capture face and store it as training image"));
		connect(captureTrainingImageButton, SIGNAL(clicked()), this, SLOT(captureTrainingImage()));

		trainRecognizerButton = new QPushButton(tr("Train recognizer"));
		connect(trainRecognizerButton, SIGNAL(clicked()), this, SLOT(trainRecognizer()));

		recognizeFaceButton = new QPushButton(tr("Recognize Visible Faces"));
		connect(recognizeFaceButton, SIGNAL(clicked()), this, SLOT(recognizeFaces()));

		QWidget *centralWidget = new QWidget;
		QVBoxLayout *mainLayout = new QVBoxLayout;
		QHBoxLayout *buttonsLayout = new QHBoxLayout;

		buttonsLayout->addWidget(captureTrainingImageButton);
		buttonsLayout->addWidget(trainRecognizerButton);
		buttonsLayout->addWidget(recognizeFaceButton);
	
		mainLayout->addWidget(webcamWidget);
		mainLayout->addLayout(buttonsLayout);
		mainLayout->addWidget(logWidget);

		centralWidget->setLayout(mainLayout);

		Logger::getInstance().setLogWidget(logWidget);
		
		setCentralWidget(centralWidget);
	}

	void VisualControl::captureTrainingImage()
	{
		cv::Mat frame;
		cognition::Detector::RectVector faces;

		captureFrameAndFaces(faces, frame);
				
		if(faces.size() == 0)
		{
			QMessageBox::information(this, "No faces found", "The detector did not find any faces!");
		} 
		else 
		{
			frame = frame.clone();
			cognition::TrainerImage convertor(testImageSize, true, "images/");
			cv::Mat faceRegion;
			for(std::vector<cv::Rect>::iterator face = faces.begin(); face != faces.end(); ++face)
			{
				faceRegion = frame(*face);
				QString filename = QInputDialog::getText(this,
					tr("Image name"),
					tr("enter image name (enter extension too, it determines the image format!)"));

				if(filename.size() < 1) continue;

				if(!convertor.processAndSaveImage(faceRegion, filename.toStdString()))
					QMessageBox::information(this, "Failed", "Could not process and save the image!");
			}
		}
	}

	void VisualControl::trainRecognizer()
	{
		using namespace boost::filesystem;

		path dir("images");
		directory_iterator end;

		for(directory_iterator file(dir); file != end; ++file)
		{
			if(is_regular_file(*file))
				recognizer->addTrainingImage(file->path().string(), file->filename());
		}

		if(recognizer->train())
			QMessageBox::information(this, "Success", "The recognizer has succesfully finished training!");
		else
			QMessageBox::information(this, "Error", "The recognizer has indicated that it did not train correctly!");
	}

	void VisualControl::recognizeFaces()
	{
		if(recognizer->trained())
		{
			cv::Mat frame;
			cognition::Detector::RectVector faces;

			captureFrameAndFaces(faces, frame);
				
			if(faces.size() == 0)
				QMessageBox::information(this, "No faces found", "The detector did not find any faces!");
			else 
			{
				frame = frame.clone();
				cognition::TrainerImage convertor(testImageSize, true);
				cv::Mat faceRegion;
				cv::Mat processedFace;

				for(std::vector<cv::Rect>::iterator face = faces.begin(); face != faces.end(); ++face)
				{
					faceRegion = frame(*face);
					processedFace = convertor.processImage(faceRegion);
					std::string name = recognizer->recognize(processedFace);
					Logger::getInstance().log(std::string("Recognized: ") + name);
				}
			}
		}
		else 
		{
			QMessageBox::information(this, "Recognizer is not trained", "Recognizer is not trained or failed to train, add enough training images and train the recognizer!");
		}
	}

	void VisualControl::captureFrameAndFaces(cognition::Detector::RectVector &rects, cv::Mat &frame)
	{
		rects = webcamWidget->getCurrentFaces();
		frame = webcamWidget->getCurrentFrame();
		//cognition::FaceDetector detector("C:/OpenCV2.1/data/haarcascades/haarcascade_frontalface_alt.xml");
		//*frameCapture->getCaptureDevice() >> frame;

		//detector.receiveFrame(frame);
		//detector.processFrame();
		//rects = detector.getAreas();
	}
}