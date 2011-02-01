#include "trainerimage.h"

#include<highgui.h>
#include<cmath>

namespace cognition
{
	cv::Mat TrainerImage::processImage(cv::Mat& image)
	{
		cv::Mat resized;
		cv::Size originalSize = image.size();

		if(keepAspectRatio)
		{
			float ratio = static_cast<float>(goalSize.height) / originalSize.height;
			cv::Size newSize((int) (originalSize.width * ratio), (int) (originalSize.height * ratio));
	
			//fix possible rounding error by float
			if(newSize.height != goalSize.height) newSize.height = goalSize.height;

			cv::resize(image, resized, newSize);

			if(resized.size().width != goalSize.width)
			{
				if(keepAspectRatio)
				{
					int delta = goalSize.width - resized.size().width;
		
					if(delta < 0)
					{
						cv::Rect clipRect(std::abs(delta) / 2, 0, goalSize.width, resized.size().height);
						resized = resized(clipRect);
					}
					else if (delta > 0)
					{
						//width needs to be widened, create bigger mat, get region of 
						//interest at the center that matches the size of the resized   
						//image, and copy the resized image into that ROI

						cv::Mat widened(goalSize, resized.type());
						cv::Rect widenRect(delta / 2, 0, goalSize.width, goalSize.height);
						cv::Mat widenedCenter = widened(widenRect);
						resized.copyTo(widenedCenter);
						resized = widened; //we return resized, so set widened to resized
					}
				}
			}
		}
		else 
			cv::resize(image, resized, goalSize);
	
		cv::Mat grayFrame;
		cv::cvtColor(resized, grayFrame, CV_BGR2GRAY);

		return grayFrame;
	}

	bool TrainerImage::processAndSaveImage(cv::Mat& image, const std::string& name)
	{
		cv::Mat processed = processImage(image);

		return cv::imwrite(directoryPath + name, processed);
	}
}
