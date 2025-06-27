#pragma once
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/rational.hpp>

namespace TheCalculater::math {
    using _fraction = boost::rational<boost::multiprecision::cpp_int>;
}