#include "assert.h"

int main(void) {
	test_case("assertions working") {
		assert_true(true);
	} end_test_case;
}
