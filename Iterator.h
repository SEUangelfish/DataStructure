#pragma once
#include "pch.h"

namespace dsl {
	template<typename _DSTy>
	class Iterator {
	protected:
		// �ṹ��Ԫ������
		using _SrcType = _DSTy::_ElemType;

	public:
		// Ĭ�Ϲ���
		Iterator() = default;
		// ���ι���
		// _src��Դ����ָ��
		Iterator(_SrcType* _src) : src(_src) {}
		// ��������
		Iterator(const Iterator& cp) : src(cp.src) {}
		// �ƶ�����
		Iterator(Iterator&& mv) noexcept :src(mv.src) {
			mv.src = nullptr;
		}

		// ������
		virtual ~Iterator() {
			this->src = nullptr;
		}

		// ����
		virtual Iterator& operator++() = 0;
		// �Լ�
		virtual Iterator& operator--() = 0;
		// �е�
		ptrdiff_t operator==(const Iterator& cpr) { return this->src == cpr.src; }
		// ������
		_SrcType& operator*() { return *this->src; }
		// ָ��
		_SrcType* operator->() { return this->src; }


	protected:
		// Դ����ָ��
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
		// Ĭ�Ϲ���
		SplayTree_Iterator() = default;
		// ���ι���
		// _src��Դ����ָ��
		SplayTree_Iterator(Iterator<_DSTy>::_SrcType* _src) : Iterator<_DSTy>(_src) {}
		// ��������
		SplayTree_Iterator(const SplayTree_Iterator& cp) : Iterator<_DSTy>(cp) {}
		// �ƶ�����
		SplayTree_Iterator(SplayTree_Iterator&& mv) noexcept :Iterator<_DSTy>(std::move(mv)) {}
		// ������
		virtual ~SplayTree_Iterator() {}

		// ����
		virtual Iterator<_DSTy>& operator++() {
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

		// �Լ�
		virtual Iterator<_DSTy>& operator--() {
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

	};
}
