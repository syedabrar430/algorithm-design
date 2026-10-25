// greedy algorithm
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <numeric>
#include <random>
#include <vector>
using namespace std;

struct Load { double wash, dry; };

vector<int> johnson_order(const vector<Load>& loads) {
    vector<int> first, last;
    for (int i = 0; i < (int)loads.size(); i++)
        (loads[i].wash <= loads[i].dry ? first : last).push_back(i);
    sort(first.begin(), first.end(), [&](int a, int b) { return loads[a].wash < loads[b].wash; });
    sort(last.begin(), last.end(), [&](int a, int b) { return loads[a].dry > loads[b].dry; });
    first.insert(first.end(), last.begin(), last.end());
    return first;
}

double finish_time(const vector<Load>& loads, const vector<int>& order) {
    double washer_free = 0, dryer_free = 0;
    for (int i : order) {
        washer_free += loads[i].wash;
        dryer_free = max(dryer_free, washer_free) + loads[i].dry;
    }
    return dryer_free;
}

int main() {
    mt19937_64 rng(12345);
    uniform_real_distribution<double> random_time(0.01, 1.01);
    auto make_loads = [&](int n) {
        vector<Load> loads(n);
        for (auto& l : loads) { l.wash = random_time(rng); l.dry = random_time(rng); }
        return loads;
    };

    uniform_int_distribution<int> small(1, 10);
    for (int test = 0; test < 2000; test++) {
        int n = 1 + rng() % 7;
        vector<Load> loads(n);
        for (auto& l : loads) { l.wash = small(rng); l.dry = small(rng); }
        vector<int> order(n);
        iota(order.begin(), order.end(), 0);
        double best = 1e18;
        do best = min(best, finish_time(loads, order));
        while (next_permutation(order.begin(), order.end()));
        if (finish_time(loads, johnson_order(loads)) != best) { puts("MISMATCH"); return 1; }
    }
    puts("Johnson matches brute force on 2000 random instances");

   
    vector<double> sizes, times;
    volatile int keep = 0;  
    for (int k = 14; k <= 22; k++) {
        int n = 1 << k;
        vector<Load> loads = make_loads(n);
        double fastest = 1e18;
        for (int run = 0; run < 5; run++) {
            auto start = chrono::steady_clock::now();
            vector<int> order = johnson_order(loads);
            auto stop = chrono::steady_clock::now();
            keep = keep + order[0];
            fastest = min(fastest, chrono::duration<double>(stop - start).count());
        }
        printf("n=%8d  time=%.6f s\n", n, fastest);
        sizes.push_back(n);
        times.push_back(fastest);
    }

    double top = 0, bottom = 0;
    for (size_t i = 0; i < sizes.size(); i++) {
        double x = sizes[i] * log2(sizes[i]);
        top += x * times[i];
        bottom += x * x;
    }
    double c = top / bottom;
    double line = times[0] / sizes[0];   

    FILE* f = fopen("greedy_runtime.csv", "w");
    fprintf(f, "n,seconds,norm,ref,lin\n");  
    for (size_t i = 0; i < sizes.size(); i++) {
        double nlogn = sizes[i] * log2(sizes[i]);
        fprintf(f, "%d,%.9f,%.4f,%.9f,%.9f\n", (int)sizes[i], times[i],
                times[i] / nlogn * 1e9, c * nlogn, line * sizes[i]);
    }
    fclose(f);
    printf("c = %.3e\n", c);

    FILE* q = fopen("greedy_quality.csv", "w");
    fprintf(q, "n,johnson_over_lb,random_over_lb\n");
    for (int n : {10, 100, 1000, 10000}) {
        vector<Load> loads = make_loads(n);
        double wash_total = 0, dry_total = 0, min_wash = 1e18, min_dry = 1e18;
        for (auto& l : loads) {
            wash_total += l.wash;  dry_total += l.dry;
            min_wash = min(min_wash, l.wash);  min_dry = min(min_dry, l.dry);
        }
        double lower_bound = max(wash_total + min_dry, dry_total + min_wash);
        vector<int> random_order(n);
        iota(random_order.begin(), random_order.end(), 0);
        shuffle(random_order.begin(), random_order.end(), rng);
        fprintf(q, "%d,%.4f,%.4f\n", n,
                finish_time(loads, johnson_order(loads)) / lower_bound,
                finish_time(loads, random_order) / lower_bound);
    }
    fclose(q);
}