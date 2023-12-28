# DataStructureLibrary
this warehouse contains some useful data structures unavailable in STL. All of them are implemented by C++ generic programming. 

below are data structures which have been implemented and tested for availability: 
1. IntervalHeap  # It is an extension of the general heap structure. It can query both maximum and minimum in O(lgn).
2. SplaySet # Its function is similar to set in stl. However, it is based on SplayTree which can be extended to more functions. Besides, it is recommended to use RecycleAllocator as his allocator. Detailed reasons can be found in the comments of RecycleAllocator.
3. SplayMap # Its function is similar to map in stl.
