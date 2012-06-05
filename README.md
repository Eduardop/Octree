Octree
======

Octree library in C++.

Contained entirely in a header file.

Usage:
------

    // Create class that holds your data.
    class Node
    {
        ...
    };

    // Initialize octree.
    // cellSize is the minimum subdivided cell size.
    Octree<Node> octree(min, max, cellSize);

    // Add data to the octree.
    // Data will be added to the leaves.
    for (int i = 0; i < SIZE; i++)
    {
        // getCell returns the octree cell (of size
        // at most cellSize) that contains the passed
        // position.
        Node& n = octree.getCell(position[i]);
        // Set your data in the cell
        // e.g. add something to a std::vector, etc
        n. ... = ...;
    }
    
    // Traverse octree
    // Create callback class
    class Callback: public Octree<Node>::Callback
    {
        ... // state
        // Callback functor. Return value: true = continue; false = abort.
        virtual bool operator()(const float min[3], const float max[3], N& nodeData)
        {
            ...
        }
    };
    Callback cb;
    octree.traverse(cb);

See sample.cpp for a more extensive sample.

