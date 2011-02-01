#ifndef COGNITION_RECOGNIZER_H
#define COGNITION_RECOGNIZER_H

#include <string>

namespace cv {
	class Mat;
}

namespace cognition
{

	/*!
	 * \brief Base class for recognizers. Provides the interface  
	 *		  for different kinds of recognizers.
	 *
	 * \todo add method to add already loaded training images (cv::Mat)
	 * \author Christophe Hesters 29-1-2011
	 */
	class Recognizer
	{
	public:

		Recognizer()
			:isTrained(false){}
		
		/*!
		 * \brief Adds a training image path to the training set of known images
		 *		  after you have added 2 or more images, call train to learn and
		 *		  set yourself up for recognition. All images must be the same size!
		 * 
		 * \param filename	the path where to find the image (all of the same size!)
		 * \param name		the name you want to attach to the image
		 * \return bool		true if the path is added succesfully
		 */
		virtual bool addTrainingImage(const std::string &filename, const std::string &name) = 0;

		/*!
		 * \brief starts the learning process on all the known images that
		 *		  are added trough addTrainingImage. You can add more training images
		 *		  after training, but you have to call train again. While training
		 *		  you cannot use recognize()!
		 * 
		 * \return bool		true if trained and ready, false otherwise
		 */
		virtual bool train() = 0;

		/*!
		 * \brief does recognition on the face, and returns the most likely match. 
		 *		  This face must grayscale and be exactly the same size as the training
		 *		  images.
		 * 
		 * \param face		the matrix containing the face
		 * \return string	name of closest match in the set of training images
		 */
		virtual std::string recognize(cv::Mat &face) = 0;

		/*!
		 * \brief gets the number of training images available
		 * 
		 * \return size_t	the number of registered training images
		 */
		virtual std::size_t numTrainingImages() = 0;

		bool trained() { return isTrained; }

	protected:
		bool isTrained;
	};
}
#endif //COGNITION_RECOGNIZER_H