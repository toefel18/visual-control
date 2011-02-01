#ifndef COGNITION_TRAINERIMAGE_H
#define COGNITION_TRAINERIMAGE_H

#include<cv.h>
#include<string>

namespace cognition
{

	/*!
	 * \brief Class used to prepare and store trainig images to be used in
	 *		  recognizer classes. This class basically resizes the image and
	 *		  transforms it´s color to grayscale. Input is assumed to be in
	 *		  BGR format. (captured from cv::VideoCapture). 
	 *
	 * \author Christophe Hesters 31-1-2011
	 */
	class TrainerImage
	{
	public:
		/*!
		 * \brief Constructs this class to resize(either with or without keeping the 
		 *		  aspect ratio), grayscale and optionally store images.
		 *
		 * \param goalSize			the size to which images are to be scaled
		 * \param keepAspectRatio	true if the image should be proportionally scaled
		 * \param directoryPath		the directory where to store the images (default = current working dir.)
		 */
		TrainerImage(cv::Size goalSize = cv::Size(140,200), bool keepAspectRatio = true, const std::string& directoryPath = "")
			:directoryPath(directoryPath), goalSize(goalSize), keepAspectRatio(keepAspectRatio)
		{
			//if the user gave a directory which does not end with a slash, add one
			if( this->directoryPath.size() > 0 && (*(--this->directoryPath.end()) != '/' && *(--this->directoryPath.end()) == '\\') )
				this->directoryPath.append("/");
		}
	
		/*!
		 * \brief processes the image according to the configuration of this object
		 *		  and returns the processed image.
		 *
		 * \param image		the image to processed
		 * \return the processed matrix
		 */
		cv::Mat processImage(cv::Mat& image);

		/*!
		 * \brief processes the image according to the configuration of this object
		 *		  and stores the processed image in the directoryPath, with the given
		 *		  name. 
		 *
		 * \param image		the image to be processed
		 * \param name		the name to save this image under
		 * \return true if succesfully saved, false otherwise
		 */
		bool processAndSaveImage(cv::Mat& image, const std::string& name);

	protected:
	
		std::string directoryPath;
		cv::Size goalSize;
		bool keepAspectRatio;
	};

}

#endif //COGNITION_TRAINERIMAGE_H