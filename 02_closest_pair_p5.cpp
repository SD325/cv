// Spandan Das
// Period 4


#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <utility>
#include <vector>
#include <chrono>
#include <unordered_map>

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


static vector<Point> pts;
static int num_pts;

double random_double() {
    return (double) rand() / RAND_MAX;
}

double dist(Point p1, Point p2) {
    return sqrt((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y));
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


void findClosestPair(vector<Point> points) {
    pts = std::move(points);
    random_shuffle(pts.begin(), pts.end());
    vector<int> randomized_results = rand_finder();
    cout << pts[randomized_results[0]].x << " " << pts[randomized_results[0]].y << endl;
    cout << pts[randomized_results[1]].x << " " << pts[randomized_results[1]].y << endl;
}

int main() {
    srand(time(nullptr));

    ifstream infile("points.txt");
    infile >> num_pts;
    vector<Point> points;
    double x, y;
    while (infile) {
        infile >> x >> y;
        points.emplace_back(x, y);
    }
    infile.close();
    findClosestPair(points);

    return 0;
}
