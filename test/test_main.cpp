/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <boost/multiprecision/gmp.hpp>
#include <boost/rational.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cmath>
#include <format>
#include <numbers>
#include <string>
#include <string_view>

import prbegd.thecalculater.math;

using namespace thecalculater::math;

// Helper to create Rational from string
Rational R(std::string_view s) { return makeRational(s); }

// Helper to compare rationals exactly
bool equal(const Rational& a, const Rational& b)
{
    return a == b; // boost::rational supports exact equality
}

// Helper to check approx difference when irrational results are returned
void checkApprox(const Rational& value, const Rational& expected, const Rational& tolerance = Rational(1, 1000000000000000))
{
    if (value == expected) {
        return;
    }
    auto diff = value > expected ? value - expected : expected - value;
    CHECK(diff <= tolerance);
}

// Helper to format rational for printing
std::string toStr(const Rational& r)
{
    return format(r);
}

// ------------------- makeRational (string) -------------------
TEST_CASE("makeRational(string) normal cases", "[makeRational][string]")
{
    // exactly representable
    auto r1 = makeRational(" 123 ");
    CHECK(equal(r1, Rational(123)));
    auto r2 = makeRational("-3/4");
    CHECK(equal(r2, Rational(-3, 4)));
    auto r3 = makeRational("+5/1");
    CHECK(equal(r3, Rational(5)));
    auto r4 = makeRational("0");
    CHECK(equal(r4, Rational(0)));
    // leading/trailing whitespace accepted
    auto r5 = makeRational("\t  22/7 \n");
    CHECK(equal(r5, Rational(22, 7)));
}

TEST_CASE("makeRational(string) exceptions", "[makeRational][string]")
{
    // Invalid format: inner spaces
    CHECK_THROWS_AS(makeRational("12 3/4"), InvalidRationalParseException<std::string>);
    // Invalid characters
    CHECK_THROWS_AS(makeRational("abc"), InvalidRationalParseException<std::string>);
    // Zero denominator
    CHECK_THROWS_AS(makeRational("1/0"), boost::bad_rational);
    // Zero denominator with whitespace
    CHECK_THROWS_AS(makeRational("  5/0  "), boost::bad_rational);
    // Validate exception message content (optional)
    try {
        makeRational("a");
    } catch (const InvalidRationalParseException<std::string>& e) {
        CHECK(e.origin == "a");
    }
    try {
        makeRational("1/0");
    } catch (const boost::bad_rational&) {
        SUCCEED("bad_rational caught");
    }
}

// ------------------- makeRational (double) -------------------
TEST_CASE("makeRational(double) normal cases", "[makeRational][double]")
{
    auto r1 = makeRational(0.0);
    CHECK(equal(r1, Rational(0)));
    auto r2 = makeRational(1.5);
    CHECK(equal(r2, Rational(3, 2)));
    auto r3 = makeRational(-3.14); // will be approximated
    // just ensure no throw
    CHECK_NOTHROW(makeRational(-3.14));
}

TEST_CASE("makeRational(double) exceptions", "[makeRational][double]")
{
    // infinity
    CHECK_THROWS_AS(makeRational(INFINITY), InvalidRationalParseException<double>);
    // NaN
    CHECK_THROWS_AS(makeRational(NAN), InvalidRationalParseException<double>);
    // negative infinity
    CHECK_THROWS_AS(makeRational(-INFINITY), InvalidRationalParseException<double>);
    // Check exception origin
    try {
        makeRational(NAN);
    } catch (const InvalidRationalParseException<double>& e) {
        CHECK(std::isnan(e.origin));
    }
}

// ------------------- format -------------------
TEST_CASE("format normal cases", "[format]")
{
    Rational r(22, 7);
    RationalFormatOptions opts;
    opts.output = ExpressionOutputFormat::PlainText;
    opts.numeric = RationalNumericFormat::AlwaysFraction;
    auto s1 = format(r, opts);
    CHECK(s1.find("22/7") != std::string::npos);
    // LaTeX fraction
    opts.output = ExpressionOutputFormat::LaTeX;
    auto s2 = format(r, opts);
    CHECK(s2.find("\\frac{22}{7}") != std::string::npos);
    // Integer preferred
    opts.output = ExpressionOutputFormat::PlainText;
    opts.numeric = RationalNumericFormat::PreferInteger;
    Rational r2(4, 2);
    auto s3 = format(r2, opts);
    CHECK(s3.find("2") != std::string::npos);
}

// ------------------- reciprocal -------------------
TEST_CASE("reciprocal normal", "[reciprocal]")
{
    CHECK(equal(reciprocal(Rational(2)), Rational(1, 2)));
    CHECK(equal(reciprocal(Rational(-3, 4)), Rational(-4, 3)));
    CHECK(equal(reciprocal(Rational(7)), Rational(1, 7)));
}

TEST_CASE("reciprocal exception", "[reciprocal]")
{
    CHECK_THROWS_AS(reciprocal(Rational(0)), boost::bad_rational);
    CHECK_THROWS_AS(reciprocal(R("0")), boost::bad_rational);
    // different representation of zero
    CHECK_THROWS_AS(reciprocal(Rational(0, 5)), boost::bad_rational);
}

// ------------------- pow -------------------
TEST_CASE("pow normal", "[pow]")
{
    RationalCalculationOptions opt;
    opt.approximation.enabled = true; // allow irrational results approximated
    CHECK(equal(pow(Rational(2), Rational(3)), R("8")));
    CHECK(equal(pow(Rational(1, 2), Rational(2)), Rational(1, 4)));
    CHECK(equal(pow(Rational(-2), Rational(3)), Rational(-8)));
    CHECK(equal(pow(Rational(4), Rational(1, 2)), Rational(2))); // sqrt 4
}

TEST_CASE("pow exceptions", "[pow]")
{
    RationalCalculationOptions opt;
    // 0^0 -> IndeterminateForm
    CHECK_THROWS_AS(pow(Rational(0), Rational(0)), RationalCalculationException);
    try {
        pow(Rational(0), Rational(0));
    } catch (const RationalCalculationException& e) {
        CHECK(e.type == RationalCalculationException::Type::IndeterminateForm);
        CHECK(e.operation == RationalCalculationException::Operation::Power);
    }
    // negative base, even denominator exponent -> Domain
    CHECK_THROWS_AS(pow(Rational(-2), Rational(1, 2)), RationalCalculationException);
    try {
        pow(Rational(-2), Rational(1, 2));
    } catch (const RationalCalculationException& e) {
        CHECK(e.type == RationalCalculationException::Type::Domain);
    }
    // 0 ^ negative -> Pole
    CHECK_THROWS_AS(pow(Rational(0), Rational(-1)), RationalCalculationException);
    try {
        pow(Rational(0), Rational(-1));
    } catch (const RationalCalculationException& e) {
        CHECK(e.type == RationalCalculationException::Type::Pole);
    }
    // irrational result with approximation disabled
    opt.approximation.enabled = false;
    CHECK_THROWS_AS(pow(Rational(2), Rational(1, 2), opt), RationalCalculationException);
    try {
        pow(Rational(2), Rational(1, 2), opt);
    } catch (const RationalCalculationException& e) {
        CHECK(e.type == RationalCalculationException::Type::IrrationalResult);
    }
}

// ------------------- factorial -------------------
TEST_CASE("factorial normal", "[factorial]")
{
    CHECK(factorial(Integer(0)) == Integer(1));
    CHECK(factorial(Integer(1)) == Integer(1));
    CHECK(factorial(Integer(5)) == Integer(120));
    CHECK(factorial(Integer(10)) == Integer(3628800));
}

TEST_CASE("factorial exception", "[factorial]")
{
    CHECK_THROWS_AS(factorial(Integer(-1)), RationalCalculationException);
    try {
        factorial(Integer(-1));
    } catch (const RationalCalculationException& e) {
        CHECK(e.type == RationalCalculationException::Type::Pole);
        CHECK(e.operation == RationalCalculationException::Operation::Factorial);
    }
    CHECK_THROWS_AS(factorial(Integer(-5)), RationalCalculationException);
    try {
        factorial(Integer(-100));
    } catch (const RationalCalculationException& e) {
        CHECK(e.type == RationalCalculationException::Type::Pole);
    }
}

// ------------------- mod and operator% -------------------
TEST_CASE("mod normal", "[mod]")
{
    CHECK(equal(mod(Rational(5), Rational(2)), Rational(1)));
    CHECK(equal(mod(Rational(10), Rational(3)), Rational(1)));
    CHECK(equal(mod(Rational(-7), Rational(3)), Rational(2))); // depends on implementation, but must not throw
    CHECK(equal(Rational(9) % Rational(4), Rational(1)));
}

TEST_CASE("mod exception", "[mod]")
{
    CHECK_THROWS_AS(mod(Rational(1), Rational(0)), boost::bad_rational);
    CHECK_THROWS_AS(mod(Rational(0), Rational(0)), boost::bad_rational);
    CHECK_THROWS_AS(Rational(5) % Rational(0), boost::bad_rational);
}

// ------------------- root -------------------
TEST_CASE("root normal", "[root]")
{
    RationalCalculationOptions opt;
    CHECK(equal(root(Rational(9), Rational(2)), Rational(3)));
    CHECK(equal(root(Rational(27), Rational(3)), Rational(3)));
    CHECK(equal(root(Rational(-8), Rational(3)), Rational(-2)));
    CHECK(equal(root(Rational(0), Rational(5)), Rational(0)));
}

TEST_CASE("root exception", "[root]")
{
    RationalCalculationOptions opt;
    // index zero
    CHECK_THROWS_AS(root(Rational(5), Rational(0)), RationalCalculationException);
    try {
        root(Rational(5), Rational(0));
    } catch (const RationalCalculationException& e) {
        CHECK(e.type == RationalCalculationException::Type::Domain);
    }
    // negative radicand with even index
    CHECK_THROWS_AS(root(Rational(-4), Rational(2)), RationalCalculationException);
    try {
        root(Rational(-4), Rational(2));
    } catch (const RationalCalculationException& e) {
        CHECK(e.type == RationalCalculationException::Type::Domain);
    }
}

// ------------------- sqrt -------------------
TEST_CASE("sqrt normal", "[sqrt]")
{
    CHECK(equal(sqrt(Rational(0)), Rational(0)));
    CHECK(equal(sqrt(Rational(1)), Rational(1)));
    CHECK(equal(sqrt(Rational(4)), Rational(2)));
    CHECK(equal(sqrt(Rational(9)), Rational(3)));
}

TEST_CASE("sqrt exception", "[sqrt]")
{
    CHECK_THROWS_AS(sqrt(Rational(-1)), RationalCalculationException);
    CHECK_THROWS_AS(sqrt(Rational(-100)), RationalCalculationException);
    try {
        sqrt(Rational(-1));
    } catch (const RationalCalculationException& e) {
        CHECK(e.type == RationalCalculationException::Type::Domain);
    }
}

// ------------------- cbrt -------------------
TEST_CASE("cbrt normal", "[cbrt]")
{
    CHECK(equal(cbrt(Rational(8)), Rational(2)));
    CHECK(equal(cbrt(Rational(-8)), Rational(-2)));
    CHECK(equal(cbrt(Rational(0)), Rational(0)));
    CHECK(equal(cbrt(Rational(27)), Rational(3)));
}
// cbrt has no declared exceptions, so no exception tests.

// ------------------- trig functions -------------------
TEST_CASE("sin/cos normal & exceptions", "[trig]")
{
    RationalCalculationOptions opt;
    opt.approximation.enabled = true;
    // sin(0)=0
    CHECK(equal(sin(Rational(0)), Rational(0)));
    // cos(0)=1
    CHECK(equal(cos(Rational(0)), Rational(1)));
    // sin(pi/2) approx 1
    auto pi_half = Rational(7853981633974483, 5000000000000000); // pi/2 approximation
    CHECK(equal(sin(pi_half, opt), Rational(1))); // with approximation should be close to 1
    // cos(pi/2) approx 0
    auto res = cos(pi_half, opt);
    CHECK(res < Rational(1, 1000000000000000)); // near zero
    // exception: irrational with approximation disabled
    opt.approximation.enabled = false;
    CHECK_THROWS_AS(sin(Rational(1), opt), RationalCalculationException);
    try {
        sin(Rational(1), opt);
    } catch (const RationalCalculationException& e) {
        CHECK(e.type == RationalCalculationException::Type::IrrationalResult);
        CHECK(e.operation == RationalCalculationException::Operation::Sine);
    }
    CHECK_THROWS_AS(cos(Rational(1), opt), RationalCalculationException);
}

TEST_CASE("tan/cot/sec/csc poles & irrational", "[trig]")
{
    RationalCalculationOptions opt;
    opt.approximation.enabled = false;
    // tan(pi/2) -> Pole
    auto pi_half = Rational(7853981633974483, 5000000000000000);
    CHECK_THROWS_AS(tan(pi_half, opt), RationalCalculationException);
    try {
        tan(pi_half, opt);
    } catch (const RationalCalculationException& e) {
        CHECK(e.type == RationalCalculationException::Type::Pole);
        CHECK(e.operation == RationalCalculationException::Operation::Tangent);
    }
    // sec(pi/2) -> Pole
    CHECK_THROWS_AS(sec(pi_half, opt), RationalCalculationException);
    // cot(0) -> Pole
    CHECK_THROWS_AS(cot(Rational(0), opt), RationalCalculationException);
    try {
        cot(Rational(0), opt);
    } catch (const RationalCalculationException& e) {
        CHECK(e.type == RationalCalculationException::Type::Pole);
        CHECK(e.operation == RationalCalculationException::Operation::Cotangent);
    }
    // csc(0) -> Pole
    CHECK_THROWS_AS(csc(Rational(0), opt), RationalCalculationException);
    // irrational results
    CHECK_THROWS_AS(tan(Rational(1), opt), RationalCalculationException);
    CHECK_THROWS_AS(cot(Rational(1), opt), RationalCalculationException);
    CHECK_THROWS_AS(sec(Rational(1), opt), RationalCalculationException);
    CHECK_THROWS_AS(csc(Rational(1), opt), RationalCalculationException);
}

// ------------------- inverse trig -------------------
TEST_CASE("arcsin/arccos normal & domain", "[invtrig]")
{
    RationalCalculationOptions opt;
    opt.approximation.enabled = true;
    CHECK(equal(arcsin(Rational(0)), Rational(0)));
    CHECK(equal(arccos(Rational(1)), Rational(0)));
    // arcsin(1) = pi/2
    auto pi_half = Rational(7853981633974483, 5000000000000000);
    checkApprox(arcsin(Rational(1)), pi_half);
    // domain errors
    CHECK_THROWS_AS(arcsin(Rational(2)), RationalCalculationException);
    try {
        arcsin(Rational(2));
    } catch (const RationalCalculationException& e) {
        CHECK(e.type == RationalCalculationException::Type::Domain);
    }
    CHECK_THROWS_AS(arccos(Rational(-2)), RationalCalculationException);
}

TEST_CASE("arctan/arccot normal", "[invtrig]")
{
    RationalCalculationOptions opt;
    opt.approximation.enabled = true;
    CHECK(equal(arctan(Rational(0)), Rational(0)));
    auto pi_4 = Rational(7853981633974483, 10000000000000000); // pi/4 approx
    checkApprox(arctan(Rational(1)), pi_4);
    checkApprox(arccot(Rational(1)), pi_4);
}

TEST_CASE("arcsec/arccsc domain", "[invtrig]")
{
    RationalCalculationOptions opt;
    opt.approximation.enabled = true;
    CHECK(equal(arcsec(Rational(1)), Rational(0)));
    auto pi_3 = Rational(7853981633974483, 7500000000000000); // pi/3 approx
    checkApprox(arcsec(Rational(2)), pi_3);
    // domain: values in (-1, 1) throw
    CHECK_THROWS_AS(arcsec(Rational(0)), RationalCalculationException);
    CHECK_THROWS_AS(arcsec(Rational(1, 2)), RationalCalculationException);
    CHECK_THROWS_AS(arccsc(Rational(0)), RationalCalculationException);
    try {
        arcsec(Rational(0));
    } catch (const RationalCalculationException& e) {
        CHECK(e.type == RationalCalculationException::Type::Domain);
    }
}

// ------------------- floor / ceil -------------------
TEST_CASE("floor/ceil normal", "[rounding]")
{
    CHECK(floor(Rational(3, 2)) == Integer(1));
    CHECK(ceil(Rational(3, 2)) == Integer(2));
    CHECK(floor(Rational(-3, 2)) == Integer(-2));
    CHECK(ceil(Rational(-3, 2)) == Integer(-1));
    CHECK(floor(Rational(4)) == Integer(4));
    CHECK(ceil(Rational(4)) == Integer(4));
}

// ------------------- ln -------------------
TEST_CASE("ln normal", "[log]")
{
    RationalCalculationOptions opt;
    opt.approximation.enabled = true;
    CHECK(equal(ln(Rational(1)), Rational(0)));
    auto euler = Rational(6795704571147613, 2500000000000000);
    checkApprox(ln(euler), Rational(1));
    checkApprox(ln(Rational(2)), Rational(6931471805599453, 10000000000000000)); // approx ln2
}

TEST_CASE("ln exceptions", "[log]")
{
    RationalCalculationOptions opt;
    opt.approximation.enabled = false;
    // argument 0 -> Pole
    CHECK_THROWS_AS(ln(Rational(0)), RationalCalculationException);
    try {
        ln(Rational(0));
    } catch (const RationalCalculationException& e) {
        CHECK(e.type == RationalCalculationException::Type::Pole);
    }
    // negative argument -> Domain
    CHECK_THROWS_AS(ln(Rational(-1)), RationalCalculationException);
    try {
        ln(Rational(-1));
    } catch (const RationalCalculationException& e) {
        CHECK(e.type == RationalCalculationException::Type::Domain);
    }
    // irrational with approximation disabled
    CHECK_THROWS_AS(ln(Rational(2), opt), RationalCalculationException);
}

// ------------------- log -------------------
TEST_CASE("log normal", "[log]")
{
    RationalCalculationOptions opt;
    opt.approximation.enabled = true;
    CHECK(equal(log(Rational(8), Rational(2)), Rational(3)));
    CHECK(equal(log(Rational(1, 8), Rational(2)), Rational(-3)));
    CHECK(equal(log(Rational(9), Rational(3)), Rational(2)));
}

TEST_CASE("log exceptions", "[log]")
{
    RationalCalculationOptions opt;
    opt.approximation.enabled = false;
    // base = 1 -> IndeterminateForm
    CHECK_THROWS_AS(log(Rational(2), Rational(1)), RationalCalculationException);
    try {
        log(Rational(2), Rational(1));
    } catch (const RationalCalculationException& e) {
        CHECK(e.type == RationalCalculationException::Type::IndeterminateForm);
    }
    // base negative -> Domain
    CHECK_THROWS_AS(log(Rational(2), Rational(-2)), RationalCalculationException);
    // argument negative -> Domain
    CHECK_THROWS_AS(log(Rational(-2), Rational(2)), RationalCalculationException);
    // irrational
    CHECK_THROWS_AS(log(Rational(2), Rational(3), opt), RationalCalculationException);
}

// ------------------- lg -------------------
TEST_CASE("lg normal", "[log]")
{
    RationalCalculationOptions opt;
    opt.approximation.enabled = true;
    CHECK(equal(lg(Rational(10)), Rational(1)));
    CHECK(equal(lg(Rational(100)), Rational(2)));
    CHECK(equal(lg(Rational(1)), Rational(0)));
}

TEST_CASE("lg exceptions", "[log]")
{
    RationalCalculationOptions opt;
    opt.approximation.enabled = false;
    // argument negative -> Domain
    CHECK_THROWS_AS(lg(Rational(-1)), RationalCalculationException);
    try {
        lg(Rational(-1));
    } catch (const RationalCalculationException& e) {
        CHECK(e.type == RationalCalculationException::Type::Domain);
    }
    // irrational
    CHECK_THROWS_AS(lg(Rational(2), opt), RationalCalculationException);
}

// ==================== BENCHMARKS ====================
// Provide small, medium, large inputs for each target

TEST_CASE("benchmark makeRational(string)", "[bench]")
{
    BENCHMARK("small") { return makeRational("123/456"); };
    BENCHMARK("medium")
    {
        return makeRational("12345678901234567890/9876543210987654321");
    };
    BENCHMARK("large")
    {
        return makeRational("123456789012345678901234567890/98765432109876543210987654321");
    };
}

TEST_CASE("benchmark makeRational(double)", "[bench]")
{
    BENCHMARK("small") { return makeRational(3.14); };
    BENCHMARK("medium") { return makeRational(123456.789); };
    BENCHMARK("large") { return makeRational(5.2124486563e12); };
}

TEST_CASE("benchmark reciprocal", "[bench]")
{
    auto small = R("2/3");
    auto medium = R("12345678901234567890/9876543210987654321");
    auto large = R("123456789012345678901234567890/987654321098765432109876543210");
    BENCHMARK("small") { return reciprocal(small); };
    BENCHMARK("medium") { return reciprocal(medium); };
    BENCHMARK("large") { return reciprocal(large); };
}

TEST_CASE("benchmark pow", "[bench]")
{
    RationalCalculationOptions opt;
    BENCHMARK("small") { return pow(Rational(2), Rational(3)); };
    BENCHMARK("medium") { return pow(Rational(123), Rational(5), opt); };
    BENCHMARK("large") { return pow(Rational(2), Rational(50), opt); };
}

TEST_CASE("benchmark factorial", "[bench]")
{
    BENCHMARK("small") { return factorial(Integer(10)); };
    BENCHMARK("medium") { return factorial(Integer(50)); };
    BENCHMARK("large") { return factorial(Integer(100)); };
}

TEST_CASE("benchmark mod", "[bench]")
{
    auto small1 = R("10"), small2 = R("3");
    auto med1 = R("123456789"), med2 = R("98765");
    auto large1 = R("12345678901234567890"), large2 = R("9876543210987654321");
    BENCHMARK("small") { return mod(small1, small2); };
    BENCHMARK("medium") { return mod(med1, med2); };
    BENCHMARK("large") { return mod(large1, large2); };
}

TEST_CASE("benchmark root", "[bench]")
{
    RationalCalculationOptions opt;
    BENCHMARK("small") { return root(Rational(27), Rational(3)); };
    BENCHMARK("medium") { return root(Rational(1000000), Rational(6), opt); };
    BENCHMARK("large") { return root(Rational(2), Rational(10), opt); };
}

TEST_CASE("benchmark sqrt", "[bench]")
{
    BENCHMARK("small") { return sqrt(Rational(9)); };
    BENCHMARK("medium") { return sqrt(Rational(123456789)); };
    BENCHMARK("large") { return sqrt(Rational(1234567890123456789)); };
}

TEST_CASE("benchmark cbrt", "[bench]")
{
    BENCHMARK("small") { return cbrt(Rational(27)); };
    BENCHMARK("medium") { return cbrt(Rational(123456789)); };
    BENCHMARK("large") { return cbrt(Rational(1234567890123456789)); };
}

TEST_CASE("benchmark trig", "[bench]")
{
    RationalCalculationOptions opt;
    BENCHMARK("sin small") { return sin(Rational(1), opt); };
    BENCHMARK("sin medium") { return sin(Rational(100), opt); };
    BENCHMARK("sin large") { return sin(Rational(1000), opt); };
    BENCHMARK("cos small") { return cos(Rational(1), opt); };
    BENCHMARK("cos medium") { return cos(Rational(100), opt); };
    BENCHMARK("cos large") { return cos(Rational(1000), opt); };
    BENCHMARK("tan small") { return tan(Rational(1), opt); };
    BENCHMARK("tan medium") { return tan(Rational(100), opt); };
    BENCHMARK("tan large") { return tan(Rational(1000), opt); };
}

TEST_CASE("benchmark inverse trig", "[bench]")
{
    RationalCalculationOptions opt;
    BENCHMARK("arcsin small") { return arcsin(Rational(1, 2), opt); };
    BENCHMARK("arcsin medium") { return arcsin(Rational(9, 10), opt); };
    BENCHMARK("arcsin large") { return arcsin(Rational(99, 100), opt); };
    BENCHMARK("arctan small") { return arctan(Rational(1), opt); };
    BENCHMARK("arctan medium") { return arctan(Rational(10), opt); };
    BENCHMARK("arctan large") { return arctan(Rational(100), opt); };
}

TEST_CASE("benchmark floor/ceil", "[bench]")
{
    auto small = R("3/2");
    auto medium = R("123456789/2");
    auto large = R("12345678901234567890/987654321");
    BENCHMARK("floor small") { return floor(small); };
    BENCHMARK("floor medium") { return floor(medium); };
    BENCHMARK("floor large") { return floor(large); };
    BENCHMARK("ceil small") { return ceil(small); };
    BENCHMARK("ceil medium") { return ceil(medium); };
    BENCHMARK("ceil large") { return ceil(large); };
}

TEST_CASE("benchmark ln/log/lg", "[bench]")
{
    RationalCalculationOptions opt;
    BENCHMARK("ln small") { return ln(Rational(2), opt); };
    BENCHMARK("ln medium") { return ln(Rational(123456), opt); };
    BENCHMARK("ln large") { return ln(Rational(1234567890), opt); };
    BENCHMARK("log small") { return log(Rational(8), Rational(2), opt); };
    BENCHMARK("log medium") { return log(Rational(123456), Rational(2), opt); };
    BENCHMARK("log large") { return log(Rational(1234567890), Rational(2), opt); };
    BENCHMARK("lg small") { return lg(Rational(10), opt); };
    BENCHMARK("lg medium") { return lg(Rational(1000), opt); };
    BENCHMARK("lg large") { return lg(Rational(1000000), opt); };
}