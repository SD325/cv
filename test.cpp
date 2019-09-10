#include <iostream>
#include <fstream>
#include <string>

using namespace std;

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

int main() {
    ofstream fout("test.ppm");
    fout << "P3 800 800 1" << endl;
    col ppm[800][800];
    fout << endl;
    fout.close();
    return 0;
}