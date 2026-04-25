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

struct RankCmp {
    const unordered_map<string, Student>* mp;
    bool operator()(const string &a, const string &b) const {
        if (a == b) return false;
        const Student &sa = mp->at(a);
        const Student &sb = mp->at(b);
        int aa = avg_score(sa), bb = avg_score(sb);
        if (aa != bb) return aa > bb;
        for(int i=0;i<9;i++){
            if (sa.score[i] != sb.score[i]) return sa.score[i] > sb.score[i];
        }
        if (sa.name != sb.name) return sa.name < sb.name;
        return false;
    }
};

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    unordered_map<string, Student> stu;
    stu.reserve(20000);
    vector<string> leaderboard; // displayed order of names
    unordered_map<string,int> rank_pos; // name -> 1-based rank
    rank_pos.reserve(20000);

    // Maintains up-to-date ranking structure for fast FLUSH
    // Only valid after START
    unique_ptr< set<string, RankCmp> > sorted;

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
                // build ranking set
                RankCmp cmp{&stu};
                sorted = make_unique< set<string, RankCmp> >(cmp);
                for (auto &p : stu) sorted->insert(p.first);
                // initialize displayed leaderboard to current ranking
                leaderboard.clear();
                leaderboard.reserve(stu.size());
                rank_pos.clear();
                int rk=1;
                for (const auto &name : *sorted){
                    leaderboard.push_back(name);
                    rank_pos[name]=rk++;
                }
            } else {
                // START appears once; ignore if repeated
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
                if (started && sorted){
                    // erase using old key before mutation
                    sorted->erase(name);
                    it->second.score[code] = score;
                    sorted->insert(name);
                } else {
                    it->second.score[code] = score;
                }
            }
        } else if (cmd == "FLUSH"){
            // synchronize displayed leaderboard to current ranking
            if (started && sorted){
                leaderboard.clear();
                leaderboard.reserve(sorted->size());
                rank_pos.clear();
                int rk=1;
                for (const auto &name : *sorted){
                    leaderboard.push_back(name);
                    rank_pos[name]=rk++;
                }
            } else {
                // before START, nothing to do
                leaderboard.clear();
                rank_pos.clear();
            }
        } else if (cmd == "PRINTLIST"){
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
                // Should not happen per problem guarantees; fallback to compute on the fly
                RankCmp cmp{&stu};
                set<string, RankCmp> temp(cmp);
                for (auto &p : stu) temp.insert(p.first);
                int pos = 1; int ans = -1;
                for (const auto &nm : temp){ if (nm==name){ ans=pos; break; } ++pos; }
                if (ans==-1) cout << "[Error]Query failed." << '\n';
                else cout << "STUDENT " << name << " NOW AT RANKING " << ans << '\n';
            } else {
                cout << "STUDENT " << name << " NOW AT RANKING " << rp->second << '\n';
            }
        } else if (cmd == "END"){
            break;
        } else {
            // Unknown command: ignore
        }
    }
    return 0;
}
