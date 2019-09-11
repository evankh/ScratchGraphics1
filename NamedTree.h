#ifndef __EKH_SCRATCH_GRAPHICS_1_NAMED_TREE__
#define __EKH_SCRATCH_GRAPHICS_1_NAMED_TREE__

#include <string>
#include "NamedContainer.h"

template<class Type>
class NamedTree {
private:
	struct TreeNode {
		std::string name;
		std::vector<TreeNode> children;
		TreeNode(std::string n, TreeNode* parent) {
			this->name = n;
			this->parent = parent;
		}
		~TreeNode() {
			this->children.clear();
		}
		TreeNode* parent;
	};
	NamedContainer<Type> mItems;
	TreeNode* mRoot;
	NamedContainer<std::vector<int>> mTreeLookup;
	TreeNode* lookup(std::string key) const {
		TreeNode* node = mRoot;
		std::vector<int> lookup = mTreeLookup.get(key);
		for (auto i : lookup)
			node = &node->children[i];
		return node;
	}
	void depthFirstTraversal(TreeNode* node, bool preorder, std::vector<std::string>& vector) const {
		if (preorder) vector.push_back(node->name);
		for (TreeNode& i : node->children)
			depthFirstTraversal(&i, preorder, vector);
		if (!preorder) vector.push_back(node->name);
	}
public:
	NamedTree() {
		mRoot = new TreeNode("", nullptr);
	}
	~NamedTree() {
		clear();
		delete mRoot;
	}
	inline void add(std::string key, Type data) {
		if (this->mItems.contains(key))
			return;
		this->mItems.add(key, data);
		std::vector<int> lookup;
		lookup.push_back(this->mRoot->children.size());
		this->mRoot->children.push_back(TreeNode(key, nullptr));
		this->mTreeLookup.add(key, lookup);
	}
	inline void add(std::string key, std::string parent, Type data) {
		if (this->mItems.contains(key))
			return;
		if (!this->mItems.contains(parent))
			throw std::out_of_range(parent);
		this->mItems.add(key, data);
		std::vector<int> lookup = mTreeLookup.get(parent);
		TreeNode* node = mRoot;
		for (auto i : lookup)
			node = &node->children[i];
		lookup.push_back(node->children.size());
		node->children.push_back(TreeNode(key, node));
		this->mTreeLookup.add(key, lookup);
	}
	inline Type get(std::string key) const {
		return this->mItems.get(key);	// Handles the exception for us
	}
	inline std::vector<std::string> getChildren() const {
		std::vector<std::string> ret;
		for (auto i : this->mRoot->children)
			ret.push_back(i.name);
		return ret;
	}
	inline std::vector<std::string> getChildren(std::string key) const {
		if (!this->mItems.contains(key))
			throw std::out_of_range(key);
		std::vector<int> lookup = this->mTreeLookup.get(key);
		TreeNode* node = lookup(key)
		std::vector<std::string> ret;
		for (auto i : node->children)
			ret.push_back(i.name);
		return ret;
	}
	inline bool hasParent(std::string key) const {
		if (!this->mItems.contains(key))
			throw std::out_of_range(key);
		TreeNode* node = lookup(key);
		return node->parent != nullptr;
	}
	inline std::string getParent(std::string key) const {
		if (!this->mItems.contains(key))
			throw std::out_of_range(key);
		TreeNode* node = lookup(key);
		if (node->parent) return node->parent->name;
		return "";
	}
	inline bool contains(std::string key) const {
		return this->mItems.contains(key);
	}
	inline int count() const {
		return this->mItems.count();
	}
	inline void clear() {
		this->mItems.clear();
		delete this->mRoot;
		this->mRoot = new TreeNode("", nullptr);
	}
	inline void remove(std::string key) {
		if (!this->mItems.contains(key))
			return;
		// Need to figure out what to do with the children
		// And there seem to be several equally-valid strategies:
		//  - Remove them too
		//  - Give them the removed node's parent
		//  - Give them root as a parent
		// Not to mention the actual issues with implementing it...
		// Will forego writing this part until I figure out what I actually will need it for
		return;
	}
	// Breadth-order & depth-order traversal, probably
	inline std::vector<std::string> breadthFirstTraversal() const {
		std::vector<TreeNode*> queue;
		std::vector<std::string> ret;
		for (TreeNode& next : mRoot->children)
			queue.push_back(&next);
		for (unsigned int i = 0; i < queue.size(); i++) {
			ret.push_back(queue[i]->name);
			for (TreeNode& next : queue[i]->children)
				queue.push_back(&next);
		}
		return ret;
	}
	inline std::vector<std::string> depthFirstTraversal(bool preorder = true) const {
		std::vector<std::string> ret;
		depthFirstTraversal(mRoot, preorder, ret);
		return ret;
	}
};

#endif//__EKH_SCRATCH_GRAPHICS_1_NAMED_TREE__