Octree
======

Octree library in C++.

Contained entirely in a header file.

Usage:
------

```c++
    // Class that holds your data.
    class Node
    {
        ...
    };

    // Initialize octree.
    // cellSize is the minimum subdivided cell size.
    float min[3] = {0.0f, 0.0f, 0.0f};
    float max[3] = {1.0f + EPSILON, 1.0f + EPSILON, 1.0f + EPSILON};
    float cellSize[3] = {0.1, 0.1, 0.1};
    Octree<Node> octree(min, max, cellSize);

    // Add data to the octree.
    // Data will be added to the leaves.
    for (int i = 0; i < NUM_ELEMENTS; i++)
    {
        // getCell returns the octree leaf cell (of size
        // cellSize or larger) that contains the passed
        // position.
        Node& n = octree.getCell(position[i]);
        // Set your data in the cell.
        // e.g. add something to a std::vector, etc.
        n. ... = ...;
    }
    
    // Traverse octree.
    // Create callback class.
    class Callback: public Octree<Node>::Callback
    {
    public:
        // State variables.
        ...
        // Callback functor. 
        // Returns: true = continue and traverse subcells; false = abort branch.
        virtual bool operator()(const float min[3], const float max[3], Node& nodeData)
        {
            ...
        }
    };
    // Invoke traversal.
    Callback cb;
    octree.traverse(&cb);
```

See sample.cpp for a more extensive sample.

