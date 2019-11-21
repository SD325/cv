//
// Created by Spandan Das on 11/21/2019.
//
#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <vector>

#define N 800 // resolution of ppm file
#define num_pts 1000 // number of points

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
static Point pts[num_pts];
static col ppm[N][N];

double random() {
    return (double) rand() / RAND_MAX;
}

double dist(Point p1, Point p2) {
    return sqrt((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y));
}

void drawpt(int x, int y, int r, int g, int b) {
    if (x < 0 || x >= N || y < 0 || y >= N) return;
    ppm[x][y].r = r;
    ppm[x][y].g = g;
    ppm[x][y].b = b;
}

vector<int> brute_force(vector<int> curr, int len) {
    double min_dist = LONG_MAX;
    double this_dist = 0;
    vector<int> minInd(2);
    for (int i = 0; i < len; i++) {
        for (int j = i+1; j < len; j++) {
            this_dist = dist(pts[curr.at(i)], pts[curr.at(j)]);
            if (this_dist < min_dist) {
                minInd.at(0) = curr.at(i);
                minInd.at(1) = curr.at(j);
                min_dist = this_dist;
            }
        }
    }
    return minInd;
}


vector<int> merge_helper(vector<int> curr, int len) {
    if (len <= 3) {
        return brute_force(curr, len);
    }
    else {
        int mid = len/2;
        vector<int> left;
        vector<int> right;
        for (int i = 0; i < mid; i++) left.push_back(curr.at(i));
        for (int i = mid; i < len; i++) right.push_back(curr.at(i));
        vector<int> left_min = merge_helper(left, left.size());
        vector<int> right_min = merge_helper(right, right.size());
        double left_dist = dist(pts[left_min.at(0)], pts[left_min.at(1)]);
        double right_dist = dist(pts[right_min.at(0)], pts[right_min.at(1)]);
        double min_dist;
        vector<int> min_ind;
        if (left_dist < right_dist) {
            min_dist = left_dist;
            min_ind.push_back(left_min.at(0));
            min_ind.push_back(left_min.at(1));
        }
        else {
            min_dist = right_dist;
            min_ind.push_back(right_min.at(0));
            min_ind.push_back(right_min.at(1));
        }

        double mid_x = (pts[curr.at(mid)].x + pts[curr.at(mid+1)].x) / 2.0;
        //double mid_x = pts[curr.at(mid)].x;
        vector<int> inside_strip;
        for (int i = 0; i < len; i++) {
            if (abs(pts[curr.at(i)].x - mid_x) < min_dist) {
                inside_strip.push_back(curr.at(i));
            }
        }

        double this_dist = 0;
        for (int i = 0; i < (int) inside_strip.size(); i++) {
            for (int j = i+1; j < (int) inside_strip.size(); j++) {
                this_dist = dist(pts[inside_strip.at(i)], pts[inside_strip.at(j)]);
                if (this_dist < min_dist) {
                    min_dist = this_dist;
                    min_ind.at(0) = inside_strip.at(i);
                    min_ind.at(1) = inside_strip.at(j);
                }
            }
        }
        return min_ind;
    }
}


vector<int> merge_find() {
    sort(pts, pts+num_pts);
    vector<int> temp;
    for (int i = 0; i < num_pts; i++) temp.push_back(i);
    return merge_helper(temp, num_pts);
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