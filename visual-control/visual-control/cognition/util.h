#ifndef COGNITION_UTIL_H
#define COGNITION_UTIL_H

//#include <cmath>
#include <algorithm>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <cv.h>
#include "framereceiver.h"

#include <boost/enable_shared_from_this.hpp>
#include "detector.h"
#include "controller.h"

namespace cognition
{

	//// Controller forwarder for types that do not work or are
	//// not able to work with shared_ptrs
	//// forwards stateChanges from detector to T
	////  
	//// @note unused, since the framework does not use shared pointers for now
	//// @author Christophe Hesters
	//template<typename T>
	//class ControllerForwarder : public Controller, public boost::enable_shared_from_this<ControllerForwarder<T> >
	//{
	//public:
	//	typedef boost::shared_ptr< cognition::Detector > DetectorPtr;

	//	
	//	ControllerForwarder(T *destination,
	//		DetectorPtr recognizer, 
	//		bool autoRegister = false)
	//		:destination(destination)//, recognizer(recognizer)
	//	{
	//		if(autoRegister && recognizer != 0)
	//			recognizer->addController(shared_from_this());
	//	}

	//	~ControllerForwarder(){
	//		//do not unregister here, it will call  the destructor again!
	//	}

	//	//signal that the destination does not want to receive events anymore
	//	void unregister(){
	//		if(DetectorPtr r = recognizer.lock())
	//			r->removeController(shared_from_this());
	//	}

	//	void stateChanged(Detector *recognizer){
	//		destination->stateChanged(recognizer);
	//	}

	//private:
	//	T *destination;
	//	boost::weak_ptr<Detector> recognizer;
	//};

	//// Frame forwarder for types that do not work with shared_ptrs
	////  This class should be stored inside a shared ptr!
	////  forwards frames from existingCaptureDevice to T
	//// @note Unused since the framework does not use shared_ptr anymore
	//template<typename T>
	//class FrameForwarder : public FrameReceiver, public boost::enable_shared_from_this<FrameForwarder<T> >
	//{
	//public:
	//	typedef boost::shared_ptr<FrameCapture> FrameCapturePtr;

	//	FrameForwarder(T *destination,
	//		FrameCapturePtr existingCaptureDevice,
	//		bool autoRegister = false)
	//		:destination(destination), captureDevice(existingCaptureDevice)
	//	{
	//		if(autoRegister)
	//			if(FrameCapturePtr capDevice = captureDevice.lock())
	//				capDevice->addFrameReceiver(shared_from_this());
	//	}

	//	~FrameForwarder()
	//	{
	//		//do not unregister here, it will call the destructor again
	//		//if(FrameCapturePtr capDevice = captureDevice.lock())
	//		//	capDevice->removeFrameReceiver(shared_ptr<FrameReceiver>(this));
	//	}

	//	//remove from the capture devices
	//	void unregister(){
	//		if(FrameCapturePtr capDevice = captureDevice.lock())
	//			capDevice->removeFrameReceiver(shared_from_this());
	//	}

	//	virtual void receiveFrame(const cv::Mat &frame){
	//		destination->receiveFrame(frame);
	//	}

	//private:
	//	T *destination;
	//	boost::weak_ptr<FrameCapture> captureDevice;
	//};

	//struct Point {
	//	float x, y;
	//	Point(float x = 0, float y = 0)
	//		:x(x), y(y){}
	//};

	//class Rect {
	//public:

	//	Rect(float x1 = 0, float y1 = 0, float x2 = 0, float y2 = 0)
	//		:p1(x1, y1), p2(x2, y2)
	//	{
	//		configurePointOrientation();
	//	}

	//	Rect(Point p1, Point p2)
	//		:p1(p1), p2(p2)
	//	{
	//		configurePointOrientation();
	//	}


	//	Point getPoint1(){
	//		return p1;
	//	}
	//	
	//	Point getPoint2(){
	//		return p2;
	//	}

	//	float getWidth(){
	//		return std::fabs(p2.x - p1.x);
	//	}

	//	float getHeight(){
	//		return std::fabs(p2.y - p1.y);
	//	}

	//	//implement contains and intersect for points and other rects!

	//protected:

	//	//makes p1 the top-left and and p2 bottom-right point
	//	void configurePointOrientation()
	//	{
	//		if(p1.x > p2.x)
	//			std::swap(p1.x, p2.x);
	//		if(p2.y > p1.y)
	//			std::swap(p1.y, p2.y);
	//	}
	//	struct Point p1, p2;
	//};

}

#endif //COGNITION_UTIL_H