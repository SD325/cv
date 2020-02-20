#include "iostream"
#include "opencv2/opencv.hpp"
using namespace std;
using namespace cv;

int main() {
    Mat img=imread("circle_tester.png");

    if (img.empty()) {
        cout << "Error" << endl;
        return -1;
    }
    imshow("circle_tester.png", img);
    waitKey(2000);
    return 0;
}