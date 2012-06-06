//
//  sample.cpp
//  
//
//  Created by Eduardo Poyart on 6/5/12.
//

/*
Copyright (c) 2012, Eduardo Poyart.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the following disclaimer
in the documentation and/or other materials provided with the
distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <iostream>
// The Octree library resides in a single header file.
#include "Octree.h"
#include <vector>
#include <iomanip>
#include <math.h>
#include <stdlib.h>
using namespace std;

// Any 3D vector implementation can be used, as long as it can be converted
// to float[3]. Here we use MyPoint.
struct MyPoint
{
    float x; 
    float y; 
    float z;
    MyPoint(const MyPoint& p2): x(p2.x), y(p2.y), z(p2.z) {}
    MyPoint& operator=(const MyPoint& p2) { x = p2.x; y = p2.y; z = p2.z; return *this;}
    MyPoint(float in_x, float in_y, float in_z): x(in_x), y(in_y), z(in_z) {}
    MyPoint(const float p2[3]): x(p2[0]), y(p2[1]), z(p2[2]) {}
    operator float*() { return &x; }
    operator const float*() const { return &x; }
    MyPoint operator+(const MyPoint& p2) const { return MyPoint(x+p2.x, y+p2.y, z+p2.z); }
    MyPoint operator-(const MyPoint& p2) const { return MyPoint(x-p2.x, y-p2.y, z-p2.z); }
    MyPoint operator*(float f) const { return MyPoint(x*f, y*f, z*f); }
    float lengthsq() { return x*x + y*y + z*z; }
};

// Data that we're manipulating.
struct Particle
{
    MyPoint pos;
    Particle(float x, float y, float z): pos(x, y, z) {}
};

// A node in the quadtree.
struct Node
{
    vector<Particle*> particles;
};

#define NUM_PARTICLES 10000000
#define R 0.01f
#define EPSILON 0.0001f

// This class does the work of counting particles while traversing the octree.
class CallbackTraverse: public Octree<Node>::Callback
{
public:
    int count;
    Particle* p0;
    virtual bool operator()(const float min[3], const float max[3], Node& n)
    {
        MyPoint pmin(min), pmax(max);
        float cellSizeSq = (pmax - pmin).lengthsq();
        float maxDist = (sqrtf(cellSizeSq) * 0.5f) + R + EPSILON;
        
        MyPoint center = (pmin + pmax) * 0.5f;
        MyPoint vectCenter = center - p0->pos;
        float distCenterSq = vectCenter.lengthsq();
        if (distCenterSq > maxDist * maxDist)
            return false; // Too far; don't subdivide cell.
        
        // Iterate through particles in this cell.
        vector<Particle*>::const_iterator it = n.particles.begin();
        for (; it != n.particles.end(); it++)
        {
            Particle* p = *it;
            if (p == p0)
                continue;
            float dsq = (p->pos - p0->pos).lengthsq();
            // If particle is within the radius, increment counter.
            if (dsq <= R * R)
                count++;
        }
        // Subdivide cell if needed.
        return true;
    }
};
                
int main()
{
    cout << "Number of particles: " << NUM_PARTICLES << endl;
    cout << "Initializing particles, uniform distribution in cube ([0,1], [0,1], [0,1])." << endl;
    vector<Particle> myParticles;
    for (int i = 0; i < NUM_PARTICLES; i++)
        myParticles.push_back(Particle(
            (float)rand()/RAND_MAX, (float)rand()/RAND_MAX, (float)rand()/RAND_MAX));

    cout << endl;
    cout << "Count number of particles within a radius of " << R << "." << endl;
    cout << "Output format: 'p: c' where c is the number of particles" << endl;
    cout << "around particle p." << endl << endl;
    
    cout << "Computing result with brute force:" << endl;
    for (int i = 0; i < 120; i++)
    {
        cout << setw(3) << i << ": ";
        int count = 0;
        MyPoint pi(myParticles[i].pos);
        for (int j = 0; j < NUM_PARTICLES; j++)
        {
            if (i == j)
                continue;
            MyPoint pj(myParticles[j].pos);
            float dsq = (pj - pi).lengthsq();
            if (dsq <= R * R)
                count++;
        }
        cout << count << "  ";
        if (i % 8 == 7)
            cout << endl;
    }
    
    cout << endl << "Building octree." << endl;
    // Initialize octree.
    // Minimum coordinates.
    float min[3] = {0.0f, 0.0f, 0.0f};
    // Maximum coordinates.
    float max[3] = {1.0f + EPSILON, 1.0f + EPSILON, 1.0f + EPSILON};
    // Minimum size to use when subdividing cells.
    float cellSize[3] = {0.1, 0.1, 0.1};
    Octree<Node> octree(min, max, cellSize);
    
    // Add all particles to the octree.
    for (int i = 0; i < NUM_PARTICLES; i++)
    {
        Node& n = octree.getCell(myParticles[i].pos);
        n.particles.push_back(&myParticles[i]);
    }
    cout << "Building octree done." << endl << endl;
    
    cout << "Computing result with octree:" << endl;

    for (int i = 0; i < 120; i++)
    {
        cout << setw(3) << i << ": ";
        // Prepare the callback class.
        CallbackTraverse ct;
        ct.count = 0;
        ct.p0 = &myParticles[i];
        // Invoke the traversal.
        octree.traverse(&ct);
        cout << ct.count << "  ";
        if (i % 8 == 7)
            cout << endl;
    }

    return 0;
}
