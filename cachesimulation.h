#ifndef CACHESIMULATION_H
#define CACHESIMULATION_H
#include <string>
#include <cmath>
#include <vector>
#include <fstream>
using namespace std;
class cacheSimulation
{
public:
    unsigned int insMis = 0;
    unsigned int insHit = 0;
    unsigned int rdataMis = 0;
    unsigned int rdataHit = 0;
    unsigned int wdataMis = 0;
    unsigned int wdataHit = 0;
    int mode;
    int unifiedCacheSize;
    int dataCacheSize;
    int insCacheSize;
    int blockSize;
    int xLD;
    int tHCL;
    int yQCL;
    int xCL;
    int dKCL;
    unsigned int miss;
    unsigned int hit;
    string address;

    vector<vector<int>> LRUhelp;
    vector<pair<int, string>> instructions;
    void LRU(int, int);
    void FIFO(int, int);
    void RAND(int, int);
    void loadFile();
    vector<double> result;
public:
    cacheSimulation(int mode, int unifiedCacheSize, int dataCacheSize, int insCacheSize, int blockSize, int xLD,
                                     int tHCL, int yQCL, int xCL, int dKCL, string address) : mode(mode), unifiedCacheSize(unifiedCacheSize),
                                        dataCacheSize(dataCacheSize), insCacheSize(insCacheSize), blockSize(blockSize), xLD(xLD),
                                        tHCL(tHCL), yQCL(yQCL), xCL(xCL), dKCL(dKCL), address(address){
        miss = 0;
        hit = 0;
        loadFile();
    }
    ~cacheSimulation(){
    }

    void singleStep(int);
    void singleStepMode2(int);
    double runAlong();

};

#endif // CACHESIMULATION_H
