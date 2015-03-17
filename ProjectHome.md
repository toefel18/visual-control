By detecting objects in the input of a video capturing device and interpreting their meaning, we try to construct commands. The commands can be used to control anything you like.

To start off, we show head tracking trough face detection with a web cam. Next we will try to recognize familiar persons (using the eigenfaces technique) and fetch their relative information (as students, it will fetch their schedules etc.) Depending on time, we might also implement gesture detection so they can visually control the information they want to see. The system is supposed to be trained with the images of persons in order to recognize them, after that, it should be work automatically whenever a person appears.

This project uses C++ (and possibly some C++0x features) in combination with the following libraries:
  * Boost 1.45.0
  * OpenCV 2.1 (build with TBB support)
  * Qt 4.7.1

To get this project running, see the [Compiling](Compiling.md) wiki!

View the demonstration video [here](http://www.youtube.com/watch?v=yFSPnu6_TZY)