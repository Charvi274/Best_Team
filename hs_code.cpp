#include <bits/stdc++.h>
using namespace std;

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, M;
    cin >> N >> M;

    vector<long long> rating(N + 1);
    for(int i = 1; i <= N; i++) cin >> rating[i];

    vector<unordered_set<int>> conflicts(N + 1);
    for(int i = 0; i < M; i++){
        int u, v;
        cin >> u >> v;
        conflicts[u].insert(v);
        conflicts[v].insert(u);
    }

    // greedy
    vector<int> order(N);
    iota(order.begin(), order.end(), 1);
    sort(order.begin(), order.end(), [&](int a, int b){
        double ra = (double)rating[a] / (conflicts[a].size() + 1);
        double rb = (double)rating[b] / (conflicts[b].size() + 1);
        return ra > rb;
    });

    vector<bool> selected(N + 1, false);
    vector<bool> blocked(N + 1, false);
    long long totalScore = 0;

    for(int u : order){
        if(!blocked[u]){
            selected[u] = true;
            totalScore += rating[u];
            for(int nb : conflicts[u]) blocked[nb] = true;
        }
    }

    // local search
    auto start = chrono::steady_clock::now();
    auto timeElapsed = [&]() -> double {
        return chrono::duration<double>(chrono::steady_clock::now() - start).count();
    };

    mt19937 rng(42);
    bool improved = true;

    while(improved && timeElapsed() < 280.0){
        improved = false;

        vector<int> teamList;
        teamList.reserve(N);
        for(int i = 1; i <= N; i++) if(selected[i]) teamList.push_back(i);
        shuffle(teamList.begin(), teamList.end(), rng);

        for(int u : teamList){
            if(timeElapsed() > 280.0) break;

            unordered_set<int> candidates;
            for(int nb : conflicts[u]){
                if(!selected[nb]){
                    bool ok = true;
                    for(int nb2 : conflicts[nb]){
                        if(nb2 != u && selected[nb2]){ ok = false; break; }
                    }
                    if(ok) candidates.insert(nb);
                }
            }

            vector<int> clist(candidates.begin(), candidates.end());
            if(clist.size() > 30) clist.resize(30);

            for(int i = 0; i < (int)clist.size(); i++){
                for(int j = i + 1; j < (int)clist.size(); j++){
                    int a = clist[i], b = clist[j];
                    if(conflicts[a].count(b)) continue;
                    long long gain = rating[a] + rating[b] - rating[u];
                    if(gain > 0){
                        selected[u] = false;
                        selected[a] = true;
                        selected[b] = true;
                        totalScore += gain;
                        improved = true;
                        goto next;
                    }
                }
            }

            {
                int best = -1;
                long long bestGain = 0;
                for(int nb : conflicts[u]){
                    if(selected[nb]) continue;
                    long long g = rating[nb] - rating[u];
                    if(g > bestGain){
                        bool ok = true;
                        for(int nb2 : conflicts[nb]){
                            if(nb2 != u && selected[nb2]){ ok = false; break; }
                        }
                        if(ok){ bestGain = g; best = nb; }
                    }
                }
                if(best != -1){
                    selected[u] = false;
                    selected[best] = true;
                    totalScore += bestGain;
                    improved = true;
                }
            }

            next:;
        }
    }

    cout << totalScore << "\n";
    bool first = true;
    for(int i = 1; i <= N; i++){
        if(selected[i]){
            if(!first) cout << " ";
            cout << i;
            first = false;
        }
    }
    cout << "\n";

    return 0;
}
