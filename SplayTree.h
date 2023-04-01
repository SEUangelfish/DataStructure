#pragma once
#include "pch.h"
#include "Allocater.h"
#include "Algorithm.h"

#define INF		0xffffffffffffffffui64

namespace dsl {
	// Splay树		
	// 键与值相等
	// _Ty			键、值类型
	// _Cmpr		比较器
	// _Alloc		分配器(实际的元素单元是Node类，所以这里需要使用模板嵌套)
	template<typename _Ty, typename _Cmpr = std::less<_Ty>, template<typename _NdTy> typename _Alloc = Allocater>
	class SplayTree {
	protected:
		// 结点类
		class Node {
			friend class SplayTree;
		public:
			// 默认构造
			Node() :fa(INF), ch{ INF,INF } {}
			// 带参构造
			template<typename... Args>
			Node(size_t _fa, size_t _ch[2], Args&&... _val) : fa(_fa), ch({ _ch[0],_ch[1] }), val(_val...) {}

			// 默认析构
			virtual ~Node() {
				fa = INF;
				ch[0] = ch[1] = INF;
			}

			// 重载小于符号
			// 不一定是小于比较准则，只是为了后续方便使用
			bool operator < (const Node& cpr) const { return _Cmpr()(this->val, cpr.val); }
			bool operator < (const _Ty& cpr) const { return _Cmpr()(this->val, cpr); }

			// 重载==符号
			bool operator == (const Node& cpr) const {
				_Cmpr cpr;
				return cpr(this->val, cpr.val) == cpr(cpr.val, this->val);
			}

		protected:
			// 父结点下标
			size_t fa;
			// 左右孩子结点下标
			size_t ch[2];
			// 键值
			_Ty val;
		};

	protected:
		// 询问子结点，维护本节点信息
		// 供子类扩展SplayTree功能
		inline virtual void PushUp(size_t x) {}

		// 旋转函数
		// 将指定下标结点向上旋转(左旋/右旋)一个位置
		// 默认认为本结点有父结点，若无父结点则会出错
		// x		结点下标
		void Rotate(size_t x) {
			Node* tr = (Node*)this->src;
			// y	父结点
			// z	爷爷结点
			size_t y = tr[x].fa, z = tr[y].fa;
			bool chk = tr[y].ch[1] == x;
			// 调整父结点关系
			tr[y].fa = x;
			tr[y].ch[chk] = tr[x].ch[!chk];
			if (tr[x].ch[!chk]) tr[x].ch[!chk].fa = y;
			// 调整本节点关系
			tr[x].fa = z;
			tr[x].ch[!chk] = y;
			if (z ^ INF) tr[z].ch[tr[z].ch[1] == y] = x;
			this->PushUp(y);
			this->PushUp(x);
		}

		// 扩容函数(每次扩容2倍)
		void Expand() {
			// 分配资源
			size_t cap = this->capacity ? this->capacity << 1 : 2;
#ifdef EXCEPTION_DETECTION
			if (cap <= 0) throw std::exception("fail to expand");
#endif // EXCEPTION_DETECTION
			this->Reserve(cap);
		}

		// 寻找插入位置
		// val		新值
		// 返回父结点下标
		virtual size_t Find(const _Ty& val) const {
			// 空树
			if (!this->size) return INF;
			// 一直搜索到叶子结点处
			size_t cur = this->root, fa = INF;
			Node* tr = (Node*)this->src;
			while (cur ^ INF) {
				fa = cur;
				cur = tr[cur] < val ? tr[cur].ch[0] : tr[cur].ch[1];
			}
			return fa;
		}

	public:
		// 默认构造
		SplayTree() :src(nullptr), root(INF), size(0), capacity(0) {}

		// 默认析构
		virtual ~SplayTree() {
			_Alloc<Node>().Free(this->src, this->size);
			this->root = INF;
			this->size = 0;
			this->capacity = 0;
		}

		// 伸展函数
		// 将下标为x的结点旋转到下标为k的结点下面
		void Splay(size_t x, size_t k = INF) {
			Node* tr = (Node*)this->src;
			while (tr[x].fa ^ k) {
				size_t y = tr[x].fa, z = tr[y].fa;
				if (z ^ k) {
					if ((tr[y].ch[1] == x) ^ (tr[z].ch[1] == y)) this->Rotate(x);
					else this->Rotate(y);
				}
				this->Rotate(x);
			}
		}

		// 设置容器容量
		// 如果新容量小于当前元素个数则会出现错误
		void Reserve(size_t cap) {
#ifdef EXCEPTION_DETECTION
			if (cap < this->size) throw std::exception("insufficient capacity");
#endif // EXCEPTION_DETECTION

			// 申请资源
			void* buf = _Alloc<Node>().New(cap);

			// 拷贝源数据
			if (this->src) {
				// 拷贝数据
				memcpy_s(buf, cap * sizeof(Node), this->src, this->size * sizeof(Node));
				// 释放旧资源
				free(this->src);
			}
			this->src = buf;
			this->capacity = cap;
		}


		template<typename... Args>
		void Emplace(Args&&... args) {
			// 容量不足则扩容
			if (this->size == this->capacity) this->Expand();
			Node* tr = (Node*)this->src;
			// 构造新值
			new(&tr[this->size]) Node(INF, { INF,INF }, args...);
			// 寻找插入位置
			size_t x = this->size;
			size_t fa = this->Find(tr[this->size++]);
			// 更新结点信息
			if (fa ^ INF) {
				tr[x].fa = fa;
				tr[fa].ch[]
			}
			this->Splay(x);
		}

	protected:
		// 源数据
		void* src;
		// 根结点下标
		size_t root;
		// 元素个数
		size_t size;
		// 容器容量
		size_t capacity;

	};
}