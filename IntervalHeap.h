#pragma once
#include "pch.h"
#include "Allocator.h"
#include "Algorithm.h"

#define L		first
#define R		second

namespace dsl {
	// ������ṹ
	// _Ty			Ԫ������
	// LessTag		�Ƚ�����־(�����ô��ڱȽ�������true)
	// _Cmpr		�Ƚ���(Ĭ�ϲ���С�ڱȽ���)
	// _Alloc		������ģ��
	template<typename _Ty, bool LessTag = false, typename _Cmpr = std::less<_Ty>, template<typename> typename _Alloc = dsl::Allocator>
	class IntervalHeap {
	public:
		// Ԫ�ط���������
		using _ElemAlloc = _Alloc<_Ty>;

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
			if (offset < 1 || offset > this->size) throw std::exception("object of IntervalHeap: invalid offset by Erase()");
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
			// i�����ӽ���±�
			// cnt��������
			size_t i, cnt;

			// С�ѵ���
			if (minHeap) {
				_Ty tmp = std::move(p[x].L);
				cnt = (this->size >> 1) + (this->size & 1);

				// ����ڲ�����  
				// ���Ԫ�ظ���Ϊ������β��㲻�õ���
				if (x < cnt - (this->size & 1) && this->cpr(p[x].R, tmp)) std::swap(tmp, p[x].R);

				for (i = (x << 1) + 1; i < cnt; ++(i <<= 1)) {
					if (i + 1 < cnt && this->cpr(p[i + 1].L, p[i].L)) ++i;
					if (this->cpr(tmp, p[i].L)) break;
					p[x].L = std::move(p[i].L);
					x = i;
					// ����ڲ�����
					if (x < cnt - (this->size & 1) && this->cpr(p[x].R, tmp)) std::swap(tmp, p[x].R);
				}
				p[x].L = std::move(tmp);
			}
			// ��ѵ���
			else {
				_Ty tmp = std::move(p[x].R);
				// ���Ԫ�ظ���Ϊ������β��㲻�ô���
				cnt = this->size >> 1;
				// ����ڲ�����
				if (this->cpr(tmp, p[x].L)) std::swap(p[x].L, tmp);
				for (i = (x << 1) + 1; i < cnt; ++(i <<= 1)) {
					if (i + 1 < cnt && this->cpr(p[i].R, p[i + 1].R)) ++i;
					if (this->cpr(p[i].R, tmp)) break;
					p[x].R = std::move(p[i].R);
					x = i;
					// ����ڲ�����
					if (this->cpr(tmp, p[x].L)) std::swap(p[x].L, tmp);
				}
				p[x].R = std::move(tmp);
			}
		}
		// ���϶ѻ�
		// x:			����±�
		// minHeap:		true����С�ѣ�false�������
		void HeapUp(size_t x, bool minHeap) {
			// ���϶ѻ�ǰ�������ڲ�����
			// �ѻ��в����ٽ����ڲ�����

			std::pair<_Ty, _Ty>* p = (std::pair<_Ty, _Ty>*) this->src;
			// fa:	�����
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
				for (; i < j; ++i) if (this->cpr(p[i].R, p[i].L)) std::swap(p[i].L, p[i].R);
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
		IntervalHeap(_ElemAlloc&& _alloc) :alloc(std::forward<_ElemAlloc>(_alloc)) {}
		// _cpr���Ƚ���
		// _alloc��������
		IntervalHeap(const _Cmpr& _cpr, _ElemAlloc&& _alloc) :cpr{ _cpr }, alloc(std::forward<_ElemAlloc>(_alloc)) {}
		// _cpr���Ƚ���
		// _alloc��������
		IntervalHeap(_Cmpr&& _cpr, _ElemAlloc&& _alloc) :cpr{ std::move(_cpr) }, alloc(std::forward<_ElemAlloc>(_alloc)) {}
		// ǳ����(С��ʹ�ã�ע����������)
		IntervalHeap(const IntervalHeap& cp, bool Shallowcopy) :cpr{ cp.cpr }, alloc(cp.alloc), src(cp.src), size(cp.size), capacity(cp.capacity) {}
		// ��������(���)
		IntervalHeap(const IntervalHeap& cp) : cpr{ cp.cpr }, alloc(cp.alloc), src(alloc.New(cp.capacity)), size(cp.size), capacity(cp.capacity) {
			std::copy_n(cp.src, cp.size, this->src);
		}
		// �ƶ�����
		IntervalHeap(IntervalHeap&& mv) noexcept :cpr{ std::move(mv.cpr) }, alloc(std::move(mv.alloc)), src(mv.src), size(mv.size), capacity(mv.capacity) {
			memset(&mv, 0, sizeof(IntervalHeap));
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
		IntervalHeap(_Init st, _Init ed, _ElemAlloc&& _alloc) :IntervalHeap(std::forward<_ElemAlloc>(_alloc)) {
			this->BuildHeap(st, ed);
		}
		// ������
		// st����Ԫ�ص�ַ
		// ed��βԪ�ص���һ��λ��
		// _cpr���Ƚ���
		// _alloc��������
		template<typename _Init>
		IntervalHeap(_Init st, _Init ed, const _Cmpr& _cpr, _ElemAlloc&& _alloc) :IntervalHeap(_cpr, std::forward<_ElemAlloc>(_alloc)) {
			this->BuildHeap(st, ed);
		}
		// ������
		// st����Ԫ�ص�ַ
		// ed��βԪ�ص���һ��λ��
		// _cpr���Ƚ���
		// _alloc��������
		template<typename _Init>
		IntervalHeap(_Init st, _Init ed, _Cmpr&& _cpr, _ElemAlloc&& _alloc) :IntervalHeap(std::move(_cpr), std::forward<_ElemAlloc>(_alloc)) {
			this->BuildHeap(st, ed);
		}

		// �б��ʼ��
		IntervalHeap(std::initializer_list<_Ty> lst) :IntervalHeap(lst.begin(), lst.end()) {}

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
			if (this->src) this->alloc.Free(this->src, this->size);
		}

		// ѹ��Ԫ��
		void Push(const _Ty& val) { this->Emplace(val); }
		// ѹ��Ԫ��
		void Push(_Ty&& val) { this->Emplace(std::move(val)); }

		// ֱ�ӹ���
		template<typename... _Args>
		void Emplace(_Args&&... args) {
			// ��������������
			if (this->size == this->capacity) this->Expand();
			new(this->src + this->size) _Ty(std::forward<_Args>(args)...);
			// ��Ԫ�����ڽ���±�
			size_t x = this->size >> 1;

			// ���б�����������Ԫ��ʱ
			// ��Ԫ��ѹ��β���
			// ��ʱ��Ҫ�Ƚ����ڲ������������϶ѻ�
			if (this->size & 1) {
				if (this->cpr(this->src[this->size], this->src[this->size - 1])) {
					std::swap(this->src[this->size], this->src[this->size - 1]);
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
					std::swap(p[fa].R, p[x].L);
					this->HeapUp(fa, false);
				}
			}
			++this->size;
		}

		// ɾ�����ֵ
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
				// ע�⣺Erase�Ѿ���size-1��
				this->src[1] = std::move(this->src[this->size]);
				this->HeapDown(0, false);
			}
		}

		// ɾ�����ֵ
		// popVal������ɾ����Ԫ��
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

		// ɾ����Сֵ
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
				// ע�⣺Erase�Ѿ���size-1��
				this->src[0] = std::move(this->src[this->size]);
				this->HeapDown(0, true);
			}
		}

		// ɾ����Сֵ
		// popVal������ɾ����Ԫ��
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

		// �������ֵ
		_Ty Max()const {
#ifdef EXCEPTION_DETECTION
			if (!this->size) throw std::exception("object of IntervalHeap: none element by PopMax()");
#endif // EXCEPTION_DETECTION
			return this->src[this->size > 1];
		}
		// ������Сֵ
		_Ty Min()const {
#ifdef EXCEPTION_DETECTION
			if (!this->size) throw std::exception("object of IntervalHeap: none element by PopMax()");
#endif // EXCEPTION_DETECTION
			return this->src[0];
		}

		// �жϺ����Ƿ�Ϊ��
		inline bool Empty()const {
			return !this->size;
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
		_Ty* Source() {
			return this->src;
		}

		// ���Ԫ�أ����ͷ���Դ
		void Clear() {
			if constexpr (std::is_class<_Ty>::value) for (_Ty* st = src, *ed = src + this->size; st != ed; ++st) st->~_Ty();
			this->size = 0;
		}

	protected:
		// �Ƚ���
		Cmpr cpr;
		// ������
		_ElemAlloc alloc;
		// Դ���ݵ�ַ
		// ÿ�������һ�����ݣ���С�Ҵ�
		_Ty* src = nullptr;
		// Ԫ�ظ���
		size_t size = 0;
		// ������С
		size_t capacity = 0;

	};
}