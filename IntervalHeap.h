#pragma once
#include "pch.h"
#include "Allocator.h"
#include "Algorithm.h"

#define L		first
#define R		second

namespace dsl {
	// 间隔树结构
	// _Ty			元素类型
	// LessTag		比较器标志(若采用大于比较器则置true)
	// _Cmpr		比较器(默认采用小于比较器)
	// _Alloc		分配器模板
	template<typename _Ty, bool LessTag = false, typename _Cmpr = std::less<_Ty>, template<typename> typename _Alloc = dsl::Allocator>
	class IntervalHeap {
	public:
		// 元素分配器类型
		using _ElemAlloc = _Alloc<_Ty>;

	protected:
		// 实际使用的比较器
		struct Cmpr
		{
			// 封装用户传递的比较器，保证采用小于比较准则
			bool operator() (_Ty& v1, _Ty& v2) {
				return LessTag ^ _cpr(v1, v2);
			}
			_Cmpr _cpr{};
		};

		// 删除某一元素
		// offset:		第几个元素(从1开始)
		void Erase(size_t offset) {
#ifdef EXCEPTION_DETECTION
			if (offset < 1 || offset > this->size) throw std::exception("object of IntervalHeap: invalid offset by Erase()");
#endif // EXCEPTION_DETECTION
			if (std::is_class_v<_Ty>) this->src[offset - 1].~_Ty();
			--this->size;
		}

		// 扩容函数(默认扩容2倍)
		void Expand() {
			this->Reserve(this->capacity ? this->capacity << 1 : 2);
		}

		// 向下堆化
		// x:			结点下标
		// minHeap:		true调整小堆，false调整大堆
		void HeapDown(size_t x, bool minHeap) {
			std::pair<_Ty, _Ty>* p = (std::pair<_Ty, _Ty>*) this->src;
			// i：孩子结点下标
			// cnt：结点个数
			size_t i, cnt;

			// 小堆调整
			if (minHeap) {
				_Ty tmp = std::move(p[x].L);
				cnt = (this->size >> 1) + (this->size & 1);

				// 结点内部调整  
				// 如果元素个数为奇数则尾结点不用调整
				if (x < cnt - (this->size & 1) && this->cpr(p[x].R, tmp)) std::swap(tmp, p[x].R);

				for (i = (x << 1) + 1; i < cnt; ++(i <<= 1)) {
					if (i + 1 < cnt && this->cpr(p[i + 1].L, p[i].L)) ++i;
					if (this->cpr(tmp, p[i].L)) break;
					p[x].L = std::move(p[i].L);
					x = i;
					// 结点内部调整
					if (x < cnt - (this->size & 1) && this->cpr(p[x].R, tmp)) std::swap(tmp, p[x].R);
				}
				p[x].L = std::move(tmp);
			}
			// 大堆调整
			else {
				_Ty tmp = std::move(p[x].R);
				// 如果元素个数为奇数则尾结点不用处理
				cnt = this->size >> 1;
				// 结点内部调整
				if (this->cpr(tmp, p[x].L)) std::swap(p[x].L, tmp);
				for (i = (x << 1) + 1; i < cnt; ++(i <<= 1)) {
					if (i + 1 < cnt && this->cpr(p[i].R, p[i + 1].R)) ++i;
					if (this->cpr(p[i].R, tmp)) break;
					p[x].R = std::move(p[i].R);
					x = i;
					// 结点内部调整
					if (this->cpr(tmp, p[x].L)) std::swap(p[x].L, tmp);
				}
				p[x].R = std::move(tmp);
			}
		}
		// 向上堆化
		// x:			结点下标
		// minHeap:		true调整小堆，false调整大堆
		void HeapUp(size_t x, bool minHeap) {
			// 向上堆化前先自行内部调整
			// 堆化中不用再进行内部调整

			std::pair<_Ty, _Ty>* p = (std::pair<_Ty, _Ty>*) this->src;
			// fa:	父结点
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

		// 批构造建堆函数
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

			// 数据拷贝
			std::copy(st, ed, this->src);

			// 堆化
			if (cnt > 1) {
				size_t i = cnt >> 2, j = cnt >> 1;
				// 叶子结点内部调整
				std::pair<_Ty, _Ty>* p = (std::pair<_Ty, _Ty>*)this->src;
				for (; i < j; ++i) if (this->cpr(p[i].R, p[i].L)) std::swap(p[i].L, p[i].R);
				// 分支结点向下堆化
				for (i = cnt >> 2;; --i) {
					this->HeapDown(i, false);
					this->HeapDown(i, true);
					if (!i) break;
				}
			}

			// 若有奇数个元素，则末尾元素在最后插入
			if (cnt & 1) this->Push(*ed);
		}

	public:
		// 默认构造
		IntervalHeap() = default;
		// _cpr：比较器
		IntervalHeap(const _Cmpr& _cpr) :cpr{ _cpr } {}
		// _cpr：比较器
		IntervalHeap(_Cmpr&& _cpr) :cpr{ std::move(_cpr) } {}
		// _alloc：分配器
		IntervalHeap(_ElemAlloc&& _alloc) :alloc(std::forward<_ElemAlloc>(_alloc)) {}
		// _cpr：比较器
		// _alloc：分配器
		IntervalHeap(const _Cmpr& _cpr, _ElemAlloc&& _alloc) :cpr{ _cpr }, alloc(std::forward<_ElemAlloc>(_alloc)) {}
		// _cpr：比较器
		// _alloc：分配器
		IntervalHeap(_Cmpr&& _cpr, _ElemAlloc&& _alloc) :cpr{ std::move(_cpr) }, alloc(std::forward<_ElemAlloc>(_alloc)) {}
		// 浅拷贝(小心使用，注意析构问题)
		IntervalHeap(const IntervalHeap& cp, bool Shallowcopy) :cpr{ cp.cpr }, alloc(cp.alloc), src(cp.src), size(cp.size), capacity(cp.capacity) {}
		// 拷贝构造(深拷贝)
		IntervalHeap(const IntervalHeap& cp) : cpr{ cp.cpr }, alloc(cp.alloc), src(alloc.New(cp.capacity)), size(cp.size), capacity(cp.capacity) {
			std::copy_n(cp.src, cp.size, this->src);
		}
		// 移动构造
		IntervalHeap(IntervalHeap&& mv) noexcept :cpr{ std::move(mv.cpr) }, alloc(std::move(mv.alloc)), src(mv.src), size(mv.size), capacity(mv.capacity) {
			memset(&mv, 0, sizeof(IntervalHeap));
		}
		// 批构造
		// st：首元素地址
		// ed：尾元素的下一个位置
		template<typename _Init>
		IntervalHeap(_Init st, _Init ed) {
			this->BuildHeap(st, ed);
		}
		// 批构造
		// st：首元素地址
		// ed：尾元素的下一个位置
		// _cpr：比较器
		template<typename _Init>
		IntervalHeap(_Init st, _Init ed, const _Cmpr& _cpr) :IntervalHeap(_cpr) {
			this->BuildHeap(st, ed);
		}
		// 批构造
		// st：首元素地址
		// ed：尾元素的下一个位置
		// _cpr：比较器
		template<typename _Init>
		IntervalHeap(_Init st, _Init ed, _Cmpr&& _cpr) :IntervalHeap(std::move(_cpr)) {
			this->BuildHeap(st, ed);
		}
		// 批构造
		// st：首元素地址
		// ed：尾元素的下一个位置
		// _alloc：分配器
		template<typename _Init>
		IntervalHeap(_Init st, _Init ed, _ElemAlloc&& _alloc) :IntervalHeap(std::forward<_ElemAlloc>(_alloc)) {
			this->BuildHeap(st, ed);
		}
		// 批构造
		// st：首元素地址
		// ed：尾元素的下一个位置
		// _cpr：比较器
		// _alloc：分配器
		template<typename _Init>
		IntervalHeap(_Init st, _Init ed, const _Cmpr& _cpr, _ElemAlloc&& _alloc) :IntervalHeap(_cpr, std::forward<_ElemAlloc>(_alloc)) {
			this->BuildHeap(st, ed);
		}
		// 批构造
		// st：首元素地址
		// ed：尾元素的下一个位置
		// _cpr：比较器
		// _alloc：分配器
		template<typename _Init>
		IntervalHeap(_Init st, _Init ed, _Cmpr&& _cpr, _ElemAlloc&& _alloc) :IntervalHeap(std::move(_cpr), std::forward<_ElemAlloc>(_alloc)) {
			this->BuildHeap(st, ed);
		}

		// 列表初始化
		IntervalHeap(std::initializer_list<_Ty> lst) :IntervalHeap(lst.begin(), lst.end()) {}

		// 赋值(深拷贝)
		IntervalHeap& operator= (const IntervalHeap& cp) {
			this->~IntervalHeap();
			new (this) IntervalHeap(cp);
			return *this;
		}
		// 赋值(移动赋值)
		IntervalHeap& operator= (IntervalHeap&& mv) noexcept {
			this->~IntervalHeap();
			new (this) IntervalHeap(std::move(mv));
			return *this;
		}

		// 析构函数
		~IntervalHeap() {
			if (this->src) this->alloc.Free(this->src, this->size);
		}

		// 压入元素
		void Push(const _Ty& val) { this->Emplace(val); }
		// 压入元素
		void Push(_Ty&& val) { this->Emplace(std::move(val)); }

		// 直接构造
		template<typename... _Args>
		void Emplace(_Args&&... args) {
			// 容量不足则扩容
			if (this->size == this->capacity) this->Expand();
			new(this->src + this->size) _Ty(std::forward<_Args>(args)...);
			// 新元素所在结点下标
			size_t x = this->size >> 1;

			// 堆中本来有奇数个元素时
			// 新元素压入尾结点
			// 此时需要先进行内部调整，再向上堆化
			if (this->size & 1) {
				if (this->cpr(this->src[this->size], this->src[this->size - 1])) {
					std::swap(this->src[this->size], this->src[this->size - 1]);
					this->HeapUp(x, true);
				}
				else this->HeapUp(x, false);
			}
			// 堆中本来由偶数个元素时
			// 新建尾结点
			// 根据父结点的值选择堆，再向上堆化
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

		// 删除最大值
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
				// 注意：Erase已经把size-1了
				this->src[1] = std::move(this->src[this->size]);
				this->HeapDown(0, false);
			}
		}

		// 删除最大值
		// popVal：接收删除的元素
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

		// 删除最小值
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
				// 注意：Erase已经把size-1了
				this->src[0] = std::move(this->src[this->size]);
				this->HeapDown(0, true);
			}
		}

		// 删除最小值
		// popVal：接收删除的元素
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

		// 返回最大值
		_Ty Max()const {
#ifdef EXCEPTION_DETECTION
			if (!this->size) throw std::exception("object of IntervalHeap: none element by PopMax()");
#endif // EXCEPTION_DETECTION
			return this->src[this->size > 1];
		}
		// 返回最小值
		_Ty Min()const {
#ifdef EXCEPTION_DETECTION
			if (!this->size) throw std::exception("object of IntervalHeap: none element by PopMax()");
#endif // EXCEPTION_DETECTION
			return this->src[0];
		}

		// 判断函数是否为空
		inline bool Empty()const {
			return !this->size;
		}

		// 返回元素个数
		inline size_t Size()const { return this->size; }
		// 返回容器容量
		inline size_t Capacity()const { return this->capacity; }

		// 设置容器容量
		// 返回0	正常
		// 返回-1	元素被截断
		int Reserve(size_t cap) {
			// 申请资源
			_Ty* buf = this->alloc.New(cap);

			// 拷贝源数据
			if (this->src) {
				// 拷贝数据(浅拷贝)
				memcpy(buf, this->src, std::min(cap, this->size) * sizeof(_Ty));
				// 释放旧资源
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

		// 返回源数据地址
		_Ty* Source() {
			return this->src;
		}

		// 清空元素，不释放资源
		void Clear() {
			if constexpr (std::is_class<_Ty>::value) for (_Ty* st = src, *ed = src + this->size; st != ed; ++st) st->~_Ty();
			this->size = 0;
		}

	protected:
		// 比较器
		Cmpr cpr;
		// 分配器
		_ElemAlloc alloc;
		// 源数据地址
		// 每个结点是一对数据，左小右大
		_Ty* src = nullptr;
		// 元素个数
		size_t size = 0;
		// 容量大小
		size_t capacity = 0;

	};
}