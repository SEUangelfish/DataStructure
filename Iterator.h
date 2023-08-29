#pragma once
#include "pch.h"

namespace dsl {
	template<typename _DSTy>
	class Iterator {
	public:
		// �ṹ��Ԫ������
		using _ElemType = typename _DSTy::_ElemType;

	public:
		// Ĭ�Ϲ���
		Iterator() = default;
		// ���ι���
		// _src��Դ����ָ��
		Iterator(_ElemType* _src) : src(_src) {}
		// ��������
		Iterator(const Iterator& cp) : src(cp.src) {}
		// �ƶ�����
		Iterator(Iterator&& mv) noexcept :src(mv.src) {
			mv.src = nullptr;
		}

		// ������
		virtual ~Iterator() {}

		// ����
		virtual Iterator& operator++() = 0;
		// �Լ�
		virtual Iterator& operator--() = 0;
		// �е�
		bool operator==(const Iterator& cpr) { return this->src == cpr.src; }
		// ����
		bool operator!=(const Iterator& cpr) { return this->src != cpr.src; }
		// ������
		_ElemType& operator*() { return *this->src; }
		// ָ��
		_ElemType* operator->() { return this->src; }


	protected:
		// Դ����ָ��
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
		// �ṹ��Ԫ������
		using _ElemType = typename _DSTy::_ElemType;

	public:
		// Ĭ�Ϲ���
		SplayTreeIterator() = default;
		// ���ι���
		// _src��Դ����ָ��
		SplayTreeIterator(_ElemType* _src) : Iterator<_DSTy>(_src) {}
		// ��������
		SplayTreeIterator(const SplayTreeIterator& cp) : Iterator<_DSTy>(cp) {}
		// �ƶ�����
		SplayTreeIterator(SplayTreeIterator&& mv) noexcept :Iterator<_DSTy>(std::move(mv)) {}
		// ������
		virtual ~SplayTreeIterator() {}

		// ����
		Iterator<_DSTy>& operator++() {
#ifdef EXCEPTION_DETECTION
			if (!this->src) throw exception("object of SplayTree iterator��invalid operation by ++ (null pointer of source data)");
			if (this->src->end) throw exception("object of SplayTree iterator��can't be ++ any more");
#endif // EXCEPTION_DETECTION
			if (this->src->ch[1]) {
				// ��������������
				this->src = this->src->ch[1];
				while (this->src->ch[0]) this->src = this->src->ch[0];
			}
			else {
				// �������Լ�Ϊ���ڵ����ڵ�
				while (this->src == this->src->fa->ch[1]) this->src = this->src->fa;
				this->src = this->src->fa;
			}
			return *this;
		};
		// ��������ת�����ڵ�
		Iterator<_DSTy>& Next(_DSTy& tree) {
			this->operator++();
			tree.Splay(this->src);
		}

		// �Լ�
		Iterator<_DSTy>& operator--() {
#ifdef EXCEPTION_DETECTION
			if (!this->src) throw exception("object of SplayTree iterator��invalid operation by -- (null pointer of source data)");
#endif // EXCEPTION_DETECTION

			if (this->src->ch[0]) {
				// �����������ҽ��
				this->src = this->src->ch[0];
				while (this->src->ch[1]) this->src = this->src->ch[1];
			}
			else {
				// �������Լ�Ϊ���ڵ���ҽڵ�
				while (this->src->fa && this->src == this->src->fa->ch[0]) this->src = this->src->fa;
#ifdef EXCEPTION_DETECTION
				if (!this->src->fa) throw exception("object of SplayTree iterator��can't be -- any more");
#endif // EXCEPTION_DETECTION
				this->src = this->src->fa;
			}
			return *this;
		};
		// �Լ�����ת�����ڵ�
		Iterator<_DSTy>& Prev(_DSTy& tree) {
			this->operator--();
			tree.Splay(this->src);
		}

	};
}
