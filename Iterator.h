#pragma once
#include "pch.h"

namespace dsl {
	template<typename _DSTy>
	class Iterator {
	public:
		// 结构中元素类型
		using _ElemType = typename _DSTy::_ElemType;

	public:
		// 默认构造
		Iterator() = default;
		// 带参构造
		// _src：源数据指针
		Iterator(_ElemType* _src) : src(_src) {}
		// 拷贝构造
		Iterator(const Iterator& cp) : src(cp.src) {}
		// 移动构造
		Iterator(Iterator&& mv) noexcept :src(mv.src) {
			mv.src = nullptr;
		}

		// 虚析构
		virtual ~Iterator() {}

		// 自增
		virtual Iterator& operator++() = 0;
		// 自减
		virtual Iterator& operator--() = 0;
		// 判等
		bool operator==(const Iterator& cpr) { return this->src == cpr.src; }
		// 不等
		bool operator!=(const Iterator& cpr) { return this->src != cpr.src; }
		// 解引用
		_ElemType& operator*() { return *this->src; }
		// 指针
		_ElemType* operator->() { return this->src; }


	protected:
		// 源数据指针
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
		// 结构中元素类型
		using _ElemType = typename _DSTy::_ElemType;

	public:
		// 默认构造
		SplayTreeIterator() = default;
		// 带参构造
		// _src：源数据指针
		SplayTreeIterator(_ElemType* _src) : Iterator<_DSTy>(_src) {}
		// 拷贝构造
		SplayTreeIterator(const SplayTreeIterator& cp) : Iterator<_DSTy>(cp) {}
		// 移动构造
		SplayTreeIterator(SplayTreeIterator&& mv) noexcept :Iterator<_DSTy>(std::move(mv)) {}
		// 虚析构
		virtual ~SplayTreeIterator() {}

		// 自增
		Iterator<_DSTy>& operator++() {
#ifdef EXCEPTION_DETECTION
			if (!this->src) throw exception("object of SplayTree iterator：invalid operation by ++ (null pointer of source data)");
			if (this->src->end) throw exception("object of SplayTree iterator：can't be ++ any more");
#endif // EXCEPTION_DETECTION
			if (this->src->ch[1]) {
				// 右子树的最左结点
				this->src = this->src->ch[1];
				while (this->src->ch[0]) this->src = this->src->ch[0];
			}
			else {
				// 搜索到自己为父节点的左节点
				while (this->src == this->src->fa->ch[1]) this->src = this->src->fa;
				this->src = this->src->fa;
			}
			return *this;
		};
		// 自增后旋转到根节点
		Iterator<_DSTy>& Next(_DSTy& tree) {
			this->operator++();
			tree.Splay(this->src);
		}

		// 自减
		Iterator<_DSTy>& operator--() {
#ifdef EXCEPTION_DETECTION
			if (!this->src) throw exception("object of SplayTree iterator：invalid operation by -- (null pointer of source data)");
#endif // EXCEPTION_DETECTION

			if (this->src->ch[0]) {
				// 左子树的最右结点
				this->src = this->src->ch[0];
				while (this->src->ch[1]) this->src = this->src->ch[1];
			}
			else {
				// 搜索到自己为父节点的右节点
				while (this->src->fa && this->src == this->src->fa->ch[0]) this->src = this->src->fa;
#ifdef EXCEPTION_DETECTION
				if (!this->src->fa) throw exception("object of SplayTree iterator：can't be -- any more");
#endif // EXCEPTION_DETECTION
				this->src = this->src->fa;
			}
			return *this;
		};
		// 自减后旋转到根节点
		Iterator<_DSTy>& Prev(_DSTy& tree) {
			this->operator--();
			tree.Splay(this->src);
		}

	};
}
