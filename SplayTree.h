#pragma once
#include "pch.h"
#include "Allocater.h"
#include "Algorithm.h"

#define INF		0xffffffffffffffffui64

namespace dsl {
	// Splay��		
	// ����ֵ���
	// _Ty			����ֵ����
	// _Cmpr		�Ƚ���
	// _Alloc		������(ʵ�ʵ�Ԫ�ص�Ԫ��Node�࣬����������Ҫʹ��ģ��Ƕ��)
	template<typename _Ty, typename _Cmpr = std::less<_Ty>, template<typename _NdTy> typename _Alloc = Allocater>
	class SplayTree {
	protected:
		// �����
		class Node {
			friend class SplayTree;
		public:
			// Ĭ�Ϲ���
			Node() :fa(INF), ch{ INF,INF } {}
			// ���ι���
			template<typename... Args>
			Node(size_t _fa, size_t _ch[2], Args&&... _val) : fa(_fa), ch({ _ch[0],_ch[1] }), val(_val...) {}

			// Ĭ������
			virtual ~Node() {
				fa = INF;
				ch[0] = ch[1] = INF;
			}

			// ����С�ڷ���
			// ��һ����С�ڱȽ�׼��ֻ��Ϊ�˺�������ʹ��
			bool operator < (const Node& cpr) const { return _Cmpr()(this->val, cpr.val); }
			bool operator < (const _Ty& cpr) const { return _Cmpr()(this->val, cpr); }

			// ����==����
			bool operator == (const Node& cpr) const {
				_Cmpr cpr;
				return cpr(this->val, cpr.val) == cpr(cpr.val, this->val);
			}

		protected:
			// ������±�
			size_t fa;
			// ���Һ��ӽ���±�
			size_t ch[2];
			// ��ֵ
			_Ty val;
		};

	protected:
		// ѯ���ӽ�㣬ά�����ڵ���Ϣ
		// ��������չSplayTree����
		inline virtual void PushUp(size_t x) {}

		// ��ת����
		// ��ָ���±���������ת(����/����)һ��λ��
		// Ĭ����Ϊ������и���㣬���޸����������
		// x		����±�
		void Rotate(size_t x) {
			Node* tr = (Node*)this->src;
			// y	�����
			// z	үү���
			size_t y = tr[x].fa, z = tr[y].fa;
			bool chk = tr[y].ch[1] == x;
			// ����������ϵ
			tr[y].fa = x;
			tr[y].ch[chk] = tr[x].ch[!chk];
			if (tr[x].ch[!chk]) tr[x].ch[!chk].fa = y;
			// �������ڵ��ϵ
			tr[x].fa = z;
			tr[x].ch[!chk] = y;
			if (z ^ INF) tr[z].ch[tr[z].ch[1] == y] = x;
			this->PushUp(y);
			this->PushUp(x);
		}

		// ���ݺ���(ÿ������2��)
		void Expand() {
			// ������Դ
			size_t cap = this->capacity ? this->capacity << 1 : 2;
#ifdef EXCEPTION_DETECTION
			if (cap <= 0) throw std::exception("fail to expand");
#endif // EXCEPTION_DETECTION
			this->Reserve(cap);
		}

		// Ѱ�Ҳ���λ��
		// val		��ֵ
		// ���ظ�����±�
		virtual size_t Find(const _Ty& val) const {
			// ����
			if (!this->size) return INF;
			// һֱ������Ҷ�ӽ�㴦
			size_t cur = this->root, fa = INF;
			Node* tr = (Node*)this->src;
			while (cur ^ INF) {
				fa = cur;
				cur = tr[cur] < val ? tr[cur].ch[0] : tr[cur].ch[1];
			}
			return fa;
		}

	public:
		// Ĭ�Ϲ���
		SplayTree() :src(nullptr), root(INF), size(0), capacity(0) {}

		// Ĭ������
		virtual ~SplayTree() {
			_Alloc<Node>().Free(this->src, this->size);
			this->root = INF;
			this->size = 0;
			this->capacity = 0;
		}

		// ��չ����
		// ���±�Ϊx�Ľ����ת���±�Ϊk�Ľ������
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

		// ������������
		// ���������С�ڵ�ǰԪ�ظ��������ִ���
		void Reserve(size_t cap) {
#ifdef EXCEPTION_DETECTION
			if (cap < this->size) throw std::exception("insufficient capacity");
#endif // EXCEPTION_DETECTION

			// ������Դ
			void* buf = _Alloc<Node>().New(cap);

			// ����Դ����
			if (this->src) {
				// ��������
				memcpy_s(buf, cap * sizeof(Node), this->src, this->size * sizeof(Node));
				// �ͷž���Դ
				free(this->src);
			}
			this->src = buf;
			this->capacity = cap;
		}


		template<typename... Args>
		void Emplace(Args&&... args) {
			// ��������������
			if (this->size == this->capacity) this->Expand();
			Node* tr = (Node*)this->src;
			// ������ֵ
			new(&tr[this->size]) Node(INF, { INF,INF }, args...);
			// Ѱ�Ҳ���λ��
			size_t x = this->size;
			size_t fa = this->Find(tr[this->size++]);
			// ���½����Ϣ
			if (fa ^ INF) {
				tr[x].fa = fa;
				tr[fa].ch[]
			}
			this->Splay(x);
		}

	protected:
		// Դ����
		void* src;
		// ������±�
		size_t root;
		// Ԫ�ظ���
		size_t size;
		// ��������
		size_t capacity;

	};
}