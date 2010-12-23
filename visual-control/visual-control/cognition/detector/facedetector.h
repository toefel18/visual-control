#ifndef COGNITION_FACEDETECTOR_H
#define COGNITION_FACEDETECTOR_H
#include "../detector.h"

#include <boost/thread/mutex.hpp>

namespace cognition
{
	/*!
	 * \brief Basic detector that detects faces inside a frame
	 * 
	 * \author Christophe Hesters
	 */
	class FaceDetector : public Detector
	{
	public:

		/*! \brief Constructs the Face Detector
		 * 
		 * \param faceCascadePath		the path to the haar cascade xml file, loads the classifier!
		 * \param captureDevice			if 0, nothing else it will register itself as a frame receiver
		 * \param optimalSubregionDetect if true, only scan area's where a face was detected 
		 * \param roiScaleFactor		how far beyond the former face rect should be scanned?
		 * \param name					an name for this object
		 * 
		 * @author Christophe hesters
		 */
		FaceDetector(const std::string& faceCascadePath, 
			FrameCapture* captureDevice = 0,
			bool optimalSubregionDetect = false,
			double roiScaleFactor = 1.16,
			const std::string& name = "face detector");
		virtual ~FaceDetector(void);

		/*!
		 * \brief Starts the object detection on the latest frame
		 */
		virtual void processFrame();

		/*!
		 * \brief Checks if the internal classifier is empty
		 * 
		 * \returns true if the internal faceclassifier is empty.
		 */
		bool empty() const { 
			return faceClassifier.empty(); 
		}

	protected:
		cv::CascadeClassifier faceClassifier;

		double roiScaleFactor;
		bool optimalSubregionDetect;
		
		/*!
		 * \brief Runs the face detection classifier
		 * 
		 * \param frame		the frame to detect faces in
		 * \param results	the vector to store the result rectangles in
		 */
		void runFaceDetect(const cv::Mat &frame, RectVector &results);

		/*!
		 * \brief Detects faces inside the frame. If there were previous results
		 *		  and optimalSubregionDetect is on, it will automatically call
		 *		  detectFacesInRoi for more optimal subregion detection.
		 * 
		 * \param frame		the frame to detect faces in
		 */
		void detectFaces(const cv::Mat &frame);

		/*!
		 * \brief Detects faces inside subregions of a frame using the previous
		 *		  results.
		 * 
		 * \param lastRects		the last detection results
		 * \param frame			the current frame
		 */
		void detectFacesInROI(RectVector &lastRects, const cv::Mat &frame);
		
		/*!
		 * \brief loads the internal classifier. Now protected because it is not thread-safe
		 *
		 * \param faceCascadePath	the path to the face cascade xml file
		 * \returns true on success, false otherwise
		 */
		bool loadClassifier(const std::string& faceCascadePath){
			return faceClassifier.load(faceCascadePath);
		}
	};
}

#endif //COGNITION_FACEDETECTOR_H