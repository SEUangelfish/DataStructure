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
		// off:		�ڼ���Ԫ��(��1��ʼ)
		inline void Del(size_t off) {
			if (std::is_class_v<_Ty>) ((_Ty*)this->src)[off - 1].~_Ty();
			--this->size;
		}

		// ���ݺ���(ÿ������2��)
		void Expand() {
			// ������Դ
			size_t cap = this->capacity ? this->capacity << 1 : 2;
			if (cap <= 0) throw std::exception("fail to expand");
			this->Reserve(cap);
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
				if ((x < cnt - 1 || this->size % 2 == 0) && cpr(p[x].R, p[x].L)) Swap(&p[x].L, &p[x].R, sizeof(_Ty));
				while (true) {
					if (l >= cnt) return;
					tar = r >= cnt || cpr(p[l].L, p[r].L) ? l : r;
					if (cpr(p[x].L, p[tar].L)) return;
					Swap(&p[tar].L, &p[x].L, sizeof(_Ty));
					x = tar;
					// ����ڲ�����
					if ((x < cnt - 1 || this->size % 2 == 0) && cpr(p[x].R, p[x].L)) Swap(&p[x].L, &p[x].R, sizeof(_Ty));
					l = (x << 1) + 1, r = l + 1;
				}
			}
			// ��ѵ���
			else {
				// ���Ԫ�ظ���Ϊ���������һ����㲻�ô���
				cnt = this->size >> 1;
				// ����ڲ�����
				if (cpr(p[x].R, p[x].L)) Swap(&p[x].L, &p[x].R, sizeof(_Ty));
				while (true) {
					if (l >= cnt) return;
					tar = r >= cnt || cpr(p[r].R, p[l].R) ? l : r;
					if (cpr(p[tar].R, p[x].R)) return;
					Swap(&p[x].R, &p[tar].R, sizeof(_Ty));
					x = tar;
					// ����ڲ�����
					if (cpr(p[x].R, p[x].L)) Swap(&p[x].L, &p[x].R, sizeof(_Ty));
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
						Swap(&p[x].L, &p[fa].L, sizeof(_Ty));
						x = fa;
					}
					else return;
				}
				// ��ѵ���
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
		// Ĭ�Ϲ���
		IntervalHeap() :src(nullptr), size(0), capacity(0) {}
		// ǳ����(С��ʹ�ã�ע����������)
		IntervalHeap(const IntervalHeap& cp, bool ShallowCopy) :src(cp.src), size(cp.size), capacity(cp.capacity) {}
		// ��������(���)
		IntervalHeap(const IntervalHeap& cp) : src(_Alloc::New(cp.capacity)), size(cp.size), capacity(cp.capacity) {
			if (!this->src) return;
			// ���ݿ���
			Memcpy<_Ty>(cp.src, cp.size, this->src, this->capacity);
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
		template<typename T>
		IntervalHeap(T* _src, size_t cnt) :src(_Alloc::New(cnt)), size(cnt), capacity(cnt) {
			if (!this->src) return;

			// ���ݿ���
			Memcpy<_Ty>(_src, cnt, this->src, this->capacity);

			// �ѻ�
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
		// �б��ʼ��
		template<typename T>
		IntervalHeap(const std::initializer_list<T>& lst) :IntervalHeap<T>(lst.begin(), lst.size()) {}

		// ��ֵ(���)
		IntervalHeap& operator= (const IntervalHeap& cp) {
			// �ͷž���Դ
			this->Free();

			// ��������Դ
			this->size = cp.size;
			this->capacity = cp.capacity;
			this->src = _Alloc::New(cp.capacity);
			if (!src) return *this;

			// ���ݿ���
			Memcpy<_Ty>(cp.src, cp.size, this->src, this->capacity);
			return *this;
		}
		// ��ֵ(�ƶ���ֵ)
		IntervalHeap& operator= (IntervalHeap&& mv) noexcept {
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
			Allocater<_Ty> alloc;
			alloc.Free(this->src, this->size);
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
			new((char*)this->src + sizeof(_Ty) * this->size) _Ty(args...);
			// ��Ԫ�����ڽ���±�
			size_t x = this->size >> 1;

			// ���б�����������Ԫ��ʱ
			// ��Ԫ��ѹ��β���
			// ��ʱ��Ҫ�Ƚ����ڲ������������϶ѻ�
			if (this->size % 2) {
				_Ty* p = (_Ty*)this->src;
				if (cpr(p[this->size], p[this->size - 1])) {
					Swap(&p[this->size], &p[this->size - 1], sizeof(_Ty));
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
					Swap(&p[fa].R, &p[x].L, sizeof(_Ty));
					this->HeapUp(fa, false);
				}
			}
			++this->size;
		}

		// ɾ�����ֵ
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

		// ɾ����Сֵ
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

		// �������ֵ
		_Ty Max()const {
			if (!this->size) throw std::exception("none element");
			_Ty* p = (_Ty*)this->src;
			return this->size == 1 ? p[0] : p[1];
		}
		// ������Сֵ
		_Ty Min()const {
			if (!this->size) throw std::exception("none element");
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
			void* buf = _Alloc::New(cap);

			// ����Դ����
			if (this->src) {
				// ��������
				memcpy_s(buf, cap * sizeof(_Ty), this->src, this->size * sizeof(_Ty));
				// �ͷž���Դ
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

		// ����Դ���ݵ�ַ
		_Ty* Data() {
			return (_Ty*)this->src;
		}

		// ���Ԫ�أ����ͷ���Դ
		void Clear() { this->Free(); }

	protected:
		// Դ���ݵ�ַ
		// ÿ�������һ�����ݣ���С�Ҵ�
		void* src;
		// Ԫ�ظ���
		size_t size;
		// ������С
		size_t capacity;
	};
}