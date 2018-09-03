#include <cstdint>
#include <type_traits>

static_assert(sizeof(void*) == 8, "expected 64-bit platform");

static const std::uint64_t default_buffer_size =
	std::conditional<sizeof(void*) == 8,
		std::integral_constant<std::uint64_t, 1024 * 1024 * 1024>,
		std::integral_constant<std::uint64_t, 100 * 1024 * 1024>
	>::type::value;

// we are on 64-bit
static_assert(default_buffer_size == 1024 * 1024 * 1024, "Expecting 64-bit platform");

int main() {
}

