
#pragma once

template<class t_class>
class Singleton {
public:
	static t_class& Instance() {
		static t_class _instance;
		return _instance;
	}
	~Singleton() {};

protected:
	Singleton() {};

	Singleton(const Singleton&) {};
	Singleton& operator = (const Singleton&) {}
};