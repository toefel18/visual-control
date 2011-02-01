#include "eigenfacerecognizer.h"

#include <highgui.h>
#include <cvaux.h>

#include <limits>

namespace cognition
{

	EigenfaceRecognizer::EigenfaceRecognizer(void)
		:images(0), 
		numTrainedImages(0), 
		numEigenvalues(0),
		eigenVectors(0),
		averageImage(0),
		eigenvalues(0),
		projectedTrainImage(0)
	{
	}


	EigenfaceRecognizer::~EigenfaceRecognizer(void)
	{
	}


	bool EigenfaceRecognizer::addTrainingImage(const std::string &filename, const std::string &name)
	{
		trainingImages[filename] = name;
		return true;
	} 

	void EigenfaceRecognizer::freeMemory(int fromIndex)
	{
		for(; fromIndex >= 0; fromIndex--)
			cvFree(&images[fromIndex]);

		numTrainedImages = 0;

		cvFree(images);
	}

	bool EigenfaceRecognizer::train()
	{
		isTrained = false;

		if(!loadTrainingImages())
			return false;

		//do principal component analysis
		doPCA();

		// project the training images onto the PCA subspace
		projectedTrainImage = cvCreateMat( numTrainedImages, numEigenvalues, CV_32FC1 );
	
		int offset = projectedTrainImage->step / sizeof(float);
	
		for(int i = 0; i < numTrainedImages; i++)
		{
			//int offset = i * nEigens;
			cvEigenDecomposite(
				images[i],
				numEigenvalues,
				eigenVectors,
				0, 0,
				averageImage,
				//projectedTrainFaceMat->data.fl + i*nEigens);
				projectedTrainImage->data.fl + i*offset);
		}

		isTrained = true;

		return isTrained;
	}


	bool EigenfaceRecognizer::loadTrainingImages()
	{
		if(trainingImages.size() < 2)
			return false;

		if(images && numTrainedImages > 0)
			freeMemory(numTrainedImages-1);//cvFree(images); //deletes images and sets images to 0

		//remove all the index to filename, name mappings
		recognitionDescriptor.clear();

		//number of images we are going to use for training
		numTrainedImages = trainingImages.size();

		//allocate space for an IplImage array
		images = (IplImage **) cvAlloc(sizeof(IplImage *) * numTrainedImages);

		int currentImage = 0;

		for(StringMap::iterator current = trainingImages.begin(); current != trainingImages.end(); ++current)
		{
			images[currentImage] = cvLoadImage(current->first.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
		
			if(images[currentImage] == NULL) break;
		
			//add checks for size!

			//add filename and name as the descriptors images[current], used after recognition
			recognitionDescriptor.push_back(*current);

			currentImage++;
		}

		//not all images loaded
		if(currentImage != numTrainedImages)
		{
			freeMemory(currentImage);
		
			return false;
		}

		return true;
	}

	void EigenfaceRecognizer::doPCA()
	{
		// set the number of eigenvalues to use
		numEigenvalues = numTrainedImages-1;

		//the number of face images
		CvSize faceImgSize;

		// allocate the eigenvector images
		faceImgSize.width  = images[0]->width;
		faceImgSize.height = images[0]->height;

	
		eigenVectors = (IplImage**)cvAlloc(sizeof(IplImage*) * numEigenvalues);
	
		for(int i = 0; i < numEigenvalues; i++)
			eigenVectors[i] = cvCreateImage(faceImgSize, IPL_DEPTH_32F, 1);

		// allocate the eigenvalue array
		eigenvalues = cvCreateMat( 1, numEigenvalues, CV_32FC1 );

		// allocate the averaged image
		averageImage = cvCreateImage(faceImgSize, IPL_DEPTH_32F, 1);

		// set the PCA termination criterion
		CvTermCriteria calcLimit = cvTermCriteria( CV_TERMCRIT_ITER, numEigenvalues, 1);

		// compute average image, eigenvalues, and eigenvectors
		// the new C++ api has a cv::PCA object for this, upgrade this in the future!
		cvCalcEigenObjects(
			numTrainedImages,
			(void*)images,
			(void*)eigenVectors,
			CV_EIGOBJ_NO_CALLBACK,
			0,
			0,
			&calcLimit,
			averageImage,
			eigenvalues->data.fl);

		cvNormalize(eigenvalues, eigenvalues, 1, 0, CV_L1, 0);
	}

	std::string EigenfaceRecognizer::recognize(cv::Mat &face)
	{
		//int i, nTestFaces  = 0;         // the number of test images
		//CvMat * trainPersonNumMat = 0;  // the person numbers during training
		//float * projectedTestFace = 0;

		IplImage faceToRecognize = (IplImage) face;
		//IplImage *faceToRecognize = cvLoadImage("s1/1.pgm", CV_LOAD_IMAGE_GRAYSCALE);

		float *projectedTestImage = (float *)cvAlloc(sizeof(float) * numEigenvalues);
	
		//project the test image onto the PCA subspace
		cvEigenDecomposite(
			&faceToRecognize,
			numEigenvalues,
			eigenVectors,
			0, 0,
			averageImage,
			projectedTestImage);

		int nearest = findNearestNeighbor(projectedTestImage);

		//take threshold into account, mark as not recognized if not found!

		return recognitionDescriptor[nearest].second;
	}

	int EigenfaceRecognizer::findNearestNeighbor(float *projectedTestImage)
	{
		//set the least distance to the maximum double value
		double leastDistanceSquare = std::numeric_limits<double>::max();

		//the index of the nearest element
		int nearestImage = 0;

		//calculate distance between each training image and the projected test face
		for(int current = 0; current < numTrainedImages; current++)
		{
			double distanceSquare = 0;

			for(int eigen = 0; eigen < numEigenvalues; eigen++)
			{
				//calculate the distance in for the current eigen
				float distance = projectedTestImage[eigen] - 
					projectedTrainImage->data.fl[current * numEigenvalues + eigen];

				//square the distance and add it to the total value
				distanceSquare += distance * distance;
			}

			if(distanceSquare < leastDistanceSquare)
			{
				leastDistanceSquare = distanceSquare;
				nearestImage = current;
			}
		}

		return nearestImage;
	}
}