#pragma once
#include "pch.h"
#include "Allocator.h"
#include "Algorithm.h"

#define L		first
#define R		second

namespace dsl {
	// _Ty			element type
	// greaterTag	true if greater-than comparator is used 
	// _Cmpr		comparator type
	// _Alloc		allocator template
	template<typename _Ty, bool greaterTag = false, typename _Cmpr = std::less<_Ty>, template<typename> typename _Alloc = Allocator>
	class IntervalHeap {
	public:
		using _ElemType = _Ty;
		// allocator type
		using _ElemAlloc = _Alloc<_Ty>;

	protected:
		// comparator adaptation
		// encapsulate _Cmpr type to ensure that less-than comparison criteria are adopted
		struct Cmpr
		{
			bool operator() (_Ty& v1, _Ty& v2) {
				return greaterTag ^ _cpr(v1, v2);
			}
			_Cmpr _cpr{};
		};

		// destruct an element
		// offset(start with 1): the number of element to be destruct
		void Erase(size_t offset) {
#ifdef EXCEPTION_DETECTION
			if (offset < 1 || offset > this->size) throw std::exception("object of IntervalHeap: invalid offset by Erase()");
#endif // EXCEPTION_DETECTION
			if constexpr (std::is_class_v<_Ty>) this->src[offset - 1].~_Ty();
			--this->size;
		}

		void Expand() {
			this->Reserve(this->capacity ? this->capacity << 1 : 2);
		}

		// x: index
		// minHeap: if true adjust the small heap, else big heap
		void HeapDown(size_t x, bool minHeap) {
			std::pair<_Ty, _Ty>* p = (std::pair<_Ty, _Ty>*) this->src;
			// i: index of child nodes
			// cnt: number of nodes
			size_t i, cnt;

			if (minHeap) {
				_Ty tmp = std::move(p[x].L);
				cnt = (this->size >> 1) + (this->size & 1);

				// interval adjustment of node 
				// adjust the tail node noly if cnt is even
				if (x < cnt - (this->size & 1) && this->cpr(p[x].R, tmp)) std::swap(tmp, p[x].R);

				for (i = (x << 1) + 1; i < cnt; ++(i <<= 1)) {
					if (i + 1 < cnt && this->cpr(p[i + 1].L, p[i].L)) ++i;
					if (this->cpr(tmp, p[i].L)) break;
					p[x].L = std::move(p[i].L);
					x = i;
					// interval adjustment of node 
					if (x < cnt - (this->size & 1) && this->cpr(p[x].R, tmp)) std::swap(tmp, p[x].R);
				}
				p[x].L = std::move(tmp);
			}
			else {
				_Ty tmp = std::move(p[x].R);
				cnt = this->size >> 1;
				if (this->cpr(tmp, p[x].L)) std::swap(p[x].L, tmp);
				for (i = (x << 1) + 1; i < cnt; ++(i <<= 1)) {
					if (i + 1 < cnt && this->cpr(p[i].R, p[i + 1].R)) ++i;
					if (this->cpr(p[i].R, tmp)) break;
					p[x].R = std::move(p[i].R);
					x = i;
					if (this->cpr(tmp, p[x].L)) std::swap(p[x].L, tmp);
				}
				p[x].R = std::move(tmp);
			}
		}
		// x: index
		// minHeap: if true adjust the small heap, else big heap
		void HeapUp(size_t x, bool minHeap) {
			// interval adjustment before heap up
			// no interval adjustment when heap up

			std::pair<_Ty, _Ty>* p = (std::pair<_Ty, _Ty>*) this->src;
			size_t fa;

			if (minHeap) {
				_Ty tmp = std::move(p[x].L);
				while (x && this->cpr(tmp, p[fa = (x - 1) >> 1].L)) {
					p[x].L = std::move(p[fa].L);
					x = fa;
				}
				p[x].L = std::move(tmp);
			}
			else {
				_Ty tmp = std::move(p[x].R);
				while (x && this->cpr(p[fa = (x - 1) >> 1].R, tmp)) {
					p[x].R = std::move(p[fa].R);
					x = fa;
				}
				p[x].R = std::move(tmp);
			}
		}

		// O(n) build heap
		template<typename _Init>
		void BuildHeap(_Init st, _Init ed) {
			size_t cnt = ed - st;
			this->src = this->alloc.New(cnt);
			this->capacity = cnt;
			this->size = cnt;

			if (cnt & 1) {
				--ed;
				--this->size;
			}

			std::copy(st, ed, this->src);

			if (cnt > 1) {
				size_t i = cnt >> 2, j = cnt >> 1;
				std::pair<_Ty, _Ty>* p = (std::pair<_Ty, _Ty>*)this->src;
				for (; i < j; ++i) if (this->cpr(p[i].R, p[i].L)) std::swap(p[i].L, p[i].R);
				for (i = cnt >> 2;; --i) {
					this->HeapDown(i, false);
					this->HeapDown(i, true);
					if (!i) break;
				}
			}

			// if cnt is odd, the last element is inserted at end
			if (cnt & 1) this->Push(*ed);
		}

	public:
		IntervalHeap() = default;
		IntervalHeap(const _Cmpr& _cpr) :cpr{ _cpr } {}
		IntervalHeap(_Cmpr&& _cpr) :cpr{ std::move(_cpr) } {}
		IntervalHeap(_ElemAlloc&& _alloc) :alloc(std::forward<_ElemAlloc>(_alloc)) {}
		IntervalHeap(const _Cmpr& _cpr, _ElemAlloc&& _alloc) :cpr{ _cpr }, alloc(std::forward<_ElemAlloc>(_alloc)) {}
		IntervalHeap(_Cmpr&& _cpr, _ElemAlloc&& _alloc) :cpr{ std::move(_cpr) }, alloc(std::forward<_ElemAlloc>(_alloc)) {}
		// attention: shallow copy!
		IntervalHeap(const IntervalHeap& cp, bool Shallowcopy) :cpr{ cp.cpr }, alloc(cp.alloc), src(cp.src), size(cp.size), capacity(cp.capacity) {}
		IntervalHeap(const IntervalHeap& cp) : cpr{ cp.cpr }, alloc(cp.alloc), src(alloc.New(cp.capacity)), size(cp.size), capacity(cp.capacity) {
			std::copy_n(cp.src, cp.size, this->src);
		}
		IntervalHeap(IntervalHeap&& mv) noexcept :cpr{ std::move(mv.cpr) }, alloc(std::move(mv.alloc)), src(mv.src), size(mv.size), capacity(mv.capacity) {
			memset(&mv, 0, sizeof(IntervalHeap));
		}
		template<typename _Init>
		IntervalHeap(_Init st, _Init ed) {
			this->BuildHeap(st, ed);
		}
		template<typename _Init>
		IntervalHeap(_Init st, _Init ed, const _Cmpr& _cpr) :IntervalHeap(_cpr) {
			this->BuildHeap(st, ed);
		}
		template<typename _Init>
		IntervalHeap(_Init st, _Init ed, _Cmpr&& _cpr) : IntervalHeap(std::move(_cpr)) {
			this->BuildHeap(st, ed);
		}
		template<typename _Init>
		IntervalHeap(_Init st, _Init ed, _ElemAlloc&& _alloc) : IntervalHeap(std::forward<_ElemAlloc>(_alloc)) {
			this->BuildHeap(st, ed);
		}
		template<typename _Init>
		IntervalHeap(_Init st, _Init ed, const _Cmpr& _cpr, _ElemAlloc&& _alloc) : IntervalHeap(_cpr, std::forward<_ElemAlloc>(_alloc)) {
			this->BuildHeap(st, ed);
		}
		template<typename _Init>
		IntervalHeap(_Init st, _Init ed, _Cmpr&& _cpr, _ElemAlloc&& _alloc) : IntervalHeap(std::move(_cpr), std::forward<_ElemAlloc>(_alloc)) {
			this->BuildHeap(st, ed);
		}
		IntervalHeap(std::initializer_list<_Ty> lst) :IntervalHeap(lst.begin(), lst.end()) {}

		IntervalHeap& operator= (const IntervalHeap& cp) {
			this->~IntervalHeap();
			new (this) IntervalHeap(cp);
			return *this;
		}
		IntervalHeap& operator= (IntervalHeap&& mv) noexcept {
			this->~IntervalHeap();
			new (this) IntervalHeap(std::move(mv));
			return *this;
		}

		~IntervalHeap() {
			if (this->src) this->alloc.Free(this->src, this->size);
		}

		void Push(const _Ty& val) { this->Emplace(val); }
		void Push(_Ty&& val) { this->Emplace(std::move(val)); }

		template<typename... _Args>
		void Emplace(_Args&&... args) {
			if (this->size == this->capacity) this->Expand();
			new(this->src + this->size) _Ty(std::forward<_Args>(args)...);

			size_t x = this->size >> 1;

			if (this->size & 1) {
				if (this->cpr(this->src[this->size], this->src[this->size - 1])) {
					std::swap(this->src[this->size], this->src[this->size - 1]);
					this->HeapUp(x, true);
				}
				else this->HeapUp(x, false);
			}
			else if (this->size > 1) {
				std::pair<_Ty, _Ty>* p = (std::pair<_Ty, _Ty>*)this->src;
				size_t fa = (x - 1) >> 1;
				if (this->cpr(p[x].L, p[fa].L)) this->HeapUp(x, true);
				else if (this->cpr(p[fa].R, p[x].L)) {
					std::swap(p[fa].R, p[x].L);
					this->HeapUp(fa, false);
				}
			}
			++this->size;
		}

		void PopMax() {
			if (!this->size) {
#ifdef EXCEPTION_DETECTION
				throw std::exception("object of IntervalHeap: none element by PopMax()");
#endif // EXCEPTION_DETECTION
				return;
			}
			if (this->size <= 2) this->Erase(this->size);
			else {
				this->Erase(2);
				this->src[1] = std::move(this->src[this->size]);
				this->HeapDown(0, false);
			}
		}

		// popVal: the popped value
		void PopMax(_Ty& popVal) {
			if (!this->size) {
#ifdef EXCEPTION_DETECTION
				throw std::exception("object of IntervalHeap: none element by PopMax()");
#endif // EXCEPTION_DETECTION
				return;
			}
			if (this->size <= 2) popVal = std::move(this->src[--this->size]);
			else {
				popVal = std::move(this->src[1]);
				this->src[1] = std::move(this->src[--this->size]);
				this->HeapDown(0, false);
			}
		}

		void PopMin() {
			if (!this->size) {
#ifdef EXCEPTION_DETECTION
				throw std::exception("object of IntervalHeap: none element by PopMax()");
#endif // EXCEPTION_DETECTION
				return;
			}
			if (this->size == 1) this->Erase(1);
			else {
				this->Erase(1);
				this->src[0] = std::move(this->src[this->size]);
				this->HeapDown(0, true);
			}
		}

		// popVal: the popped value
		void PopMin(_Ty& popVal) {
			if (!this->size) {
#ifdef EXCEPTION_DETECTION
				throw std::exception("object of IntervalHeap: none element by PopMax()");
#endif // EXCEPTION_DETECTION
				return;
			}
			if (this->size == 1) popVal = std::move(this->src[--this->size]);
			else {
				popVal = std::move(this->src[0]);
				this->src[0] = std::move(this->src[--this->size]);
				this->HeapDown(0, true);
			}
		}

		_Ty Max()const {
#ifdef EXCEPTION_DETECTION
			if (!this->size) throw std::exception("object of IntervalHeap: none element by PopMax()");
#endif // EXCEPTION_DETECTION
			return this->src[this->size > 1];
		}
		_Ty Min()const {
#ifdef EXCEPTION_DETECTION
			if (!this->size) throw std::exception("object of IntervalHeap: none element by PopMax()");
#endif // EXCEPTION_DETECTION
			return this->src[0];
		}

		// true if empty
		inline bool Empty()const {
			return !this->size;
		}

		inline size_t Size()const { return this->size; }
		inline size_t Capacity()const { return this->capacity; }

		// return 0 if success
		// return -1 if elements were truncated
		int Reserve(size_t cap) {
			_Ty* buf = this->alloc.New(cap);

			if (this->src) {
				memcpy(buf, this->src, std::min(cap, this->size) * sizeof(_Ty));
				this->alloc.Free(this->src, 0);
			}
			this->src = buf;
			this->capacity = cap;
			if (cap < this->size) {
				this->size = cap;
				return -1;
			}
			return 0;
		}

		_Ty* Source() {
			return this->src;
		}

		// tip: not free resources
		void Clear() {
			if constexpr (std::is_class<_Ty>::value) for (_Ty* st = src, *ed = src + this->size; st != ed; ++st) st->~_Ty();
			this->size = 0;
		}

	protected:
		Cmpr cpr;
		_ElemAlloc alloc;
		_Ty* src = nullptr;
		size_t size = 0;
		size_t capacity = 0;
	};
}