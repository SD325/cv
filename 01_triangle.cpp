//
// Created by Spandan Das on 9/9/2019.
//
#include <iostream>
#include <cmath>
#include <time.h>
#include <fstream>

using namespace std;


typedef struct Point {
    int x;
    int y;
    Point(double x, double y) {
        this -> x = x/1;
        this -> y = y/1;
    }
}pt;

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
double random(double min, double max) {
    return (double) (min + (max - min) * ((double) rand() / RAND_MAX)); //--> old
}

double dist(double x1, double y1, double x2, double y2) {
    return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}

// barycentric coordinates of incenter: (a:b:c)
// ==> same as (a/(a+b+c) : b/(a+b+c) : c/(a+b+c))
void getIncenter(double in[2], double v[3][2], double a, double b, double c) {
    in[0] = (a / (a+b+c)) * v[0][0] + (b / (a+b+c)) * v[1][0] + (c / (a+b+c)) * v[2][0];
    in[1] = (a / (a+b+c)) * v[0][1] + (b / (a+b+c)) * v[1][1] + (c / (a+b+c)) * v[2][1];
}

// barycentric coordinates of circumcenter:
// (a^2 * (- a^2 + b^2 + c^2) : b^2 * (a^2 + b^2 + c^2) : c^2 * (a^2 + b^2 - c^2))
// ==> we divide by the sum to normalize
void getCircumcenter(double cir[2], double v[3][2], double a, double b, double c) {
    double l1 = a*a * (b*b + c*c - a*a);
    double l2 = b*b * (c*c + a*a - b*b);
    double l3 = c*c * (a*a + b*b - c*c);
    double sum = l1+l2+l3;
    l1 /= sum; l2 /= sum; l3 /= sum;
    cir[0] = l1*v[0][0] + l2*v[1][0] + l3*v[2][0];
    cir[1] = l1*v[0][1] + l2*v[1][1] + l3*v[2][1];
}

void drawpt(int x, int y, int r, int g, int b) {
    ppm[x][y].r = r; ppm[x][y].g = g; ppm[x][y].b = b;
}

void draw(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int j = y1;
    int eps = dy - dx;
    for (int i = x1; i < x2; i++) {
        drawpt(i, j, 0, 0, 0);

    }
}

int main() {
    srand(time(nullptr));
    //cout << "Hello, World!" << std::endl;
    double v[3][2] = {{random(0.0, 800.0), random(0.0, 800.0)},
                      {random(0.0, 800.0), random(0.0, 800.0)},
                      {random(0.0, 800.0), random(0.0, 800.0)}};

    //print to three decimal places
    cout.precision(3);
    cout << "p0: " << fixed << "(" << v[0][0] << ", " << v[0][1] << ")" << endl;
    cout << "p1: " << fixed << "(" << v[1][0] << ", " << v[1][1] << ")" << endl;
    cout << "p2: " << fixed << "(" << v[2][0] << ", " << v[2][1] << ")" << endl;

    double a = dist(v[1][0], v[1][1], v[2][0], v[2][1]);
    double b = dist(v[0][0], v[0][1], v[2][0], v[2][1]);
    double c = dist(v[0][0], v[0][1], v[1][0], v[1][1]);

    double incenter[2];
    getIncenter(incenter, v, a, b, c);

    double circumcenter[2];
    getCircumcenter(circumcenter, v, a, b, c);

    cout << "incenter: (" << incenter[0] << ", " << incenter[1] << ")" << endl;
    cout << "circumcenter: (" << circumcenter[0] << ", " << circumcenter[1] << ")" << endl;

    
    // TO-DO: euler line, nine point circle, and graphics
    pt vert[3] = {Point(v[0][0], v[0][1]), Point(v[1][0], v[1][1]), Point(v[2][0], v[2][1])};
//    for (auto & i : vert) {
//        cout << i.x << ", " << i.y << endl;
//    }

    ofstream fout;
    fout.open("01_triangle.ppm");
    fout << "P3 800 800 1" << endl;
    draw(300, 500, 150, 170);
    //draw(vert[0].x, vert[0].y, vert[1].x, vert[1].y);
    for (auto & i : ppm) {
        for (auto &j : i) {
            fout << j.r << " " << j.g << " " << j.b << " ";
        }
        fout << endl;
    }
    fout.close();
    return 0;
}