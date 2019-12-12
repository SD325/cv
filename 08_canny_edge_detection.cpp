//
// Created by Spandan Das on 11/21/2019.
//
#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>
#include <chrono>
#include <vector>

#define threshold 70
#define filename "coinsEasy.ppm"

using namespace std;


vector<double> convolution(vector<double> kernel, int kRows, int kCols, double factor, vector<double> in, int M, int N) {
    // find center position of kernel (half of kernel size)
    int kCenterX = kCols / 2;
    int kCenterY = kRows / 2;
    vector<double> out(N*M);
    for(int i=0; i < N; ++i)              // rows
    {
        for(int j=0; j < M; ++j)          // columns
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
                    if (ii >= 0 && ii < N && jj >= 0 && jj < M) {
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
    ifstream file(filename, ios::in | ios::binary);
    int max_val;
    string image_type;
    file >> image_type >> M >> N;
    file >> max_val;
    vector<double> a;
    if (image_type == "P3") {
        double inp1;
        double inp2;
        double inp3;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                file >> inp1 >> inp2 >> inp3;
                a.push_back((inp1 + inp2 + inp3) / 3.0);
            }
        }
    }
    else {
        file.get();
        unsigned char buffer[3];
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                file.read((char*) buffer , 3);
                a.push_back((buffer[0] + buffer[1] + buffer[2])/3.0);
            }
        }
    }

    vector<double> kernel1 = {2,  4,  5,  4,  2,
                   4,  9, 12,  9,  4,
                   5, 12, 15, 12,  5,
                   4,  9, 12,  9,  4,
                   2,  4,  5,  4,  2};
    double factor1 = 1.0/159.0;
    // cout << a.size() << endl;
    a = convolution(kernel1, 5, 5, factor1, a, M, N);
    // cout << a.size() << endl;

    vector<double> kernel2 = {-1, 0, 1,
                              -2, 0, 2,
                              -1, 0, 1};
    vector<double> g_x = convolution(kernel2, 3, 3, 1, a, M, N);

    vector<double> kernel3 = { 1, 2, 1,
                               0, 0, 0,
                               -1,-2,-1};
    vector<double> g_y = convolution(kernel3, 3, 3, 1, a, M, N);

    vector<double> finalized;
    for (int i = 0; i < (int) a.size(); i++) {
        finalized.push_back(sqrt((g_x[i]*g_x[i]) + (g_y[i]*g_y[i])));
    }
    for (double& i : finalized) {
        if (i < threshold) i = 0;
    }
    file.close();

        // WRITE TO PPM
    ofstream image("08_edge_detection.ppm");
    image << "P3 " << M << " " << N << " 255" << endl;

    int index;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            index = M*i+j;
            if (finalized[index]) {
                image << 255 << " " << 255 << " " << 255 << " ";
            }
            else {
                image << 0 << " " << 0 << " " << 0 << " ";
            }
//            int x = (int) a[index];
//            image << x << " " << x << " " << x << " ";
        }
        image << endl;
    }
    image.close();
    return 0;
}