// divideandconquer algorithm 
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <vector>
using namespace std;
using u16 = uint16_t;

static u16 *team_a, *team_b;
static size_t games_per_round;  
static long long games_written;  

void schedule(int first, int n, int slot) {
    if (n == 2) {                                
        team_a[slot] = first;
        team_b[slot] = first + 1;
        games_written++;
        return;
    }
    int half = n / 2;
    schedule(first, half, slot);                    
    schedule(first + half, half, slot + half / 2);  

    for (int r = 0; r < half; r++) {               
        size_t base = (size_t)(half - 1 + r) * games_per_round + slot;
        for (int i = 0; i < half; i++) {
            team_a[base + i] = first + i;
            team_b[base + i] = first + half + (i + r) % half;
        }
        games_written += half;
    }
}

bool schedule_is_valid(int teams) {
    vector<char> met((size_t)teams * teams, 0);
    for (int r = 0; r < teams - 1; r++) {
        vector<int> plays(teams, 0);
        for (int s = 0; s < teams / 2; s++) {
            int a = team_a[r * games_per_round + s];
            int b = team_b[r * games_per_round + s];
            plays[a]++;
            plays[b]++;
            size_t pair = (size_t)min(a, b) * teams + max(a, b);
            if (met[pair]) return false;
            met[pair] = 1;
        }
        for (int t = 0; t < teams; t++)
            if (plays[t] != 1) return false;
    }
    return true;
}

int main() {
    vector<u16> table_a, table_b;
    auto prepare = [&](int teams) {
        games_per_round = teams / 2;
        table_a.assign((teams - 1) * games_per_round, 0);
        table_b.assign((teams - 1) * games_per_round, 0);
        team_a = table_a.data();
        team_b = table_b.data();
    };

    for (int k = 1; k <= 9; k++) {
        int teams = 1 << k;
        prepare(teams);
        games_written = 0;
        schedule(0, teams, 0);
        if (!schedule_is_valid(teams) || games_written != (long long)teams * (teams - 1) / 2) {
            printf("INVALID for n=%d\n", teams);
            return 1;
        }
    }
    printf("Schedule valid for n = 2 .. 512\n");

    vector<double> sizes, times;
    volatile u16 keep = 0;  
    for (int k = 4; k <= 13; k++) {
        int teams = 1 << k;
        prepare(teams);
        long long repeats = max(1LL, 200000000LL / ((long long)teams * teams));
        vector<double> windows;
        for (int w = 0; w < 7; w++) {
            games_written = 0;
            auto start = chrono::steady_clock::now();
            for (long long i = 0; i < repeats; i++) schedule(0, teams, 0);
            auto stop = chrono::steady_clock::now();
            keep = keep + team_a[0];
            windows.push_back(chrono::duration<double>(stop - start).count() / repeats);
        }
        sort(windows.begin(), windows.end());
        double median = windows[3];
        printf("n=%5d  time=%.6f s  time/n^2=%.3e  games=%lld (expected %lld)\n",
               teams, median, median / ((double)teams * teams),
               games_written / repeats, (long long)teams * (teams - 1) / 2);
        sizes.push_back(teams);
        times.push_back(median);
    }

    int count = sizes.size();
    double log_sum = 0;
    for (int i = count - 3; i < count; i++)
        log_sum += log(times[i] / (sizes[i] * sizes[i]));
    double c = exp(log_sum / 3);

    FILE* f = fopen("dc_runtime.csv", "w");
    fprintf(f, "n,seconds,ms,norm,ref\n");  
    for (int i = 0; i < count; i++)
        fprintf(f, "%d,%.9f,%.6f,%.4f,%.9f\n", (int)sizes[i], times[i], times[i] * 1e3,
                times[i] / (sizes[i] * sizes[i]) * 1e9, c * sizes[i] * sizes[i]);
    fclose(f);

    double sx = 0, sy = 0, sxx = 0, sxy = 0;
    for (int i = 0; i < count; i++) {
        double x = log(sizes[i]), y = log(times[i]);
        sx += x; sy += y; sxx += x * x; sxy += x * y;
    }
    printf("least-squares log-log slope = %.3f\n",
           (count * sxy - sx * sy) / (count * sxx - sx * sx));
    for (int i = 1; i < count; i++)
        printf("local slope %5d -> %5d: %.2f\n", (int)sizes[i - 1], (int)sizes[i],
               log(times[i] / times[i - 1]) / log(sizes[i] / sizes[i - 1]));
    printf("c = %.3e\n", c);
}