//
// Created by Spandan Das on 9/9/2019.
//
#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <vector>
#include <chrono>

#define N 800 // resolution of ppm file
#define num_pts 500000 // number of points

using namespace std;

bool y_sorting = false;
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
static Point y_order[num_pts];
static col ppm[N][N];

double random() {
    return (double) rand() / RAND_MAX;
}

double dist(Point p1, Point p2) {
    return ((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y));
}

void drawpt(int x, int y, int r, int g, int b) {
    if (x < 0 || x >= N || y < 0 || y >= N) return;
    ppm[x][y].r = r;
    ppm[x][y].g = g;
    ppm[x][y].b = b;
}

vector<int> brute_force(vector<int> x_sorted, int len) {
    double min_dist = LONG_MAX;
    double this_dist = 0;
    vector<int> minInd(2);
    for (int i = 0; i < len; i++) {
        for (int j = i+1; j < len; j++) {
            this_dist = dist(pts[x_sorted.at(i)], pts[x_sorted.at(j)]);
            if (this_dist < min_dist) {
                minInd.at(0) = x_sorted.at(i);
                minInd.at(1) = x_sorted.at(j);
                min_dist = this_dist;
            }
        }
    }
    return minInd;
}


vector<int> closest_helper(vector<int> x_sorted, vector<int> y_sorted, int len) {
    if (len <= 3) {
        return brute_force(x_sorted, len);
    }
    else {
        int mid = len/2;
        Point mid_x = pts[x_sorted.at(mid)];
        vector<int> x_left;
        vector<int> y_left;
        vector<int> x_right;
        vector<int> y_right;
        for (int i = 0; i < mid; i++) {
            x_left.push_back(x_sorted.at(i));
            if (i >= y_sorted.size()) continue;
            if (y_order[y_sorted.at(i)].x < mid_x.x) y_left.push_back(y_sorted.at(i));
            else y_right.push_back(y_sorted.at(i));
        }
        for (int i = mid; i < len; i++) {
            x_right.push_back(x_sorted.at(i));
            if (i >= y_sorted.size()) continue;
            if (y_order[y_sorted.at(i)].x < mid_x.x) {
                y_left.push_back(y_sorted.at(i));
            }
            else {
                y_right.push_back(y_sorted.at(i));
            }
        }

        vector<int> left_min = closest_helper(x_left, y_left, x_left.size());
        vector<int> right_min = closest_helper(x_right, y_right, x_right.size());
        double left_dist = dist(pts[left_min.at(0)], pts[left_min.at(1)]);
        double right_dist = dist(pts[right_min.at(0)], pts[right_min.at(1)]);
        double min_dist;
        vector<int> min_ind;

        // if the points are the same, then don't include
        if (left_dist == 0) {
            min_dist = right_dist;
            min_ind.push_back(right_min.at(0));
            min_ind.push_back(right_min.at(1));
        }
        else if (right_dist == 0) {
            min_dist = left_dist;
            min_ind.push_back(left_min.at(0));
            min_ind.push_back(left_min.at(1));
        }
        else if (left_dist < right_dist) {
            min_dist = left_dist;
            min_ind.push_back(left_min.at(0));
            min_ind.push_back(left_min.at(1));
        }
        else {
            min_dist = right_dist;
            min_ind.push_back(right_min.at(0));
            min_ind.push_back(right_min.at(1));
        }

        vector<int> inside_strip;
        for (int i = 0; i < len; i++) {
            if (i >= y_sorted.size()) break;
            if (abs(y_order[y_sorted.at(i)].x - mid_x.x) < min_dist) {
                inside_strip.push_back(y_sorted.at(i));
            }
        }

        vector<int> min_ind_in_strip;
        min_ind_in_strip.push_back(-1);
        min_ind_in_strip.push_back(-2); // temp values
        double min_dist_in_strip = LONG_MAX;
        double this_dist = 0;
        for (int i = 0; i < (int) inside_strip.size(); i++) {
            for (int j = i+1; j < (int) inside_strip.size() && j < i+8; j++) {
                this_dist = dist(y_order[inside_strip.at(i)], y_order[inside_strip.at(j)]);
                if (this_dist < min_dist_in_strip) {
                    min_dist_in_strip = this_dist;
                    min_ind_in_strip.at(0) = inside_strip.at(i);
                    min_ind_in_strip.at(1) = inside_strip.at(j);
                }
            }
        }

        if (min_dist_in_strip < min_dist) {
            // convert y_sorted index to pts index
            Point py0 = y_order[min_ind_in_strip.at(0)];
            Point py1 = y_order[min_ind_in_strip.at(1)];
            for (int i = 0; i < len; i++) {
                if (py0.x == pts[x_sorted.at(i)].x && py0.y == pts[x_sorted.at(i)].y) {
                    min_ind.at(0) = x_sorted.at(i);
                    break;
                }
            }
            for (int i = 0; i < len; i++) {
                if (py1.x == pts[x_sorted.at(i)].x && py1.y == pts[x_sorted.at(i)].y) {
                    min_ind.at(1) = x_sorted.at(i);
                    break;
                }
            }
        }
        return min_ind;
    }
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


vector<int> closest_find(vector<int> temp) {
    for (int i = 0; i < num_pts; i++) {
        y_order[i] = pts[i];
    }
    sort(pts, pts+num_pts);
    y_sorting = true;
    sort(y_order, y_order + num_pts);
    return closest_helper(temp, temp, num_pts);
}


int main() {
    srand(time(nullptr));
    vector<int> temp;
    for (int i = 0; i < num_pts; i++) temp.push_back(i);
    // white background
//    for (auto &i : ppm) {
//        for (auto &j : i) {
//            j.r = 1;
//            j.g = 1;
//            j.b = 1;
//        }
//    }
    auto start = chrono::high_resolution_clock::now();
    for (int trials = 0; trials < 1; trials++) {
        for (auto &pt : pts) {
            pt = Point(random(), random());
//            int roundedX = (int) (N * pt.x);
//            int roundedY = (int) (N * pt.y);
//            drawpt(roundedX, roundedY, 0, 0, 0);
        }

        vector<int> minInd = brute_force(temp, num_pts);
        //vector<int> minInd = closest_find(temp);
        //vector<int> minInd = merge_helper(temp, num_pts);

//        cout << pts[brute.at(0)].x << " " << pts[brute.at(0)].y << endl;
//        cout << pts[brute.at(1)].x << " " << pts[brute.at(1)].y << endl;
//        drawpt((int) (N * pts[brute.at(0)].x), (int) (N * pts[brute.at(0)].y), 0, 1, 0);
//        drawpt((int) (N * pts[brute.at(1)].x), (int) (N * pts[brute.at(1)].y), 0, 1, 0);
//        vector<int> minInd = closest_find();
//        cout << pts[minInd.at(0)].x << " " << pts[minInd.at(0)].y << endl;
//        cout << pts[minInd.at(1)].x << " " << pts[minInd.at(1)].y << endl;
//        cout << "----------" << endl;
//        drawpt((int) (N * pts[minInd.at(0)].x), (int) (N * pts[minInd.at(0)].y), 1, 0, 0);
//        drawpt((int) (N * pts[minInd.at(1)].x), (int) (N * pts[minInd.at(1)].y), 1, 0, 0);
    }
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    //cout << duration.count() << endl;
    cout << duration.count()/1e6 << endl;
    cout << "average: " << duration.count()/1e8 << endl;


//    // WRITE TO PPM
//    ofstream image("02_closest_pair.ppm");
//    image << "P3 " << N << " " << N << " 1" << endl;
//
//    for (auto &i : ppm) {
//        for (auto &j : i) {
//            image << j.r << " " << j.g << " " << j.b << " ";
//        }
//        image << endl;
//    }
//    image.close();
    return 0;
}