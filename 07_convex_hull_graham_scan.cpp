//
// Created by Spandan Das on 11/21/2019.
//
#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <vector>
#include <set>
#include <stack>

#define num_trials 10
#define N 400 // resolution of ppm file
#define num_pts 1000000 // number of points

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
Point p0;

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

Point second(stack<Point> &S) {
    Point p = S.top();
    S.pop();
    Point res = S.top();
    S.push(p);
    return res;
}

void swap(Point &p1, Point &p2) {
    Point temp = p1;
    p1 = p2;
    p2 = temp;
}

int orientation(Point p, Point q, Point r)
{
    double val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);

    if (val == 0.0) return 0; // collinear
    if (val > 0.0) return 1; // clockwise
    return 2; // counterclockwise
}

int compare(const void *vp1, const void *vp2) {
    auto *p1 = (Point *)vp1;
    auto *p2 = (Point *)vp2;

    int o = orientation(p0, *p1, *p2);
    if (o == 0) return (dist(p0, *p2) >= dist(p0, *p1))? -1 : 1;

    if (o == 2) return -1;
    return 1;
}
void graham_scan() {
    double ymin = pts[0].y;
    int min = 0;
    for (int i = 1; i < num_pts; i++) {
        double y = pts[i].y;
        if ((y < ymin) || (ymin == y && pts[i].x < pts[min].x)) {
            ymin = pts[i].y;
            min = i;
        }
    }

    swap(pts[0], pts[min]);

    p0 = pts[0];
    qsort(&pts[1], num_pts-1, sizeof(Point), compare);


    int m = 1; // size of modified array
    for (int i = 1; i < num_pts; i++) {
        while (i < num_pts-1 && orientation(p0, pts[i], pts[i+1]) == 0) {
            i++;
        }

        pts[m] = pts[i];
        m++;  // Update size of modified array
    }

    // convex hull not possible
    if (m < 3) return;

    stack<Point> S;
    S.push(pts[0]);
    S.push(pts[1]);
    S.push(pts[2]);

    for (int i = 3; i < m; i++) {
        while (orientation(second(S), S.top(), pts[i]) != 2){
            S.pop();
        }
        S.push(pts[i]);
    }

    Point first = S.top();
    bool fst = true;
    Point prev;
    while (!S.empty()) {
        Point curr = S.top();
        if (fst) {
            fst = false;
            prev = curr;
            continue;
        }
        drawLine(curr.x, curr.y, prev.x, prev.y);
        prev = curr;
        S.pop();
    }
    drawLine(first.x, first.y, prev.x, prev.y);
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

    auto start = chrono::high_resolution_clock::now();
    for (int trials = 0; trials < num_trials; trials++) graham_scan();
    auto stop = chrono::high_resolution_clock::now();
    auto durationRec = chrono::duration_cast<chrono::microseconds>(stop - start);
    cout << "Time: " << (durationRec.count()/1e6/num_trials) << endl << "-----" << endl;

    // WRITE TO PPM
    ofstream image("07_graham_scan.ppm");
    image << "P3 " << N << " " << N << " 1" << endl;

//    for (auto &i : ppm) {
//        for (auto &j : i) {
//            image << j.r << " " << j.g << " " << j.b << " ";
//        }
//        image << endl;
//    }
//    image.close();
    return 0;
}