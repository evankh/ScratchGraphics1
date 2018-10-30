#ifndef __EKH_SCRATCH_GRAPHICS_1_NAMED_CONTAINER__
#define __EKH_SCRATCH_GRAPHICS_1_NAMED_CONTAINER__

#include <map>
#include <string>

template<class Type>
class NamedContainerBase {
protected:
	std::map<std::string, Type> mItems;
	Type mDefault;
public:
	inline NamedContainerBase(Type default) { this->mDefault = default; };
	inline void add(std::string key, Type item) {
		if (this->mItems.count(key))
			return;
		mItems[key] = item;
	};
	inline Type get(std::string key) const {
		if (mItems.count(key))
			return this->mItems.at(key);
		else
			return mDefault;
	};
	virtual void clear() = 0;
	virtual void remove(std::string key) = 0;
};

template<class Type>
class NamedContainer :public NamedContainerBase<Type> {
public:
	inline NamedContainer(Type default) :NamedContainerBase(default) {};
	inline void clear() {
		this->mItems.clear();
	};
	inline void remove(std::string key) {
		if (this->mItems.at(key))
			this->mItems.erase(key)
	};
};

template<class Type>
class NamedContainer<Type*> :public NamedContainerBase<Type*> {
public:
	inline NamedContainer(Type* default) :NamedContainerBase(default) {};
	inline void clear() {
		for (auto item : this->mItems)
			delete item.second;
		this->mItems.clear();
	};
	inline void remove(std::string key) {
		if (auto x = this->mItems.at(key)) {
			delete x;
			this->mItems.erase(key);
		}
	};
};

template<typename Ret, typename... Args>
class NamedContainer<Ret(*)(Args...)> :public NamedContainerBase<Ret(*)(Args...)> {
public:
	inline NamedContainer(Ret(*default)(Args...)) :NamedContainerBase(default) {};
	inline void clear() {
		this->mItems.clear();
	};
	inline void remove(std::string key) {
		if (this->mItems.count(key))
			this->mItems.erase(key);
	};
};

#endif//__EKH_SCRATCH_GRAPHICS_1_NAMED_CONTAINER__