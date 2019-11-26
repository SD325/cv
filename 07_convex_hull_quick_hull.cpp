//
// Created by Spandan Das on 11/21/2019.
//
#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>
#include <vector>

#define N 800 // resolution of ppm file
#define num_pts 75 // number of points

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
static vector<Point> hull;

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

void drawLineNeg(int x1, int y1, int x2, int y2) {
    if (x1 < x2) {
        swap(x1, x2);
        swap(y1, y2);
    }
    int dx = x2 - x1;
    int dy = y2 - y1;
    bool greater = false;
    if (abs(dy) > abs(dx)) {
        greater = true;
        int t1 = x1 - (y2 - y1);
        int t2 = y1 - (x2 - x1);
        x2 = t1;
        y2 = t2;
    }
    dx = x2 - x1;
    dy = y2 - y1;
    double m = abs(dy / (double) dx);
    int j = y1;
    double eps = m - 1.0;
    for (int i = x1 - 1; i > x2; i--) {
        if (greater) {
            drawpt(x1 - (j - y1), y1 - (i - x1), 0, 0, 0);
        }
        else drawpt(i, j, 0, 0, 0);
        if (eps >= 0) {
            j++;
            eps -= 1.0;
        }
        eps += m;
    }

}

void drawLinePos(int x1, int y1, int x2, int y2) {
    if (x1 > x2) {
        swap(x1, x2);
        swap(y1, y2);
    }
    int dx = x2 - x1;
    int dy = y2 - y1;
    double m = (double) dy / (double) dx;
    bool switched = dy > dx;
    if (switched) {
        swap(x1, y1);
        swap(x2, y2);
        m = 1. / m;
    }
    int j = y1;
    double eps = m - 1.0;
    for (int i = x1; i < x2; i++) {
        if (switched)
            drawpt(j, i, 0, 0, 0);
        else drawpt(i, j, 0, 0, 0);
        if (eps >= 0) {
            j++;
            eps -= 1.0;
        }
        eps += m;
    }

}

void drawLine(int x1, int y1, int x2, int y2) {
    double slope = (double) (y2 - y1) / (double) (x2 - x1);
    if (slope > 0) {
        drawLinePos(x1, y1, x2, y2);
        //cout << "positive" << endl;
    }
    else {
        drawLineNeg(x1, y1, x2, y2);
        //cout << "negative" << endl;
    }
}

int findSide(Point p1, Point p2, Point p) {
    double val = (p.y - p1.y) * (p2.x - p1.x) - (p2.y - p1.y) * (p.x - p1.x);

    if (val > 0.0)
        return 1;
    if (val < 0.0)
        return -1;
    return 0;
}

double lineDist(Point p1, Point p2, Point p) {
    return abs ((p.y - p1.y) * (p2.x - p1.x) - (p2.y - p1.y) * (p.x - p1.x));
}

void qh_helper(Point p1, Point p2, int side) {
    int ind = -1;
    double max_dist = 0;

    for (int i=0; i < num_pts; i++) {
        double temp = lineDist(p1, p2, pts[i]);
        if (findSide(p1, p2, pts[i]) == side && temp > max_dist) {
            ind = i;
            max_dist = temp;
        }
    }

    if (ind == -1){
        hull.push_back(p1);
        hull.push_back(p2);
        drawLine(p1.x, p1.y, p2.x, p2.y);
        return;
    }

    qh_helper(pts[ind], p1, -findSide(pts[ind], p1, p2));
    qh_helper(pts[ind], p2, -findSide(pts[ind], p2, p1));
}

void quickHull() {
    int min_x = 0;
    int max_x = 0;
    for (int i=1; i < num_pts; i++) {
        if (pts[i].x < pts[min_x].x)
            min_x = i;
        if (pts[i].x > pts[max_x].x)
            max_x = i;
    }

    qh_helper(pts[min_x], pts[max_x], 1);
    qh_helper(pts[min_x], pts[max_x], -1);
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
    Point this_pt;
    for (auto &pt : pts) {
        this_pt = Point(random(), random());
        int roundedX = (int) (N * this_pt.x);
        int roundedY = (int) (N * this_pt.y);
        pt = Point(roundedX, roundedY);
        drawpt(roundedX, roundedY, 0, 0, 0);
    }

    quickHull();

    // WRITE TO PPM
    ofstream image("07_quick_hull.ppm");
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