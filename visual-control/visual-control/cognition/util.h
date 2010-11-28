#ifndef COGNITION_UTIL_H
#define COGNITION_UTIL_H

#include <cmath>
#include <algorithm>

namespace cognition
{
	struct Point {
		float x, y;
		Point(float x = 0, float y = 0)
			:x(x), y(y){}
	};

	class Rect {
	public:

		Rect(float x1 = 0, float y1 = 0, float x2 = 0, float y2 = 0)
			:p1(x1, y1), p2(x2, y2)
		{
			configurePointOrientation();
		}

		Rect(Point p1, Point p2)
			:p1(p1), p2(p2)
		{
			configurePointOrientation();
		}


		Point getPoint1(){
			return p1;
		}
		
		Point getPoint2(){
			return p2;
		}

		float getWidth(){
			return std::fabs(p2.x - p1.x);
		}

		float getHeight(){
			return std::fabs(p2.y - p1.y);
		}

	protected:

		//makes p1 the top-left and and p2 bottom-right point
		void configurePointOrientation()
		{
			if(p1.x > p2.x)
				std::swap(p1.x, p2.x);
			if(p2.y > p1.y)
				std::swap(p1.y, p2.y);
		}
		struct Point p1, p2;
	};

}

#endif //COGNITION_UTIL_H