#ifndef COGNITION_FACEDETECTOR_H
#define COGNITION_FACEDETECTOR_H
#include "../detector.h"

#include <boost/thread/mutex.hpp>

//#include <cv.h> //not needed, included in base interfaces

namespace cognition
{
	// Face detector
	// detects a face insida an image
	// 
	// @author Christophe hesters
	class FaceDetector : public Detector
	{
	public:

		// Constructor
		// @param faceCascadePath		the path to the haar cascade xml file, loads the classifier!
		// @param captureDevice			if 0, nothing else it will register itself as a frame receiver
		// @param optimalSubregionDetect if true, only scan area's where a face was detected 
		// @param roiScaleFactor		how far beyond the former face rect should be scanned?
		// @param name					an name for this object
		// 
		// @author Christophe hesters
		FaceDetector(const std::string& faceCascadePath, 
			FrameCapture* captureDevice = 0,
			bool optimalSubregionDetect = false,
			double roiScaleFactor = 1.16,
			const std::string& name = "face detector");
		virtual ~FaceDetector(void);

		//this will be called by the processing loop! or by the client if the threading system is not used!
		virtual void processFrame();

		bool empty() const { 
			return faceClassifier.empty(); 
		}

	protected:
		cv::CascadeClassifier faceClassifier;

		double roiScaleFactor;
		bool optimalSubregionDetect;

		//calls the HAAR classifier
		inline void runDetect(cv::Mat &frame, RectVector &results);

		//detects all faces
		void detectFaces();

		//detects faces in the lastRects areas
		void detectFacesInROI(RectVector &lastRects, cv::Mat &frame);
		
		//loads a classified, now protected because it needs locks for concurrency control
		bool loadClassifier(const std::string& faceCascadePath){
			return faceClassifier.load(faceCascadePath);
		}
	};
}

#endif //COGNITION_FACEDETECTOR_H