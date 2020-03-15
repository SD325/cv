#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
using namespace cv;
using namespace std;
int main(int argc, char** argv) {
    const char* filename = argc >=2 ? argv[1] : "C:\\Users\\Spandan Das\\CLionProjects\\cv\\coinsHarder_comp.png";
    // Loads an image
    Mat src = imread( samples::findFile( filename ), IMREAD_COLOR );
    // Check if image is loaded properly
    if(src.empty()) {
        printf(" Error opening image\n");
        printf(" Program Arguments: [image_name -- default %s] \n", filename);
        return EXIT_FAILURE;
    }
    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);
    medianBlur(gray, gray, 5);
    vector<Vec3f> circles;
    HoughCircles(gray, circles, HOUGH_GRADIENT, 1,
                 20,  // change this value to detect circles with different distances to each other
                 30, 40, 17, 55 // change the last two parameters
            // (min_radius & max_radius) to detect larger circles
    );



    for( size_t i = 0; i < circles.size(); i++ ) {
        Vec3i c = circles[i];
        int radius = c[2];
        Point center = Point(c[0], c[1]);
        // circle center
//        circle( src, center, 1, Scalar(0,100,100), 3, LINE_AA);
        // circle outline
        circle( src, center, radius, Scalar(0,0,255), 3, LINE_AA);

    }
    imwrite("coins_OpenCV_output.png", src);
    waitKey();
    return EXIT_SUCCESS;
}