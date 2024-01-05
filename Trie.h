#include "pch.h"
#include "Allocator.h"

namespace dsl {
	template<size_t range>
	class TrieNode {
	public:
		TrieNode() = default;
		TrieNode(size_t _prefix) :prefix(_prefix) {}

		~TrieNode() = default;

		TrieNode*& operator[](size_t idx) {
			return this->nexts[idx];
		}

		size_t& Prefix() {
			return this->prefix;
		}

		size_t& Words() {
			return this->words;
		}

	protected:
		size_t words = 0;
		size_t prefix = 0;
		TrieNode* nexts[range]{};
	};

	// range:	character range
	// _Ty:		character type
	// _Map:	type of mapping function which maps characters to different indexes
	//			eg: to all lowercase letters, the function can be "mod 26" or "- 'a'" 
	// _Alloc:	allocator template
	template <size_t range = 26, typename _Ty = char, typename _Map = std::function<size_t(const _Ty&)>, template<typename> typename _Alloc = Allocator>
	class Trie {
	public:
		using _Elemtype = TrieNode<range>;
		using _ElemAlloc = _Alloc<TrieNode<range>>;
		using _Node = _Elemtype;

	protected:
		void Clear(_Node* rt) {
			_Node* u, * head = rt;
			head->Words() = NULL;
			while (head) {
				u = head;
				head = (_Node*)head->Words();
				for (size_t i = 0; i < range; ++i) {
					if ((*u)[i]) {
						(*u)[i]->Words() = (size_t)head;
						head = (*u)[i];
						(*u)[i] = nullptr;
					}
				}
				this->alloc.Free(u, 1);
			}
		}

	public:
		Trie() = default;
		explicit Trie(const _Map& _map) :map(_map) {}
		explicit Trie(const _ElemAlloc& _alloc) :alloc(_alloc) {}
		explicit Trie(_Map&& _map) :map(std::move(_map)) {}
		explicit Trie(_ElemAlloc&& _alloc) :alloc(std::move(_alloc)) {}
		Trie(const _Map& _map, const _ElemAlloc& _alloc) :map(_map), alloc(_alloc) {}
		Trie(const _Map& _map, _ElemAlloc&& _alloc) :map(_map), alloc(std::move(_alloc)) {}
		Trie(_Map&& _map, const _ElemAlloc& _alloc) :map(std::move(_map)), alloc(_alloc) {}
		Trie(_Map&& _map, _ElemAlloc&& _alloc) :map(std::move(_map)), alloc(std::move(_alloc)) {}

		Trie(const Trie& cp) :map(cp.map), alloc(cp.alloc) {
			this->root->Prefix() = (size_t)cp.root;
			_Node* mould, * tail = this->root, * cur = this->root, * nxt;
			while (cur) {
				mould = (_Node*)cur->Prefix();
				cur->Prefix() = mould->Prefix();

				for (size_t i = 0; i < range; ++i) {
					if ((*mould)[i]) {
						(*cur)[i] = this->alloc.New0(1);
						tail->Words() = (size_t)(*cur)[i];
						tail = (*cur)[i];
						tail->Prefix() = (size_t)(*mould)[i];
					}
				}

				nxt = (_Node*)cur->Words();
				cur->Words() = mould->Words();
				cur = nxt;
			}
		}

		Trie(Trie&& mv) noexcept :root(mv.root), map(std::move(mv.map)), alloc(std::move(mv.alloc)) {
			mv.root = nullptr;
		}

		Trie& operator=(Trie&& mv) noexcept {
			this->~Trie();
			new (this) Trie(std::move(mv));
			return *this;
		}

		Trie& operator=(const Trie& mv) noexcept {
			this->~Trie();
			new (this) Trie(mv);
			return *this;
		}

		~Trie() {
			if (this->root) {
				this->Clear();
				this->alloc.Free(this->root, 1);
			}
		}

		// remove all words from trie including null string 
		void Clear() {
			_Node* u = this->root;
			u->Words() = u->Prefix() = 0;
			for (size_t i = 0; i < range; ++i) {
				if ((*u)[i]) {
					this->Clear((*u)[i]);
					(*u)[i] = nullptr;
				}
			}
		}

		// insert a word into the trie including null string (more accurately, the length equals 0)
		void Insert(const _Ty* word, size_t length) {
			_Node* u = this->root;
			size_t idx;
			for (size_t i = 0; i < length; ++i) {
				++u->Prefix();
				idx = this->map(word[i]);
				if ((*u)[idx] == nullptr) {
					(*u)[idx] = this->alloc.New(1);
					u = (*u)[idx];
					new (u) _Node();
				}
				else u = (*u)[idx];
			}
			++u->Prefix();
			++u->Words();
		}

		// return true if it contains the word including null string 
		bool Contains(const _Ty* word, size_t length) {
			if (!length) return this->root->Words();
			_Node* u = this->root;
			size_t idx;
			for (size_t i = 0; i < length; ++i) {
				idx = this->map(word[i]);
				if ((*u)[idx] == nullptr) return false;
				u = (*u)[idx];
			}
			return u->Words();
		}

		// remove the word from trie including null string 
		// ensure: true if it must contains the word
		// ps: if there are the same words in trie, this call can only remove one
		void Remove(const _Ty* word, size_t length, bool ensure) {
			if (!ensure && !this->Contains(word, length)) return;
			--this->root->Prefix();
			if (!length) {
				--this->root->Words();
				return;
			}
			_Node* pre, * cur = this->root;
			for (size_t i = 0, idx; i < length;) {
				pre = cur;
				idx = this->map(word[i++]);
				cur = (*cur)[idx];
				if (!--cur->Prefix()) {
					(*pre)[idx] = nullptr;
					while (i < length) {
						pre = cur;
						cur = (*cur)[this->map(word[i++])];
						this->alloc.Free(pre, 1);
					}
					this->alloc.Free(cur, 1);
					return;
				}
			}
			--cur->Words();
		}

		// remove all words with prefix of parameter 1
		// ensure: true if it must contains word with prefix of parameter 1
		void RemoveAll(const _Ty* prefix, size_t length) {
			if (!length) {
				this->Clear();
				return;
			}
			size_t cnt = this->CountWordsWithPrefix(prefix, length);
			if (!cnt) return;
			this->root->Prefix() -= cnt;
			_Node* cur = this->root, * pre = nullptr;
			size_t i = 0, idx;
			for (; i < length; ++i) {
				pre = cur;
				idx = this->map(prefix[i]);
				cur = (*cur)[idx];
				if (!(cur->Prefix() -= cnt)) {
					(*pre)[idx] = nullptr;
					this->Clear(cur);
					return;
				}
			}
			(*pre)[idx] = nullptr;
			this->Clear(cur);
		}

		// return number of words
		size_t Size() {
			return this->root->Prefix();
		}

		size_t CountWordsWithPrefix(const _Ty* prefix, size_t length) {
			_Node* u = this->root;
			for (size_t i = 0; i < length && u; ++i) u = (*u)[this->map(prefix[i])];
			return u ? u->Prefix() : 0;
		}

	protected:
		_ElemAlloc alloc;
		_Map map = [](const _Ty& x)->size_t { return x % 26; };
		_Node* root = this->alloc.New0(1);
	};
}