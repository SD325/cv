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
#define num_pts 4 // number of points

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

vector<int> brute_force(Point x_sorted[], int len) {
    double min_dist = LONG_MAX;
    double this_dist = 0;
    vector<int> minInd(2);
    for (int i = 0; i < len; i++) {
        for (int j = i+1; j < len; j++) {
            this_dist = dist(x_sorted[i], x_sorted[j]);
            if (this_dist < min_dist) {
                minInd.at(0) = i;
                minInd.at(1) = j;
                min_dist = this_dist;
            }
        }
    }
    return minInd;
}


vector<int> closest_helper(Point x_sorted[], Point y_sorted[], int len) {
    if (len <= 3) {
        return brute_force(x_sorted, len);
    }

    int mid = len/2;
    Point midpoint = x_sorted[mid];
    Point x_l[len-mid];
    Point x_r[mid];
    int x_l_len = 0;
    int x_r_len = 0;
    for (int i = 0; i < len; i++) {
        if (x_sorted[i].x <= midpoint.x) {
            x_l[x_l_len++] = x_sorted[i];
        }
        else {
            x_r[x_r_len++] = x_sorted[i];
        }
    }

    Point y_l[len-mid];
    Point y_r[mid];
    int y_l_len = 0;
    int y_r_len = 0;
    for (int i = 0; i < len; i++) {
        if (y_sorted[i].x <= midpoint.x) {
            y_l[y_l_len++] = y_sorted[i];
        }
        else {
            y_r[y_r_len++] = y_sorted[i];
        }
    }

    if (x_l_len != y_l_len || x_r_len != y_r_len) {
        cout << "NOT SAME" << endl;
    }
    vector<int> left_min = closest_helper(x_l, y_l, x_l_len);
    vector<int> right_min = closest_helper(x_r, y_r, x_r_len);
    int offset = mid;
    if (len % 2 == 1) offset++;
    double left_dist = dist(x_sorted[left_min.at(0)], x_sorted[left_min.at(1)]);
    double right_dist = dist(x_sorted[right_min.at(0) + offset], x_sorted[right_min.at(1) + offset]);
    double min_dist;
    vector<int> min_ind;
    if (left_dist < right_dist) {
        min_dist = left_dist;
        min_ind.push_back(left_min.at(0));
        min_ind.push_back(left_min.at(1));
    }
    else {
        min_dist = right_dist;
        min_ind.push_back(right_min.at(0) + offset);
        min_ind.push_back(right_min.at(1) + offset);
    }

    Point in_strip_y_sorted[len];
    int strip_len = 0;
    for (int i = 0; i < len; i++) {
        if (abs(y_sorted[i].x - midpoint.x) < min_dist) {
            in_strip_y_sorted[strip_len++] = y_sorted[i];
        }
    }


    // minimum inside strip
    double min_dist_in_strip = LONG_MAX;
    double this_dist = 0;
    Point minPoints[2];
    for (int i = 0; i < strip_len; i++) {
        for (int j = i+1; j < strip_len && j < i+8; j++) {
            this_dist = dist(in_strip_y_sorted[i], in_strip_y_sorted[j]);
            if (this_dist < min_dist_in_strip) {
                minPoints[0] = in_strip_y_sorted[i];
                minPoints[1] = in_strip_y_sorted[j];
                min_dist_in_strip = this_dist;
            }
        }
    }

    if (min_dist_in_strip < min_dist) {
        for (int i = 0; i < len; i++) {
            if (minPoints[0].x == x_sorted[i].x && minPoints[0].y == x_sorted[i].y) {
                min_ind.at(0) = i;
                break;
            }
        }
        for (int i = 0; i < len; i++) {
            if (minPoints[1].x == x_sorted[i].x && minPoints[1].y == x_sorted[i].y) {
                min_ind.at(1) = i;
                break;
            }
        }
    }
    return min_ind;
}


vector<int> closest() {
    Point x_sorted[num_pts];
    Point y_sorted[num_pts];
    for (int i = 0; i < num_pts; i++) {
        x_sorted[i] = pts[i];
        y_sorted[i] = pts[i];
    }
    sort(x_sorted, x_sorted+num_pts);
    y_sorting = true;
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