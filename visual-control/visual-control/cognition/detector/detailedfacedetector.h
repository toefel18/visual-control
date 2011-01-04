#ifndef COGNITION_DETAILEDFACEDETECTOR_H
#define COGNITION_DETAILEDFACEDETECTOR_H
#include "facedetector.h"

#include <map>
#include <boost/thread/mutex.hpp>

namespace cognition
{
	/*!
	 * \brief
	 * Detects faces and the corresponding eyes nose and mouth that are part of that face.
	 * 
	 * Always tries to detect a face first. On succesfull detection it will try
	 * to detect the eyes, nose and mouth too. These options can turned on/off
	 * 
	 * \author Christophe Hesters
	 */
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
			EYES_AND_NOSE = 7,
			MOUTH = 8,
			EYES_AND_MOUTH = 11,
			NOSE_AND_MOUTH = 12,
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

		/*! 
		 * \brief Initializes this object. If you wish to detect more features
		 *        than just the face, load the approrpriate classifiers trough
		 *		  loadCascade before starting detection! otherwise you can expect
		 *        a crash!
		 *
		 * \param detectWhat			needed to specify what to detect, use loadCascade to load required classifiers
		 * \param faceCascadePath		the path to the haar cascade xml file, loads the classifier!
		 * \param captureDevice			if 0, nothing else it will register itself as a frame receiver
		 * \param optimalSubregionDetect if true, only scan area's where a face was detected 
		 * \param roiScaleFactor		how far beyond the former face rect should be scanned?
		 * \param name					an name for this object
		 */
		DetailedFaceDetector(DetectParams detectWhat,
			const std::string& faceCascadePath,
			FrameCapture* captureDevice = 0,
			bool optimalSubregionDetect = false,
			double roiScaleFactor = 1.16,
			const std::string& name = "detailed face detector");
		
		virtual ~DetailedFaceDetector(void);

		/*!
		 * \brief Loads the specified internal classifier with the given cascade.
		 *		  this is required if these detection features are enabled. This
		 *		  has to be called before detection starts and it is NOT thread-safe!
		 * 
		 * \param which			the classifier to load possibilities are EYES, NOSE and MOUTH
		 * \param cascadePath	the path at which the xml haar cascade resides
		 * 
		 * \returns true on success, false on failure
		 */
		bool loadCascade(DetectParams which, const std::string& cascadePath);

		/*!
		 * \brief configures this object to detect the specified features
		 *		  make sure all the corresponding classifiers are loaded before enabling this
		 * 
		 * \param featuresToDetect configures what this object detects
		 */
		void setDetectParams(DetectParams featuresToDetect) {detectWhat = featuresToDetect;}

		/*!
		 * \brief fetches the current frame and detects the selected features.
		 *		  this method is called by the client or the threading system.
		 */
		virtual void processFrame();

		/*!
		 * \brief Returns most recent detection results (thread-safe)
		 * 
		 * \returns A structure which maps detected face rectangles to the other features of that face
		 * \see definition of DetailedFaces
		 */
		DetailedFaces getDetailedFaceInfo();

		/*!
		 * \brief Searches for the eyes in the FaceDetails structure and 
		 *		  returns the angle in degrees. (0 means the face is straight ahead)
		 *		  a negative value means a rotation left, positive means a rotation right
		 *
		 * \param faceDetails	The detailed features of a face
		 * \returns the angle that describes face rotation, negative is left, positive is right
		 */
		static float getFaceRotation(const FaceDetails& faceDetails);

	protected:
		//should only contain options from DetectParams
		DetectParams detectWhat;

		cv::CascadeClassifier eyeClassifier;
		cv::CascadeClassifier noseClassifier;
		cv::CascadeClassifier mouthClassifier;

		/*!
		 * \brief initiates detection of the selected features on all the detected faces
		 *
		 * \param frame	the current frame 
		 */
		void detectFeatures(const cv::Mat &frame);

		/*!
		 * \brief Tries to detect eyes at the most probable location inside the frame and
		 *		  stores the result in a FaceDetails structure
		 * 
		 * \param frame		the current complete frame(no subregion!)
		 * \param faceRect	the rectangle of the current face
		 * \param results	the structure to store the results in
		 */
		void detectEyes(const cv::Mat &frame, const cv::Rect& faceRect, FaceDetails &results);

		/*!
		 * \brief Tries to detect the nose at the most probable location inside the frame and
		 *		  stores the result in a FaceDetails structure
		 * 
		 * \param frame		the current complete frame(no subregion!)
		 * \param faceRect	the rectangle of the current face
		 * \param results	the structure to store the results in
		 */
		void detectNose(const cv::Mat &frame, const cv::Rect& faceRect, FaceDetails &results);

		/*!
		 * \brief Tries to detect the mouth at the most probable location inside the frame and
		 *		  stores the result in a FaceDetails structure
		 * 
		 * \param frame		the current complete frame(no subregion!)
		 * \param faceRect	the rectangle of the current face
		 * \param results	the structure to store the results in
		 */
		void detectMouth(const cv::Mat &frame, const cv::Rect& faceRect, FaceDetails &results);

		/*!
		 * \brief Updates the latest detection result (thread-safe)
		 * 
		 * \param facesDetails	updates the latest detection result with facesDetails
		 */
		void setDetailedFaceInfo(const DetailedFaces& facesDetails);

	private:
		//a lock for reading/writing detailedFacesInfo
		boost::mutex detailedFaceInfoLock;

		//the latest detectin results
		DetailedFaces detailedFacesInfo; 
	};
}

#endif //COGNITION_DETAILEDFACEDETECTOR_H