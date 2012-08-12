
#pragma once

#include "ofMain.h"
#include "BSpline.h"


//--------------------------------------------------------------
//--------------------------------------------------------------


typedef double datum;


typedef struct {
    
    int M;
    float xmin;
    float xmax;
    float DX;
    float mean;
    vector < float > coefficients;
    int origDataSize;
    
    
} splineInfo;


class splineConverter {
    
public: 

    splineInfo splineFromData(vector < float > data, int nCoefs);
    vector < ofPoint > dataFromSpline( splineInfo & info, int nPts = 0);    // 0 = orig number of pts. 
    typedef BSpline<datum> SplineT;

    
};


