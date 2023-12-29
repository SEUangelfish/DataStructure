#include "pch.h"
#include "Allocator.h"

namespace dsl {
	template <size_t range = 26, typename _Ty = char, template<typename> typename _Allocator = Allocator>
	class Trie;

	template<size_t range>
	class TrieNode {
		template<size_t range, typename _Ty, template<typename> typename _Allocator>
		friend class Trie;
	public:
		TrieNode() = default;
		~TrieNode() = default;

		TrieNode& operator[](size_t idx) {
			return this->data[idx];
		}

	protected:
		size_t count = 0;
		TrieNode* data[range]{};
	};

	template <size_t range, typename _Ty, template<typename> typename _Allocator>
	class Trie {
	public:
		using _Elemtype = TrieNode<range>;
		using _ElemAlloc = Allocator<TrieNode<range>>;
		using _Node = _Elemtype;

	public:

		void Insert(const _Ty* word, size_t length) {
			TrieNode* u = this->root;
			for (size_t i = 0; i < length; ++i) {

			}
		}

	protected:
		size_t(*map)(const _Ty& x);
		_ElemAlloc alloc;
		_Node* root = this->alloc.New0(1);


		//void insert(string word);
		//bool search(string word);
		//void remove(string word);
		//bool startsWithPrefix(string prefix);
		//int countWordsWithPrefix(string prefix);
		//void traverse();
		//int getSize();
		//void clear();
	};
}