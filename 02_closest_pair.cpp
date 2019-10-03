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
        for (int j = l; j < i; j++) {
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
    vector<int> left_pts = merge_helper(l, middle, middle-l+1);
    vector<int> right_pts = merge_helper(middle+1, r, middle+1-r+1);
    double left_dist = dist(pts[left_pts.at(0)], pts[left_pts.at(1)]);
    double right_dist = dist(pts[right_pts.at(0)], pts[right_pts.at(1)]);

    double min_dist = min(left_dist, right_dist);
    bool min_is_left = (min_dist == left_dist);

    Point inside_mid[size_];
    vector<int> inside_ind;
    Point midpoint = pts[middle];
    int index = 0;
    for (int i = l; i <= r; i++) {
        if (abs(pts[i].x - midpoint.x) < min_dist) {
            inside_mid[index++] = pts[i];
            inside_ind.push_back(i);
        }
    }
    vector<int> inside_fake = brute_force(0, index-1, inside_mid);
    vector<int> inside {inside_ind.at(inside_fake.at(0)), inside_ind.at(inside_fake.at(1))};
    double dist_inside = dist(pts[inside.at(0)], pts[inside.at(1)]);
    // return smallest distance
    if (dist_inside < min_dist) return inside;
    else if (min_is_left) return left_pts;
    else return right_pts;
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