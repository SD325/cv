#include "testlib.h"
using namespace std;
int main(int argc, char* argv[]) {
    registerValidation(argc, argv);
    int n = inf.readInt(1, 1000000, "n");

    for(int i = 0; i < n; i++) {
        inf.readToken("[a-z]{1,100}", "s");
        inf.readEoln();
    }
    inf.readEof();
}
