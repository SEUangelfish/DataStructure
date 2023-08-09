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
	template<typename _Ty, bool LessTag = false, typename _Cmpr = std::less<_Ty>, typename _Alloc = dsl::Allocater<_Ty>>
	class IntervalHeap {
	protected:
		// ʵ��ʹ�õıȽ���
		struct Cmpr
		{
			// ��װ�û����ݵıȽ�������֤����С�ڱȽ�׼��
			bool operator() (_Ty& v1, _Ty& v2) {
				return LessTag ^ _cpr(v1, v2);
			}
			_Cmpr _cpr{};
		};

		// ɾ��ĳһԪ��
		// offset:		�ڼ���Ԫ��(��1��ʼ)
		void Erase(size_t offset) {
#ifdef EXCEPTION_DETECTION
			if (offset < 1 || offset > this->size) throw std::exception("object of IntervalHeap��invalid offset by Erase()");
#endif // EXCEPTION_DETECTION
			if (std::is_class_v<_Ty>) this->src[offset - 1].~_Ty();
			--this->size;
		}

		// ���ݺ���(Ĭ������2��)
		void Expand() {
			this->Reserve(this->capacity ? this->capacity << 1 : 2);
		}

		// ���¶ѻ�
		// x:			����±�
		// minHeap:		true����С�ѣ�false�������
		void HeapDown(size_t x, bool minHeap) {
			std::pair<_Ty, _Ty>* p = (std::pair<_Ty, _Ty>*) this->src;
			// ���Һ����±�
			size_t l = (x << 1) + 1, r = l + 1;
			// Ŀ��ֵ���±�
			size_t tar;
			// ������
			size_t cnt;

			char buf[sizeof(_Ty)];
			_Ty* tmp = (_Ty*)buf;

			// С�ѵ���
			if (minHeap) {
				memcpy(tmp, &p[x].L, sizeof(_Ty));
				cnt = (this->size >> 1) + (this->size & 1);
				// ����ڲ�����  
				// ���Ԫ�ظ���Ϊ������β��㲻�õ���
				if ((x < cnt - 1 || !(this->size & 1)) && this->cpr(p[x].R, *tmp)) dsl::Swap(*tmp, p[x].R);
				while (l < cnt) {
					tar = r >= cnt || this->cpr(p[l].L, p[r].L) ? l : r;
					if (this->cpr(*tmp, p[tar].L)) break;
					memcpy(&p[x].L, &p[tar].L, sizeof(_Ty));
					x = tar;
					// ����ڲ�����
					if ((x < cnt - 1 || !(this->size & 1)) && this->cpr(p[x].R, *tmp)) dsl::Swap(*tmp, p[x].R);
					l = (x << 1) + 1, r = l + 1;
				}
				memcpy(&p[x].L, tmp, sizeof(_Ty));
			}
			// ��ѵ���
			else {
				memcpy(tmp, &p[x].R, sizeof(_Ty));
				// ���Ԫ�ظ���Ϊ������β��㲻�ô���
				cnt = this->size >> 1;
				// ����ڲ�����
				if (this->cpr(*tmp, p[x].L)) dsl::Swap(p[x].L, *tmp);
				while (l < cnt) {
					tar = r >= cnt || this->cpr(p[r].R, p[l].R) ? l : r;
					if (this->cpr(p[tar].R, *tmp)) break;
					memcpy(&p[x].R, &p[tar].R, sizeof(_Ty));
					x = tar;
					// ����ڲ�����
					if (this->cpr(*tmp, p[x].L)) dsl::Swap(p[x].L, *tmp);
					l = (x << 1) + 1, r = l + 1;
				}
				memcpy(&p[x].R, tmp, sizeof(_Ty));
			}
		}
		// ���϶ѻ�
		// x:			����±�
		// minHeap:		true����С�ѣ�false�������
		void HeapUp(size_t x, bool minHeap) {
			// ���϶ѻ�ǰ�������ڲ�����
			// �ѻ��в����ٽ����ڲ�����

			std::pair<_Ty, _Ty>* p = (std::pair<_Ty, _Ty>*) this->src;
			// fa	�����
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

		// �����콨�Ѻ���
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

			// ���ݿ���
			std::copy(st, ed, this->src);

			// �ѻ�
			if (cnt > 1) {
				size_t i = cnt >> 2, j = cnt >> 1;
				// Ҷ�ӽ���ڲ�����
				std::pair<_Ty, _Ty>* p = (std::pair<_Ty, _Ty>*)this->src;
				for (; i < j; ++i) if (this->cpr(p[i].R, p[i].L)) dsl::Swap(p[i].L, p[i].R);
				// ��֧������¶ѻ�
				for (i = cnt >> 2;; --i) {
					this->HeapDown(i, false);
					this->HeapDown(i, true);
					if (!i) break;
				}
			}

			// ����������Ԫ�أ���ĩβԪ����������
			if (cnt & 1) this->Push(*ed);
		}

	public:
		// Ĭ�Ϲ���
		IntervalHeap() = default;
		// _cpr���Ƚ���
		IntervalHeap(const _Cmpr& _cpr) :cpr{ _cpr } {}
		// _cpr���Ƚ���
		IntervalHeap(_Cmpr&& _cpr) :cpr{ std::move(_cpr) } {}
		// _alloc��������
		IntervalHeap(_Alloc&& _alloc) :alloc(std::forward<_Alloc>(_alloc)) {}
		// _cpr���Ƚ���
		// _alloc��������
		IntervalHeap(const _Cmpr& _cpr, _Alloc&& _alloc) :cpr{ _cpr }, alloc(std::forward<_Alloc>(_alloc)) {}
		// _cpr���Ƚ���
		// _alloc��������
		IntervalHeap(_Cmpr&& _cpr, _Alloc&& _alloc) :cpr{ std::move(_cpr) }, alloc(std::forward<_Alloc>(_alloc)) {}
		// ǳ����(С��ʹ�ã�ע����������)
		IntervalHeap(const IntervalHeap& cp, bool Shallowcopy) :cpr{ cp.cpr }, alloc(cp.alloc), src(cp.src), size(cp.size), capacity(cp.capacity) {}
		// ��������(���)
		IntervalHeap(const IntervalHeap& cp) : cpr{ cp.cpr }, alloc(cp.alloc), src(alloc.New(cp.capacity)), size(cp.size), capacity(cp.capacity) {
			std::copy_n(cp.src, cp.size, this->src);
		}
		// �ƶ�����
		IntervalHeap(IntervalHeap&& mv) noexcept :cpr{ std::move(mv.cpr) }, alloc(std::move(mv.alloc)), src(mv.src), size(mv.size), capacity(mv.capacity) {
			memset(&mv, 0, sizeof(IntervalHeap<_Ty, LessTag, _Cmpr, _Alloc>));
		}
		// ������
		// st����Ԫ�ص�ַ
		// ed��βԪ�ص���һ��λ��
		template<typename _Init>
		IntervalHeap(_Init st, _Init ed) {
			this->BuildHeap(st, ed);
		}
		// ������
		// st����Ԫ�ص�ַ
		// ed��βԪ�ص���һ��λ��
		// _cpr���Ƚ���
		template<typename _Init>
		IntervalHeap(_Init st, _Init ed, const _Cmpr& _cpr) :IntervalHeap(_cpr) {
			this->BuildHeap(st, ed);
		}
		// ������
		// st����Ԫ�ص�ַ
		// ed��βԪ�ص���һ��λ��
		// _cpr���Ƚ���
		template<typename _Init>
		IntervalHeap(_Init st, _Init ed, _Cmpr&& _cpr) :IntervalHeap(std::move(_cpr)) {
			this->BuildHeap(st, ed);
		}
		// ������
		// st����Ԫ�ص�ַ
		// ed��βԪ�ص���һ��λ��
		// _alloc��������
		template<typename _Init>
		IntervalHeap(_Init st, _Init ed, _Alloc&& _alloc) :IntervalHeap(std::forward<_Alloc>(_alloc)) {
			this->BuildHeap(st, ed);
		}
		// ������
		// st����Ԫ�ص�ַ
		// ed��βԪ�ص���һ��λ��
		// _cpr���Ƚ���
		// _alloc��������
		template<typename _Init>
		IntervalHeap(_Init st, _Init ed, const _Cmpr& _cpr, _Alloc&& _alloc) :IntervalHeap(_cpr, std::forward<_Alloc>(_alloc)) {
			this->BuildHeap(st, ed);
		}
		// ������
		// st����Ԫ�ص�ַ
		// ed��βԪ�ص���һ��λ��
		// _cpr���Ƚ���
		// _alloc��������
		template<typename _Init>
		IntervalHeap(_Init st, _Init ed, _Cmpr&& _cpr, _Alloc&& _alloc) :IntervalHeap(std::move(_cpr), std::forward<_Alloc>(_alloc)) {
			this->BuildHeap(st, ed);
		}

		// �б��ʼ��
		IntervalHeap(const std::initializer_list<_Ty>& lst) :IntervalHeap(lst.begin(), lst.size()) {}

		// ��ֵ(���)
		IntervalHeap& operator= (const IntervalHeap& cp) {
			this->~IntervalHeap();
			new (this) IntervalHeap(cp);
			return *this;
		}
		// ��ֵ(�ƶ���ֵ)
		IntervalHeap& operator= (IntervalHeap&& mv) noexcept {
			this->~IntervalHeap();
			new (this) IntervalHeap(std::move(mv));
			return *this;
		}

		// ��������
		~IntervalHeap() {
			this->alloc.Free(this->src, this->size);
			memset(this, 0, sizeof(IntervalHeap<_Ty, LessTag, _Cmpr, _Alloc>));
		}

		// ѹ��Ԫ��
		void Push(const _Ty& val) { this->Emplace(val); }
		// ѹ��Ԫ��
		void Push(_Ty&& val) { this->Emplace(std::move(val)); }
		// ֱ�ӹ���
		template<typename... Args>
		void Emplace(Args&&... args) {
			// ��������������
			if (this->size == this->capacity) this->Expand();
			new(this->src + this->size) _Ty(std::forward<Args>(args)...);
			// ��Ԫ�����ڽ���±�
			size_t x = this->size >> 1;

			// ���б�����������Ԫ��ʱ
			// ��Ԫ��ѹ��β���
			// ��ʱ��Ҫ�Ƚ����ڲ������������϶ѻ�
			if (this->size & 1) {
				if (this->cpr(this->src[this->size], this->src[this->size - 1])) {
					dsl::Swap(this->src[this->size], this->src[this->size - 1]);
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
				if (this->cpr(p[x].L, p[fa].L)) this->HeapUp(x, true);
				else if (this->cpr(p[fa].R, p[x].L)) {
					dsl::Swap(p[fa].R, p[x].L);
					this->HeapUp(fa, false);
				}
			}
			++this->size;
		}

		// ɾ�����ֵ
		void PopMax() {
			if (!this->size) {
#ifdef EXCEPTION_DETECTION
				throw std::exception("object of IntervalHeap��none element by PopMax()");
#endif // EXCEPTION_DETECTION
				return;
			}
			if (this->size <= 2) this->Erase(this->size);
			else {
				this->Erase(2);
				// ע�⣺Erase�Ѿ���size-1��
				memcpy(this->src + 1, this->src + this->size, sizeof(_Ty));
				this->HeapDown(0, false);
			}
		}

		// ɾ�����ֵ
		// popVal������ɾ����Ԫ��
		void PopMax(_Ty& popVal) {
			if (!this->size) {
#ifdef EXCEPTION_DETECTION
				throw std::exception("object of IntervalHeap��none element by PopMax()");
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

		// ɾ����Сֵ
		void PopMin() {
			if (!this->size) {
#ifdef EXCEPTION_DETECTION
				throw std::exception("object of IntervalHeap��none element by PopMax()");
#endif // EXCEPTION_DETECTION
				return;
		}
			if (this->size == 1) this->Erase(1);
			else {
				this->Erase(1);
				// ע�⣺Erase�Ѿ���size-1��
				memcpy(this->src, this->src + this->size, sizeof(_Ty));
				this->HeapDown(0, true);
			}
}

		// ɾ����Сֵ
		// popVal������ɾ����Ԫ��
		void PopMin(_Ty& popVal) {
			if (!this->size) {
#ifdef EXCEPTION_DETECTION
				throw std::exception("object of IntervalHeap��none element by PopMax()");
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

		// �������ֵ
		_Ty Max()const {
#ifdef EXCEPTION_DETECTION
			if (!this->size) throw std::exception("object of IntervalHeap��none element by PopMax()");
#endif // EXCEPTION_DETECTION
			return this->size == 1 ? this->src[0] : this->src[1];
		}
		// ������Сֵ
		_Ty Min()const {
#ifdef EXCEPTION_DETECTION
			if (!this->size) throw std::exception("object of IntervalHeap��none element by PopMax()");
#endif // EXCEPTION_DETECTION
			return this->src[0];
		}

		// �жϺ����Ƿ�Ϊ��
		inline bool Empty()const {
			return this->size;
		}

		// ����Ԫ�ظ���
		inline size_t Size()const { return this->size; }
		// ������������
		inline size_t Capacity()const { return this->capacity; }

		// ������������
		// ����0	����
		// ����-1	Ԫ�ر��ض�
		int Reserve(size_t cap) {
			// ������Դ
			_Ty* buf = this->alloc.New(cap);

			// ����Դ����
			if (this->src) {
				// ��������(ǳ����)
				memcpy(buf, this->src, std::min(cap, this->size) * sizeof(_Ty));
				// �ͷž���Դ
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

		// ����Դ���ݵ�ַ
		_Ty* Data() {
			return this->src;
		}

		// ���Ԫ�أ����ͷ���Դ
		void Clear() {
			for (_Ty* st = src, *ed = src + this->size; st != ed; ++st) st->~_Ty();
			this->size = 0;
		}

	protected:
		// �Ƚ���
		Cmpr cpr;
		// ������
		_Alloc alloc;
		// Դ���ݵ�ַ
		// ÿ�������һ�����ݣ���С�Ҵ�
		_Ty* src = nullptr;
		// Ԫ�ظ���
		size_t size = 0;
		// ������С
		size_t capacity = 0;

		};
}