//
// Created by Spandan Das on 11/21/2019.
//
#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>
#include <chrono>
#include <vector>

//static int M;
//static int N;

using namespace std;


class Point {
public:
    double x;
    double y;

    Point(double x_, double y_) {
        x = x_;
        y = y_;
    }

    Point() {
        Point(0, 0);
    }
    bool operator <(const Point & pt) const {
        return x < pt.x;
    }

};

typedef struct Color {
    int r;
    int g;
    int b;

    Color() {
        r = 0;
        g = 0;
        b = 0;
    }

    Color(int r, int g, int b) {
        this->r = r;
        this->g = g;
        this->b = b;
    }
} col;

vector<double> convolution(vector<double> kernel, int kRows, int kCols, double factor, vector<double> in, int M, int N) {
    // find center position of kernel (half of kernel size)
    int kCenterX = kCols / 2;
    int kCenterY = kRows / 2;
    vector<double> out(M*N);
    for(int i=0; i < M; ++i)              // rows
    {
        for(int j=0; j < N; ++j)          // columns
        {
            int index_1 = i * M + j;
            for(int m=0; m < kRows; ++m)     // kernel rows
            {
                int mm = kRows - 1 - m;      // row index of flipped kernel

                for(int n=0; n < kCols; ++n) // kernel columns
                {
                    int nn = kCols - 1 - n;  // column index of flipped kernel

                    // index of input signal, used for checking boundary
                    int ii = i + (kCenterY - mm);
                    int jj = j + (kCenterX - nn);

                    // ignore input samples which are out of bound
                    if (ii >= 0 && ii < M && jj >= 0 && jj < N ) {
                        int index_2 = ii * M + jj;
                        int index_3 = mm * kRows + nn;
                        out[index_1] += in[index_2] * kernel[index_3];
                    }
                }
            }
        }
    }
    for (double & i : out) i *= factor;
    return out;
}

int main() {
    srand(time(nullptr));

    int M;
    int N;
    vector<double> a;
    string filename = "Valve_gaussian.ppm";
    ifstream file(filename.c_str());
    int max_val;
//    if (file.good())
//    {
//        for (int j = 0; j < 5; j++) {
//            string sLine;
//            getline(file, sLine);
//            cout << sLine << endl;
//        }
//    }
    string image_type, file_source;
    file >> image_type >> file_source >> M >> N;
    file >> max_val;
    double inp1 = 0.0;
    double inp2 = 0.0;
    double inp3 = 0.0;
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            file >> inp1 >> inp2 >> inp3;
            a.push_back((inp1 + inp2 + inp3)/3.0);
        }
    }
    vector<double> kernel1 = {2,  4,  5,  4,  2,
                              4,  9, 12,  9,  4,
                              5, 12, 15, 12,  5,
                              4,  9, 12,  9,  4,
                              2,  4,  5,  4,  2};
    double factor1 = 1.0/159.0;
    a = convolution(kernel1, 5, 5, factor1, a, M, N);


    file.close();
    // white background
//    for (auto &i : ppm) {
//        for (auto &j : i) {
//            j.r = 1;
//            j.g = 1;
//            j.b = 1;
//        }
//    }
//    Point this_pt;
//    for (auto &pt : pts) {
//        this_pt = Point(random(), random());
//        int roundedX = (int) (N * this_pt.x);
//        int roundedY = (int) (N * this_pt.y);
//        pt = Point(roundedX, roundedY);
//        drawpt(roundedX, roundedY, 0, 0, 0);
//    }
//
//        // WRITE TO PPM
//    ofstream image("07_quick_hull.ppm");
//    image << "P3 " << N << " " << N << " 1" << endl;
//
////    for (auto &i : ppm) {
////        for (auto &j : i) {
////            image << j.r << " " << j.g << " " << j.b << " ";
////        }
////        image << endl;
////    }
////    image.close();
    return 0;
}