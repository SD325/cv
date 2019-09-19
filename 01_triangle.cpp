//
// Created by Spandan Das on 9/9/2019.
//
#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>

using namespace std;

typedef struct Point {
    int x;
    int y;

    // convert to integers
    Point(double x, double y) {
        this->x = x / 1;
        this->y = y / 1;
    }
} pt;

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
    // generates random double in the range [0, 1)
    // then scales up to max (in this lab, 800)
    return (double) (min + (max - min) * ((double) rand() / RAND_MAX)); //--> old
}

double dist(double x1, double y1, double x2, double y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

double area_using_side_len(double a, double b, double c) {
    double s = (a+b+c)/2.0;
    return sqrt(s*(s-a)*(s-b)*(s-c));
}

double getCircumrad(double a, double b, double c) {
    double area = area_using_side_len(a, b, c);
    return (a*b*c)/(4.0*area);
}

double getInrad(double a, double b, double c) {
    double area = area_using_side_len(a, b, c);
    double s = (a+b+c)/2.0;
    return area/s;
}

// barycentric coordinates of incenter: (a:b:c)
// ==> same as (a/(a+b+c) : b/(a+b+c) : c/(a+b+c))
void getIncenter(double in[2], double v[3][2], double a, double b, double c) {
    in[0] = (a / (a + b + c)) * v[0][0] + (b / (a + b + c)) * v[1][0] + (c / (a + b + c)) * v[2][0];
    in[1] = (a / (a + b + c)) * v[0][1] + (b / (a + b + c)) * v[1][1] + (c / (a + b + c)) * v[2][1];
}

// barycentric coordinates of circumcenter:
// (a^2 * (- a^2 + b^2 + c^2) : b^2 * (a^2 + b^2 + c^2) : c^2 * (a^2 + b^2 - c^2))
// ==> we divide by the sum to normalize
void getCircumcenter(double cir[2], double v[3][2], double a, double b, double c) {
    double l1 = a * a * (b * b + c * c - a * a);
    double l2 = b * b * (c * c + a * a - b * b);
    double l3 = c * c * (a * a + b * b - c * c);
    double sum = l1 + l2 + l3;
    l1 /= sum;
    l2 /= sum;
    l3 /= sum;
    cir[0] = l1 * v[0][0] + l2 * v[1][0] + l3 * v[2][0];
    cir[1] = l1 * v[0][1] + l2 * v[1][1] + l3 * v[2][1];
}

// same concept as circumcenter
void getNinePointCenter(double cir[2], const double midpoint0[2], const double midpoint1[2], const double midpoint2[2], double a, double b, double c) {
    double l1 = a * a * (b * b + c * c - a * a);
    double l2 = b * b * (c * c + a * a - b * b);
    double l3 = c * c * (a * a + b * b - c * c);
    double sum = l1 + l2 + l3;
    l1 /= sum;
    l2 /= sum;
    l3 /= sum;
    cir[0] = l1 * midpoint0[0] + l2 * midpoint1[0] + l3 * midpoint2[0];
    cir[1] = l1 * midpoint0[1] + l2 * midpoint1[1] + l3 * midpoint2[1];
}

void drawpt(int x, int y, int r, int g, int b) {
    if (x < 0 || x >= 800 || y < 0 || y >= 800) return;
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

void drawCircle(int cx, int cy, double r) {
    int x, y, xmax, y2, y2_new, ty;
    xmax = (int) (r * 0.70710678); // maximum x at radius/sqrt(2)
    y  = (int) r;
    y2 = y * y;
    ty = (2 * y) - 1;
    y2_new = y2;

    for (x = 0; x <= xmax; x++) {
        if ((y2 - y2_new) >= ty) {
            y2 -= ty;
            y -= 1;
            ty -= 2;
        }
        drawpt(x+cx, y+cy, 0, 0, 0);
        drawpt(x+cx, -y+cy, 0, 0, 0);
        drawpt(-x+cx, y+cy, 0, 0, 0);
        drawpt(-x+cx, -y+cy, 0, 0, 0);
        drawpt(y+cx, x+cy, 0, 0, 0);
        drawpt(y+cx, -x+cy, 0, 0, 0);
        drawpt(-y+cx, x+cy, 0, 0, 0);
        drawpt(-y+cx, -x+cy, 0, 0, 0);

        y2_new -= (2 * x) - 3;

    }

}

int main() {
    //srand(time(0));
    srand(time(nullptr));
    double v[3][2] = {{random(0.0, 800.0), random(0.0, 800.0)},
                      {random(0.0, 800.0), random(0.0, 800.0)},
                      {random(0.0, 800.0), random(0.0, 800.0)}};

    //print to three decimal places
    //cout.precision(3);
//    cout << "p0: " << fixed << "(" << v[0][0] << ", " << v[0][1] << ")" << endl;
//    cout << "p1: " << fixed << "(" << v[1][0] << ", " << v[1][1] << ")" << endl;
//    cout << "p2: " << fixed << "(" << v[2][0] << ", " << v[2][1] << ")" << endl;

    double a = dist(v[1][0], v[1][1], v[2][0], v[2][1]);
    double b = dist(v[0][0], v[0][1], v[2][0], v[2][1]);
    double c = dist(v[0][0], v[0][1], v[1][0], v[1][1]);

    double incenter[2];
    getIncenter(incenter, v, a, b, c);
    double inradius = getInrad(a, b, c);
    pt incen = Point(incenter[0], incenter[1]);



    double circumcenter[2];
    getCircumcenter(circumcenter, v, a, b, c);
    double circumradius = getCircumrad(a, b, c);
    pt circumcen = Point(circumcenter[0], circumcenter[1]);

//    cout << "incenter: (" << incenter[0] << ", " << incenter[1] << ")" << endl;
//    cout << "circumcenter: (" << circumcenter[0] << ", " << circumcenter[1] << ")" << endl;


    // TO-DO: euler line, nine point circle, and graphics
    pt vert[3] = {Point(v[0][0], v[0][1]), Point(v[1][0], v[1][1]), Point(v[2][0], v[2][1])};
//    cout << "p0: " << fixed << "(" << vert[0].x << ", " << vert[0].y << ")" << endl;
//    cout << "p1: " << fixed << "(" << vert[1].x << ", " << vert[1].y << ")" << endl;
//    cout << "p2: " << fixed << "(" << vert[2].x << ", " << vert[2].y << ")" << endl;

    ofstream image("01_triangle.ppm");
    image << "P3 800 800 1" << endl;

    // white background
    for (auto & i : ppm) {
        for (auto & j : i) {
            j.r = 1;
            j.g = 1;
            j.b = 1;
        }
    }

    //drawLine(100, 100, 600, 450); // positive slope and < 1
    //drawLine(600, 750, 100, 100); // positive slope and > 1
    //drawLine(100, 700, 350, 600); // negative slope and < 1
    //drawLine(100, 700, 200, 100); // negative slope and > 1
    drawLine(vert[0].x, vert[0].y, vert[1].x, vert[1].y);
    drawLine(vert[0].x, vert[0].y, vert[2].x, vert[2].y);
    drawLine(vert[1].x, vert[1].y, vert[2].x, vert[2].y);

    drawCircle(incen.x, incen.y, inradius);  //incircle
    drawCircle(circumcen.x, circumcen.y, circumradius);  //circumcircle

    double midpoint0[2] = {(v[1][0]+v[2][0])/2.0, (v[1][1]+v[2][1])/2.0};
    double midpoint1[2] = {(v[0][0]+v[2][0])/2.0, (v[0][1]+v[2][1])/2.0};
    double midpoint2[2] = {(v[0][0]+v[1][0])/2.0, (v[0][1]+v[1][1])/2.0};

    double middist0 = dist(midpoint1[0], midpoint1[1], midpoint2[0], midpoint2[1]);
    double middist1 = dist(midpoint0[0], midpoint0[1], midpoint2[0], midpoint2[1]);
    double middist2 = dist(midpoint0[0], midpoint0[1], midpoint1[0], midpoint1[1]);

    double nineCenter[2];
    getNinePointCenter(nineCenter, midpoint0, midpoint1, midpoint2, middist0, middist1, middist2);
    pt ninecen = Point(nineCenter[0], nineCenter[1]);

    drawCircle(ninecen.x, ninecen.y, dist(ninecen.x, ninecen.y, midpoint0[0], midpoint0[1]));

    // print so that (0,0) is bottom left like Cartesian
    for (auto &i : ppm) {
        for (auto &j : i) {
            image << j.r << " " << j.g << " " << j.b << " ";
        }
        image << endl;
    }
    image.close();
    return 0;
}