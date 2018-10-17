#include <tuple>
#include <ctime>

// old API
void adjust_values(double * alpha1,
		double * beta1,
		double * alpha2,
		double * beta2) {
	(void)alpha1;
	(void)beta1;
	(void)alpha2;
	(void)beta2;
}


// new design
class location {};

class reading {
public:
	double alpha_value(location l, time_t t) const {(void)l; (void)t; return 0.1;}
	double beta_value(location l, time_t t) const {(void)l; (void)t; return 0.2;}
};


std::tuple<double, double, double, double> get_adjusted_values(
		const reading &r,
		location l, time_t t1, time_t t2) {
	double alpha1 = r.alpha_value(l, t1);
	double beta1 = r.beta_value(l, t1);

	double alpha2 = r.alpha_value(l, t2);
	double beta2 = r.beta_value(l, t2);

	adjust_values(&alpha1, &beta1, &alpha2, &beta2);

	return std::make_tuple(alpha1, beta1, alpha2, beta2);
}

// templated version
template <typename Reading>
std::tuple<double, double, double, double> get_adjusted_values_2(
		const Reading &r,
		location l, time_t t1, time_t t2) {
	double alpha1 = r.alpha_value(l, t1);
	double beta1 = r.beta_value(l, t1);

	double alpha2 = r.alpha_value(l, t2);
	double beta2 = r.beta_value(l, t2);

	adjust_values(&alpha1, &beta1, &alpha2, &beta2);

	return std::make_tuple(alpha1, beta1, alpha2, beta2);
}

// more templated version
template <typename AlphaValue, typename BetaValue>
std::tuple<double, double, double, double> get_adjusted_values_3(
		AlphaValue alpha_value, BetaValue beta_value,
		location l, time_t t1, time_t t2) {
	double alpha1 = alpha_value(l, t1);
	double beta1 = beta_value(l, t1);

	double alpha2 = alpha_value(l, t2);
	double beta2 = beta_value(l, t2);

	adjust_values(&alpha1, &beta1, &alpha2, &beta2);

	return std::make_tuple(alpha1, beta1, alpha2, beta2);
}



// C functions params
template <typename F>
struct make_tuple_of_derefed_params;

template <typename Ret, typename... Args>
struct make_tuple_of_derefed_params<Ret (Args...)>
{
	using type = std::tuple<std::remove_pointer_t<Args>...>;
//                                                         ^
//                                                         Everythng left of the `...` operator is replicated for each type in `Args`
};

// convinience function
template <typename F>
using make_tuple_of_derefed_params_t = typename make_tuple_of_derefed_params<F>::type;


template <std::size_t FunctionIndex,
	 typename FunctionsTuple,
	 typename Params,
	 std::size_t... I>
auto dispatch_params(FunctionsTuple & functions,
		Params & params,
		std::index_sequence<I...>)
{
	return (std::get<FunctionIndex>(functions))
		(std::get<I>(params)...);
}

template <typename FunctionsTuple,
	 std::size_t... I,
	 typename Params,
	 typename ParamsSeq>
auto dispatch_functions(FunctionsTuple & functions,
		std::index_sequence<I...>,
		Params & params,
		ParamsSeq params_seq)
{
	return std::make_tuple(dispatch_params<I>(functions,
				params,
				params_seq)...);
}


template <typename F, typename Tuple, std::size_t... I>
void dispatch_to_c(F f, Tuple & t, std::index_sequence<I...>)
{
	f(&std::get<I>(t)...);
}


template <typename LegacyFunction, typename... Functions, typename... Params>
auto magic_wand(LegacyFunction legacy,
		const std::tuple<Functions...>& functions,
	  	const std::tuple<Params...>& params1,
	  	const std::tuple<Params...>& params2)
{
	static const std::size_t functions_count = sizeof...(Functions);
	static const std::size_t params_count = sizeof...(Params);

	auto params = std::tuple_cat(
			dispatch_functions(functions,
				std::make_index_sequence<functions_count>(),
				params1,
				std::make_index_sequence<params_count>()),
			dispatch_functions(functions,
				std::make_index_sequence<functions_count>(),
				params2,
				std::make_index_sequence<params_count>()));

	static constexpr auto t_count = std::tuple_size<decltype(params)>::value;

	dispatch_to_c(legacy, params, std::make_index_sequence<t_count>());
	return params;
}


int main() {
	// ...

	reading r;
	location l;
	time_t t1 = 0;
	time_t t2 = 0;
	// ...
	auto res = get_adjusted_values_3(
			[&r](location l, time_t t){ return r.alpha_value(l, t); },
			[&r](location l, time_t t){ return r.beta_value(l, t); },
			l, t1, t2);
	(void)res;

	const reading& rconst = r;
	magic_wand(adjust_values, std::make_tuple(
			[&rconst](location l, time_t t) { return rconst.alpha_value(l, t); },
			[&rconst](location l, time_t t) { return rconst.beta_value(l, t); }),
			std::make_tuple(l, t1),
			std::make_tuple(l, t2));
}

