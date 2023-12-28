#pragma once
#include "pch.h"

// allocators of DSL container must have New() and Free() functions
// the parameter list and return value must be the same as Allocator<_Ty>
// the reason for not using polymorphism is to increase efficiency

namespace dsl {

	template<typename _Ty>
	class Allocator {
	public:
		Allocator() = default;

		virtual ~Allocator() {}

		// resources request function
		// no initialization operation is performed
		// cnt: number of elements
		// return: heap resourse pointer of type _Ty
		virtual _Ty* New(size_t cnt) {
			_Ty* res = (_Ty*)operator new(cnt * sizeof(_Ty));
#ifdef EXCEPTION_DETECTION
			if (!res) throw std::bad_alloc();
#endif // EXCEPTION_DETECTION
			memset(res, 0, sizeof(_Ty) * cnt);
			return res;
		}

		// release resources
		// class types are destructed
		// cnt: number of elements to be destructed
		virtual void Free(_Ty* src, size_t cnt) {
			if constexpr (std::is_class<_Ty>::value) for (size_t i = 0; i < cnt; ++i) src[i].~_Ty();
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
	class RecycleAllocator : public Allocator<_Ty> {
		union Block {
			_Ty null;
			struct{
				int cnt;
				Block* next;
			}link;
		};

	public:
		RecycleAllocator() = default;

		virtual ~RecycleAllocator() {
			void* tmp;
			while (this->head) {
				tmp = this->head;
				this->head = this->head->link.next;
				operator delete(tmp);
			}
		}

		RecycleAllocator(const RecycleAllocator& cp) {
#ifdef WARNING_DETECTION
			std::cerr << "copy constructor of RecycleAllocator: this function doesn't copy reserved memory" << endl;
#endif // WARNING_DETECTION
		};

		RecycleAllocator(RecycleAllocator&& mv) noexcept :head(mv.head) {
			mv.head = nullptr;
		}

		RecycleAllocator& operator=(const RecycleAllocator& cp) = delete;

		RecycleAllocator& operator=(RecycleAllocator&& cp) noexcept {
			this->~RecycleAllocator();
			new (this) RecycleAllocator(std::move(cp));
			return *this;
		};

		// resources request function
		// no initialization operation is performed
		// cnt: number of elements
		// return: heap resourse pointer of type _Ty
		virtual _Ty* New(size_t cnt) override {
			Block* pre = nullptr, * cur = this->head;
			while (cur) {
				if (cnt <= cur->link.cnt) {
					if (pre) pre->link.next = cur->link.next;
					else this->head = cur->link.next;
					memset(cur, 0, sizeof(_Ty) * cnt);
					return (_Ty*)cur;
				}
				pre = cur;
				cur = cur->link.next;
			}

			_Ty* res = (_Ty*)operator new(cnt * sizeof(_Ty));
#ifdef EXCEPTION_DETECTION
			if (!res) throw std::bad_alloc();
#endif // EXCEPTION_DETECTION
			memset(res, 0, sizeof(_Ty) * cnt);
			return res;
		}

		// release resources
		// class types are destructed
		// size: number of elements to be destructed
		virtual void Free(_Ty* src, size_t cnt) override {
			if constexpr (std::is_class<_Ty>::value) for (size_t i = 0; i < cnt; ++i) src[i].~_Ty();
			if (sizeof(_Ty) * cnt >= sizeof(Block)) {
				Block* tmp = (Block*)src;
				tmp->link.cnt = cnt;
				tmp->link.next = this->head;
				this->head = tmp;
			}
			else operator delete(src);
		}

		// release resources
		void Clear() {
			void* tmp;
			while (this->head) {
				tmp = this->head;
				this->head = this->head->next;
				operator delete(tmp);
			}
		}

	protected:
		Block* head = nullptr;
	};


}