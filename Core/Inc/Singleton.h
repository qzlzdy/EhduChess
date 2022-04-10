/*
 * Singleton.h
 *
 *  Created on: 2022年4月9日
 *      Author: qzlzdy
 */

#ifndef INC_SINGLETON_H_
#define INC_SINGLETON_H_

namespace ehdu{

template<class Derived>
class Singleton{
public:
	Singleton(const Singleton &) = delete;
	Singleton &operator=(const Singleton &) = delete;
	Singleton(Singleton &&) = default;
	Singleton &operator=(Singleton &&) = default;
	virtual ~Singleton() = default;
	static Derived *getInstance();
protected:
	Singleton() = default;
private:
	static Derived *obj;
};

template<class Derived>
Derived *Singleton<Derived>::getInstance(){
	if(obj == nullptr){
		obj = new Derived;
	}
	return obj;
}

template<class Derived>
Derived *Singleton<Derived>::obj = nullptr;

}

#endif /* INC_SINGLETON_H_ */
