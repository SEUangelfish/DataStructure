#pragma once
#include "pch.h"

// allocators of DSL container must have New() and Free() functions
// the parameter list and return value must be the same as Allocator<_Ty>
// the reason for not using polymorphism is to increase efficiency

namespace dsl {

	template<typename _Ty>
	class Allocator {
	public:
		// resources request function
		// no initialization operation is performed
		// cnt: number of elements
		// return: heap resourse pointer of type _Ty
		_Ty* New(size_t cnt) {
			_Ty* res = (_Ty*)operator new(cnt * sizeof(_Ty));
#ifdef EXCEPTION_DETECTION
			if (!res) throw std::bad_alloc();
#endif // EXCEPTION_DETECTION
			return res;
		}

		// release resources
		// class types are destructed
		// size: number of elements to be destructed
		void Free(_Ty* src, size_t size) {
			if constexpr (std::is_class<_Ty>::value) for (_Ty* st = src, *ed = src + size; st != ed; ++st) st->~_Ty();
			operator delete(src);
		}
	};

	// this allocator stores discarded memory and prioritizes reclaimed resources when requesting resources
	// all resources are released when the allocator is destroyed
	// tip: This allocator is suitable for chaom containers
	//		The number of elements allocated and destroyed each time should be as 1 as possible, otherwise it will cause memory waste 
	// tip: An individual element size should be greater than or equal to the size of one pointer
	//		otherwise it cannot be optimized, equivalent to Allocator<_Ty>
	template<typename _Ty>
	class RecycleAllocator {
		union Block {
			_Ty null;
			Block* next;
		};

	public:
		RecycleAllocator() = default;
		RecycleAllocator(const RecycleAllocator& cp) = delete;
		RecycleAllocator(RecycleAllocator&& mv) noexcept :head(mv.head) {
			memset(&mv, 0, sizeof(RecycleAllocator));
		}

		RecycleAllocator& operator=(const RecycleAllocator& cp) = delete;
		RecycleAllocator& operator=(RecycleAllocator&& cp) noexcept {
			this->~RecycleAllocator();
			new (this) RecycleAllocator(std::move(cp));
			return *this;
		};

		~RecycleAllocator() {
			if constexpr (sizeof(_Ty) < sizeof(_Ty*)) return;
			void* tmp;
			while (this->head) {
				tmp = this->head;
				this->head = this->head->next;
				operator delete(tmp);
			}
		}

		// resources request function
		// no initialization operation is performed
		// cnt(as 1 as possible): number of elements
		// return: heap resourse pointer of type _Ty
		_Ty* New(size_t cnt) {
			if constexpr (sizeof(_Ty) >= sizeof(_Ty*)) {
				if (cnt == 1 && this->head) {
					_Ty* res = (_Ty*)this->head;
					this->head = this->head->next;
					return res;
				}
			}
			_Ty* res = (_Ty*)operator new(cnt * sizeof(_Ty));
#ifdef EXCEPTION_DETECTION
			if (!res) throw std::bad_alloc();
#endif // EXCEPTION_DETECTION
			return res;
		}

		// release resources
		// class types are destructed
		// size: number of elements to be destructed
		void Free(_Ty* src, size_t size) {
			if constexpr (std::is_class<_Ty>::value)  for (_Ty* st = src, *ed = src + size; st != ed; ++st) st->~_Ty();
			if constexpr (sizeof(_Ty) >= sizeof(_Ty*)) {
				Block* tmp = (Block*)src;
				tmp->next = this->head;
				this->head = tmp;
			}
			else operator delete(src);
		}

	protected:
		Block* head = nullptr;
	};


}