//
// Created by Spandan Das on 9/9/2019.
//
#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>

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

static col ppm[800][800];

double random() {
    return (double) rand() / RAND_MAX;
}

double dist(double x1, double y1, double x2, double y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

void drawpt(int x, int y, int r, int g, int b) {
    if (x < 0 || x >= 800 || y < 0 || y >= 800) return;
    ppm[x][y].r = r;
    ppm[x][y].g = g;
    ppm[x][y].b = b;
}


int main() {
    srand(time(nullptr));
    // coordinates of vertices are doubles
    Point pts[40];
    for (auto & pt : pts) {
        pt = Point(random(), random());
        int roundedX = (int)(800 * pt.x);
        int roundedY = (int)(800 * pt.y);
        //cout << roundedX << " " << roundedY << endl;
        drawpt(roundedX, roundedY, 0, 0, 0);
    }
    double min_dist = LONG_MAX;
    int minInd[2];
    double old;
    for (int i = 0; i < 40; i++) {
        for (int j = 0; j < i; j++) {
            old = min_dist;
            min_dist = min(min_dist, dist(pts[i].x, pts[i].y, pts[j].x, pts[j].y));
            if (min_dist < old) {
                minInd[0] = i;
                minInd[1] = j;
            }
        }
    }
    int rounded_x1 = (int)(800 * pts[minInd[0]].x);
    int rounded_y1 = (int)(800 * pts[minInd[0]].y);
    drawpt(rounded_x1, rounded_y1, 1, 0, 0);
    int rounded_x2 = (int)(800 * pts[minInd[1]].x);
    int rounded_y2 = (int)(800 * pts[minInd[1]].y);
    drawpt(rounded_x2, rounded_y2, 1, 0, 0);



    // WRITE TO PPM
    ofstream image("02_closest_pair.ppm");
    image << "P3 800 800 1" << endl;

    // white background
    for (auto & i : ppm) {
        for (auto & j : i) {
            j.r = 1;
            j.g = 1;
            j.b = 1;
        }
    }

    for (auto &i : ppm) {
        for (auto &j : i) {
            image << j.r << " " << j.g << " " << j.b << " ";
            if (j.b == 0) cout << "asdf"<< endl;
        }
        image << endl;
    }

    for (auto &i : ppm) {
        for (auto &j : i) {
            cout << j.r << " " << j.g << " " << j.b << " ";
        }
        image << endl;
    }
    image.close();
    return 0;
}