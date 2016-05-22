#include "cachesimulation.h"
#include <iostream>
unsigned int insMis = 0;
unsigned int insHit = 0;
unsigned int rdataMis = 0;
unsigned int rdataHit = 0;
unsigned int wdataMis = 0;
unsigned int wdataHit = 0;
vector<vector<int>> dLRUhelp;
vector<vector<int>> iLRUhelp;
vector<vector<int>> dcache;
vector<vector<int>> icache;
vector<vector<int>> cache;
void cacheSimulation::loadFile(){
    ifstream fin(address);
    int buf1;
    string buf2;
    while(fin >> buf1 && fin >> buf2){
        instructions.push_back(make_pair(buf1, buf2));
    }
    fin.close();
}
unsigned long long stringToInt(string str){
    unsigned long long result = 0;
    for(int i = str.length() - 1; i >= 0; i--){
        if(str[i] >= 'a' && str[i] <= 'f'){
            result += pow(16, str.length() - i - 1) * (str[i] - 'a' + 10);
        }else{
            result += pow(16, str.length() - i - 1) * (str[i] - '0');
        }
    }
    return result;
}
void cacheSimulation::RAND(int index1, int mode){
    int diff = rand() % (int)pow(2, xLD);
    if(mode == 0){
        cache[index1].erase(cache[index1].begin() + diff);
    }else if(mode == 2){
        icache[index1].erase(icache[index1].begin() + diff);
    }else{
        dcache[index1].erase(dcache[index1].begin() + diff);
    }
}

void cacheSimulation::LRU(int index1, int mode){
    if(mode == 0){
        int maxTime = INT_MIN;
        auto itertag2 = LRUhelp[index1].begin();
        for(auto iter2 = LRUhelp[index1].begin(); iter2 != LRUhelp[index1].end(); ++iter2){
            //cout << *iter2;
            if(*iter2 > maxTime){
                maxTime = *iter2;
                itertag2 = iter2;
            }
        }
        //cout << endl;
        int diff = itertag2 - LRUhelp[index1].begin();
        LRUhelp[index1].erase(itertag2);
        auto iter = cache[index1].begin();
        cache[index1].erase(iter + diff);
    }else if(mode == 2){
        int maxTime = INT_MIN;
        auto itertag2 = iLRUhelp[index1].begin();
        for(auto iter2 = iLRUhelp[index1].begin(); iter2 != iLRUhelp[index1].end(); ++iter2){
            //cout << *iter2;
            if(*iter2 > maxTime){
                maxTime = *iter2;
                itertag2 = iter2;
            }
        }
        //cout << endl;
        int diff = itertag2 - iLRUhelp[index1].begin();
        iLRUhelp[index1].erase(itertag2);
        auto iter = icache[index1].begin();
        icache[index1].erase(iter + diff);
    }else if(mode == 1){
        int maxTime = INT_MIN;
        auto itertag2 = dLRUhelp[index1].begin();
        for(auto iter2 = dLRUhelp[index1].begin(); iter2 != dLRUhelp[index1].end(); ++iter2){
            //cout << *iter2;
            if(*iter2 > maxTime){
                maxTime = *iter2;
                itertag2 = iter2;
            }
        }
        //cout << endl;
        int diff = itertag2 - dLRUhelp[index1].begin();
        dLRUhelp[index1].erase(itertag2);
        auto iter = dcache[index1].begin();
        dcache[index1].erase(iter + diff);
    }
    //cout << "删除的是第" << diff << "个" << endl;
}
void cacheSimulation::FIFO(int index1, int mode){
    if(mode == 0){
        cache[index1].erase(cache[index1].begin());
    }else if(mode == 2){
        icache[index1].erase(icache[index1].begin());
    }else{
        dcache[index1].erase(dcache[index1].begin());
    }
}
void cacheSimulation::singleStep(int index){
    result.clear();
    if(cache.size() == 0){
        cache.resize(unifiedCacheSize * 1024 / (blockSize * pow(2, xLD)));
        LRUhelp.resize(cache.size());
    }
    string str = get<1>(instructions[index]);
    int index1 = (stringToInt(str) / blockSize) % cache.size();     //固定的 block number 的都在同一个index1下
    auto f = find(cache[index1].begin(), cache[index1].end(), stringToInt(str) / blockSize);
    if(f == cache[index1].end()){           //没找到
        if(cache[index1].size() < pow(2, xLD)){         //该组cache没满，直接push
            //cout << "a";
            cache[index1].push_back(stringToInt(str) / blockSize);
            if(tHCL == 0){
                LRUhelp[index1].push_back(0);
                for(int i = 0; i < LRUhelp[index1].size(); i++){
                    LRUhelp[index1][i]++;
                }
            }
        }else{
            if(tHCL == 0){  //该组cache满了，删一个最不常使用的
                LRU(index1, 0);
            }else if(tHCL == 1){
                FIFO(index1, 0);
            }else if(tHCL == 2){
                RAND(index1, 0);
            }
            cache[index1].push_back(stringToInt(str) / blockSize);
            if(tHCL == 0){
                LRUhelp[index1].push_back(0);
                for(int i = 0; i < LRUhelp[index1].size(); i++){
                    LRUhelp[index1][i]++;
                }
            }
        }
        if(get<0>(instructions[index]) == 2){
            insMis++;
        }else if(get<0>(instructions[index]) == 1){
            wdataMis++;
        }else if(get<0>(instructions[index]) == 0){
            rdataMis++;
        }
        miss++;
        //cout << "miss" << endl;
    }else{  //找到了
        hit++;
        if(get<0>(instructions[index]) == 2){
            insHit++;
        }else if(get<0>(instructions[index]) == 1){
            wdataHit++;
        }else if(get<0>(instructions[index]) == 0){
            rdataHit++;
        }
        if(tHCL == 0){
            auto iter = LRUhelp[index1].begin();
            *(iter + (f - cache[index1].begin())) = 0;
            for(int i = 0; i < LRUhelp[index1].size(); i++){
                LRUhelp[index1][i]++;
            }
        }
        //cout << "hit" << endl;
    }
    result.push_back(hit);
    result.push_back(miss);
    result.push_back(insHit);
    result.push_back(insMis);
    result.push_back(rdataHit);
    result.push_back(rdataMis);
    result.push_back(wdataHit);
    result.push_back(wdataMis);
}
void cacheSimulation::singleStepMode2(int index){
    result.clear();
    //cout << "mode2" << endl;®
    if(dcache.size() == 0 && icache.size() == 0){
        dcache.resize(dataCacheSize * 1024 / (blockSize * pow(2, xLD)));
        icache.resize(insCacheSize * 1024 / (blockSize * pow(2, xLD)));
        dLRUhelp.resize(dcache.size());
        iLRUhelp.resize(icache.size());
    }

    string str = get<1>(instructions[index]);
    int index1 = 0;
    if(get<0>(instructions[index]) == 2){
        index1 = (stringToInt(str) / blockSize) % icache.size();     //固定的 block number 的都在同一个index1下
        auto f = find(icache[index1].begin(), icache[index1].end(), stringToInt(str) / blockSize);
        if(f == icache[index1].end()){           //没找到
            if(icache[index1].size() < pow(2, xLD)){         //该组cache没满，直接push
                //cout << "a";
                icache[index1].push_back(stringToInt(str) / blockSize);
                if(tHCL == 0){
                    iLRUhelp[index1].push_back(0);
                    for(int i = 0; i < iLRUhelp[index1].size(); i++){
                        iLRUhelp[index1][i]++;
                    }
                }
            }else{
                if(tHCL == 0){  //该组cache满了，删一个最不常使用的
                    LRU(index1,2);
                }else if(tHCL == 1){
                    FIFO(index1,2);
                }else if(tHCL == 2){
                    RAND(index1,2);
                }
                icache[index1].push_back(stringToInt(str) / blockSize);
                if(tHCL == 0){
                    iLRUhelp[index1].push_back(0);
                    for(int i = 0; i < iLRUhelp[index1].size(); i++){
                        iLRUhelp[index1][i]++;
                    }
                }
            }
            insMis++;
            miss++;
            //cout << "miss" << endl;
        }else{  //找到了
            hit++;
            insHit++;
            if(tHCL == 0){
                auto iter = iLRUhelp[index1].begin();
                *(iter + (f - icache[index1].begin())) = 0;
                for(int i = 0; i < iLRUhelp[index1].size(); i++){
                    iLRUhelp[index1][i]++;
                }
            }
            //cout << "hit" << endl;
        }
    }else{
        index1 = (stringToInt(str) / blockSize) % dcache.size();     //固定的 block number 的都在同一个index1下
        auto f = find(dcache[index1].begin(), dcache[index1].end(), stringToInt(str) / blockSize);
        if(f == dcache[index1].end()){           //没找到
            if(dcache[index1].size() < pow(2, xLD)){         //该组cache没满，直接push
                //cout << "a";
                dcache[index1].push_back(stringToInt(str) / blockSize);
                if(tHCL == 0){
                    dLRUhelp[index1].push_back(0);
                    for(int i = 0; i < dLRUhelp[index1].size(); i++){
                        dLRUhelp[index1][i]++;
                    }
                }
            }else{
                if(tHCL == 0){  //该组cache满了，删一个最不常使用的
                    LRU(index1, 1);
                }else if(tHCL == 1){
                    FIFO(index1, 1);
                }else if(tHCL == 2){
                    RAND(index1, 1);
                }
                dcache[index1].push_back(stringToInt(str) / blockSize);
                if(tHCL == 0){
                    dLRUhelp[index1].push_back(0);
                    for(int i = 0; i < dLRUhelp[index1].size(); i++){
                        dLRUhelp[index1][i]++;
                    }
                }
            }
            if(get<0>(instructions[index]) == 1){
                wdataMis++;
            }else if(get<0>(instructions[index]) == 0){
                rdataMis++;
            }
            miss++;
            //cout << "miss" << endl;
        }else{  //找到了
            hit++;
            if(get<0>(instructions[index]) == 1){
                wdataHit++;
            }else if(get<0>(instructions[index]) == 0){
                rdataHit++;
            }
            if(tHCL == 0){
                auto iter = dLRUhelp[index1].begin();
                *(iter + (f - dcache[index1].begin())) = 0;
                for(int i = 0; i < dLRUhelp[index1].size(); i++){
                    dLRUhelp[index1][i]++;
                }
            }
            //cout << "hit" << endl;
        }
    }

    result.push_back(hit);
    result.push_back(miss);
    result.push_back(insHit);
    result.push_back(insMis);
    result.push_back(rdataHit);
    result.push_back(rdataMis);
    result.push_back(wdataHit);
    result.push_back(wdataMis);
}

double cacheSimulation::runAlong(){
    //cout << "替换策略: " << tHCL << endl;
    if(mode == 0){
        cache.resize(unifiedCacheSize * 1024 / (blockSize * pow(2, xLD)));
        LRUhelp.resize(cache.size());
        for(int i = 0; i < instructions.size(); i++){
            singleStep(i);
        }
    }else if(mode == 1){
        dcache.resize(dataCacheSize * 1024 / (blockSize * pow(2, xLD)));
        dLRUhelp.resize(dcache.size());
        icache.resize(insCacheSize * 1024 / (blockSize * pow(2, xLD)));
        iLRUhelp.resize(icache.size());
        for(int i = 0; i < instructions.size(); i++){
            singleStepMode2(i);
        }
    }
    cout << "访问总次数：" << (hit + miss) << endl;
    cout << "miss :" << miss << " hit: " << hit << " 不命中率: " << (double)miss / (hit + miss) << endl;   
    double tag = (double)miss / (hit + miss);
    insMis = insHit = rdataHit = rdataMis = wdataHit = wdataMis = 0;
    cache.clear();
    dcache.clear();
    icache.clear();
    iLRUhelp.clear();
    dLRUhelp.clear();
    return tag;
}

