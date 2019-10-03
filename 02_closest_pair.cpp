//
// Created by Spandan Das on 9/9/2019.
//
#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <vector>

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

vector<int> brute_force(int l, int r, Point p[]) {
    double min_dist = LONG_MAX;
    double old;
    vector<int> minInd(2);
    for (int i = l; i <= r; i++) {
        for (int j = i+1; j <=r; j++) {
            old = min_dist;
            min_dist = min(min_dist, dist(p[i], p[j]));
            if (min_dist < old) {
                minInd.at(0) = i;
                minInd.at(1) = j;
            }
        }
    }
    return minInd;
}


vector<int> merge_helper(int l, int r, int size_) {
    if (size_ <= 3) {
        return brute_force(l, r, pts);
    }
    int middle = size_/2;
    vector<int> left_pts = merge_helper(l, middle-1, middle-l);
    vector<int> right_pts = merge_helper(middle, r, r-middle+1);
    double left_dist = dist(pts[left_pts.at(0)], pts[left_pts.at(1)]);
    double right_dist = dist(pts[right_pts.at(0)], pts[right_pts.at(1)]);

    vector<int> min_ind;
    double min_dist;
    if (left_dist < right_dist) {
        min_dist = left_dist;
        min_ind.push_back(left_pts.at(0));
        min_ind.push_back(left_pts.at(1));
    }
    else {
        min_dist = right_dist;
        min_ind.push_back(right_pts.at(0));
        min_ind.push_back(right_pts.at(1));
    }
    vector<Point> inside;
    double mid_x = (pts[middle-1].x + pts[middle].x)/2.0;
    for (int i = l; i <= r; i++) {
        if (abs(pts[i].x - mid_x) < min_dist) {
            inside.push_back(pts[i]);
        }
    }

    double thisdist;
    for (int i = l; i <= r; i++) {
        for (int j = i+1; j <=r; j++) {
            thisdist = dist(inside.at(i), inside.at(j));
            if (thisdist < min_dist) {
                min_dist = thisdist;
                min_ind.at(0) = i;
                min_ind.at(1) = j;
            }
        }
    }
    return min_ind;
}


vector<int> merge_find() {
    sort(pts, pts+size);
    return merge_helper(0, size-1, size);
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

    vector<int> minInd = merge_find();
    drawpt((int) (N * pts[minInd.at(0)].x), (int) (N * pts[minInd.at(0)].y), 1, 0, 0);
    drawpt((int) (N * pts[minInd.at(1)].x), (int) (N * pts[minInd.at(1)].y), 1, 0, 0);

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