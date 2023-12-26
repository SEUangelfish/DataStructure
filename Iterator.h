#pragma once
#include "pch.h"



namespace dsl {
	template<typename _DSTy>
	class Iterator {
	public:
		using _ElemType = typename _DSTy::_ElemType;

	public:
		Iterator(_ElemType* _src) : src(_src) {}
		Iterator(const Iterator& cp) : src(cp.src) {}

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
		using Iterator = Iterator<_DSTy>;

	public:
		SplayTreeIterator() = default;
		SplayTreeIterator(_ElemType* _src, _DSTy* _tree) : Iterator(_src), tree(_tree) {}
		SplayTreeIterator(const SplayTreeIterator& cp) : Iterator(cp), tree(cp.tree) {}

		virtual ~SplayTreeIterator() {}

		// shift to successor
		Iterator& operator++() {
#ifdef EXCEPTION_DETECTION
			if (!this->src || !this->tree) throw std::exception("object of SplayTree iterator: invalid operation by ++ (null pointer of source data)");
			if (this->src == this->tree->End().src) throw std::exception("object of SplayTree iterator: can't be ++ any more");
#endif // EXCEPTION_DETECTION
			if (this->src->ch[1]) {
				this->src = this->src->ch[1];
				while (this->src->ch[0]) this->src = this->src->ch[0];
			}
			else {
				while (this->src == this->src->fa->ch[1]) this->src = this->src->fa;
				this->src = this->src->fa;
			}

			this->tree->Splay(this->src);
			return *this;
		};

		// shift to precursor
		Iterator& operator--() {
#ifdef EXCEPTION_DETECTION
			if (!this->src || !this->tree) throw std::exception("object of SplayTree iterator: invalid operation by -- (null pointer of source data)");
			if (*this == this->tree->Begin()) throw std::exception("object of SplayTree iterator: can't be -- any more");
#endif // EXCEPTION_DETECTION

			if (this->src->ch[0]) {
				this->src = this->src->ch[0];
				while (this->src->ch[1]) this->src = this->src->ch[1];
			}
			else {
				while (this->src->fa && this->src == this->src->fa->ch[0]) this->src = this->src->fa;
				this->src = this->src->fa;
			}


			this->tree->Splay(this->src);
			return *this;
		};

	protected:
		_DSTy* tree = nullptr;
	};
}
