#pragma once
#include "pch.h"
#include "Allocater.h"
#include "Algorithm.h"

namespace dsl {

	template<class _KTy, class _VTy>
	class SplayNode;

	// 伸展树	
	// _KTy			键类型
	// _VTy			值类型
	// _Cmpr		比较器
	// _Alloc		分配器模板
	template<typename _KTy, typename _VTy, typename _Cmpr = std::less<_KTy>, template<typename _NTy> typename _Alloc = dsl::Allocater>
	class SplayTree {
		using SNode = SplayNode<_KTy, _VTy>;

	protected:
		// 向上维护
		// x：维护的结点
		virtual void PushUp(SNode* x) {
			x->sum = x->cnt;
			if (x->ch[0]) x->sum += x->ch[0]->sum;
			if (x->ch[1]) x->sum += x->ch[1]->sum;
		}

		// 向下维护
		// x：维护的结点
		virtual void PushDown(SNode* x) {
			// 具体功能通过多态实现
		}

		// 旋转函数：向上旋转一次（左旋/右旋）
		// x：旋转的结点
		void Rotate(SNode* x) {
			SNode* y = x->fa, * z = y->fa;
			bool idx = x == y->ch[1];
			y->ch[idx] = x->ch[!idx];
			y->fa = x;
			if (x->ch[!idx]) x->ch[!idx]->fa = y;
			x->ch[!idx] = y;
			x->fa = z;
			if (z) z->ch[z->ch[1] == y] = x;
			PushUp(y);
			PushUp(x);
		}

		// 核心函数
		// x：旋转起点
		// fa：最终位置的父节点，默认旋转到根节点
		void Splay(SNode* x, SNode* fa = nullptr) {
			while (x->fa != fa) {
				SNode* y = x->fa, * z = y->fa;
				if (z != fa) {
					if ((x == y->ch[1]) == (y == z->ch[1])) this->Rotate(y);
					else this->Rotate(x);
				}
				this->Rotate(x);
			}
			if (!fa) this->root = x;
		}

	public:
		// 默认构造
		SplayTree() = default;
		
		// 虚析构函数
		virtual ~SplayTree() {
			
		}

		// 插入函数
		void Insert() {

		}

	protected:
		// 根结点
		SNode* root = nullptr;
		// 比较器
		_Cmpr cpr;
		// 分配器
		_Alloc<SNode> alloc;
	};

	// 伸展树结点
	// _KTy			键类型
	// _VTy			值类型
	template<class _KTy, class _VTy>
	class SplayNode {
		template<typename _KTy, typename _VTy, typename _Cmpr, template<typename _NTy> typename _Alloc>
		friend class SplayTree;
	public:
		//默认构造
		SplayNode() = default;

	protected:
		// 父节点
		SplayNode* fa = nullptr;
		// 0：左孩子
		// 1：右孩子
		SplayNode* ch[2]{ nullptr, nullptr };
		// 相同元素个数
		size_t cnt = 1;
		// 子树元素总数
		size_t sum = 1;
		// 键
		_KTy key;
		// 值
		_KTy val;
	};
}