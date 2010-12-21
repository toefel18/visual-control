#include "detailedfacedetector.h"
#include <string>

namespace cognition
{
	DetailedFaceDetector::DetailedFaceDetector(DetectParams detectWhat,
		const std::string& faceCascadePath,
		FrameCapture* captureDevice,
		bool optimalSubregionDetect,
		double roiScaleFactor,
		const std::string& name)
	:FaceDetector(faceCascadePath, 
		captureDevice, 
		optimalSubregionDetect,
		roiScaleFactor, name), detectWhat(detectWhat)
	{
		//when the face dector has updates, do not automatically 
		//notify the controllers, but do that later!
		setAutoNotify(false);
	}

	DetailedFaceDetector::~DetailedFaceDetector() {}

	
	void DetailedFaceDetector::processFrame()
	{
		cv::Mat frame = getMostRecentFrame();

		if(frame.empty()) return;

		detectFaces(frame);

		//only detect features if the FaceDetector has updates!
		if(hasUpdates)
		{
			//only detect other features if the face detector had updates
			if(detectWhat > 0)
				detectFeatures(frame);

			//since this object disables autoNotify, do it manually now!
			//this is to avoid double updates to the controllers and 
			//only update when necessary 
			notifyControllers();
		}
	}

	bool DetailedFaceDetector::loadCascade(DetectParams which, const std::string& cascadePath)
	{
		switch(which)
		{
		case FACE:  return faceClassifier.load(cascadePath); 
		case LEFT_EYE: case RIGHT_EYE: case EYES:  return eyeClassifier.load(cascadePath);
		case NOSE:  return noseClassifier.load(cascadePath); 
		case MOUTH: return mouthClassifier.load(cascadePath);
		default:
			return false;
		}
	}

	void DetailedFaceDetector::detectFeatures(const cv::Mat &frame)
	{
		//get the face rectangles
		RectVector lastRects = getAreas();

		//create a new detailed result
		DetailedFaces newDetails;
	
		for(RectVectorItr faceRect = lastRects.begin(); faceRect != lastRects.end(); ++faceRect)
		{
			//create a new entry
			FaceDetails &currentFace = newDetails[*faceRect];

			if(detectWhat && EYES)
				detectEyes(frame, *faceRect, currentFace);

			if(detectWhat && NOSE)
				detectNose(frame, *faceRect, currentFace);

			if(detectWhat && MOUTH)
				detectMouth(frame, *faceRect, currentFace);
		}

		setDetailedFaceInfo(newDetails);

		notifyControllers();
	}


	void DetailedFaceDetector::detectEyes(const cv::Mat &frame, const cv::Rect& faceRect, FaceDetails &currentFace)
	{	
		//locate a region where eyes would most likely reside
		cv::Rect probableEyeLocation(faceRect);

		int originalHeight = probableEyeLocation.height;
		
		//this is integer division, this might cause inaccuracy!
		probableEyeLocation.height /= 3; //only search in a 3th of the frame
				
		//shif the frame a little bit down.
		int shiftOverY = ((originalHeight / 2) - probableEyeLocation.height);
		
		//now shift the window a little bit down
		probableEyeLocation.y += shiftOverY;
				
		//get the submatrix to scan for eyes
		cv::Mat eyeRegion = frame(probableEyeLocation);

		//create a temporary vector with storage 3. usually there will be 
		//2 eyes being detected, but capacity = 3 to avoid reallocation if more
		RectVector eyeResults(3);

		//run the detection
		eyeClassifier.detectMultiScale(eyeRegion, /*the frame, input frame for detection*/
			eyeResults, /*the the result vector, the output*/
			1.13,  /*the the scale factor opencv uses to increase the window each pass, default 1.1*/
			3,     /*minNeighbors, default: 3 (the min. number of rects to group together to call it a face)*/
			cv::CascadeClassifier::DO_CANNY_PRUNING, 
   				/*flags, Canny Prunning runs the canny edge detector to elimiate regions 
				  which are unlikely to contain faces*/
			cv::Size(10,10) /*min rect check size, the minimum!*/
			);

		if(eyeResults.size() >= 2)
		{	//recalculate the original location 
			eyeResults[0].x += faceRect.x; 
			eyeResults[0].y += faceRect.y + shiftOverY; 
			eyeResults[1].x += faceRect.x; 
			eyeResults[1].y += faceRect.y + shiftOverY; 

			//update the face map
			currentFace[LEFT_EYE] = eyeResults[0].x < eyeResults[1].x ? eyeResults[0]: eyeResults[1];
			currentFace[RIGHT_EYE] = eyeResults[0].x < eyeResults[1].x ? eyeResults[1]: eyeResults[0];
		}
		else //invalid detection, just set default rect
			currentFace[LEFT_EYE] = currentFace[RIGHT_EYE] = cv::Rect();
	}

	void DetailedFaceDetector::detectNose(const cv::Mat &frame, const cv::Rect& faceRect, FaceDetails &currentFace)
	{
		//locate a region where nose would most likely reside
		cv::Rect probableNoseLocation(faceRect);

		//these values are based on tests
		probableNoseLocation.width *= 0.43;
		probableNoseLocation.height *= 0.43;
				
		//shif the frame a little bit down.
		int shiftOverX = ((faceRect.width - probableNoseLocation.width) / 2);
		int shiftOverY = ((faceRect.height - probableNoseLocation.height) / 2);
		
		//now shift the window a little bit down
		probableNoseLocation.x += shiftOverX;
		probableNoseLocation.y += shiftOverY;
				
		//get the submatrix to scan for eyes
		cv::Mat noseRegion = frame(probableNoseLocation);

		//create a temporary vector with storage 2. usually there will be 
		//1 eyes being detected, but capacity = 2 to avoid reallocation if more
		RectVector noseResults(2);

		//run the detection
		noseClassifier.detectMultiScale(noseRegion, /*the frame, input frame for detection*/
			noseResults, /*the the result vector, the output*/
			1.13,  /*the the scale factor opencv uses to increase the window each pass, default 1.1*/
			3,     /*minNeighbors, default: 3 (the min. number of rects to group together to call it a face)*/
			cv::CascadeClassifier::DO_CANNY_PRUNING, 
   				/*flags, Canny Prunning runs the canny edge detector to elimiate regions 
				  which are unlikely to contain faces*/
			cv::Size(10,10) /*min rect check size, the minimum!*/
			);

		if(noseResults.size() >= 1)
		{	//recalculate the original location 
			noseResults[0].x += faceRect.x + shiftOverX; 
			noseResults[0].y += faceRect.y + shiftOverY; 
			
			currentFace[NOSE] = noseResults[0];
		}
		else //invalid detection, just set default rect
			currentFace[NOSE] = cv::Rect();
	}

	void DetailedFaceDetector::detectMouth(const cv::Mat &frame, const cv::Rect& faceRect, FaceDetails &currentFace)
	{
		//locate a region where the mouth would most likely reside
		cv::Rect probableMouthLocation(faceRect);

		//these values are based on tests
		probableMouthLocation.width /= 2;
		probableMouthLocation.height /= 2;
				
		//shif the frame a little bit down.
		int shiftOverX = ((faceRect.width - probableMouthLocation.width) / 2);
		int shiftOverY = probableMouthLocation.height;
		
		//now shift the window a little bit down
		probableMouthLocation.x += shiftOverX;
		probableMouthLocation.y += shiftOverY;
				
		//get the submatrix to scan for eyes
		cv::Mat mouthRegion = frame(probableMouthLocation);

		//create a temporary vector with storage 2. usually there will be 
		//1 eyes being detected, but capacity = 2 to avoid reallocation if more
		RectVector mouthResults(2);

		//run the detection
		mouthClassifier.detectMultiScale(mouthRegion, /*the frame, input frame for detection*/
			mouthResults, /*the the result vector, the output*/
			1.13,  /*the the scale factor opencv uses to increase the window each pass, default 1.1*/
			3,     /*minNeighbors, default: 3 (the min. number of rects to group together to call it a face)*/
			cv::CascadeClassifier::DO_CANNY_PRUNING, 
   				/*flags, Canny Prunning runs the canny edge detector to elimiate regions 
				  which are unlikely to contain faces*/
			cv::Size(20,10) /*min rect check size, the minimum!*/
			);

		if(mouthResults.size() >= 1)
		{	//recalculate the original location 
			mouthResults[0].x += faceRect.x + shiftOverX; 
			mouthResults[0].y += faceRect.y + shiftOverY; 
			
			currentFace[MOUTH] = mouthResults[0];
		}
		else //invalid detection, just set default rect
			currentFace[MOUTH] = cv::Rect();
	}

	void DetailedFaceDetector::setDetailedFaceInfo(const DetailedFaces& facesDetails)
	{
		boost::lock_guard<boost::mutex>(this->detailedFaceInfoLock);

		detailedFacesInfo = facesDetails;
	}

	DetailedFaceDetector::DetailedFaces DetailedFaceDetector::getDetailedFaceInfo()
	{
		boost::lock_guard<boost::mutex>(this->detailedFaceInfoLock);
		//the scope is not left until return statement is completed!
		return detailedFacesInfo;
	}

	float DetailedFaceDetector::getFaceRotation(const FaceDetails& faceDetails)
	{
		return 0.0f;
	}
}