#pragma once
/* include/function.hpp */

#include <deque>
#include <functional>


template<typename T>
class Function{
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

private:

	// 全体の符号
	bool sgn;

	// 次との結合(+, -, *, /)
	Operation operation;
	
	// 関数f(x)
	std::deque<Function*> _func;


public:

	// 引数なしコンストラクタ
	Function() : sgn(true){}

	// コピーコンストラクタ
	Function(const Function<T>& arg) : sgn(arg){}

	// 関数指定コンストラクタ 
	Function(const functor_type func);


	// デストラクタ
	~Function() = default;	

	/* 数値f(x)を返す */
	T f(T x)
	{

	}
	/* 数値df/dx(x)を返す */
	T df(T x)
	{

	}


public:
	// 演算子オーバーロード

	// ----単項演算子
	inline Function<T> operator+();
	inline Function<T> operator-();
		
	// ----代入演算子
	
	// 関数コピー代入
	inline Function<T>& operator=(Function<T> right);

	// 加算代入 
	inline Function<T>& operator+=(Function<T> right);

	// 減算代入
	inline Function<T>& operator-=(Function<T> right);
	
	// 乗算代入
	inline Function<T>& operator-=(Function<T> right);
	
	// 除算代入
	inline Function<T>& operator-=(Function<T> right);

	// ----メンバ二項演算子
	
	// 加算	
	inline Function<T> operator+(Function<T> right);
	
	// 減算
	inline Function<T> operator-(Function<T> right);

	// 乗算
	inline Function<T> operator*(Function<T> right);

	// 除算
	inline Function<T> operator/(Function<T> right);



	// ----フレンド二項演算子
	// 加算	
	inline Function<T> operator+(Function<T> left, Function<T> right);
	
	// 減算
	inline Function<T> operator-(Function<T> left, Function<T> right);

	// 乗算
	inline Function<T> operator*(Function<T> left, Function<T> right);

	// 除算
	inline Function<T> operator/(Function<T> left, Function<T> right);


private:
	static inline Function<T> sin(Function<T> _x);	
	static inline Function<T> cos(Function<T> _x);
	static inline Function<T> tan(Function<T> _x);
	static inline Function<T> arcsin(Function<T> _x);
	static inline Function<T> arccos(Function<T> _x);
	static inline Function<T> arctan(Function<T> _x);
	static inline Function<T> sinh(Function<T> _x);
	static inline Function<T> cosh(Function<T> _x);
	static inline Function<T> exp(Function<T> _x);

public:

	// 数式関数 
	static inline Function<T> sin(Function<T> _x);	
	static inline Function<T> cos(Function<T> _x);
	static inline Function<T> tan(Function<T> _x);
	static inline Function<T> arcsin(Function<T> _x);
	static inline Function<T> arccos(Function<T> _x);
	static inline Function<T> arctan(Function<T> _x);
	static inline Function<T> sinh(Function<T> _x);
	static inline Function<T> cosh(Function<T> _x);
	static inline Function<T> exp(Function<T> _x);



};






