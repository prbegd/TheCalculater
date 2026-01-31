/**
 * @file test_main.cpp
 * @author prbegd
 * @date 2025-12-21
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
// TODO: Use catch2 v3 to replace this old-fashioned temporary solution.
#include <string_view>
#define CATCH_CONFIG_RUNNER
#include "catch2/catch.hpp" // IWYU pragma: keep

#include "spdlog/spdlog.h"

#include "TheCalculater/math/fraction.hpp"
#include "TheCalculater/settings.hpp"
#include "TheCalculater/util/json.hpp"
#include <QResource>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/rational.hpp>
#include <json/value.h>

using namespace TheCalculater::math;

TEST_CASE("makeFraction - string")
{
    SECTION("Valid fraction strings")
    {
        CHECK(makeFraction(std::string("1/2")) == Fraction(1, 2));
        CHECK(makeFraction(std::string("-3/4")) == Fraction(-3, 4));
        CHECK(makeFraction(std::string("5")) == Fraction(5, 1));
        CHECK(makeFraction(std::string("0")) == Fraction(0, 1));
    }

    SECTION("Valid decimal strings")
    {
        CHECK(makeFraction(std::string("0.5")) == Fraction(1, 2));
        CHECK(makeFraction(std::string("-3.14")) == Fraction(-157, 50));
        CHECK(makeFraction(std::string("2.0")) == Fraction(2, 1));
        CHECK(makeFraction(std::string("0.125")) == Fraction(1, 8));
    }

    SECTION("Invalid strings throw")
    {
        CHECK_THROWS_AS(makeFraction(std::string("")), std::invalid_argument);
        CHECK_THROWS_AS(makeFraction(std::string("abc")), std::invalid_argument);
        CHECK_THROWS_AS(makeFraction(std::string("1/0")), boost::bad_rational);
        CHECK_THROWS_AS(makeFraction(std::string("1/2/3")), std::invalid_argument);
    }
}

TEST_CASE("makeFraction - double")
{
    CHECK(makeFraction(0.5) == Fraction(1, 2));
    CHECK(makeFraction(0.25) == Fraction(1, 4));
    CHECK(makeFraction(2.0) == Fraction(2, 1));
    CHECK(makeFraction(-0.75) == Fraction(-3, 4));
}

TEST_CASE("reciprocal")
{
    SECTION("Valid reciprocals")
    {
        CHECK(reciprocal(Fraction(1, 2)) == Fraction(2, 1));
        CHECK(reciprocal(Fraction(3, 4)) == Fraction(4, 3));
        CHECK(reciprocal(Fraction(-2, 3)) == Fraction(-3, 2));
        CHECK(reciprocal(Fraction(5, 1)) == Fraction(1, 5));
        auto max = std::numeric_limits<boost::multiprecision::cpp_int>::max();
    }

    SECTION("Zero throws domain_error")
    {
        CHECK_THROWS_AS(reciprocal(Fraction(0, 1)), std::domain_error);
    }
}

TEST_CASE("pow")
{
    SECTION("Integer exponents")
    {
        CHECK(pow(Fraction(2, 3), Fraction(2, 1)) == Fraction(4, 9));
        CHECK(pow(Fraction(1, 2), Fraction(3, 1)) == Fraction(1, 8));
        CHECK(pow(Fraction(3, 2), Fraction(0, 1)) == Fraction(1, 1));
        CHECK(pow(Fraction(4, 9), Fraction(1, 2)) == Fraction(2, 3)); // Square root
    }

    SECTION("Negative exponents")
    {
        CHECK(pow(Fraction(2, 3), Fraction(-1, 1)) == Fraction(3, 2));
        CHECK(pow(Fraction(1, 2), Fraction(-2, 1)) == Fraction(4, 1));
    }

    SECTION("Fractional exponents")
    {
        CHECK(pow(Fraction(4, 1), Fraction(1, 2)) == Fraction(2, 1)); // sqrt(4)
        CHECK(pow(Fraction(27, 8), Fraction(1, 3)) == Fraction(3, 2)); // cbrt(27/8)
    }
}

TEST_CASE("factorial")
{
    CHECK(factorial(Fraction(0, 1)) == Fraction(1, 1));
    CHECK(factorial(Fraction(1, 1)) == Fraction(1, 1));
    CHECK(factorial(Fraction(2, 1)) == Fraction(2, 1));
    CHECK(factorial(Fraction(3, 1)) == Fraction(6, 1));
    CHECK(factorial(Fraction(4, 1)) == Fraction(24, 1));
    CHECK(factorial(Fraction(5, 1)) == Fraction(120, 1));
}

TEST_CASE("mod operator")
{
    CHECK(Fraction(7, 1) % Fraction(3, 1) == Fraction(1, 1));
    CHECK(Fraction(10, 1) % Fraction(3, 1) == Fraction(1, 1));
    CHECK(Fraction(5, 2) % Fraction(1, 2) == Fraction(0, 1));
    CHECK(Fraction(-7, 1) % Fraction(3, 1) == Fraction(2, 1)); // -7 mod 3 = 2
}

TEST_CASE("mod function")
{
    CHECK(mod(Fraction(7, 1), Fraction(3, 1)) == Fraction(1, 1));
    CHECK(mod(Fraction(10, 1), Fraction(3, 1)) == Fraction(1, 1));
    CHECK(mod(Fraction(5, 2), Fraction(1, 2)) == Fraction(0, 1));
    CHECK(mod(Fraction(-7, 1), Fraction(3, 1)) == Fraction(2, 1));
}

TEST_CASE("root")
{
    SECTION("Square root")
    {
        CHECK(root(Fraction(4, 1), 2) == Fraction(2, 1));
        CHECK(root(Fraction(9, 4), 2) == Fraction(3, 2));
        CHECK(root(Fraction(0, 1), 2) == Fraction(0, 1));
    }

    SECTION("Cube root")
    {
        CHECK(root(Fraction(8, 1), 3) == Fraction(2, 1));
        CHECK(root(Fraction(27, 8), 3) == Fraction(3, 2));
        CHECK(root(Fraction(-8, 1), 3) == Fraction(-2, 1));
    }

    SECTION("Invalid n throws")
    {
        CHECK_THROWS_AS(root(Fraction(4, 1), 0), std::invalid_argument);
        CHECK_THROWS_AS(root(Fraction(4, 1), -2), std::invalid_argument);
    }

    SECTION("Negative number with even n throws")
    {
        CHECK_THROWS_AS(root(Fraction(-4, 1), 2), std::domain_error);
        CHECK_THROWS_AS(root(Fraction(-9, 4), 4), std::domain_error);
    }
}

TEST_CASE("sqrt function")
{
    CHECK(sqrt(Fraction(4, 1)) == Fraction(2, 1));
    CHECK(sqrt(Fraction(9, 4)) == Fraction(3, 2));
    CHECK(sqrt(Fraction(0, 1)) == Fraction(0, 1));
    CHECK_THROWS_AS(sqrt(Fraction(-4, 1)), std::domain_error);
}

TEST_CASE("cbrt function")
{
    CHECK(cbrt(Fraction(8, 1)) == Fraction(2, 1));
    CHECK(cbrt(Fraction(27, 8)) == Fraction(3, 2));
    CHECK(cbrt(Fraction(-8, 1)) == Fraction(-2, 1));
    CHECK(cbrt(Fraction(0, 1)) == Fraction(0, 1));
}

TEST_CASE("Trigonometric functions - basic angles")
{
    Fraction zero(0, 1);
    Fraction pi_6 = makeFraction(std::string("3.14159265358979323846")) / 6; // Approximation
    Fraction pi_4 = makeFraction(std::string("3.14159265358979323846")) / 4;
    Fraction pi_3 = makeFraction(std::string("3.14159265358979323846")) / 3;
    Fraction pi_2 = makeFraction(std::string("3.14159265358979323846")) / 2;

    SECTION("sin")
    {
        CHECK(sin(zero) == zero);
        // Note: Actual values would depend on implementation
    }

    SECTION("cos")
    {
        CHECK(cos(zero) == Fraction(1, 1));
    }

    SECTION("tan")
    {
        CHECK(tan(zero) == zero);
        CHECK_THROWS_AS(tan(pi_2), std::domain_error);
    }

    SECTION("cot")
    {
        CHECK_THROWS_AS(cot(zero), std::domain_error);
    }

    SECTION("sec")
    {
        CHECK(sec(zero) == Fraction(1, 1));
        CHECK_THROWS_AS(sec(pi_2), std::domain_error);
    }

    SECTION("csc")
    {
        CHECK_THROWS_AS(csc(zero), std::domain_error);
    }
}

TEST_CASE("Inverse trigonometric functions")
{
    SECTION("arcsin")
    {
        CHECK(arcsin(Fraction(0, 1)) == Fraction(0, 1));
        CHECK(arcsin(Fraction(1, 1)) == makeFraction(std::string("1.57079632679489661923"))); // π/2
        CHECK_THROWS_AS(arcsin(Fraction(2, 1)), std::domain_error);
        CHECK_THROWS_AS(arcsin(Fraction(-2, 1)), std::domain_error);
    }

    SECTION("arccos")
    {
        CHECK(arccos(Fraction(1, 1)) == Fraction(0, 1));
        CHECK(arccos(Fraction(0, 1)) == makeFraction(std::string("1.57079632679489661923"))); // π/2
        CHECK_THROWS_AS(arccos(Fraction(2, 1)), std::domain_error);
    }

    SECTION("arctan")
    {
        CHECK(arctan(Fraction(0, 1)) == Fraction(0, 1));
        CHECK(arctan(Fraction(1, 1)) == makeFraction(std::string("0.78539816339744830962"))); // π/4
    }

    SECTION("arccot")
    {
        CHECK(arccot(Fraction(1, 1)) == makeFraction(std::string("0.78539816339744830962"))); // π/4
    }

    SECTION("arcsec")
    {
        CHECK_THROWS_AS(arcsec(Fraction(1, 2)), std::domain_error);
        CHECK(arcsec(Fraction(1, 1)) == Fraction(0, 1));
    }

    SECTION("arccsc")
    {
        CHECK_THROWS_AS(arccsc(Fraction(1, 2)), std::domain_error);
        CHECK(arccsc(Fraction(1, 1)) == makeFraction(std::string("1.57079632679489661923"))); // π/2
    }
}

TEST_CASE("floor and ceil")
{
    CHECK(floor(Fraction(3, 2)) == 1);
    CHECK(floor(Fraction(7, 3)) == 2);
    CHECK(floor(Fraction(-3, 2)) == -2);
    CHECK(floor(Fraction(5, 1)) == 5);

    CHECK(ceil(Fraction(3, 2)) == 2);
    CHECK(ceil(Fraction(7, 3)) == 3);
    CHECK(ceil(Fraction(-3, 2)) == -1);
    CHECK(ceil(Fraction(5, 1)) == 5);
}

TEST_CASE("Logarithmic functions")
{
    SECTION("ln")
    {
        CHECK(ln(Fraction(1, 1)) == Fraction(0, 1));
        CHECK(ln(makeFraction(std::string("2.71828182845904523536"))) == Fraction(1, 1)); // ln(e) ≈ 1
    }

    SECTION("log with base")
    {
        CHECK(log(Fraction(100, 1), Fraction(10, 1)) == Fraction(2, 1));
        CHECK(log(Fraction(8, 1), Fraction(2, 1)) == Fraction(3, 1));
        CHECK(log(Fraction(1, 1), Fraction(2, 1)) == Fraction(0, 1));
    }

    SECTION("lg (base 10)")
    {
        CHECK(lg(Fraction(1, 1)) == Fraction(0, 1));
        CHECK(lg(Fraction(10, 1)) == Fraction(1, 1));
        CHECK(lg(Fraction(100, 1)) == Fraction(2, 1));
        CHECK(lg(Fraction(1000, 1)) == Fraction(3, 1));
    }
}

TEST_CASE("Edge cases and special values")
{
    SECTION("pow with zero base")
    {
        CHECK(pow(Fraction(0, 1), Fraction(2, 1)) == Fraction(0, 1));
        CHECK(pow(Fraction(0, 1), Fraction(0, 1)) == Fraction(1, 1)); // 0^0 = 1
    }

    SECTION("mod with zero divisor")
    {
        CHECK_THROWS_AS(Fraction(5, 1) % Fraction(0, 1), boost::bad_rational);
        CHECK_THROWS_AS(mod(Fraction(5, 1), Fraction(0, 1)), boost::bad_rational);
    }

    SECTION("factorial with non-integer")
    {
        // This should fail since documentation says x MUST be integer >= 0
        // Implementation may throw or have undefined behavior
    }

    SECTION("factorial with negative")
    {
        // Should fail similarly
    }
}

int main(int argc, char* argv[])
{
    if (!QResource::registerResource("./resources.rcc")) {
        SPDLOG_CRITICAL("Failed to load resource file");
        return 1;
    }

    TheCalculater::settings::setSettingsFilePath("settings.json5");
    TheCalculater::settings::loadConfigTemplate(TheCalculater::util::parse(TheCalculater::util::readResourcesFile(":/resources/data/config_template.json5").constData()));
    std::unordered_map<std::string, std::string> errors;
    TheCalculater::settings::parseSettings(errors);
    if (!errors.empty()) {
        std::ostringstream oss;
        for (const auto& [key, value] : errors) {
            oss << "Key: '" << key << "' Error: '" << value << "'\n";
        }
        SPDLOG_ERROR("Errors parsing settings:\n{}", oss.str());
    }

    return Catch::Session().run(argc, argv);
}
