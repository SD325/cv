//
// Created by Spandan Das on 9/9/2019.
//
#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>
#include <algorithm>

#define N 200 // resolution of ppm file
#define size 4 // number of points

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

// coordinates of vertices are doubles
static Point pts[size];
static col ppm[N][N];

double random() {
    return (double) rand() / RAND_MAX;
}

double dist(Point p1, Point p2) {
    return sqrt((p1.x - p1.y)*(p1.x - p1.y) + (p2.x - p2.y)*(p2.x - p2.y));
}

void drawpt(int x, int y, int r, int g, int b) {
    if (x < 0 || x >= N || y < 0 || y >= N) return;
    ppm[x][y].r = r;
    ppm[x][y].g = g;
    ppm[x][y].b = b;
}

int* brute_force(Point p[], int size_, int minInd[2]) {
    double min_dist = LONG_MAX;
    double old;
    for (int i = 0; i < size_; i++) {
        for (int j = 0; j < i; j++) {
            old = min_dist;
            min_dist = min(min_dist, dist(p[i], p[j]));
            if (min_dist < old) {
                minInd[0] = i;
                minInd[1] = j;
            }
        }
    }
    return minInd;
}


int* merge_helper(Point p[], int size_) {
    if (size_ <= 3) {
        int minInd[2];
        return brute_force(p, size_, minInd);
    }
    int middle = size_/2;
    int* left_pts = merge_helper(p, middle);
    int* right_pts = merge_helper(p + middle+1, size_ - middle);
    double left_dist = dist(pts[left_pts[0]], pts[left_pts[1]]);
    // minimum of left and right distance
    double min_dist = min(left_dist, dist(pts[right_pts[0]], pts[right_pts[1]]));
    bool min_is_left = (min_dist == left_dist);

    Point inside_mid[size_];
    Point midpoint = p[middle];
    int index = 0;
    for (int i = 0; i < size_; i++) {
        if (abs(p[i].x - midpoint.x) < min_dist) inside_mid[index++] = p[i];
    }
    int* min_index_inside = brute_force(inside_mid, index);
    double dist_inside = dist(p[min_index_inside[0]], p[min_index_inside[1]]);
    // return smallest distance
    if (dist_inside < min_dist) return min_index_inside;
    else if (min_is_left) return left_pts;
    else return right_pts;
}


int* merge_find() {
//    Point sortedX[size]; // points sorted by x values
//    for (int i = 0; i < size; i++) sortedX[i] = pts[i];
    sort(pts, pts+size);
    return merge_helper(pts, size);
}


int main() {
    srand(time(nullptr));

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

    int* minInd = merge_find();
    drawpt((int) (N * pts[minInd[0]].x), (int) (N * pts[minInd[0]].y), 1, 0, 0);
    drawpt((int) (N * pts[minInd[1]].x), (int) (N * pts[minInd[1]].y), 1, 0, 0);

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