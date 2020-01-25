/**** type traits ****/

#ifndef TRAITS_H
#define TRAITS_H

namespace traits
{
	/**** remove reference ****/
	template <typename T>
	struct remove_reference
	{
		using type = T;
	};

	template <typename T>
	struct remove_reference<T&>
	{
		typedef T type;
	};

	template <typename T>
	struct remove_reference<T&&>
	{
		typedef T type;
	};

	// convenience alias template
	template <typename T>
	using remove_reference_t = typename remove_reference<T>::type;

	/**** remove const ****/
	template <typename T>
	struct remove_const
	{
		using type = T;
	};

	template <typename T>
	struct remove_const<const T>
	{
		using type = T;
	};

	// convenience alias template
	template <typename T>
	using remove_const_t = typename remove_const<T>::type;

	/**** remove volatile ****/
	template <typename T>
	struct remove_volatile
	{
		using type = T;
	};

	template <typename T>
	struct remove_volatile<volatile T>
	{
		using type = T;
	};

	// convenience alias template
	template <typename T>
	using remove_volatile_t = typename remove_volatile<T>::type;

	/**** remove const-volatile ****/
	template <typename T>
	struct remove_const_volatile : remove_volatile<remove_const_t<T>>
	{
	};

	// convenience alias template
	template <typename T>
	using remove_const_volatile_t = typename remove_const_volatile<T>::type;

	/**** decay ****/
	template <typename T>
	struct decay : remove_const_volatile<T>
	{
	};

	template <typename T>
	struct decay<T[]>
	{
		typedef T *type;
	};

	template <typename T, unsigned N>
	struct decay<T[N]>
	{
		typedef T *type;
	};

	template <typename ReturnType, typename... Args>
	struct decay<ReturnType(Args...)>
	{
		typedef ReturnType(*type)(Args...);
	};

	// convenience alias template
	template <typename T>
	using decay_t = typename decay<T>::type;

	/******** predicate traits ********/

	/**** is same ****/
	// template <typename T1, typename T2>
	// struct is_same
	// {
	//     static constexpr bool value = false;
	// };

	// template <typename T>
	// struct is_same<T,T>
	// {
	//     static constexpr bool value = true;
	// };

	// convenience variable template
	// template <typename T1, typename T2>
	// constexpr bool is_same_v = is_same<T1,T2>::value;

	template <bool b>
	struct bool_constant
	{
		using type = bool_constant<b>;
		static constexpr bool value = b;
	};

	typedef bool_constant<true> true_type;
	typedef bool_constant<false> false_type;

	template <typename T1, typename T2>
	struct is_same : false_type  // bool_constant<false>
	{
	};

	template <typename T>
	struct is_same<T, T> : true_type  // bool_constant<true>
	{
	};

	// convenience variable template
	template <typename T1, typename T2>
	constexpr bool is_same_v = is_same<T1, T2>::value;

	// convenience alias template
	template <typename T1, typename T2>
	using is_same_t = typename is_same<T1, T2>::type;
}

#endif    // TRAITS_H