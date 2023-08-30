#pragma once
#include "pch.h"

namespace dsl {
	template<typename _DSTy>
	class Iterator {
	public:
		using _ElemType = typename _DSTy::_ElemType;

	public:
		Iterator() = default;
		Iterator(_ElemType* _src) : src(_src) {}
		Iterator(const Iterator& cp) : src(cp.src) {}
		Iterator(Iterator&& mv) noexcept :src(mv.src) {
			memset(&mv, 0, sizeof(Iterator<_DSTy>));
		}

		virtual ~Iterator() {}

		// shift to successor
		virtual Iterator& operator++() = 0;
		// shift to precursor
		virtual Iterator& operator--() = 0;

		bool operator==(const Iterator& cpr) { return this->src == cpr.src; }
		bool operator!=(const Iterator& cpr) { return this->src != cpr.src; }
		_ElemType& operator*() { return *this->src; }
		_ElemType* operator->() { return this->src; }

		_ElemType* Source() { return this->src; }

	protected:
		_ElemType* src = nullptr;
	};

	template<typename _Node, typename _Cmpr, typename _Alloc>
	class SplayTree;

	template<typename _DSTy>
	class SplayTreeIterator : public Iterator<_DSTy>
	{
		template<typename _Node, typename _Cmpr, typename _Alloc>
		friend class SplayTree;
	public:
		using _ElemType = typename _DSTy::_ElemType;

	public:
		SplayTreeIterator() = default;
		SplayTreeIterator(_ElemType* _src) : Iterator<_DSTy>(_src) {}
		SplayTreeIterator(const SplayTreeIterator& cp) : Iterator<_DSTy>(cp) {}
		SplayTreeIterator(SplayTreeIterator&& mv) noexcept :Iterator<_DSTy>(std::move(mv)) {
			memset(&mv, 0, sizeof(SplayTreeIterator));
		}
		virtual ~SplayTreeIterator() {}

		// shift to successor
		Iterator<_DSTy>& operator++() {
#ifdef EXCEPTION_DETECTION
			if (!this->src) throw exception("object of SplayTree iterator£ºinvalid operation by ++ (null pointer of source data)");
			if (this->src->end) throw exception("object of SplayTree iterator£ºcan't be ++ any more");
#endif // EXCEPTION_DETECTION
			if (this->src->ch[1]) {
				this->src = this->src->ch[1];
				while (this->src->ch[0]) this->src = this->src->ch[0];
			}
			else {
				while (this->src == this->src->fa->ch[1]) this->src = this->src->fa;
				this->src = this->src->fa;
			}
			return *this;
		};

		// shift to successor then splay to the root
		Iterator<_DSTy>& Next(_DSTy& tree) {
			this->operator++();
			tree.Splay(this->src);
		}

		// shift to precursor
		Iterator<_DSTy>& operator--() {
#ifdef EXCEPTION_DETECTION
			if (!this->src) throw exception("object of SplayTree iterator£ºinvalid operation by -- (null pointer of source data)");
#endif // EXCEPTION_DETECTION

			if (this->src->ch[0]) {
				this->src = this->src->ch[0];
				while (this->src->ch[1]) this->src = this->src->ch[1];
			}
			else {
				while (this->src->fa && this->src == this->src->fa->ch[0]) this->src = this->src->fa;
#ifdef EXCEPTION_DETECTION
				if (!this->src->fa) throw exception("object of SplayTree iterator£ºcan't be -- any more");
#endif // EXCEPTION_DETECTION
				this->src = this->src->fa;
			}
			return *this;
		};
		// shift to precursor then splay to the root
		Iterator<_DSTy>& Prev(_DSTy& tree) {
			this->operator--();
			tree.Splay(this->src);
		}

	};
}
