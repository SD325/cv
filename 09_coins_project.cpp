//
// Created by Spandan Das on 11/21/2019.
//
#include <bits/stdc++.h>
#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>
#include <chrono>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#define hi_thresh 70
#define low_thresh 40
#define filename "coinsEasy_compress.ppm"

using namespace std;

bool sortbysecdesc(const pair<string,int> &a, const pair<string,int> &b){
    return a.second > b.second;
}

vector<double> convolution(vector<double> kernel, int kRows, int kCols, double factor, vector<double> in, int M, int N) {
    // find center position of kernel (half of kernel size)
    int kCenterX = kCols / 2;
    int kCenterY = kRows / 2;
    vector<double> out(N*M);
    for(int i=0; i < N; ++i)              // rows
    {
        for(int j=0; j < M; ++j)          // columns
        {
            int index_1 = i * M + j;
            for(int m=0; m < kRows; ++m)     // kernel rows
            {
                int mm = kRows - 1 - m;      // row index of flipped kernel

                for(int n=0; n < kCols; ++n) // kernel columns
                {
                    int nn = kCols - 1 - n;  // column index of flipped kernel

                    // index of input signal, used for checking boundary
                    int ii = i + (kCenterY - mm);
                    int jj = j + (kCenterX - nn);

                    // ignore input samples which are out of bound
                    if (ii >= 0 && ii < N && jj >= 0 && jj < M) {
                        int index_2 = ii * M + jj;
                        int index_3 = mm * kRows + nn;
                        out[index_1] += in[index_2] * kernel[index_3];
                    }
                }
            }
        }
    }
    for (double & i : out) i *= factor;
    return out;
}

static vector<double> finalized;
static unordered_set<int> visited;
void dfs(int ind, int M, int N) {
    visited.insert(ind);
    if (low_thresh <= finalized[ind] && finalized[ind] < hi_thresh) finalized[ind] = (double) hi_thresh;
    vector<int> neigh = {ind-M-1, ind-M, ind-M+1, ind-1, ind+1, ind+M-1, ind+M, ind+M+1};
    for (int& n : neigh) {
        if (n < 0 || n >= M*N || (ind % M == 0 && n % M == M-1) || (ind % M == M-1 && n % M == 0)) continue;
        if (visited.find(n) != visited.end()) continue;
        if (finalized[n] < low_thresh) continue;
        dfs(n, M, N);
    }
}

int main() {
    int M;
    int N;
    ifstream file(filename, ios::in | ios::binary);
    int max_val;
    string image_type;
    file >> image_type >> M >> N;
    file >> max_val;
    vector<double> a;
    if (image_type == "P3") {
        double inp1;
        double inp2;
        double inp3;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                file >> inp1 >> inp2 >> inp3;
                a.push_back((inp1 + inp2 + inp3) / 3.0);
            }
        }
    }
    else {
        file.get();
        unsigned char buffer[3];
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                file.read((char*) buffer , 3);
                a.push_back((buffer[0] + buffer[1] + buffer[2])/3.0);
            }
        }
    }
    file.close();

    vector<double> kernel1 = {2,  4,  5,  4,  2,
                              4,  9, 12,  9,  4,
                              5, 12, 15, 12,  5,
                              4,  9, 12,  9,  4,
                              2,  4,  5,  4,  2};
    double factor1 = 1.0/159.0;
    vector<double> temp_ = convolution(kernel1, 5, 5, factor1, a, M, N);

    vector<double> kernel2 = {-1, 0, 1,
                              -2, 0, 2,
                              -1, 0, 1};
    vector<double> g_x = convolution(kernel2, 3, 3, 1, temp_, M, N);

    vector<double> kernel3 = { 1, 2, 1,
                               0, 0, 0,
                               -1,-2,-1};
    vector<double> g_y = convolution(kernel3, 3, 3, 1, temp_, M, N);
    vector<double> G;
    for (int i = 0; i < M*N; i++) G.push_back(sqrt((g_x[i]*g_x[i]) + (g_y[i]*g_y[i])));
    vector<double> theta;
    double temp;
    for (int i = 0; i < M*N; i++) {
        temp = atan2(g_y[i], g_x[i])*180/M_PI;
        temp = round(temp/45);
        theta.push_back(temp);  // -4, -3, ... , 3, 4
    }
    int ind1, ind2;
    for (int i = 0; i < M*N; i++) {
        finalized.push_back(G[i]);
        if (theta[i] == 0 || theta[i] == 4 || theta[i] == -4) {
            ind1 = i-1;
            ind2 = i+1;
            // check
            if (ind1 % M != M-1) {
                if (G[i] <= G[ind1]) {
                    finalized[i] = 0;
                    continue;
                }
            }
            if (ind2 % M != 0) {
                if (G[i] <= G[ind2]) {
                    finalized[i] = 0;
                    continue;
                }
            }
        }
        else if (theta[i] == 2 || theta[i] == -2) {
            ind1 = i-M;
            ind2 = i+M;
            // check
            if (ind1 >= 0) {
                if (G[i] <= G[ind1]) {
                    finalized[i] = 0;
                    continue;
                }
            }
            if (ind2 < M*N) {
                if (G[i] <= G[ind2]) {
                    finalized[i] = 0;
                    continue;
                }
            }
        }
        else if (theta[i] == 3 || theta[i] == -1) {
            ind1 = i-M-1;
            ind2 = i+M+1;
            // check
            if (ind1 >= 0 && ind1 % M != M-1) {
                if (G[i] <= G[ind1]) {
                    finalized[i] = 0;
                    continue;
                }
            }
            if (ind2 < M*N && ind2 % M != 0) {
                if (G[i] <= G[ind2]) {
                    finalized[i] = 0;
                    continue;
                }
            }
        }
        else if (theta[i] == 1 || theta[i] == -3) {
            ind1 = i-M+1;
            ind2 = i+M-1;
            // check
            if (ind2 >= 0 && ind2 % M != M-1) {
                if (G[i] <= G[ind2]) {
                    finalized[i] = 0;
                    continue;
                }
            }
            if (ind1 < M*N && ind1 % M != 0) {
                if (G[i] <= G[ind1]) {
                    finalized[i] = 0;
                    continue;
                }
            }
        }
        else {
            cout << "Whoops! Something went wrong!" << endl;
            break;
        }
        if (finalized[i] < low_thresh) finalized[i] = 0;
    }
    for (int i = 0; i < M*N; i++) {
        if (finalized[i] >= hi_thresh && visited.find(i) == visited.end()) {
            dfs(i, M, N);
        }
    }
    for (double& i : finalized) {
        if (i < hi_thresh) i = 0;
    }


    // Circle Detection
    int rmin = 5;
    int rmax = 60;
    // double theta_step = 5.0;
    int circ_threshold = 15;

    int curr_index;
    int a_, b_;
    double t;
    map<string, int> acc = {};
    string st;
    for (int x = 0; x < N; x++) {
        for (int y = 0; y < M; y++) {
            curr_index = M*x+y;
            if (finalized[curr_index]) {
                for (int sign = -2; sign < 2; sign+=1) {
                    t = atan2(g_y[curr_index], g_x[curr_index])  + (double) sign * M_PI / 2.0;
                    for (int r = rmin; r <= rmax; r++) {
                        a_ = x - (int) r * cos(t);
                        b_ = y - (int) r * sin(t);
                        if (a_ < 0 || a_ >= N || b_ < 0 || b_ >= M) break;
                        st = to_string(a_) + " " + to_string(b_) + " " + to_string(r);
                        acc[st]++;
                    }
                }
            }
        }
    }

    vector<pair<string, int>> votes;
    for (auto i : acc) votes.push_back(make_pair(i.first, i.second));
    sort(votes.begin(), votes.end(), sortbysecdesc);

//    typedef function<bool(pair<string, int>, pair<string, int>)> Comparator;
//    Comparator compFunctor =
//            [](pair<string, int> elem1 ,pair<string, int> elem2) {
//                return elem1.second > elem2.second;
//            };
//    set<pair<string, int>, Comparator> acc_(acc.begin(), acc.end(), compFunctor);


    unordered_set<int> circ_poss;
    map<int, int> loc_to_rad;
    int check_x, check_y, check_r;
    int itr;
    int r = 0;
    vector<int> abr;
    int frq = 0;
    bool circle_check;
    int min_rad = max(M, N);
    for (auto i : votes) {
        st = i.first;
        frq = i.second;
        abr.clear();
        istringstream ss(st);
        for (itr = 0; itr < 3; itr++) {
            string word;
            ss >> word;
            abr.push_back(stoi(word));
            // cout << word << " ";
        }
        // cout << endl;
        a_ = abr[0]; b_ = abr[1]; r = abr[2];
        if (frq >= circ_threshold) {
            circle_check = true;
            for (auto checker : loc_to_rad) {
                check_x = checker.first / M;
                check_y = checker.first % M;
                check_r = checker.second;
                if ((a_ - check_x)*(a_ - check_x) + (b_ - check_y)*(b_ - check_y) <= check_r * check_r) {
                    circle_check = false;
                    break;
                }
            }
            if (!circle_check) continue;
            // cout << frq << " " << a_ << " " << b_ << " " << r << endl;
            circ_poss.insert(M * a_ + b_);
            loc_to_rad[M * a_ + b_] = r;
            min_rad = min(min_rad, r);
        }
    }

    int cents = 0;
    vector<double> diam_ratios =
            {750. / 705., 835. / 705., 705. / 705., 955. / 705., 1205. / 705., 1043. / 705.}; // p, n, d, q, hD, D

    int penn = 0; int nick = 0; int dime = 0; int quar = 0; int half = 0; int doll = 0;
    int closest_ind = 0; double min_diff; double this_diff;
    for (auto i : loc_to_rad) {
        check_r = i.second;
        min_diff = (double) max(M, N);
        for (itr = 0; itr < 6; itr++) {
            this_diff = abs(check_r / (double) min_rad - diam_ratios[itr]);
            if (this_diff < min_diff) {
                min_diff = this_diff;
                closest_ind = itr;
            }
        }
        if (closest_ind == 0) penn++;
        else if (closest_ind == 1) nick++;
        else if (closest_ind == 2) dime++;
        else if (closest_ind == 3) quar++;
        else if (closest_ind == 4) half++;
        else if (closest_ind == 5) doll++;
    }
//    cout << "p: " << penn << endl;
//    cout << "n: " << nick << endl;
//    cout << "d: " << dime << endl;
//    cout << "q: " << quar << endl;
//    cout << "hD: " << half << endl;
//    cout << "D: " << doll << endl;
    int sum = penn + 5 * nick + 10 * dime + 25 * quar + 50 * half + 100 * doll;

    cout << "Total: " << sum << " cents" << endl;


    // WRITE TO PPM
    ofstream image("09_coins_project.ppm");
    image << "P3 " << M << " " << N << " 255" << endl;

    int index;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            index = M*i+j;
            if (circ_poss.find(index) != circ_poss.end()) {
                image << 255 << " " << 0 << " " << 0 << " ";
            }
            else if (finalized[index]) {
                image << 255 << " " << 255 << " " << 255 << " ";
            }
            else {
                image << 0 << " " << 0 << " " << 0 << " ";
            }
//            int x = (int) a[index];
//            image << x << " " << x << " " << x << " ";
        }
        image << endl;
    }
    image.close();
    return 0;
}