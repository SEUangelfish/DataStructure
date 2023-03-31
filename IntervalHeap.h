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
	template<typename _Ty, bool LessTag = true, typename _Cmpr = std::less<_Ty>, typename _Alloc = Allocater<_Ty>>
	class IntervalHeap {
	protected:
		// 实际使用的比较器
		struct Cmpr
		{
			// 封装用户传递的比较器，保证采用小于比较准则
			bool operator() (const _Ty& v1, const _Ty& v2) const {
				return LessTag ? _cpr(v1, v2) : !_cpr(v1, v2);
			}
			_Cmpr _cpr;
		};

		// 删除某一元素
		// off:		第几个元素(从1开始)
		inline void Del(size_t off) {
			if (std::is_class_v<_Ty>) ((_Ty*)this->src)[off - 1].~_Ty();
			--this->size;
		}

		// 扩容函数(每次扩容2倍)
		void Expand() {
			// 分配资源
			size_t cap = this->capacity ? this->capacity << 1 : 2;
			if (cap <= 0) throw std::exception("fail to expand");
			this->Reserve(cap);
		}

		// 向下堆化
		// x:			结点下标
		// minHeap:		true调整小堆，false调整大堆
		void HeapDown(size_t x, bool minHeap) {
			Cmpr cpr;
			std::pair<_Ty, _Ty>* p = (std::pair<_Ty, _Ty>*) this->src;
			// 左右孩子下标
			size_t l = (x << 1) + 1, r = l + 1;
			// 目标值的下标
			size_t tar;
			// 结点个数
			size_t cnt;

			// 小堆调整
			if (minHeap) {
				cnt = (this->size >> 1) + this->size % 2;
				// 结点内部调整
				// 如果元素个数为奇数则尾结点不用调整
				if ((x < cnt - 1 || this->size % 2 == 0) && cpr(p[x].R, p[x].L)) Swap(&p[x].L, &p[x].R, sizeof(_Ty));
				while (true) {
					if (l >= cnt) return;
					tar = r >= cnt || cpr(p[l].L, p[r].L) ? l : r;
					if (cpr(p[x].L, p[tar].L)) return;
					Swap(&p[tar].L, &p[x].L, sizeof(_Ty));
					x = tar;
					// 结点内部调整
					if ((x < cnt - 1 || this->size % 2 == 0) && cpr(p[x].R, p[x].L)) Swap(&p[x].L, &p[x].R, sizeof(_Ty));
					l = (x << 1) + 1, r = l + 1;
				}
			}
			// 大堆调整
			else {
				// 如果元素个数为奇数则最后一个结点不用处理
				cnt = this->size >> 1;
				// 结点内部调整
				if (cpr(p[x].R, p[x].L)) Swap(&p[x].L, &p[x].R, sizeof(_Ty));
				while (true) {
					if (l >= cnt) return;
					tar = r >= cnt || cpr(p[r].R, p[l].R) ? l : r;
					if (cpr(p[tar].R, p[x].R)) return;
					Swap(&p[x].R, &p[tar].R, sizeof(_Ty));
					x = tar;
					// 结点内部调整
					if (cpr(p[x].R, p[x].L)) Swap(&p[x].L, &p[x].R, sizeof(_Ty));
					l = (x << 1) + 1, r = l + 1;
				}
			}
		}
		// 向上堆化
		// x:			结点下标
		// minHeap:		true调整小堆，false调整大堆
		void HeapUp(size_t x, bool minHeap) {
			Cmpr cpr;
			std::pair<_Ty, _Ty>* p = (std::pair<_Ty, _Ty>*) this->src;
			// fa	父结点
			size_t fa = (x - 1) >> 1;
			while (x) {
				// 小堆调整
				if (minHeap) {
					if (cpr(p[x].L, p[fa].L)) {
						Swap(&p[x].L, &p[fa].L, sizeof(_Ty));
						x = fa;
					}
					else return;
				}
				// 大堆调整
				else {
					if (cpr(p[fa].R, p[x].R)) {
						Swap(&p[fa].R, &p[x].R, sizeof(_Ty));
						x = fa;
					}
					else return;
				}

				fa = (x - 1) >> 1;
			}
		}

	public:
		// 默认构造
		IntervalHeap() :src(nullptr), size(0), capacity(0) {}
		// 浅拷贝(小心使用，注意析构问题)
		IntervalHeap(const IntervalHeap& cp, bool ShallowCopy) :src(cp.src), size(cp.size), capacity(cp.capacity) {}
		// 拷贝构造(深拷贝)
		IntervalHeap(const IntervalHeap& cp) : src(_Alloc::New(cp.capacity)), size(cp.size), capacity(cp.capacity) {
			if (!this->src) return;
			// 数据拷贝
			Memcpy<_Ty>(cp.src, cp.size, this->src, this->capacity);
		}
		// 移动构造
		IntervalHeap(IntervalHeap&& mv) noexcept :src(mv.src), size(mv.size), capacity(mv.capacity) {
			mv.src = nullptr;
			mv.size = 0;
			mv.capacity = 0;
		}
		// 批构造
		// 第一个参数传源数据地址
		// 第二个参数传元素个数
		template<typename T>
		IntervalHeap(T* _src, size_t cnt) :src(_Alloc::New(cnt)), size(cnt), capacity(cnt) {
			if (!this->src) return;

			// 数据拷贝
			Memcpy<_Ty>(_src, cnt, this->src, this->capacity);

			// 堆化
			std::pair<T, T>* p = (std::pair<T, T>*)this->src;
			cnt = (cnt >> 1) - 1;
			if (cnt < 0) return;
			while (1) {
				this->HeapDown(cnt, false);
				this->HeapDown(cnt, true);
				if (!cnt) return;
				--cnt;
			}
		}
		// 列表初始化
		template<typename T>
		IntervalHeap(const std::initializer_list<T>& lst) :IntervalHeap<T>(lst.begin(), lst.size()) {}

		// 赋值(深拷贝)
		IntervalHeap& operator= (const IntervalHeap& cp) {
			// 释放旧资源
			this->Free();

			// 申请新资源
			this->size = cp.size;
			this->capacity = cp.capacity;
			this->src = _Alloc::New(cp.capacity);
			if (!src) return *this;

			// 数据拷贝
			Memcpy<_Ty>(cp.src, cp.size, this->src, this->capacity);
			return *this;
		}
		// 赋值(移动赋值)
		IntervalHeap& operator= (IntervalHeap&& mv) noexcept {
			this->src = mv.src;
			this->size = mv.size;
			this->capacity = mv.capacity;
			mv.src = nullptr;
			mv.size = 0;
			mv.capacity = 0;
			return *this;
		}

		// 析构函数
		~IntervalHeap() {
			Allocater<_Ty> alloc;
			alloc.Free(this->src, this->size);
			this->src = nullptr;
			this->size = 0;
			this->capacity = 0;
		}

		// 压入元素
		void Push(const _Ty& val) { this->Emplace(val); }
		// 直接构造
		template<typename... Args>
		void Emplace(Args&&... args) {
			Cmpr cpr;
			// 容量不足则扩容
			if (this->size == this->capacity) this->Expand();
			new((char*)this->src + sizeof(_Ty) * this->size) _Ty(args...);
			// 新元素所在结点下标
			size_t x = this->size >> 1;

			// 堆中本来有奇数个元素时
			// 新元素压入尾结点
			// 此时需要先进行内部调整，再向上堆化
			if (this->size % 2) {
				_Ty* p = (_Ty*)this->src;
				if (cpr(p[this->size], p[this->size - 1])) {
					Swap(&p[this->size], &p[this->size - 1], sizeof(_Ty));
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
				if (cpr(p[x].L, p[fa].L)) this->HeapUp(x, true);
				else if (cpr(p[fa].R, p[x].L)) {
					Swap(&p[fa].R, &p[x].L, sizeof(_Ty));
					this->HeapUp(fa, false);
				}
			}
			++this->size;
		}

		// 删除最大值
		void PopMax() {
			if (!this->size) throw std::exception("none element");
			if (this->size == 1) this->Del(1);
			else if (this->size == 2) this->Del(2);
			else {
				_Ty* p = (_Ty*)this->src;
				Swap(&p[1], &p[this->size - 1], sizeof(_Ty));
				this->Del(this->size);
				this->HeapDown(0, false);
			}
		}

		// 删除最小值
		void PopMin() {
			if (!this->size) throw std::exception("none element");
			if (this->size == 1) this->Del(1);
			else {
				_Ty* p = (_Ty*)this->src;
				Swap(&p[0], &p[this->size - 1], sizeof(_Ty));
				this->Del(this->size);
				this->HeapDown(0, true);
			}
		}

		// 返回最大值
		_Ty Max()const {
			if (!this->size) throw std::exception("none element");
			_Ty* p = (_Ty*)this->src;
			return this->size == 1 ? p[0] : p[1];
		}
		// 返回最小值
		_Ty Min()const {
			if (!this->size) throw std::exception("none element");
			return ((_Ty*)this->src)[0];
		}

		// 判断函数是否为空
		inline bool Empty()const {
			return this->size;
		}

		// 返回元素个数
		inline size_t Size()const { return this->size; }
		// 返回容器容量
		inline size_t Capacity()const { return this->size; }

		// 设置容器容量
		// 返回0	正常
		// 返回-1	元素被截断
		int Reserve(size_t cap) {
			// 申请资源
			void* buf = _Alloc::New(cap);

			// 拷贝源数据
			if (this->src) {
				// 拷贝数据
				memcpy_s(buf, cap * sizeof(_Ty), this->src, this->size * sizeof(_Ty));
				// 释放旧资源
				free(this->src);
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
			return (_Ty*)this->src;
		}

		// 清空元素，不释放资源
		void Clear() { this->Free(); }

	protected:
		// 源数据地址
		// 每个结点是一对数据，左小右大
		void* src;
		// 元素个数
		size_t size;
		// 容量大小
		size_t capacity;
	};
}