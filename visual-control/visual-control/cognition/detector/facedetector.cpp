#include "facedetector.h"
#include <boost/thread/locks.hpp>
#include <algorithm>
#include <string>

namespace cognition
{
	FaceDetector::FaceDetector(const std::string& faceCascadePath,
		FrameCapture* captureDevice,
		bool optimalSubregionDetect,
		double roiScaleFactor,
		const std::string& name)
		:Detector(name, captureDevice), 
		 roiScaleFactor(roiScaleFactor), 
		 optimalSubregionDetect(optimalSubregionDetect)
	{
		faceClassifier.load(faceCascadePath);
	}

	FaceDetector::~FaceDetector(void)
	{
	}

	void FaceDetector::processFrame()
	{
		cv::Mat frame = getMostRecentFrame();

		if(frame.empty()) return;

		detectFaces(frame);
	}

	
	void FaceDetector::detectFaces(const cv::Mat &frame)
	{
		if(faceClassifier.empty()) return;
				
		//getAreas is threadsafe
		RectVector lastRects = getAreas();

		//
		try{

			if(!(optimalSubregionDetect && lastRects.size() > 0))
			{
				RectVector tempRects;

				runFaceDetect(frame, tempRects);

				hasUpdates = setAreas(tempRects);
			} 
			else 
			{
				//last time, we detected faces somewhere, now only scan those regions!
				detectFacesInROI(lastRects, frame);
			}

		} 
		catch (cv::Exception &exc)
		{
			//just for easy debugging, this can be removed later on!
			std::string func = exc.func;
			std::string what = exc.what();
			std::string file = exc.file;
			int line = exc.line;
			int code = exc.code;
			std::string err = exc.err;
			std::string msg = exc.msg;
			
			return ;
		}
	}

	//more optimal face detection on smaller regions!
	void FaceDetector::detectFacesInROI(RectVector &lastRects, const cv::Mat &frame)
	{
		RectVector newRects;

		for(RectVectorItr i = lastRects.begin(); i != lastRects.end(); ++i)
		{
			RectVector tempRects;

			//make sure the current rectange doesn't contain unrealistic values!
			//this can cause openCV to throw!
			if(	i->x < 0 ||
				i->y < 0 ||
				i->width < 2 ||
				i->height < 2) 
				continue;

			//openCV detects a rect, so only the width is sufficent
			int adjustFactor = std::max((static_cast<int>(i->width * roiScaleFactor) - i->width) / 2, 35);

			//set the region of interest to the previous frame
			cv::Mat roiFrame = frame(*i);

			//make the frame bigger, but do not exceed boundaries!
			roiFrame.adjustROI(adjustFactor*1.10, adjustFactor*1.10, adjustFactor, adjustFactor);

			//detect on smaller frame
			runFaceDetect(roiFrame, tempRects);

			//add the results to the newRects vector!
			for(RectVectorItr iRect = tempRects.begin(); iRect != tempRects.end(); ++iRect)
			{
				iRect->x += i->x - adjustFactor;
				iRect->y += i->y - adjustFactor;
				newRects.push_back(*iRect);
			}
		}

		//if no faces were found, scan the whole image!
		if(newRects.empty())
			runFaceDetect(frame, newRects);
		
		//update the area 'result' variable
		setAreas(newRects);
	}

	//inline function to centralize the parameters!
	inline void FaceDetector::runFaceDetect(const cv::Mat &frame, RectVector &results)
	{
		faceClassifier.detectMultiScale(frame, /*the frame, input frame for detection*/
			results, /*the the result vector, the output*/
			1.13,  /*the the scale factor opencv uses to increase the window each pass, default 1.1*/
			3,     /*minNeighbors, default: 3 (the min. number of rects to group together to call it a face)*/
			cv::CascadeClassifier::DO_CANNY_PRUNING, 
   				/*flags, Canny Prunning runs the canny edge detector to elimiate regions 
				  which are unlikely to contain faces*/
			cv::Size(55, 55) /*min rect check size, the minimum!*/
			);
	}
}