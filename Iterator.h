#pragma once
#include "pch.h"

namespace dsl {
	template<typename _DSTy>
	class Iterator {
	protected:
		// 结构中元素类型
		using _SrcType = _DSTy::_ElemType;

	public:
		// 默认构造
		Iterator() = default;
		// 带参构造
		// _src：源数据指针
		Iterator(_SrcType* _src) : src(_src) {}
		// 拷贝构造
		Iterator(const Iterator& cp) : src(cp.src) {}
		// 移动构造
		Iterator(Iterator&& mv) noexcept :src(mv.src) {
			mv.src = nullptr;
		}

		// 虚析构
		virtual ~Iterator() {
			this->src = nullptr;
		}

		// 自增
		virtual Iterator& operator++() = 0;
		// 自减
		virtual Iterator& operator--() = 0;
		// 判等
		ptrdiff_t operator==(const Iterator& cpr) { return this->src == cpr.src; }
		// 解引用
		_SrcType& operator*() { return *this->src; }
		// 指针
		_SrcType* operator->() { return this->src; }


	protected:
		// 源数据指针
		_SrcType* src = nullptr;
	};

	template<typename _KTy, typename _VTy, typename _Cmpr, template<typename _NTy> typename _Alloc>
	class SplayTree;

	template<typename _DSTy>
	class SplayTree_Iterator : public Iterator<_DSTy>
	{
		template<typename _KTy, typename _VTy, typename _Cmpr, template<typename _NTy> typename _Alloc>
		friend class SplayTree;
	public:
		// 默认构造
		SplayTree_Iterator() = default;
		// 带参构造
		// _src：源数据指针
		SplayTree_Iterator(Iterator<_DSTy>::_SrcType* _src) : Iterator<_DSTy>(_src) {}
		// 拷贝构造
		SplayTree_Iterator(const SplayTree_Iterator& cp) : Iterator<_DSTy>(cp) {}
		// 移动构造
		SplayTree_Iterator(SplayTree_Iterator&& mv) noexcept :Iterator<_DSTy>(std::move(mv)) {}
		// 虚析构
		virtual ~SplayTree_Iterator() {}

		// 自增
		virtual Iterator<_DSTy>& operator++() {
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

		// 自减
		virtual Iterator<_DSTy>& operator--() {
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

	};
}
