/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   rand_prof.cpp
 * Author: rap027
 * Input:  double prob_dist[] - approximate probability ditribution of profile i.e. [0.7,0.2,0.1,0.1]
 *         double speed_dist[] - proportional speed ditribution of the levels of the profile i.e [1,0.75,0.5,0.1]
 *         double speed - free floe speed of the segment i.e. 130
 *         int prof_num - number of probablity levels i.e. 4 (must be the same as length of double prob_dist[])
 *         int class - class of the road i.e. 3
 * Output: res [] - random probabilistic speed profile with size of 1x672 000 (each 1000 elemnts represent one time step)
 
 */

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <math.h>
#include <stdio.h>
#include <random>

using namespace std;

double* randpr(double prob_dist[], double speed_dist[], double speed, int prof_num, int cl) {
    std::random_device rd;
    std::mt19937 rng(rd());
    double* res = 0;
    res = new double[672000];
    double prob_dist_t[672][prof_num];
    double speed_t[672];
    for (int c = 0; c < 672; c++) {
        double prob_dist_m[prof_num];
        speed_t[c] = speed;
        for (int i = 0; i < prof_num; i++)
            prob_dist_m[i] = prob_dist[i];

        if (c > 480) {
            speed_t[c] *= 1.2;
        }
        if (c % 96 < 24 || c % 96 > 80) {
            speed_t[c] *= 0.7;
            for (int i = 1; i < prof_num; i++) {
                prob_dist_m[i] = 0.7 * prob_dist_m[i];
            }
        }
        if (cl > 3) {
            for (int i = 1; i < prof_num; i++) {
                prob_dist_m[i] = 0.3 * prob_dist_m[i];
            }
        }
        for (int i = 0; i < prof_num; i++) {
            double mi = -0.2 * prob_dist_m[i];
            double ma = 0.2 * prob_dist_m[i];
            std::uniform_real_distribution<double> uni(mi, ma);
            auto rdn = uni(rng);
            prob_dist_t[c][i] = prob_dist_m[i] + rdn;
            if (prob_dist_t[c][i] < 0)
                prob_dist_t[c][i] = 0;
        }

        if (prob_dist_t[c][0] > 1) {
            prob_dist_t[c][0] = 1;
            for (int g = 1; g < prof_num; g++) {
                prob_dist_t[c][g] = 0;
            }

        } else {
            double total = 0;
            for (int i = 0; i < prof_num; i++) {
                total += prob_dist_t[c][i];
            }
            for (int i = 0; i < prof_num; i++) {
                prob_dist_t[c][i] = prob_dist_t[c][i] / total;
            }
        }
        double z = 0;
        for (int i = 0; i < prof_num; i++) {
            prob_dist_t[c][i] = round(prob_dist_t[c][i]*1000);
            z += prob_dist_t[c][i];
        }
        if (z != 1000)
            prob_dist_t[c][0] += (1000 - z);
    }
    int s = 0;
    int w = -1;
    for (int i = 0; i < 672; i++) {
        for (int j = 0; j < prof_num; j++) {
            for (int k = 0; k < 1000; k++) {
                if (k + s < prob_dist_t[i][j] + s) {
                    res[++w] = speed_t[i] * speed_dist[j];
                }
            }
            s += prob_dist_t[i][j];
        }
    }
    return res;
}

int main() {
    double x[] = {0.9, 0.03, 0.02, 0.05};
    double y[] = {1, 0.75, 0.5, 0.25};
    double speed_t = 130;
    int e = 4;
    int cl = 4;

    double* prof = randpr(x, y, speed_t, e, cl);

    //        for (int i=0;i<672000;i++){
    //
    //                printf("%f ",prof[i]);
    //         
    //            printf(",\r\n ");
    //        }
    //
    //    return 0;

}

