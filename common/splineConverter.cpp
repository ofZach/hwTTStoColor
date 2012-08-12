//
//  splineConverter.cpp
//  ttsColor
//
//  Created by molmol on 8/7/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//


#include "splineConverter.h"




const double BoundaryConditions[3][4] =
{
    //  0       1       M-1     M
    {
        -4,
        -1,
        -1,
        -4 },
    {
        0,
        1,
        1,
        0 },
    {
        2,
        -1,
        -1,
        2 } };


inline double Beta(int m, int M)
{
    if (m > 1 && m < M-1)
        return 0.0;
    if (m >= M-1)
        m -= M-3;
    return BoundaryConditions[0][m];
}

double Basis(int m, int M, float x, float xmin, float DX){
    double y = 0;
    double xm = xmin + (m * DX);
    double z = abs((double)(x - xm) / (double)DX);
    if (z < 2.0) {
        z = 2 - z;
        y = 0.25 * (z*z*z);
        z -= 1.0;
        if (z > 0)
            y -= (z*z*z);
    }
    
    // Boundary conditions, if any, are an additional addend.
    if (m == 0 || m == 1)
        y += Beta(m, M) * Basis(-1, M, x, xmin, DX);
    else if (m == M-1 || m == M)
        y += Beta(m, M) * Basis(M+1, M, x, xmin, DX);
    
    return y;
}

float evaluate(float x, float * coeficients, float xmin, float DX, int M, float mean) {
    float y = 0;
    if (true) {
        int n = (int)((x - xmin)/DX);
        
		// M ? 
		// s-A[] ?
		
        for (int i = max(0, n-1); i <= min(M, n+2); ++i) {
            y += coeficients[i] * Basis(i, M, x, xmin, DX);
        }
        y += mean;
    }
    return y;
}

splineInfo splineConverter::splineFromData(vector < float > data, int nCoefs){
    
    
    vector<datum> xVals;
    vector<datum> yVals;
    
    for (int i = 0; i < data.size(); i++){
        xVals.push_back(i);
        yVals.push_back(data[i]);
    }
    
    SplineT::Debug(0);
    SplineT spline(&xVals[0],
                   xVals.size(),
                   &yVals[0],
                   0,
                   0,
                   nCoefs);
    splineInfo info;
    
    for (int i = 0; i <= spline.M; i++){
        //printf("%i = %f \n", i, spline.coefficient(i));
        info.coefficients.push_back(spline.coefficient(i));
        
    }
    
    info.M = spline.M;
    info.DX = spline.DX;
    info.xmin = spline.xmin;
    info.xmax = spline.xmax;
    info.mean = spline.mean;
    info.origDataSize = data.size();
    
    
    return info;
}

vector < ofPoint > splineConverter::dataFromSpline( splineInfo & info, int nPts){
    
    vector < ofPoint> ptsResult;
    
    if (nPts == 0) nPts = info.origDataSize;
    
    for (int i = 0; i < nPts; i++){
        int which = (int)(info.origDataSize * ((float)(i) / (float)(nPts-1)));
        float y = evaluate(which, &info.coefficients[0], info.xmin, info.DX, info.M, 0); //info.mean);   // not really sure about the mean ?
        ptsResult.push_back(ofPoint(which,y));
       
    }
    //ofEndShape();
    return ptsResult;
    
}


