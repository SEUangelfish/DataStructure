#pragma once
#include "pch.h"
#include "Allocater.h"
#include "Algorithm.h"

#define L		first
#define R		second

namespace dsl {
	// ������ṹ
	// _Ty			Ԫ������
	// LessTag		�Ƚ�����־(�����ô��ڱȽ�������false)
	// _Cmpr		�Ƚ���(Ĭ�ϲ���С�ڱȽ���)
	// _Alloc		�ڴ����������
	template<typename _Ty, bool LessTag = true, typename _Cmpr = std::less<_Ty>, typename _Alloc = Allocater<_Ty>>
	class IntervalHeap {
	protected:
		// ʵ��ʹ�õıȽ���
		struct Cmpr
		{
			// ��װ�û����ݵıȽ�������֤����С�ڱȽ�׼��
			bool operator() (const _Ty& v1, const _Ty& v2) const {
				return LessTag ? _cpr(v1, v2) : !_cpr(v1, v2);
			}
			_Cmpr _cpr;
		};

		// ɾ��ĳһԪ��
		// offset:		�ڼ���Ԫ��(��1��ʼ)
		inline void Erase(size_t offset) {
#ifdef EXCEPTION_DETECTION
			if (offset < 1 || offset > this->size) throw std::exception("invalid offset by erase");
#endif
			if (std::is_class_v<_Ty>) this->src[offset - 1].~_Ty();
			--this->size;
		}

		// ���ݺ���(Ĭ������2��)
		void Expand(int times = 2) {
			this->Reserve(this->capacity ? this->capacity * times : times);
		}

		// ���¶ѻ�
		// x:			����±�
		// minHeap:		true����С�ѣ�false�������
		void HeapDown(size_t x, bool minHeap) {
			Cmpr cpr;
			std::pair<_Ty, _Ty>* p = (std::pair<_Ty, _Ty>*) this->src;
			// ���Һ����±�
			size_t l = (x << 1) + 1, r = l + 1;
			// Ŀ��ֵ���±�
			size_t tar;
			// ������
			size_t cnt;

			// С�ѵ���
			if (minHeap) {
				cnt = (this->size >> 1) + this->size % 2;
				// ����ڲ�����
				// ���Ԫ�ظ���Ϊ������β��㲻�õ���
				if ((x < cnt - 1 || this->size % 2 == 0) && cpr(p[x].R, p[x].L)) Swap(p[x].L, p[x].R);
				while (true) {
					if (l >= cnt) return;
					tar = r >= cnt || cpr(p[l].L, p[r].L) ? l : r;
					if (cpr(p[x].L, p[tar].L)) return;
					Swap(p[tar].L, p[x].L);
					x = tar;
					// ����ڲ�����
					if ((x < cnt - 1 || this->size % 2 == 0) && cpr(p[x].R, p[x].L)) Swap(p[x].L, p[x].R);
					l = (x << 1) + 1, r = l + 1;
				}
			}
			// ��ѵ���
			else {
				// ���Ԫ�ظ���Ϊ���������һ����㲻�ô���
				cnt = this->size >> 1;
				// ����ڲ�����
				if (cpr(p[x].R, p[x].L)) Swap(p[x].L, p[x].R);
				while (true) {
					if (l >= cnt) return;
					tar = r >= cnt || cpr(p[r].R, p[l].R) ? l : r;
					if (cpr(p[tar].R, p[x].R)) return;
					Swap(p[x].R, p[tar].R);
					x = tar;
					// ����ڲ�����
					if (cpr(p[x].R, p[x].L)) Swap(p[x].L, p[x].R);
					l = (x << 1) + 1, r = l + 1;
				}
			}
		}
		// ���϶ѻ�
		// x:			����±�
		// minHeap:		true����С�ѣ�false�������
		void HeapUp(size_t x, bool minHeap) {
			Cmpr cpr;
			std::pair<_Ty, _Ty>* p = (std::pair<_Ty, _Ty>*) this->src;
			// fa	�����
			size_t fa = (x - 1) >> 1;
			while (x) {
				// С�ѵ���
				if (minHeap) {
					if (cpr(p[x].L, p[fa].L)) {
						Swap(p[x].L, p[fa].L);
						x = fa;
					}
					else return;
				}
				// ��ѵ���
				else {
					if (cpr(p[fa].R, p[x].R)) {
						Swap(p[fa].R, p[x].R);
						x = fa;
					}
					else return;
				}

				fa = (x - 1) >> 1;
			}
		}

	public:
		// Ĭ�Ϲ���
		IntervalHeap() :src(nullptr), size(0), capacity(0) {}
		// ǳ����(С��ʹ�ã�ע����������)
		IntervalHeap(const IntervalHeap& cp, bool Shallowcopy) :src(cp.src), size(cp.size), capacity(cp.capacity) {}
		// ��������(���)
		IntervalHeap(const IntervalHeap& cp) : src(_Alloc().New(cp.capacity)), size(cp.size), capacity(cp.capacity) {
			// ���ݿ���
			std::copy_n(cp.src, cp.size, this->src);
		}
		// �ƶ�����
		IntervalHeap(IntervalHeap&& mv) noexcept :src(mv.src), size(mv.size), capacity(mv.capacity) {
			mv.src = nullptr;
			mv.size = 0;
			mv.capacity = 0;
		}
		// ������
		// ��һ��������Դ���ݵ�ַ
		// �ڶ���������Ԫ�ظ���
		IntervalHeap(_Ty* _src, size_t cnt) :src(_Alloc().New(cnt)), size(cnt % 2 ? cnt - 1 : cnt), capacity(cnt) {
			// ���ݿ���
			std::copy_n(_src, this->size, this->src);

			// �ѻ�
			std::pair<_Ty, _Ty>* p = (std::pair<_Ty, _Ty>*)this->src;
			size_t i = cnt >> 1;
			if (i) for (--i; ; --i) {
				this->HeapDown(i, false);
				this->HeapDown(i, true);
				if (!i) break;
			}
			if (cnt % 2) this->Push(_src[cnt - 1]);
		}
		// ������
		// ��һ����������Ԫ�ص�ַ
		// �ڶ���������βԪ�ص�ַ
		template<typename _Init>
		IntervalHeap(_Init st, _Init ed) :src(_Alloc().New(ed - st)), capacity(ed - st) {
			size_t cnt = ed - st;
			this->size = cnt % 2 ? cnt - 1 : cnt;
			// ���ݿ���
			std::copy(st, cnt % 2 ? --ed : ed, src);

			// �ѻ�
			std::pair<_Ty, _Ty>* p = (std::pair<_Ty, _Ty>*)this->src;
			size_t i = cnt >> 1;
			if (i) for (--i; ; --i) {
				this->HeapDown(i, false);
				this->HeapDown(i, true);
				if (!i) break;
			}
			if (cnt % 2) this->Push(*ed);
		}


		// �б��ʼ��
		IntervalHeap(const std::initializer_list<_Ty>& lst) :IntervalHeap(lst.begin(), lst.size()) {}

		// ��ֵ(���)
		IntervalHeap& operator= (const IntervalHeap& cp) {
			// ������Ԫ��
			this->Clear();

			// ����������ֱ������Ϊcp������
			if (this->capacity < cp.capacity) {
				this->capacity = cp.capacity;
				_Alloc().Free(this->src, this->size);
				this->src = _Alloc().New(cp.capacity);
			}
			this->size = cp.size;

			// ���ݿ���
			Memcpy(cp.src, cp.size, this->src, this->capacity);
			return *this;
		}
		// ��ֵ(�ƶ���ֵ)
		IntervalHeap& operator= (IntervalHeap&& mv) noexcept {
			// ��������Դ
			_Alloc().Free(this->src, this->size);

			this->src = mv.src;
			this->size = mv.size;
			this->capacity = mv.capacity;
			mv.src = nullptr;
			mv.size = 0;
			mv.capacity = 0;
			return *this;
		}

		// ��������
		~IntervalHeap() {
			_Alloc().Free(this->src, this->size);
			this->src = nullptr;
			this->size = 0;
			this->capacity = 0;
		}

		// ѹ��Ԫ��
		void Push(const _Ty& val) { this->Emplace(val); }
		// ֱ�ӹ���
		template<typename... Args>
		void Emplace(Args&&... args) {
			Cmpr cpr;
			// ��������������
			if (this->size == this->capacity) this->Expand();
			new(this->src + this->size) _Ty(args...);
			// ��Ԫ�����ڽ���±�
			size_t x = this->size >> 1;

			// ���б�����������Ԫ��ʱ
			// ��Ԫ��ѹ��β���
			// ��ʱ��Ҫ�Ƚ����ڲ������������϶ѻ�
			if (this->size % 2) {
				_Ty* p = (_Ty*)this->src;
				if (cpr(p[this->size], p[this->size - 1])) {
					Swap(p[this->size], p[this->size - 1]);
					this->HeapUp(x, true);
				}
				else this->HeapUp(x, false);
			}
			// ���б�����ż����Ԫ��ʱ
			// �½�β���
			// ���ݸ�����ֵѡ��ѣ������϶ѻ�
			else if (this->size > 1) {
				std::pair<_Ty, _Ty>* p = (std::pair<_Ty, _Ty>*)this->src;
				size_t fa = (x - 1) >> 1;
				if (cpr(p[x].L, p[fa].L)) this->HeapUp(x, true);
				else if (cpr(p[fa].R, p[x].L)) {
					Swap(p[fa].R, p[x].L);
					this->HeapUp(fa, false);
				}
			}
			++this->size;
		}

		// ɾ�����ֵ
		void PopMax() {
			if (!this->size) {
#ifdef EXCEPTION_DETECTION
				throw std::exception("none element");
#endif // EXCEPTION_DETECTION
				return;
			}
			if (this->size == 1) this->Erase(1);
			else if (this->size == 2) this->Erase(2);
			else {
				_Ty* p = (_Ty*)this->src;
				Swap(p[1], p[this->size - 1]);
				this->Erase(this->size);
				this->HeapDown(0, false);
			}
		}

		// ɾ����Сֵ
		void PopMin() {
			if (!this->size) {
#ifdef EXCEPTION_DETECTION
				throw std::exception("none element");
#endif // EXCEPTION_DETECTION
				return;
			}
			if (this->size == 1) this->Erase(1);
			else {
				_Ty* p = (_Ty*)this->src;
				Swap(p[0], p[this->size - 1]);
				this->Erase(this->size);
				this->HeapDown(0, true);
			}
		}

		// �������ֵ
		_Ty Max()const {
#ifdef EXCEPTION_DETECTION
			if (!this->size) throw std::exception("none element");
#endif // EXCEPTION_DETECTION
			_Ty* p = (_Ty*)this->src;
			return this->size == 1 ? p[0] : p[1];
		}
		// ������Сֵ
		_Ty Min()const {
#ifdef EXCEPTION_DETECTION
			if (!this->size) throw std::exception("none element");
#endif // EXCEPTION_DETECTION
			return ((_Ty*)this->src)[0];
		}

		// �жϺ����Ƿ�Ϊ��
		inline bool Empty()const {
			return this->size;
		}

		// ����Ԫ�ظ���
		inline size_t Size()const { return this->size; }
		// ������������
		inline size_t Capacity()const { return this->size; }

		// ������������
		// ����0	����
		// ����-1	Ԫ�ر��ض�
		int Reserve(size_t cap) {
			// ������Դ
			_Ty* buf = _Alloc().New(cap);

			// ����Դ����
			if (this->src) {
				// ��������(ǳ����)
				std::copy_n((char*)this->src, this->size * sizeof(_Ty), (char*)buf);
				// �ͷž���Դ
				_Alloc().Free(this->src, 0);
			}
			this->src = buf;
			this->capacity = cap;
			if (cap < this->size) {
				this->size = cap;
				return -1;
			}
			return 0;
		}

		// ����Դ���ݵ�ַ
		_Ty* Data() {
			return (_Ty*)this->src;
		}

		// ���Ԫ�أ����ͷ���Դ
		void Clear() {
			for (size_t i = 1; i <= this->size; i++) this->Erase(i);
		}

	protected:
		// Դ���ݵ�ַ
		// ÿ�������һ�����ݣ���С�Ҵ�
		_Ty* src;
		// Ԫ�ظ���
		size_t size;
		// ������С
		size_t capacity;
	};
}