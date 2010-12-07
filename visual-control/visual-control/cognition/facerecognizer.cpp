#include "facerecognizer.h"
#include <boost/thread/locks.hpp>
#include <algorithm>
#include <string>

namespace cognition
{
	FaceRecognizer::FaceRecognizer(const std::string& faceCascadePath,
		FrameCapture* captureDevice,
		bool optimalSubregionDetect,
		double roiScaleFactor,
		const std::string& name)
		:Recognizer(name, captureDevice), 
		 roiScaleFactor(roiScaleFactor), 
		 optimalSubregionDetect(optimalSubregionDetect)
	{
		faceClassifier.load(faceCascadePath);
	}

	FaceRecognizer::~FaceRecognizer(void)
	{
	}

	void FaceRecognizer::processFrame()
	{
		detectFaces();
	}

	
	void FaceRecognizer::detectFaces()
	{
		if(faceClassifier.empty()) return;

		cv::Mat frame = getMostRecentFrame();

		if(frame.empty()) return;
		
		//getAreas is threadsafe
		RectVector lastRects = getAreas();

		//
		try{

			if(!(optimalSubregionDetect && lastRects.size() > 0))
			{
				RectVector tempRects;

				runDetect(frame, tempRects);

				setAreas(tempRects);
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
	void FaceRecognizer::detectFacesInROI(RectVector &lastRects, cv::Mat &frame)
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
			int adjustFactor = std::max((static_cast<int>(i->width * roiScaleFactor) - i->width) / 2, 10);

			//set the region of interest to the previous frame
			cv::Mat roiFrame = frame(*i);

			//make the frame bigger, but do not exceed boundaries!
			roiFrame.adjustROI(adjustFactor*1.15, adjustFactor*1.15, adjustFactor, adjustFactor);

			//detect on smaller frame
			runDetect(roiFrame, tempRects);

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
			runDetect(frame, newRects);
		
		//update the area 'result' variable
		setAreas(newRects);
	}

	//inline function to centralize the parameters!
	inline void FaceRecognizer::runDetect(cv::Mat &frame, RectVector &results)
	{
		faceClassifier.detectMultiScale(frame, /*the frame, input frame for detection*/
			results, /*the the result vector, the output*/
			1.15,  /*the the scale factor, default 1.1*/
			2,     /*the minNeighbors, default: 3 */
			cv::CascadeClassifier::DO_CANNY_PRUNING, /*flags*/
			cv::Size(40, 40) /*min rect check size, the minimum!*/
			);
	}
}