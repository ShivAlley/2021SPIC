#pragma once
#include <functional>
#include <variant>
#include <map>
#include <unordered_map>
#include <queue>
#include <any>

enum class Accessor : std::uint32_t
{
	first = 0,
	second,
	third,
	fourth,
	fifth,
	NUM,
};

template<typename>
class Visitor;

template<typename R, typename... args>
class Visitor<R(args...)>
{
public:
	//Visitor() {};
	Visitor(const args&... init)
	{
		for (int i = 0; i < 10; ++i)
		{
			placeHolders.emplace(i, std::any());
		}
		for (int i = 0; i < static_cast<int>(Accessor::NUM); ++i)
		{
			stateMachine.push(static_cast<Accessor>(i));
		}
		pick(std::forward<const args&>(init)...);
	}
	void pick() { constractCount = 0; }
	template<class Head, class... Tail>
	void pick(Head&& head, Tail&&... tail)
	{
		placeHolders.at(constractCount) = head;
		constractCount++;
		pick(std::forward<Tail>(tail)...);
	}

	template<class T>
	void operator()(T& f)
	{
		std::exception("not found operator overload");
		throw 1;
	}

	void operator()(std::function<void(void)>& f)
	{
		f();
	}
	void operator()(std::function<int(int, int)>& f)
	{
		rtnData = f(std::any_cast<int>(placeHolders.at(0)), std::any_cast<int>(placeHolders.at(1)));
	}
	void operator()(std::function<double(double)>& f)
	{
		rtnData = f(std::any_cast<double>(placeHolders.at(0)));
	}
	void operator()(std::function<double(double, double)>& f)
	{
		rtnData = f(std::any_cast<double>(placeHolders.at(0)), std::any_cast<double>(placeHolders.at(1)));
	}
	void operator()(std::function<double(double, double, double)>& f)
	{
		rtnData = f(std::any_cast<double>(placeHolders.at(0)), std::any_cast<double>(placeHolders.at(1)), std::any_cast<double>(placeHolders.at(2)));
	}

	std::any rtnData;
	using var = std::variant<std::function<void(void)>,
		std::function<int(int, int)>,
		std::function<double(double)>,
		std::function<double(double, double)>,
		std::function<double(double, double, double)>>;

	std::queue<Accessor> stateMachine;
	std::unordered_map<Accessor, var> inspector;

private:

	std::map<int, std::any> placeHolders;
	int constractCount = 0;
};
