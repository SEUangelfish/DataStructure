#pragma once
#include "pch.h"

// DSL使用的分配器必须有New和Free函数
// 不采用多态的原因是提高效率

namespace dsl {

	template<typename _Ty>
	class Allocater {
	public:
		// 资源申请函数
		// 不进行任何初始化操作
		// cnt：元素个数
		// 返回资源指针
		_Ty* New(size_t cnt) {
			_Ty* res = (_Ty*)operator new(cnt * sizeof(_Ty));
#ifdef EXCEPTION_DETECTION
			if (!res) throw std::bad_alloc();
#endif // EXCEPTION_DETECTION
			return res;
		}

		// 释放资源
		// 类类型会进行析构操作
		// size：析构的元素个数
		void Free(_Ty* src, size_t size) {
			if constexpr (std::is_class<_Ty>::value) for (_Ty* st = src, *ed = src + size; st != ed; ++st) st->~_Ty();
			operator delete(src);
		}
	};

	// 资源回收分配器
	// 该分配器会将弃置的内存保存起来，申请资源时优先使用回收的资源
	// 分配器析构时将释放所有资源
	// 注意1：该分配器适用于链式容器，每次分配和析构的元素个数尽量为1，否则会造成内存浪费
	// 注意2：单个元素大小必须大于等于指针的大小，否则编译失败
	template<typename _Ty, typename std::enable_if<sizeof(_Ty) >= sizeof(_Ty*)>::type* = nullptr>
	class RecycleAllocater {
		union Block {
			_Ty null;
			Block* next;
		};

	public:
		RecycleAllocater() = default;

		~RecycleAllocater() {
			void* tmp;
			while (this->head) {
				tmp = this->head;
				this->head = this->head->next;
				operator delete(tmp);
			}
		}

		// 资源申请函数
		// 不进行任何初始化操作
		// cnt：元素个数（尽量为1）
		// 返回资源指针
		_Ty* New(size_t cnt) {
			if (cnt == 1 && this->head) {
				_Ty* res = (_Ty*)this->head;
				this->head = this->head->next;
				return res;
			}
			_Ty* res = (_Ty*)operator new(cnt * sizeof(_Ty));
#ifdef EXCEPTION_DETECTION
			if (!res) throw std::bad_alloc();
#endif // EXCEPTION_DETECTION
			return res;
		}

		// 释放资源
		// 类类型会进行析构操作
		// size：析构的元素个数
		void Free(_Ty* src, size_t size) {
			if constexpr (std::is_class<_Ty>::value)  for (_Ty* st = src, *ed = src + size; st != ed; ++st) st->~_Ty();
			Block* head = (Block*)src;
			head->next = this->head;
			this->head = head;
		}

	protected:
		Block* head = nullptr;
	};


}