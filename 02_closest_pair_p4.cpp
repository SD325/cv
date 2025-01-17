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
#include <unordered_map>

#define num_pts 10000 // number of points
#define num_trials 1

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


// coordinates of vertices are doubles
static Point pts[num_pts];
static Point y_order[num_pts];

double random_double() {
    return (double) rand() / RAND_MAX;
}

double dist(Point p1, Point p2) {
    return sqrt((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y));
}


vector<int> brute_force(vector<int> x_sorted, int len) {
    double min_dist = 2;
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



void make_dictionary(int num_squares, double side, unordered_map<int, int> *dict, int index) {
    for (int i = 0; i < index; i++) {
        int x = (int) (pts[i].x/side);
        int y = (int) (pts[i].y/side);
        int subsquare = x + num_squares*y;
        (*dict)[subsquare] = i;
    }
}


vector<int> rand_finder() {
    vector<int> result;
    result.push_back(0); result.push_back(1);
    double orig_dist = dist(pts[0], pts[1]);
    int num_squares = (int)(2.0 / (orig_dist)) + 1;
    double side = orig_dist / 2.0;
    unordered_map<int, int> *dict = new unordered_map<int, int>;
    make_dictionary(num_squares, side, dict, 2);
    for (int i = 2; i < num_pts; i++) {
        bool changed = false;
        int x = (int) (pts[i].x/side);
        int y = (int) (pts[i].y/side);
        double min_dist = orig_dist;
        for (int dx = -2; dx < 3; dx++) {
            for (int dy = -2; dy < 3; dy++) {
                int new_x = x+dx; int new_y = y+dy;
                if (new_x < 0 || new_y < 0 || new_x >= num_squares || new_y >= num_squares) continue;
                int sub_ind = new_x + new_y*num_squares;
                if ((*dict).find(sub_ind) == (*dict).end()) continue;
                double this_dist = dist(pts[i], pts[(*dict)[sub_ind]]);
                if (this_dist < min_dist) {
                    if (!changed) changed = true;
                    min_dist = this_dist;
                    result.at(0) = i;
                    result.at(1) = (*dict)[sub_ind];
                }
            }
        }
        if (changed) {
            num_squares = (int) (2.0 / (min_dist)) + 1;
            side = min_dist / 2.0;
            orig_dist = min_dist;
            (*dict).clear();
            make_dictionary(num_squares, side, dict, i+1);
        }
        else {
            (*dict)[x + num_squares*y] = i;
        }
    }
    return result;
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
        double min_dist_in_strip = 2;
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


vector<int> closest_find(const vector<int>& temp) {
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

    for (auto &pt : pts) {
        pt = Point(random_double(), random_double());
    }

    auto startRec = chrono::high_resolution_clock::now();
    vector<int> recursive_results = closest_find(temp);
    auto stopRec = chrono::high_resolution_clock::now();
    auto durationRec = chrono::duration_cast<chrono::microseconds>(stopRec - startRec);
    cout << "Recursive Time: " << durationRec.count()/1e6 << endl << "-----" << endl;


    auto startRand = chrono::high_resolution_clock::now();
    random_shuffle(pts, pts+num_pts);
    vector<int> randomized_results = rand_finder();
    auto stopRand = chrono::high_resolution_clock::now();
    auto durationRand = chrono::duration_cast<chrono::microseconds>(stopRand - startRand);
    cout << "Randomized Time: " << durationRand.count()/1e6 << endl << "-----" << endl;

    return 0;
}
