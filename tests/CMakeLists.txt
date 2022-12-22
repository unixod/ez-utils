addTest("utils::to_signed" to-signed
    LIBS ez::utils ez::utils::tests_config)

addTest("utils::to_unsigned" to-unsigned
    LIBS ez::utils ez::utils::tests_config)

addTest("utils::cstr_to_uint" cstr-to-uint
    LIBS ez::utils ez::utils::tests_config)

addTest("utils::enum_arithmetic" enum-arithmetic
    LIBS ez::utils ez::utils::tests_config)

addTest("utils::Badge" badge
    LIBS ez::utils ez::utils::tests_config)

addTest("utils::match" match
    LIBS ez::utils ez::utils::tests_config)

addTest("utils::generator" generator
    LIBS ez::utils ez::utils::tests_config)

addTest("utils::recursive_generator" recursive_generator
    LIBS ez::utils ez::utils::tests_config)
