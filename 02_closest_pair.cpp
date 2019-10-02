//
// Created by Spandan Das on 9/9/2019.
//
#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>
#include <algorithm>

#define N 200 // resolution of ppm file
#define size 40 // number of points

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

static col ppm[N][N];

double random() {
    return (double) rand() / RAND_MAX;
}

double dist(double x1, double y1, double x2, double y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

void drawpt(int x, int y, int r, int g, int b) {
    if (x < 0 || x >= N || y < 0 || y >= N) return;
    ppm[x][y].r = r;
    ppm[x][y].g = g;
    ppm[x][y].b = b;
}

void brute_force(Point pts[size]) {
    double min_dist = LONG_MAX;
    int minInd[2];
    double old;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < i; j++) {
            old = min_dist;
            min_dist = min(min_dist, dist(pts[i].x, pts[i].y, pts[j].x, pts[j].y));
            if (min_dist < old) {
                minInd[0] = i;
                minInd[1] = j;
            }
        }
    }
    int rounded_x1 = (int) (N * pts[minInd[0]].x);
    int rounded_y1 = (int) (N * pts[minInd[0]].y);
    drawpt(rounded_x1, rounded_y1, 1, 0, 0);
    int rounded_x2 = (int) (N * pts[minInd[1]].x);
    int rounded_y2 = (int) (N * pts[minInd[1]].y);
    drawpt(rounded_x2, rounded_y2, 1, 0, 0);
}


int* merge_helper(Point sx[], Point sy[], int size_) {
    // return indices of the original array which contain two closest points
}


void merge_find(Point pts[size]) {
    Point sortedX[size]; // points sorted by x values
    Point sortedY[size]; // points sorted by y values
    for (int i = 0; i < size; i++) sortedX[i] = sortedY[i] = pts[i];
    sort(sortedX, sortedX+size);
    sort(sortedY, sortedY+size);

}


int main() {
    srand(time(nullptr));
    // coordinates of vertices are doubles
    Point pts[size];

    // white background
    for (auto &i : ppm) {
        for (auto &j : i) {
            j.r = 1;
            j.g = 1;
            j.b = 1;
        }
    }
    for (auto &pt : pts) {
        pt = Point(random(), random());
        int roundedX = (int) (N * pt.x);
        int roundedY = (int) (N * pt.y);
        drawpt(roundedX, roundedY, 0, 0, 0);
    }

    // calculate minimum distance
    //brute_force(pts);
    merge_find(pts);


    // WRITE TO PPM
    ofstream image("02_closest_pair.ppm");
    image << "P3 " << N << " " << N << " 1" << endl;

    for (auto &i : ppm) {
        for (auto &j : i) {
            image << j.r << " " << j.g << " " << j.b << " ";
        }
        image << endl;
    }
    image.close();
    return 0;
}