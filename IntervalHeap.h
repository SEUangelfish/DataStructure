#pragma once
#include "pch.h"
#include "Allocater.h"
#include "Algorithm.h"

#define L		first
#define R		second

namespace dsl {
	// 间隔树结构
	// _Ty			元素类型
	// LessTag		比较器标志(若采用大于比较器则置false)
	// _Cmpr		比较器(默认采用小于比较器)
	// _Alloc		内存分配器类型
	template<typename _Ty, bool LessTag = false, typename _Cmpr = std::less<_Ty>, typename _Alloc = dsl::Allocater<_Ty>>
	class IntervalHeap {
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
			if (offset < 1 || offset > this->size) throw std::exception("object of IntervalHeap：invalid offset by Erase()");
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
			// 左右孩子下标
			size_t l = (x << 1) + 1, r = l + 1;
			// 目标值的下标
			size_t tar;
			// 结点个数
			size_t cnt;

			char buf[sizeof(_Ty)];
			_Ty* tmp = (_Ty*)buf;

			// 小堆调整
			if (minHeap) {
				memcpy(tmp, &p[x].L, sizeof(_Ty));
				cnt = (this->size >> 1) + (this->size & 1);
				// 结点内部调整  
				// 如果元素个数为奇数则尾结点不用调整
				if ((x < cnt - 1 || !(this->size & 1)) && this->cpr(p[x].R, *tmp)) dsl::Swap(*tmp, p[x].R);
				while (l < cnt) {
					tar = r >= cnt || this->cpr(p[l].L, p[r].L) ? l : r;
					if (this->cpr(*tmp, p[tar].L)) break;
					memcpy(&p[x].L, &p[tar].L, sizeof(_Ty));
					x = tar;
					// 结点内部调整
					if ((x < cnt - 1 || !(this->size & 1)) && this->cpr(p[x].R, *tmp)) dsl::Swap(*tmp, p[x].R);
					l = (x << 1) + 1, r = l + 1;
				}
				memcpy(&p[x].L, tmp, sizeof(_Ty));
			}
			// 大堆调整
			else {
				memcpy(tmp, &p[x].R, sizeof(_Ty));
				// 如果元素个数为奇数则尾结点不用处理
				cnt = this->size >> 1;
				// 结点内部调整
				if (this->cpr(*tmp, p[x].L)) dsl::Swap(p[x].L, *tmp);
				while (l < cnt) {
					tar = r >= cnt || this->cpr(p[r].R, p[l].R) ? l : r;
					if (this->cpr(p[tar].R, *tmp)) break;
					memcpy(&p[x].R, &p[tar].R, sizeof(_Ty));
					x = tar;
					// 结点内部调整
					if (this->cpr(*tmp, p[x].L)) dsl::Swap(p[x].L, *tmp);
					l = (x << 1) + 1, r = l + 1;
				}
				memcpy(&p[x].R, tmp, sizeof(_Ty));
			}
		}
		// 向上堆化
		// x:			结点下标
		// minHeap:		true调整小堆，false调整大堆
		void HeapUp(size_t x, bool minHeap) {
			// 向上堆化前先自行内部调整
			// 堆化中不用再进行内部调整

			std::pair<_Ty, _Ty>* p = (std::pair<_Ty, _Ty>*) this->src;
			// fa	父结点
			size_t fa = (x - 1) >> 1;

			char buf[sizeof(_Ty)];
			_Ty* tmp = (_Ty*)buf;

			if (minHeap) {
				memcpy(tmp, &p[x].L, sizeof(_Ty));
				while (x && this->cpr(*tmp, p[fa].L)) {
					memcpy(&p[x].L, &p[fa].L, sizeof(_Ty));
					x = fa;
					fa = (x - 1) >> 1;
				}
				memcpy(&p[x].L, tmp, sizeof(_Ty));
			}
			else {
				memcpy(tmp, &p[x].R, sizeof(_Ty));
				while (x && this->cpr(p[fa].R, *tmp)) {
					memcpy(&p[x].R, &p[fa].R, sizeof(_Ty));
					x = fa;
					fa = (x - 1) >> 1;
				}
				memcpy(&p[x].R, tmp, sizeof(_Ty));
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
				for (; i < j; ++i) if (this->cpr(p[i].R, p[i].L)) dsl::Swap(p[i].L, p[i].R);
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
		IntervalHeap(_Alloc&& _alloc) :alloc(std::forward<_Alloc>(_alloc)) {}
		// _cpr：比较器
		// _alloc：分配器
		IntervalHeap(const _Cmpr& _cpr, _Alloc&& _alloc) :cpr{ _cpr }, alloc(std::forward<_Alloc>(_alloc)) {}
		// _cpr：比较器
		// _alloc：分配器
		IntervalHeap(_Cmpr&& _cpr, _Alloc&& _alloc) :cpr{ std::move(_cpr) }, alloc(std::forward<_Alloc>(_alloc)) {}
		// 浅拷贝(小心使用，注意析构问题)
		IntervalHeap(const IntervalHeap& cp, bool Shallowcopy) :cpr{ cp.cpr }, alloc(cp.alloc), src(cp.src), size(cp.size), capacity(cp.capacity) {}
		// 拷贝构造(深拷贝)
		IntervalHeap(const IntervalHeap& cp) : cpr{ cp.cpr }, alloc(cp.alloc), src(alloc.New(cp.capacity)), size(cp.size), capacity(cp.capacity) {
			std::copy_n(cp.src, cp.size, this->src);
		}
		// 移动构造
		IntervalHeap(IntervalHeap&& mv) noexcept :cpr{ std::move(mv.cpr) }, alloc(std::move(mv.alloc)), src(mv.src), size(mv.size), capacity(mv.capacity) {
			memset(&mv, 0, sizeof(IntervalHeap<_Ty, LessTag, _Cmpr, _Alloc>));
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
		IntervalHeap(_Init st, _Init ed, _Alloc&& _alloc) :IntervalHeap(std::forward<_Alloc>(_alloc)) {
			this->BuildHeap(st, ed);
		}
		// 批构造
		// st：首元素地址
		// ed：尾元素的下一个位置
		// _cpr：比较器
		// _alloc：分配器
		template<typename _Init>
		IntervalHeap(_Init st, _Init ed, const _Cmpr& _cpr, _Alloc&& _alloc) :IntervalHeap(_cpr, std::forward<_Alloc>(_alloc)) {
			this->BuildHeap(st, ed);
		}
		// 批构造
		// st：首元素地址
		// ed：尾元素的下一个位置
		// _cpr：比较器
		// _alloc：分配器
		template<typename _Init>
		IntervalHeap(_Init st, _Init ed, _Cmpr&& _cpr, _Alloc&& _alloc) :IntervalHeap(std::move(_cpr), std::forward<_Alloc>(_alloc)) {
			this->BuildHeap(st, ed);
		}

		// 列表初始化
		IntervalHeap(const std::initializer_list<_Ty>& lst) :IntervalHeap(lst.begin(), lst.size()) {}

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
			this->alloc.Free(this->src, this->size);
			memset(this, 0, sizeof(IntervalHeap<_Ty, LessTag, _Cmpr, _Alloc>));
		}

		// 压入元素
		void Push(const _Ty& val) { this->Emplace(val); }
		// 压入元素
		void Push(_Ty&& val) { this->Emplace(std::move(val)); }
		// 直接构造
		template<typename... Args>
		void Emplace(Args&&... args) {
			// 容量不足则扩容
			if (this->size == this->capacity) this->Expand();
			new(this->src + this->size) _Ty(std::forward<Args>(args)...);
			// 新元素所在结点下标
			size_t x = this->size >> 1;

			// 堆中本来有奇数个元素时
			// 新元素压入尾结点
			// 此时需要先进行内部调整，再向上堆化
			if (this->size & 1) {
				if (this->cpr(this->src[this->size], this->src[this->size - 1])) {
					dsl::Swap(this->src[this->size], this->src[this->size - 1]);
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
					dsl::Swap(p[fa].R, p[x].L);
					this->HeapUp(fa, false);
				}
			}
			++this->size;
		}

		// 删除最大值
		void PopMax() {
			if (!this->size) {
#ifdef EXCEPTION_DETECTION
				throw std::exception("object of IntervalHeap：none element by PopMax()");
#endif // EXCEPTION_DETECTION
				return;
			}
			if (this->size <= 2) this->Erase(this->size);
			else {
				this->Erase(2);
				// 注意：Erase已经把size-1了
				memcpy(this->src + 1, this->src + this->size, sizeof(_Ty));
				this->HeapDown(0, false);
			}
		}

		// 删除最大值
		// popVal：接收删除的元素
		void PopMax(_Ty& popVal) {
			if (!this->size) {
#ifdef EXCEPTION_DETECTION
				throw std::exception("object of IntervalHeap：none element by PopMax()");
#endif // EXCEPTION_DETECTION
				return;
		}
			if (this->size <= 2) memcpy(&popVal, this->src + (--this->size), sizeof(_Ty));
			else {
				memcpy(&popVal, this->src + 1, sizeof(_Ty));
				memcpy(this->src + 1, this->src + (--this->size), sizeof(_Ty));
				this->HeapDown(0, false);
			}
	}

		// 删除最小值
		void PopMin() {
			if (!this->size) {
#ifdef EXCEPTION_DETECTION
				throw std::exception("object of IntervalHeap：none element by PopMax()");
#endif // EXCEPTION_DETECTION
				return;
		}
			if (this->size == 1) this->Erase(1);
			else {
				this->Erase(1);
				// 注意：Erase已经把size-1了
				memcpy(this->src, this->src + this->size, sizeof(_Ty));
				this->HeapDown(0, true);
			}
}

		// 删除最小值
		// popVal：接收删除的元素
		void PopMin(_Ty& popVal) {
			if (!this->size) {
#ifdef EXCEPTION_DETECTION
				throw std::exception("object of IntervalHeap：none element by PopMax()");
#endif // EXCEPTION_DETECTION
				return;
			}
			if (this->size == 1) memcpy(&popVal, this->src + (--this->size), sizeof(_Ty));
			else {
				memcpy(&popVal, this->src, sizeof(_Ty));
				memcpy(this->src, this->src + (--this->size), sizeof(_Ty));
				this->HeapDown(0, true);
			}
		}

		// 返回最大值
		_Ty Max()const {
#ifdef EXCEPTION_DETECTION
			if (!this->size) throw std::exception("object of IntervalHeap：none element by PopMax()");
#endif // EXCEPTION_DETECTION
			return this->size == 1 ? this->src[0] : this->src[1];
		}
		// 返回最小值
		_Ty Min()const {
#ifdef EXCEPTION_DETECTION
			if (!this->size) throw std::exception("object of IntervalHeap：none element by PopMax()");
#endif // EXCEPTION_DETECTION
			return this->src[0];
		}

		// 判断函数是否为空
		inline bool Empty()const {
			return this->size;
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
		_Ty* Data() {
			return this->src;
		}

		// 清空元素，不释放资源
		void Clear() {
			for (_Ty* st = src, *ed = src + this->size; st != ed; ++st) st->~_Ty();
			this->size = 0;
		}

	protected:
		// 比较器
		Cmpr cpr;
		// 分配器
		_Alloc alloc;
		// 源数据地址
		// 每个结点是一对数据，左小右大
		_Ty* src = nullptr;
		// 元素个数
		size_t size = 0;
		// 容量大小
		size_t capacity = 0;

		};
}