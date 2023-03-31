#pragma once
// 宏定义文件
#define CACHE_POOL_SIZE		128			// 缓存池大小
#define BIG					8192		// 多线程优化阈值(如果开启了多线程优化，大于该值时使用多线程) 
#define CACHE_POOL_OPTIMIZATION			// 开启缓存池优化
#define MULTITHREADING_OPTIMIZATION		// 开启多线程优化