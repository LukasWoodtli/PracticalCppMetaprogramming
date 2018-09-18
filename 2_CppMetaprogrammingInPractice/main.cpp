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


template <typename... Functions, typename... Params>
void magic_wand(const std::tuple<Functions...>& f,
	  	const std::tuple<Params...>& p1,
	  	const std::tuple<Params...>& p2)
{
	(void)f;
	(void)p1;
	(void)p2;
//	make_tuple_of_derefered_paramy<F> params;
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
	magic_wand(std::make_tuple(
			[&rconst](location l, time_t t) { return rconst.alpha_value(l, t); },
			[&rconst](location l, time_t t) { return rconst.beta_value(l, t); }),
			std::make_tuple(l, t1),
			std::make_tuple(l, t2));
}

