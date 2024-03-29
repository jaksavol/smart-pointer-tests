// Tests for smart pointers 
// Jari Savolainen
#include <iostream>
#include <memory>
#include <cassert>
#include <type_traits>
#include <utility>

struct Obj {
	~Obj() { std::cout << "Obj::~Obj()" << std::endl; }
};

template<typename T>
T && getPF(T && o) {
	return std::forward<T>(o);
}

int main()
{
	std::cout << "---- unique ----" << std::endl;
	{
		std::unique_ptr<Obj> uptr = std::make_unique<Obj>();
		std::cout << "uptr: " << uptr.get() << std::endl;
		//auto uptr2 = std::make_unique<Obj>();
		std::unique_ptr<Obj> uptr2;
		//uptr2 = std::move(uptr);
		uptr2 = static_cast<std::unique_ptr<Obj> &&>(uptr);
		std::unique_ptr<Obj> uptr3;
		uptr3 = getPF(std::move(uptr2));
		auto uptr4 = getPF(std::make_unique<Obj>());
		std::cout << "uptr: " << uptr.get() << " uptr2: " << uptr2.get() << std::endl;
		assert(uptr.get() == nullptr && uptr == nullptr);
		assert(uptr2.get() == uptr.get());
		assert(uptr3 != uptr2);
		assert(uptr4 != uptr3);
	}
	std::cout << "---- shared ----" << std::endl;
	{
		std::shared_ptr<Obj> sptr1 = std::make_shared<Obj>();
		static_assert(std::is_same<std::shared_ptr<Obj>::element_type, Obj>::value, "Shared_ptr element_type doesn't match to Obj");
		{
			std::shared_ptr<Obj> sptr2(sptr1);
			std::cout << "sptr1 unique: " << std::boolalpha << sptr1.unique() << " sptr2 use_count: " << sptr2.use_count();// << std::endl;
			std::cout << " sptr1 use_count: " << sptr1.use_count() << std::endl;
			assert(sptr1 == sptr2);
		}
		assert(sptr1 != nullptr);
		std::cout << "sptr1 unique: " << sptr1.unique() << " use_count: " << sptr1.use_count() << std::endl;
		sptr1.reset();
		std::cout << "sptr1 unique: " << sptr1.unique() << " use_count: " << sptr1.use_count() << std::endl;
		assert(sptr1 == nullptr);
	}
	std::cout << "---- weak ----" << std::endl;
	{
		std::weak_ptr<Obj> wptr1;
		{
			std::shared_ptr<Obj> sptr1 = std::make_shared<Obj>();
			static_assert(std::is_same<typename decltype(wptr1)::element_type, typename decltype(sptr1)::element_type>::value, "Shared_ptr and weak_ptr has different element_types");
			//static_assert(std::is_same<decltype(std::declval<decltype(sptr1)>().operator*()), std::add_lvalue_reference<decltype(sptr1)::element_type>::type>::value, "mismatch on dereference"); ;
			typedef decltype(std::declval<decltype(sptr1)>().operator*()) derefeType;
			static_assert(std::is_same<derefeType, Obj &>::value, "Dereference type mismatch.");
			wptr1 = sptr1;
			std::cout << "wptr1 expired: " << wptr1.expired() << " use_count: " << wptr1.use_count() << std::endl;
			{
				std::shared_ptr<Obj> sptr2(wptr1);
				std::cout << "sptr2 unique: " << sptr2.unique() << " use_count: " << sptr2.use_count() << std::endl;
				std::cout << "wptr1 expired: " << wptr1.expired() << " use_count: " << wptr1.use_count() << std::endl;

				std::shared_ptr<Obj> sptr3 = wptr1.lock();
				std::cout << "sptr3 unique: " << sptr3.unique() << " use_count: " << sptr3.use_count() << std::endl;
				std::cout << "wptr1 expired: " << wptr1.expired() << " use_count: " << wptr1.use_count() << std::endl;
				assert(sptr2 == sptr3);
			}
		}
		std::cout << "wptr1 expired: " << wptr1.expired() << " use_count: " << wptr1.use_count() << std::endl;
		if (!wptr1.expired()) {
			std::shared_ptr<Obj> sptr1(wptr1);
			std::cout << "sptr1 unique: " << sptr1.unique() << " use_count: " << sptr1.use_count() << std::endl;
		}
		else std::cout << "wptr1 expired\n";
		if (std::shared_ptr<Obj> sptrFromLock = wptr1.lock()) {
			std::cout << "sptrFromLock unique: " << sptrFromLock.unique() << " use_count: " << sptrFromLock.use_count() << std::endl;
		}
		else std::cout << "wptr1.lock().use_count: " << sptrFromLock.use_count() << " sptrFromLock: " << sptrFromLock << std::endl;
	}
}
