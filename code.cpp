#include <bits/stdc++.h>
using namespace std;

struct Student {
    string name;
    char gender; // 'M' or 'F'
    int cls;
    int score[9];
};

static inline int avg_score(const Student &s){
    long long sum = 0;
    for(int i=0;i<9;i++) sum += s.score[i];
    return (int)(sum/9);
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    unordered_map<string, Student> stu;
    stu.reserve(20000);
    vector<string> leaderboard; // current order of names
    unordered_map<string,int> rank_pos; // name -> 1-based rank
    rank_pos.reserve(20000);

    bool started = false;

    string cmd;
    while (cin >> cmd){
        if (cmd == "ADD"){
            string name; string genderStr; int cls; int sc[9];
            cin >> name >> genderStr >> cls;
            for(int i=0;i<9;i++) cin >> sc[i];
            if (started){
                cout << "[Error]Cannot add student now." << '\n';
                continue;
            }
            if (stu.find(name) != stu.end()){
                cout << "[Error]Add failed." << '\n';
                continue;
            }
            Student s; s.name = name; s.gender = genderStr.empty()? 'M' : genderStr[0]; s.cls = cls;
            for(int i=0;i<9;i++) s.score[i]=sc[i];
            stu.emplace(name, std::move(s));
        } else if (cmd == "START"){
            if (!started){
                started = true;
                // build initial leaderboard
                leaderboard.clear();
                leaderboard.reserve(stu.size());
                for (auto &p : stu) leaderboard.push_back(p.first);
                auto cmp = [&](const string &a, const string &b){
                    const Student &sa = stu[a];
                    const Student &sb = stu[b];
                    int aa = avg_score(sa), bb = avg_score(sb);
                    if (aa != bb) return aa > bb;
                    for(int i=0;i<9;i++){
                        if (sa.score[i] != sb.score[i]) return sa.score[i] > sb.score[i];
                    }
                    if (sa.name != sb.name) return sa.name < sb.name;
                    return false;
                };
                sort(leaderboard.begin(), leaderboard.end(), cmp);
                rank_pos.clear();
                for (size_t i=0;i<leaderboard.size();++i) rank_pos[leaderboard[i]] = (int)i+1;
            } else {
                // Problem guarantees START appears once; ignore if repeated
            }
        } else if (cmd == "UPDATE"){
            string name; int code, score;
            cin >> name >> code >> score;
            auto it = stu.find(name);
            if (it == stu.end()){
                cout << "[Error]Update failed." << '\n';
                continue;
            }
            if (code>=0 && code<9){
                it->second.score[code] = score;
            }
        } else if (cmd == "FLUSH"){
            // rebuild leaderboard based on current scores
            leaderboard.clear();
            leaderboard.reserve(stu.size());
            for (auto &p : stu) leaderboard.push_back(p.first);
            auto cmp = [&](const string &a, const string &b){
                const Student &sa = stu[a];
                const Student &sb = stu[b];
                int aa = avg_score(sa), bb = avg_score(sb);
                if (aa != bb) return aa > bb;
                for(int i=0;i<9;i++){
                    if (sa.score[i] != sb.score[i]) return sa.score[i] > sb.score[i];
                }
                if (sa.name != sb.name) return sa.name < sb.name;
                return false;
            };
            sort(leaderboard.begin(), leaderboard.end(), cmp);
            rank_pos.clear();
            for (size_t i=0;i<leaderboard.size();++i) rank_pos[leaderboard[i]] = (int)i+1;
        } else if (cmd == "PRINTLIST"){
            // print in current leaderboard order; recompute avg on the fly using current data
            for (const string &name : leaderboard){
                const auto &s = stu[name];
                int av = avg_score(s);
                cout << rank_pos[name] << ' ' << s.name << ' ' << (s.gender=='M'?"male":"female") << ' ' << s.cls << ' ' << av << '\n';
            }
        } else if (cmd == "QUERY"){
            string name; cin >> name;
            auto it = stu.find(name);
            if (it == stu.end()){
                cout << "[Error]Query failed." << '\n';
                continue;
            }
            auto rp = rank_pos.find(name);
            if (rp == rank_pos.end()){
                // If never flushed/started yet but QUERY appears? Problem setup suggests START before other ops.
                // Safeguard: compute position by building current leaderboard quickly.
                vector<string> tmp;
                tmp.reserve(stu.size());
                for (auto &p : stu) tmp.push_back(p.first);
                auto cmp = [&](const string &a, const string &b){
                    const Student &sa = stu[a];
                    const Student &sb = stu[b];
                    int aa = avg_score(sa), bb = avg_score(sb);
                    if (aa != bb) return aa > bb;
                    for(int i=0;i<9;i++){
                        if (sa.score[i] != sb.score[i]) return sa.score[i] > sb.score[i];
                    }
                    if (sa.name != sb.name) return sa.name < sb.name;
                    return false;
                };
                sort(tmp.begin(), tmp.end(), cmp);
                int pos = 0;
                for (size_t i=0;i<tmp.size();++i){ if (tmp[i]==name){ pos=(int)i+1; break; } }
                if (pos==0) {
                    cout << "[Error]Query failed." << '\n';
                } else {
                    cout << "STUDENT " << name << " NOW AT RANKING " << pos << '\n';
                }
            } else {
                cout << "STUDENT " << name << " NOW AT RANKING " << rp->second << '\n';
            }
        } else if (cmd == "END"){
            break;
        } else {
            // Unknown command: ignore line tokens? We'll consume rest of line
        }
    }
    return 0;
}
