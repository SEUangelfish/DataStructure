#pragma once
#include "pch.h"
#include "Macro.h"

namespace dsl {

#ifdef CACHE_POOL_OPTIMIZATION
	// 缓存池
	static char pbuf[CACHE_POOL_SIZE]{};
#endif // CACHE_POOL_OPTIMIZATION

	// 数据深拷贝
	// src		源数据地址
	// cnt		源数据个数
	// dst		目标地址
	// cap		目标地址最大容量
	template<class _Ty>
	inline void Memcpy(void* src, size_t cnt, void* dst, size_t cap) {
		// 基本类型直接拷贝
		// 自定类型逐个构造
		if (std::is_class_v<_Ty>) {
			_Ty* p = (_Ty*)src;
			_Ty* q = (_Ty*)dst;
			size_t n = std::min(cnt, cap);
			for (size_t i = 0; i < n; i++) new(&q[i]) _Ty(p[i]);
		}
		else memcpy_s(dst, sizeof(_Ty) * cap, src, sizeof(_Ty) * cnt);
	}

	// 快速换值，直接交换二进制流
	inline void Swap(void* p, void* q, size_t bits) {
#ifdef CACHE_POOL_OPTIMIZATION
		if (bits < CACHE_POOL_SIZE) {
			memcpy(pbuf, p, bits);
			memcpy(p, q, bits);
			memcpy(q, pbuf, bits);
		}
		else {
#endif // CACHE_POOL_OPTIMIZATION
			void* buf = malloc(bits);
			if (!buf) throw std::bad_alloc();
			memcpy(buf, p, bits);
			memcpy(p, q, bits);
			memcpy(q, buf, bits);
			free(buf);
#ifdef CACHE_POOL_OPTIMIZATION
		}
#endif // CACHE_POOL_OPTIMIZATION
	}
}