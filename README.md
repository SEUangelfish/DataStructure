# DataStructureLibrary
this warehouse contains some useful data structures unavailable in STL. All of them are implemented by C++ generic programming. 

below are data structures which have been implemented and tested for availability: 
1. IntervalHeap  # It is an extension of the general heap structure. It can query both maximum and minimum in O(lgn).
2. SplaySet # Its function is similar to set in stl. However, it is based on SplayTree which performs better in memory saving and can be extended to more functions.
            # It is recommended to use RecycleAllocator as his allocator.
