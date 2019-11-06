//
// Created by Spandan Das on 9/9/2019.
//



// Need to print recursive and randomized times


#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <vector>
#include <chrono>
#include <unordered_map>

#define num_pts 100000 // number of points
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
                //if ((*dict)[sub_ind] == i) continue;
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
            //dict.erase(dict.begin(), dict.end());
            (*dict).clear();
            make_dictionary(num_squares, side, dict, i+1);
//            for (int j = 0; j < num_pts; j++) {
//                int curr_x = (int) (pts[j].x / side);
//                int curr_y = (int) (pts[j].y / side);
//                (*dict)[curr_x + curr_y*num_squares] = j;
//            }
        }
        else {
            (*dict)[x + num_squares*y] = i;
        }
    }
    return result;
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
    for (int trials = 0; trials < num_trials; trials++) {
        for (auto &pt : pts) {
            pt = Point(random_double(), random_double());
        }
    //    auto start = chrono::high_resolution_clock::now();
//        vector<int> brute_force_results = brute_force(temp, num_pts);
//        cout << pts[brute_force_results.at(0)].x << " " << pts[brute_force_results.at(0)].y << endl;
//        cout << pts[brute_force_results.at(1)].x << " " << pts[brute_force_results.at(1)].y << endl;
    //    auto stop = chrono::high_resolution_clock::now();
    //    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    //    cout << "Brute Force Time: " << duration.count()/1e6 << endl << "-----" << endl;
    //
    //    auto start2 = chrono::high_resolution_clock::now();
    //    vector<int> recursive_results = merge_find(temp);
    //    auto stop2 = chrono::high_resolution_clock::now();
    //    auto duration2 = chrono::duration_cast<chrono::microseconds>(stop2 - start2);
    //    cout << "Recursive Time: " << duration2.count()/1e6 << endl << "-----" << endl;
    //
    //    auto start3 = chrono::high_resolution_clock::now();
    //    vector<int> fast_recursive_results = closest_find(temp);
    //    auto stop3 = chrono::high_resolution_clock::now();
    //    auto duration3 = chrono::duration_cast<chrono::microseconds>(stop3 - start3);
    //    cout << "Fast Recursive Time: " << duration3.count()/1e6 << endl << "-----" << endl;


        random_shuffle(pts, pts+num_pts);
        vector<int> minInd = rand_finder();
        cout << minInd[0] << " " << minInd[1] << endl;
        cout << pts[minInd[0]].x << " " << pts[minInd[0]].y << endl;
        cout << pts[minInd[1]].x << " " << pts[minInd[1]].y << endl;
        cout << dist(pts[minInd[0]], pts[minInd[1]]) << endl;
        //cout << "Randomized Time: " << duration.count()/1e6 << endl << "-----" << endl;
    }
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    cout << "average: " << duration.count()/(num_trials*1e6) << endl; // divide by 100
    //cout << "--------------------" << endl;

    return 0;
}
