#pragma once
/* function.hpp */

#include <deque>
#include <functional>
#include <memory>
#include <type_traits>
#include <cmath>

template<typename T>
class Function{
	static_assert(std::is_floating_point<T>::value, "the type should be floating");
private:
	enum Operation{
		None,
		Plus,
		Multiply,
	};

public:
	
	using type = Function<T>;

private:

	using functor_type = std::function<Function<T>( Function<T> )>;
	using functor_const_type = std::function<T( Function<T> )>;

private:


	// 全体の符号
	bool m_sgn;

	// 次との結合(+, -, *, /)
	Operation m_operation;
	
	// 基礎初等関数であるか
	bool m_is_elementary;

	// f(x), df/dx(x)の関数オブジェクト 基礎関数のみ指定、それ以外はnull
	std::function<T(Function<T>&, T)> m_funcobj;		
	std::function<T(T)> m_dfuncobj;

	// 関数f(x)
	std::deque<std::shared_ptr<Function<T>>> m_func;

public:

	// 引数なしコンストラクタ
	Function()
	{
		m_sgn = true;
		m_operation = Operation::None;
		m_is_elementary = false;
		m_funcobj = nullptr;
		m_dfuncobj = nullptr;
	}

	// コピーコンストラクタ
	Function(const Function<T>& arg)
	{
		m_sgn = arg.m_sgn;
		m_operation = arg.m_operation;
		m_is_elementary = arg.m_is_elementary;
		m_func = arg.m_func;
		m_funcobj = arg.m_funcobj;
		m_dfuncobj = arg.m_dfuncobj;
	}

	// 関数オブジェクト指定コンストラクタ 
	Function(const functor_type func)
	{
		std::cout << "functor_type argument Constructor was called." << std::endl;
		m_sgn = true;
		m_operation = Operation::None;
		m_is_elementary = false;
		m_funcobj = nullptr;
		m_dfuncobj = nullptr;;
		m_func.push_back( std::make_shared<Function<T>>( func(_x()) ) );
		std::cout << "push_backed func(_x())" << std::endl;
	}

	// 定数関数オブジェクト指定コンストラクタ
	Function(const functor_const_type const_func)
	{
		m_sgn = true;
		m_operation = Operation::None;
		m_is_elementary = false;
		m_funcobj = nullptr;
		m_dfuncobj = nullptr;;
		m_func.push_back( std::make_shared<Function<T>>(_constant(const_func) ) );
	}

	// デストラクタ
	~Function() = default;	

	/* 数値f(x)を返す */
	T f(T x)
	{
		std::cout << "f(x) was called." << std::endl;

		if(m_is_elementary){
			std::cout << "oh, it's elementary." << std::endl;
			return m_funcobj(*this, x);
		} else {
			if(m_func.empty()){
				throw "function is not defined.";
			}
			std::cout << "oh, it's original function." << std::endl;
			T temp = static_cast<T>(0), multbuf = static_cast<T>(1); 
			for(auto it = m_func.begin(); it!=m_func.end(); ++it){
				std::cout << "waaaai" << std::endl;
				multbuf *= (*it)->f(x);
			
				std::cout << "feeee" << std::endl;	
				if((*it)->m_operation == Operation::Plus){
					temp += multbuf;
					multbuf = static_cast<T>(1);
				} else if ((*it)->m_operation == Operation::Multiply) {
					continue;
			} else if ((*it)->m_operation == Operation::None) {
					temp += multbuf;
					break;
				}
			}
			return temp;
		}
	}
	/* 数値df/dx(x)を返す */
	T df(T x)
	{
		return static_cast<T>(0);	
	}


public:
	// 演算子オーバーロード

	// ----単項演算子
	inline Function<T> operator+()
	{
		return *this;
	}
	inline Function<T> operator-()
	{
		Function<T> temp = *this;
		temp.m_sgn = !temp.m_sgn;
		return temp;
	}
		
	// ----代入演算子

	// ラムダ式代入
	inline Function<T>& operator=(functor_type right)
	{
		m_sgn = true;
		m_operation = Operation::None;
		m_is_elementary = false;;
		m_func.push_back(std::make_shared<Function<T>>(right(_x())));
		m_funcobj = nullptr;
		m_dfuncobj = nullptr;

		//*this = right(_x());
		return *this;
	}

	// 関数コピー代入
	inline Function<T>& operator=(Function<T> right)
	{
		m_sgn = right.m_sgn;
		m_operation = right.m_operation;
		m_is_elementary = right.m_is_elementary;
		m_func = right.m_func;
		m_funcobj = right.m_funcobj;
		m_dfuncobj = right.m_dfuncobj;
		return *this;
	}
	inline Function<T>& operator=(T right)
	{
		*this = _constant(right);	
	}
	
	// 加算代入 
	inline Function<T>& operator+=(Function<T> right)
	{
		m_func.back()->m_operation = Operation::Plus;
		m_func.push_back(right);
	}
	inline Function<T>& operator+=(T right)
	{
		m_func.back()->m_operation = Operation::Plus;
		m_func.push_back(_constant(right));
		return *this;	
	}
	
	// 減算代入
	inline Function<T>& operator-=(Function<T> right)
	{
		m_func.back()->m_operation = Operation::Plus;
		right.m_sgn = !right.m_sgn;
		m_func.push_back(right);
		return *this;
	}
	inline Function<T>& operator-=(T right)
	{
		m_func.back()->m_operation = Operation::Plus;
		m_func.push_back(_constant(-right));
		return *this;
	}
	
	// 乗算代入
	inline Function<T>& operator*=(Function<T> right)
	{
		m_func.back()->m_operation = Operation::Multiply;
		m_func.push_back(right);
		return *this;
	}
	inline Function<T>& operator*=(T right)
	{
		m_func.back()->m_operation = Operation::Multiply;
		m_func.push_back(_constant(right));
		return *this;
	}

	// 除算代入
	inline Function<T>& operator/=(Function<T> right)
	{
		m_func.back()->m_operation = Operation::Multiply;
		m_func.push_back(_inv(right));		
		return *this;
	}
	inline Function<T>& operator/=(T right)
	{
		m_func.back()->m_operation = Operation::Multiply;
		m_func.push_back(_constant(static_cast<T>(1) / right));
		return *this;
	}

	// ----関数呼び出し演算子
	inline T operator()(T x)
	{
		if(m_func.empty()){
			throw "function is not defined.";
		}

		m_func.begin()->get()->f(static_cast<T>(0));

		T temp = static_cast<T>(0), multbuf = static_cast<T>(1); 

		for(auto it = m_func.begin(); it!=m_func.end(); ++it){
		
			multbuf *= (*it)->f(x);	

			if((*it)->m_operation == Operation::Plus){
				temp += multbuf;
				multbuf = static_cast<T>(1);
			} else if ((*it)->m_operation == Operation::Multiply) {
				continue;
			} else if ((*it)->m_operation == Operation::None) {
				temp += multbuf;		
			}
		}
		return temp;	
	}

	// ----メンバ二項演算子
	
	// 加算	
	inline Function<T> operator+(Function<T> right)
	{
		Function<T> temp = *this;
		temp.m_func.back()->m_operation = Operation::Plus;
		temp.m_func.push_back(right);
		return temp;
	}
	inline Function<T> operator+(T right)
	{
		Function<T> temp = *this;
		temp.m_func.back()->m_operation = Operation::Plus;
		temp.m_func.push_back(_constant(right));
		return temp;
	}
	
	// 減算
	inline Function<T> operator-(Function<T> right)
	{
		Function<T> temp = *this;
		temp.m_func.back()->m_operation = Operation::Plus;
		right.m_sgn = !right.m_sgn;
		temp.m_func.push_back(right);	
		return temp;
	}
	inline Function<T> operator-(T right)
	{
		Function<T> temp = *this, const_right = _constant(right);
		temp.m_func.back()->m_operation = Operation::Plus;
		const_right.m_sgn = false;
		temp.m_func.push_back(const_right);	
		return temp;
	}
	
	
	// 乗算
	inline Function<T> operator*(Function<T> right)
	{
		Function<T> temp = *this;
		temp.m_func.back()->m_operation = Operation::Multiply;
		temp.m_func.push_back(right);
		return temp;	
	}
	inline Function<T> operator*(T right)
	{
		Function<T> temp = *this;
		temp.m_func.back()->m_operation = Operation::Multiply;
		temp.m_func.push_back(_constant(right));
		return temp;
	}


	// 除算
	inline Function<T> operator/(Function<T> right)
	{
		Function<T> temp = *this;
		temp.m_func.back()->m_operation = Operation::Multiply;
		temp.m_func.push_back(_inv(right));
		return temp;
	}

	inline Function<T> operator/(T right)
	{
		Function<T> temp = *this;
		temp.m_func.back()->m_operation = Operation::Multiply;
		temp.m_func.push_back(_constant(static_cast<T>(1) / right));
		return temp;	
	}


	// ----フレンド二項演算子
	// 加算	
	inline friend Function<T> operator+(T left, Function<T> right){}
	
	// 減算
	inline friend Function<T> operator-(T left, Function<T> right){}

	// 乗算
	inline friend Function<T> operator*(T left, Function<T> right){}

	// 除算
	inline friend Function<T> operator/(T left, Function<T> right){}

private:

	// 標準数式関数オブジェクト
	
	// 定数関数(dequeに要素なし)
	static inline Function<T> _constant(T value)
	{
		static Function<T> obj_constant;
		static bool is_initialized = false;
		if(!is_initialized){	
			obj_constant.m_sgn = true;
			obj_constant.m_operation = Operation::None;
			obj_constant.m_is_elementary = true;
			obj_constant.m_funcobj = [value](T x) -> T
			{
				return obj_constant.m_sgn ? value : -value;
			};
			obj_constant.m_dfuncobj = [](T x) -> T
			{
				return static_cast<T>(0);
			};
			is_initialized = true;
		}
		return obj_constant;	
	}

	// f(x) = x(dequeに要素なし)
	static inline Function<T> _x()
	{
		static Function<T> obj_x;
		static bool is_initialized = false;
		if(!is_initialized){	
			obj_x.m_sgn = true;
			obj_x.m_operation = Operation::None;
			obj_x.m_is_elementary = true;
			obj_x.m_funcobj = [](Function<T> ptr, T x) -> T
			{
				return ptr.m_sgn ? x : -x;
			};
			obj_x.m_dfuncobj = [](T x) -> T
			{
				return obj_x.m_sgn ? static_cast<T>(1) : static_cast<T>(-1);
			};
			is_initialized = true;

			std::cout << "the object f(x)=x was instantiated." << std::endl;
		}
		return obj_x;	
	}
	
	// f(x) = 1/x(dequeの最初の要素を引数に取る)	
	static inline Function<T> _inv()
	{
		static Function<T> obj_inv;
		static bool is_initialized = false;
		if(!is_initialized){	
			obj_inv.sgn = true;
			obj_inv.operation = Operation::None;
			obj_inv.m_is_elementary = true;
			obj_inv.m_funcobj = [](T x) -> T
			{
				T temp = obj_inv.m_func.front()->f(x);
				return obj_inv.m_sgn
					? static_cast<T>(1)/temp
					: static_cast<T>(-1)/temp;	
			};
			obj_inv.m_dfuncobj = [](T x) -> T
			{
				T temp = obj_inv.m_func.front()->f(x);
				return obj_inv.m_sgn
					? static_cast<T>(-1) / temp / temp * obj_inv.m_func.front()->df(x)
					: static_cast<T>(1) / temp / temp * obj_inv.m_func.front()->df(x);
			};
			is_initialized = true;
		}
		return obj_inv;	
	}
	
	// f(x) = pow(x) (dequeの始めのオブジェクトを引数, 二つ目を指数に取る)
	static inline Function<T> _pow()
	{
		static Function<T> obj_pow;
		static bool is_initialized = false;
		if(!is_initialized){	
			obj_pow.sgn = true;
			obj_pow.operation = Operation::None;
			obj_pow.m_is_elementary = true;
			obj_pow.m_funcobj = [](T x) -> T
			{
				return obj_pow.m_sgn
					? std::pow(obj_pow.m_func.front()->f(x), obj_pow.m_func.back()->f(x))
					: -std::pow(obj_pow.m_func.front()->f(x), obj_pow.m_func.back()->f(x));	
			};
			obj_pow.m_dfuncobj = [](T x) -> T
			{
				T temp = obj_pow.m_func.back()->f(x);
				return obj_pow.m_sgn
					? std::pow(obj_pow.m_func.front()->f(x), temp) * temp * obj_pow.m_func.front()->df(x)
					: -std::pow(obj_pow.m_func.front()->f(x), temp) * temp * obj_pow.m_func.front()->df(x);
			};
			is_initialized = true;
		}
		return obj_pow;	
	}
	// f(x) = sin(x) (dequeのはじめのオブジェクトを引数に取る)
	static inline Function<T> _sin()
	{
		static Function<T> obj_sin;
		std::cout << "address is " << &obj_sin << std::endl;
		static bool is_initialized = false;
		if(!is_initialized){	
			obj_sin.m_sgn = true;
			obj_sin.m_operation = Operation::None;
			obj_sin.m_is_elementary = true;
			obj_sin.m_funcobj = [](T x) -> T
			{
				std::cout << "sin(x).f(x) was called." << std::endl;
				std::cout << "address is " << &obj_sin << std::endl;
				std::cout << obj_sin.hoge << std::endl;
				return obj_sin.m_sgn
					? std::sin(obj_sin.m_func.front()->f(x))
					: -std::sin(obj_sin.m_func.front()->f(x));
			};
			obj_sin.m_dfuncobj = [](T x) -> T
			{
				return obj_sin.m_sgn
					? std::cos(obj_sin.m_func.front()->f(x)) * obj_sin.m_func.front()->df(x)
					: -std::cos(obj_sin.m_func.front()->f(x)) * obj_sin.m_func.front()->df(x);
			};
			is_initialized = true;
		}
		std::cout << "object of f(x)=sin(x) was instantiated." << std::endl;
		return obj_sin;
	}

	// f(x) = cos(x) (dequeのはじめのオブジェクトを引数に取る)
	static inline Function<T> _cos()
	{
		static Function<T> obj_cos;
		static bool is_initialized = false;
		if(!is_initialized){	
			obj_cos.m_sgn = true;
			obj_cos.m_operation = Operation::None;
			obj_cos.m_is_elementary = true;
			obj_cos.m_funcobj = [](T x) -> T
			{
				return obj_cos.m_sgn
				   ? std::cos(obj_cos.m_func.front()->f(x))
				   : -std::cos(obj_cos.m_func.front()->f(x));
			};
			obj_cos.m_dfuncobj = [](T x) -> T
			{
				return obj_cos.m_sgn
					? std::sin(obj_cos.m_func.front()->f(x)) * obj_cos.m_func.front()->df(x)
					: -std::sin(obj_cos.m_func.front()->f(x)) * obj_cos.m_func.front()->df(x);
			};
			is_initialized = true;
		}
		return obj_cos;
	}

	// f(x) = tan(x) (dequeのはじめのオブジェクトを引数に取る)
	static inline Function<T> _tan()
	{
		static Function<T> obj_tan;
		static bool is_initialized = false;
		if(!is_initialized){
			obj_tan.m_sgn = true;
			obj_tan.m_operation = Operation::None;
			obj_tan.m_is_elementary = true;
			obj_tan.m_funcobj = [](T x) -> T
			{
				return obj_tan.m_sgn
					? std::tan(obj_tan.m_func.front()->f(x))
					: -std::tan(obj_tan.m_func.front()->f(x));
			};
			obj_tan.m_dfuncobj = [](T x) -> T
			{
				T temp = std::tan(obj_tan.m_func.front()->f(x));
				return obj_tan.m_sgn
					? (static_cast<T>(1) + temp * temp) * obj_tan.m_func.front()->df(x)
					: -(static_cast<T>(1) + temp * temp) * obj_tan.m_func.front()->df(x);
			};
		}
		is_initialized = true;	
		return obj_tan;	
	}

	// f(x) = arcsin(x) (dequeのはじめのオブジェクトを引数に取る)
	static inline Function<T> _arcsin()
	{
		static Function<T> obj_arcsin;
		static bool is_initialized = false;
		if(!is_initialized){
			obj_arcsin.sgn = true;
			obj_arcsin.operation = Operation::None;
			obj_arcsin.m_is_elementary = true;
			obj_arcsin.m_funcobj = [](T x) -> T
			{
				return obj_arcsin.m_sgn
					? std::asin(obj_arcsin.m_func.front()->f(x))
					: -std::asin(obj_arcsin.m_func.front()->f(x));
			};
			obj_arcsin.m_dfuncobj = [](T x) -> T
			{
				T temp = obj_arcsin.m_func.front()->f(x);
				return obj_arcsin.m_sgn
					? (static_cast<T>(1) / std::sqrt(static_cast<T>(1) - temp*temp)) * obj_arcsin.m_func.front()->df(x)
					: -(static_cast<T>(1) / std::sqrt(static_cast<T>(1) - temp*temp)) * obj_arcsin.m_func.front()->df(x);
			};
		}
		is_initialized = true;	
		return obj_arcsin;
	}

	// f(x) = arccos(x) (dequeのはじめのオブジェクトを引数に取る)
	static inline Function<T> _arccos()
	{
		static Function<T> obj_arccos;
		static bool is_initialized = false;
		if(!is_initialized){
			obj_arccos.sgn = true;
			obj_arccos.operation = Operation::None;
			obj_arccos.m_is_elementary = true;
			obj_arccos.m_funcobj = [](T x) -> T
			{
				return obj_arccos.m_sgn
					? std::acos(obj_arccos.m_func.front()->f(x))
					: -std::acos(obj_arccos.m_func.front()->f(x));
			};
			obj_arccos.m_dfuncobj = [](T x) -> T
			{
				T temp = obj_arccos.m_func.front()->f(x);
				return obj_arccos.m_sgn 
						? -(static_cast<T>(1) / std::sqrt(static_cast<T>(1) - temp*temp)) * obj_arccos.m_func.front()->df(x)
						: (static_cast<T>(1) / std::sqrt(static_cast<T>(1) - temp*temp)) * obj_arccos.m_func.front()->df(x);
			};
		}
		is_initialized = true;	
		return obj_arccos;
	}

	// f(x) = arctan(x) (dequeのはじめのオブジェクトを引数に取る)
	static inline Function<T> _arctan()
	{
		static Function<T> obj_arctan;
		static bool is_initialized = false;
		if(!is_initialized){
			obj_arctan.sgn = true;
			obj_arctan.operation = Operation::None;
			obj_arctan.m_is_elementary = true;
			obj_arctan.m_funcobj = [](T x) -> T
			{
				return obj_arctan.m_sgn 
					? std::atan(obj_arctan.m_func.front()->f(x))
					: -std::atan(obj_arctan.m_func.front()->f(x));
			};
			obj_arctan.m_dfuncobj = [](T x) -> T
			{
				T temp = obj_arctan.m_func.front()->f(x);
				return obj_arctan.m_sgn
					? (static_cast<T>(1) / (static_cast<T>(1) + temp*temp)) * obj_arctan.m_func.front()->df(x)
					: (static_cast<T>(-1) / (static_cast<T>(1) + temp*temp)) * obj_arctan.m_func.front()->df(x);
			};
		}
		is_initialized = true;	
		return obj_arctan;
	}
	// f(x) = sinh(x) (dequeのはじめのオブジェクトを引数に取る)
	static inline Function<T> _sinh()
	{
		static Function<T> obj_sinh;
		static bool is_initialized = false;
		if(!is_initialized){
			obj_sinh.sgn = true;
			obj_sinh.operation = Operation::None;
			obj_sinh.m_is_elementary = true;
			obj_sinh.m_funcobj = [](T x) -> T
			{
				return obj_sinh.m_sgn
					? std::sinh(obj_sinh.m_func.front()->f(x))
					: -std::sinh(obj_sinh.m_func.front()->f(x));
			};
			obj_sinh.m_dfuncobj = [](T x) -> T
			{
				return obj_sinh.m_sgn
					? std::cosh(obj_sinh.m_func.front()->f(x)) * obj_sinh.m_func.front()->df(x)
					: -std::cosh(obj_sinh.m_func.front()->f(x)) * obj_sinh.m_func.front()->df(x);
			};
		}
		is_initialized = true;	
		return obj_sinh;
	}

	// f(x) = cosh(x) (dequeのはじめのオブジェクトを引数に取る)
	static inline Function<T> _cosh()
	{
		static Function<T> obj_cosh;
		static bool is_initialized = false;
		if(!is_initialized){
			obj_cosh.sgn = true;
			obj_cosh.operation = Operation::None;
			obj_cosh.m_is_elementary = true;
			obj_cosh.m_funcobj = [](T x) -> T
			{
				return obj_cosh.m_sgn 
					? std::cosh(obj_cosh.m_func.front()->f(x))
					: std::cosh(obj_cosh.m_func.front()->f(x));
			};
			obj_cosh.m_dfuncobj = [](T x) -> T
			{
				return obj_cosh.m_sgn
					? obj_cosh.m_sgn * std::sinh(obj_cosh.m_func.front()->f(x)) * obj_cosh.m_func.front()->df(x)
					: -obj_cosh.m_sgn * std::sinh(obj_cosh.m_func.front()->f(x)) * obj_cosh.m_func.front()->df(x);
			};
		}
		is_initialized = true;	
		return obj_cosh;
	}

	// f(x) = exp(x) (dequeのはじめのオブジェクトを引数に取る)
	static inline Function<T> _exp()
	{
		static Function<T> obj_exp;
		static bool is_initialized = false;
		if(!is_initialized){
			obj_exp.sgn = true;
			obj_exp.operation = Operation::None;
			obj_exp.m_is_elementary = true;
			obj_exp.m_funcobj = [](T x) -> T
			{
				return obj_exp.m_sgn
					? std::exp(obj_exp.m_func.front()->f(x))
					: -std::exp(obj_exp.m_func.front()->f(x));
			};
			obj_exp.m_dfuncobj = [](T x) -> T
			{
				return obj_exp.m_sgn
					? std::exp(obj_exp.m_func.front()->f(x)) * obj_exp.m_funt.front()->df(x)
					: -std::exp(obj_exp.m_func.front()->f(x)) * obj_exp.m_funt.front()->df(x);
			};
		}
		is_initialized = true;	
		return obj_exp;
	}

		
private:

	// 標準数式関数(オペレータでのみ呼び出す)
	static inline Function<T> inv(Function<T> x)
	{
		Function<T> temp = _inv();
		temp.m_func.push_back(x);
		return temp;
	}

	static inline Function<T> square(Function<T> x)
	{
		return Function<T>([](Function<T> x)->Function<T> {return x * x;});
	}	

public:

	// 標準数式関数(Function<T>型引数)
	
	// 累乗関数	
	static inline Function<T> pow(Function<T> x, Function<T> y)
	{
		Function<T> temp = _pow();
		temp.m_func.push_back(std::make_shared<Function<T>>(x));
		temp.m_func.push_back(y);
		return temp;
	}
	static inline Function<T> pow(Function<T> x, T y)
	{
		Function<T> temp = _pow();
		temp.m_func.push_back(std::make_shared<Function<T>>(x));
		temp.m_func.push_back(_constant(y));	
		return temp;
	}

	static inline Function<T> sin(Function<T> x)
	{
		Function<T> temp = _sin();
		temp.m_func.push_back(std::make_shared<Function<T>>(x));
		std::cout << "push backed x to the deque of sin()." << std::endl;
		return temp;
	}	
	static inline Function<T> cos(Function<T> x)
	{
		Function<T> temp = _cos();
		temp.m_func.push_back(std::make_shared<Function<T>>(x));
		return temp;
	}
	static inline Function<T> tan(Function<T> x)
	{
		Function<T> temp = _tan();
		temp.m_func.push_back(std::make_shared<Function<T>>(x));
		return temp;
	}
	static inline Function<T> arcsin(Function<T> x)
	{
		Function<T> temp = _arcsin();
		temp.m_func.push_back(std::make_shared<Function<T>>(x));
		return temp;
	}
	static inline Function<T> arccos(Function<T> x)
	{
		Function<T> temp = _arccos();
		temp.m_func.push_back(std::make_shared<Function<T>>(x));
		return temp;
	}
	static inline Function<T> arctan(Function<T> x)
	{
		Function<T> temp = _arctan();
		temp.m_func.push_back(std::make_shared<Function<T>>(x));
		return temp;
	}
	static inline Function<T> sinh(Function<T> x)
	{
		Function<T> temp = _sinh();
		temp.m_func.push_back(std::make_shared<Function<T>>(x));
		return temp;
	}
	static inline Function<T> cosh(Function<T> x)
	{
		Function<T> temp = _cosh();
		temp.m_func.push_back(std::make_shared<Function<T>>(x));
		return temp;
	}
	static inline Function<T> exp(Function<T> x)
	{
		Function<T> temp = _exp();
		temp.m_func.push_back(std::make_shared<Function<T>>(x));
		return temp;
	}



};





