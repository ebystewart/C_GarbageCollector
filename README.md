# C_GarbageCollector
A Garbage Collector application in C for detecting Memory Leaks
 - uses Depth-First Search (DFS algorithm) on object database (Directed Cyclic Graphs)
 - needs registration of all structures and its elements (Disjoint sets) including lists, graphs, trees, etc....
    - head nodes should be explicitly mentioned
 - needs registration of all objects created out of the structures in heap
 - the structure registration constitutes the structure database 
 - the object registration constitutes the object database to form disjoint sets of graph
 - a DFS search on the object database (child object first - reached and flagged as reachable) and all unreachable nodes are flagged as leaked objects

