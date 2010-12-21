#ifndef COGNITION_DETAILEDFACEDETECTOR_H
#define COGNITION_DETAILEDFACEDETECTOR_H
#include "facedetector.h"

#include <map>
#include <boost/thread/mutex.hpp>

namespace cognition
{
	// Detailed Face detector
	// detects faces and the corresponding eyes nose and mouth inside an image
	// 
	// @author Christophe Hesters
	class DetailedFaceDetector : public FaceDetector
	{
	public:
		//Detection parameters to configure inner workings of this class
		enum DetectParams 
		{
			FACE = 0,
			LEFT_EYE = 1,  /* leftmost eye, when loading the cascade, only load EYES!*/
			RIGHT_EYE = 2, /* rightmost eye, when loading the cascade, only load EYES!*/
			EYES = 3,
			NOSE = 4,
			MOUTH = 8,
			ALL = 15
		};

		//functor needed forusing std::map
		struct less_cvrect {
			bool operator() (const cv::Rect& x, const cv::Rect& y) const{
				if( (x.x < y.x) || (x.x == y.x && x.y < y.y) ) return true;
				return false;
			}
		};

		//maps the detected features to their rectangles 
		typedef std::map<DetectParams, cv::Rect> FaceDetails;

		//maps the face to the more detailed features of that face
		typedef std::map<cv::Rect, FaceDetails, less_cvrect> DetailedFaces;

		// Constructor
		// @param detectWhat			needed to specify what to detect, use loadCascade to load required classifiers
		// @param faceCascadePath		the path to the haar cascade xml file, loads the classifier!
		// @param captureDevice			if 0, nothing else it will register itself as a frame receiver
		// @param optimalSubregionDetect if true, only scan area's where a face was detected 
		// @param roiScaleFactor		how far beyond the former face rect should be scanned?
		// @param name					an name for this object
		// 
		// @author Christophe hesters
		DetailedFaceDetector(DetectParams detectWhat,
			const std::string& faceCascadePath,
			FrameCapture* captureDevice = 0,
			bool optimalSubregionDetect = false,
			double roiScaleFactor = 1.16,
			const std::string& name = "detailed face detector");
		
		virtual ~DetailedFaceDetector(void);

		//loads the specified classifier and updates the cascadePath
		bool loadCascade(DetectParams which, const std::string& cascadePath);
		
		//configures this object to detect the specified features
		//use one or a combination of the values in DetectParams
		void setDetectParams(int featuresToDetect);

		//calls FaceDetector->processFrame, then executes internal stuff
		virtual void processFrame();

		//gets the detailed face information
		DetailedFaces getDetailedFaceInfo();

		//returns the approximate face rotation angle
		static float getFaceRotation(const FaceDetails& faceDetails);

	protected:
		int detectWhat;

		cv::CascadeClassifier eyeClassifier;
		cv::CascadeClassifier noseClassifier;
		cv::CascadeClassifier mouthClassifier;

		void detectFeatures(const cv::Mat &frame);

		//calls the HAAR classifier
		void detectEyes(const cv::Mat &frame, const cv::Rect& faceRect, FaceDetails &results);
		void detectNose(const cv::Mat &frame, const cv::Rect& faceRect, FaceDetails &results);
		void detectMouth(const cv::Mat &frame, const cv::Rect& faceRect, FaceDetails &results);

		void setDetailedFaceInfo(const DetailedFaces& facesDetails);

	private:
		boost::mutex detailedFaceInfoLock;

		DetailedFaces detailedFacesInfo; 
	};
}

#endif //COGNITION_DETAILEDFACEDETECTOR_H