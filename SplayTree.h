#pragma once
#include "pch.h"
#include "Allocater.h"
#include "Algorithm.h"
#include "Iterator.h"

namespace dsl {

	template<class _KTy, class _VTy, typename _Cmpr>
	class SplayNode;

	// 伸展树	
	// _KTy			键类型
	// _VTy			值类型
	// _Cmpr		比较器
	// _Alloc		分配器模板
	template<typename _KTy, typename _VTy, typename _Cmpr = std::less<_KTy>, template<typename _NTy> typename _Alloc = dsl::Allocater>
	class SplayTree {
	public:
		// 结点类型
		using _ElemType = SplayNode<_KTy, _VTy, _Cmpr>;
		// 迭代器类型
		using Iterator = SplayTree_Iterator<SplayTree<_KTy, _VTy, _Cmpr, _Alloc>>;

	protected:
		// 向上维护
		// x：维护的结点
		virtual void PushUp(_ElemType* x) {
			// 重载时注意尾结点
			x->sum = x->cnt;
			if (x->ch[0]) x->sum += x->ch[0]->sum;
			if (x->ch[1]) x->sum += x->ch[1]->sum;
		}

		// 向下维护
		// x：维护的结点
		virtual void PushDown(_ElemType* x) {
			// 重载时注意尾结点
			// 具体功能通过多态实现
		}

		// 旋转函数：向上旋转一次（左旋/右旋）
		// x：旋转的结点
		void Rotate(_ElemType* x) {
			_ElemType* y = x->fa, * z = y->fa;
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
		void Splay(_ElemType* x, _ElemType* fa = nullptr) {
			while (x->fa != fa) {
				_ElemType* y = x->fa, * z = y->fa;
				if (z != fa) {
					if ((x == y->ch[1]) == (y == z->ch[1])) this->Rotate(y);
					else this->Rotate(x);
				}
				this->Rotate(x);
			}
			if (!fa) this->root = x;
		}

	public:
		// 默认构造：添加尾节点
		SplayTree() : root(this->alloc.New(1)) {
			// 不直接调用构造函数，键值对可能没有默认构造函数
			this->root->fa = this->root->cnt[0] = this->root->cnt[1] = nullptr;
			this->root->ch = this->root->sum = 1;
			this->root->end = true;
		};

		// 虚析构函数
		virtual ~SplayTree() {
			std::queue<_ElemType*> qu;
			qu.push(this->root);
			while (qu.size()) {
				this->root = qu.front();
				qu.pop();
				if (this->root->ch[0]) qu.push(this->root->ch[0]);
				if (this->root->ch[1]) qu.push(this->root->ch[1]);
				this->alloc.Free(qu.front(), !this->root->end);
			}
		}

		// 清空元素
		void Clear() {
			std::queue<_ElemType*> qu;
			qu.push(this->root);
			_ElemType* tmp;
			while (qu.size()) {
				tmp = qu.front();
				qu.pop();
				if (tmp->ch[0]) qu.push(tmp->ch[0]);
				if (tmp->ch[1]) qu.push(tmp->ch[1]);
				if (!tmp->end) this->alloc.Free(tmp, 1);
				else {
					this->root = tmp;
					this->root->fa = this->root->cnt[0] = this->root->cnt[1] = nullptr;
					this->root->ch = this->root->sum = 1;
					this->root->end = true;
				}
			}
		}

		// 元素个数
		size_t Size() { return this->root->sum - 1; }

		// 判断是否为空
		bool Empty() { return !(this->root->sum - 1); }

		// 返回根节点
		_ElemType* GetRoot() { return this->root; }

		// 返回首迭代器
		Iterator Begin() {
			Iterator res(this->root);
			while (res->src->ch[0]) res->src = res->src->ch[0];
			this->Splay(res->src);
			return res;
		}
		// 功能与Begin相同
		// 适应C++的基本范围for循环
		Iterator begin() {
			return this->Begin();
		}

		// 返回尾迭代器
		Iterator End() {
			Iterator res(this->root);
			while (res->src->ch[1]) res->src = res->src->ch[1];
			this->Splay(res->src);
			return res;
		}
		// 功能与End相同
		// 适应C++的基本范围for循环
		Iterator end() {
			return this->End();
		}

		// 通过键查询值
		// key：比较键
		// 不存在则返回尾迭代器
		Iterator Find(const _KTy& key) {
			_ElemType u = this->root;
			while (u && u != key) u = u->ch[!(u < key)];
			if (!u) return this->End();
			this->Splay(u);
			return Iterator(u);
		}

		// 通过排名查询值
		// rk：排名(从1开始)
		// 不存在则返回尾迭代器
		Iterator Find(size_t rk) {
			if (rk >= this->root->sum) return this->End();
			_ElemType* u = this->root;
			while (1) {
				if (u->ch[0]) {
					if (rk <= u->ch[0]->sum) {
						u = u->ch[0];
						continue;
					}
					rk -= u->ch[0]->sum;
				}
				if (rk <= u->cnt) break;
				rk -= u->cnt;
				u = u->ch[1];
			}
			this->Splay(u);
			return Iterator(u);
		}

		// 查询排名
		// 若有多个值，返回的是最小排名
		// key：键
		// 键值不存在则返回0
		size_t GetRank(const _KTy& key) {
			if (this->Find(key)->end) return 0;
			return this->root->ch[0] ? this->root->ch[0]->sum + 1 : 1;
		}

		// 查询排名
		// 若有多个值，返回的是最小排名
		// itr：目标节点的迭代器
		// 无效迭代器返回0或发生异常
		size_t GetRank(const Iterator& itr) {
			this->Splay(itr->src);
			return this->root->ch[1] ? (this->root->ch[0] ? this->root->ch[0]->sum + 1 : 1) : 0;
		}

		// 查询前驱节点
		// key：键
		// 不存在则返回尾迭代器
		//Iterator GetPrecursor(const _KTy& key) {
		//	Iterator res = this->Find(key);
		//	if (!res->src->ch[0]) return this->End();
		//	this->Splay((--res)->src);
		//	return res;
		//}

		// 查询后继节点
		// key：键
		// 不存在则返回尾迭代器
		//Iterator GetSuccessor(const _KTy& key) {
		//	Iterator res = this->Find(key);
		//	if (!res->src->ch[0]) return this->End();
		//	this->Splay((--res)->src);
		//	return res;
		//}

		// 插入函数
		//void Insert() {

		//}

		//template<typename... _Args>
		//void Emplace(_Args&&... args) {
		//	//// 新结点
		//	//_ElemType* newNode = this->alloc.New(1);
		//	//new (newNode) _ElemType(std::forward<_Args>(args)...);

		//	//// 空树
		//	//if (!this->root) {
		//	//	this->root = newNode;
		//	//	return;
		//	//}

		//	//// 寻找插入位置
		//	//_ElemType* u = this->root, * fa = nullptr;
		//	//while (u && (this->cpr(u->key, newNode->key) != this->cpr(newNode->key, u->key))) {
		//	//	fa = u;
		//	//	u = u->ch[this->cpr(newNode->key, u->key)];
		//	//}


		//}


	protected:
		// 分配器
		_Alloc<_ElemType> alloc;
		// 根结点
		_ElemType* root;
	};

	// 伸展树结点
	// _KTy			键类型
	// _VTy			值类型
	template<typename _KTy, typename _VTy, typename _Cmpr>
	class SplayNode {
		template<typename _KTy, typename _VTy, typename _Cmpr, template<typename _NTy> typename _Alloc>
		friend class SplayTree;
	public:
		// 比较符重载
		bool operator < (const SplayNode<_KTy, _VTy, _Cmpr>& val) {
			return !this->end && (val.end || this->cpr(this->key, val->key));
		}
		bool operator < (const _KTy& key) {
			return !this->end && this->cpr(this->key, key);
		}
		friend bool operator < (const _KTy& key, const SplayNode<_KTy, _VTy, _Cmpr>& val) {
			return val.end || this->cpr(key, val.key);
		}
		bool operator == (const SplayNode<_KTy, _VTy, _Cmpr>& val) {
			return *this < val == val < *this;
		}
		bool operator == (const _KTy& val) {
			return *this < val == val < *this;
		}
		bool operator != (const SplayNode<_KTy, _VTy, _Cmpr>& val) {
			return *this < val != val < *this;
		}
		bool operator != (const _KTy& val) {
			return *this < val != val < *this;
		}


		// 虚析构
		virtual ~SplayNode() = default;

	protected:
		// 默认构造
		SplayNode() = default;
		// 初始化键、值
		SplayNode(const _KTy& _key, const _VTy& _val) :key(_key), val(_val) {}
		// 初始化键、值
		SplayNode(_KTy&& _key, const _VTy& _val) :key(std::move(_key)), val(_val) {}
		// 初始化键、值
		SplayNode(const _KTy& _key, _VTy&& _val) :key(_key), val(std::move(_val)) {}
		// 初始化键、值
		SplayNode(_KTy&& _key, _VTy&& _val) :key(std::move(_key)), val(std::move(_val)) {}

	protected:
		// 父节点
		SplayNode* fa = nullptr;
		// 0：左孩子
		// 1：右孩子
		SplayNode* ch[2]{ nullptr, nullptr };
		// 子树元素总数
		size_t sum = 1;
		// 比较器
		_Cmpr cpr;

	public:
		// 相同元素个数
		size_t cnt = 1;
		// 键
		_KTy key;
		// 值
		_KTy val;
		// 哨兵节点标记
		bool end = false;

	};
}