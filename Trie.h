#include "pch.h"
#include "Allocator.h"

namespace dsl {
	template <size_t range = 26, template<typename> typename _Allocator = Allocator>
	class Trie;

	template<size_t range>
	class TrieNode {
		template<size_t range, template<typename> typename _Allocator>
		friend class Trie;
	public:



	protected:
		int count;
		TrieNode* data[range]{};
	};

	template <size_t range, template<typename> typename _Allocator>
	class Trie {
	public:
		using _Elemtype = TrieNode<range>;
		using _ElemAlloc = Allocator<TrieNode<range>>;
		using _Node = _Elemtype;

	public:
		

	protected:
		_ElemAlloc alloc;
		_Node* root;
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