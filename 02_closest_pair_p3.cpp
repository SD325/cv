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
#define num_pts 5 // number of points

using namespace std;


static bool y_sorting = false;
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
        if (y_sorting) return y < pt.y;
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

vector<int> brute_force(Point p[], int len) {
    double min_dist = LONG_MAX;
    double old;
    vector<int> minInd(2);
    for (int i = 0; i < len; i++) {
        for (int j = i+1; j < len; j++) {
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

vector<int> closest_in_strip(Point strip[], int len, double min_dist, vector<int> currMin) {
    // *******************************
    // NEED TO CHANGE!!!!!!!!!!!!!!!!!

    for (int i = 0; i < len; ++i) {
        for (int j = i + 1; j < len && (strip[j].y - strip[i].y) < min_dist; ++j) {
            if (dist(strip[i], strip[j]) < min_dist) {
                min_dist = dist(strip[i], strip[j]);
                currMin.at(0) = i;
                currMin.at(1) = j;
            }
        }
    }
    return currMin;
}


vector<int> closest_helper(Point sortedX[], Point sortedY[], int len) {
    if (len <= 3) {
        return brute_force(sortedX, len);
    }
    else {
        int mid = len/2;
        Point midX = sortedX[mid];
        Point left_sortedY[mid+1];
        int leftInd = 0;
        Point right_sortedY[len - (mid+1)];
        int rightInd = 0;
        for (int i = 0; i < len; i++) {
            if (sortedY[i].x <= midX.x) {
                left_sortedY[leftInd] = sortedY[i];
                leftInd++;
            }
            else {
                right_sortedY[rightInd] = sortedY[i];
                rightInd++;
            }
        }


        vector<int> left_min = closest_helper(sortedX, left_sortedY, mid);
        vector<int> right_min = closest_helper(sortedX + mid, right_sortedY, len - mid);
        double left_dist = dist(sortedX[left_min.at(0)], sortedX[left_min.at(1)]);
        double right_dist = dist(sortedX[right_min.at(0) + mid], sortedX[right_min.at(1) + mid]);
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

        //double mid_x = (pts[curr.at(mid)].x + pts[curr.at(mid+1)].x) / 2.0;
        //double mid_x = pts[curr.at(mid)].x;
        Point inside_strip[len];
        int strip_ind = 0;
        for (int i = 0; i < len; i++) {
            if (abs(sortedX[i].x - midX.x) < min_dist) {
                inside_strip[strip_ind] = sortedX[i];
                strip_ind++;
            }
        }
        return closest_in_strip(inside_strip, strip_ind, min_dist, min_ind);
//        double this_dist = 0;
//        for (int i = 0; i < len; i++) {
//            for (int j = i+1; j < len; j++) {
//                this_dist = dist(pts[curr.at(i)], pts[curr.at(j)]);
//                if (this_dist < min_dist) {
//                    min_dist = this_dist;
//                    min_ind.at(0) = curr.at(i);
//                    min_ind.at(1) = curr.at(j);
//                }
//            }
//        }
//        return min_ind;
    }
}


vector<int> closest() {
    Point x_sorted[num_pts];
    Point y_sorted[num_pts];
    vector<int> temp;
    for (int i = 0; i < num_pts; i++) {
        x_sorted[i] = pts[i];
        y_sorted[i] = pts[i];
    }
    sort(x_sorted, x_sorted+num_pts);
    sort(y_sorted, y_sorted+num_pts);
    return closest_helper(x_sorted, y_sorted, num_pts);
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

    vector<int> minInd = closest();
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