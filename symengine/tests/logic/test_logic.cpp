#include "catch.hpp"

#include <symengine/logic.h>
#include <symengine/add.h>
#include <symengine/real_double.h>
#include <symengine/symengine_exception.h>

using SymEngine::SymEngineException;
using SymEngine::Basic;
using SymEngine::Integer;
using SymEngine::integer;
using SymEngine::real_double;
using SymEngine::Interval;
using SymEngine::interval;
using SymEngine::symbol;
using SymEngine::piecewise;
using SymEngine::contains;
using SymEngine::Contains;
using SymEngine::add;
using SymEngine::boolTrue;
using SymEngine::boolFalse;
using SymEngine::is_a;
using SymEngine::vec_basic;
using SymEngine::unified_eq;
using SymEngine::zero;
using SymEngine::one;
using SymEngine::eq;
using SymEngine::boolean;
using SymEngine::logical_and;
using SymEngine::logical_or;
using SymEngine::logical_not;
using SymEngine::logical_nand;
using SymEngine::logical_nor;
using SymEngine::logical_xor;
using SymEngine::logical_xnor;
using SymEngine::set_boolean;
using SymEngine::vec_boolean;
using SymEngine::Xor;
using SymEngine::Not;

TEST_CASE("BooleanAtom : Basic", "[basic]")
{
    CHECK(boolTrue->__str__() == "True");
    CHECK(boolFalse->__str__() == "False");

    vec_basic v = boolTrue->get_args();
    vec_basic u;
    CHECK(unified_eq(v, u));

    auto x = symbol("x");
    CHECK_THROWS_AS(boolTrue->diff(x), SymEngineException);

    CHECK(not eq(*boolTrue, *boolFalse));
    CHECK(eq(*boolFalse, *boolean(false)));
}

TEST_CASE("Contains", "[logic]")
{
    auto x = symbol("x");
    auto y = symbol("y");
    auto int1 = interval(integer(1), integer(2), false, false);
    auto int2 = interval(integer(1), integer(2), true, true);

    auto p = contains(integer(1), int2);
    CHECK(eq(*p, *boolFalse));

    p = contains(integer(2), int2);
    CHECK(eq(*p, *boolFalse));

    p = contains(integer(1), int1);
    CHECK(eq(*p, *boolTrue));

    p = contains(integer(2), int1);
    CHECK(eq(*p, *boolTrue));

    p = contains(real_double(1.5), int1);
    CHECK(eq(*p, *boolTrue));

    p = contains(integer(3), int1);
    CHECK(eq(*p, *boolFalse));

    p = contains(x, int1);
    REQUIRE(is_a<Contains>(*p));
    CHECK(p->__str__() == "Contains(x, [1, 2])");
    CHECK(eq(*p, *p));

    vec_basic v = p->get_args();
    vec_basic u = {x, int1};
    CHECK(unified_eq(v, u));

    CHECK_THROWS_AS(p->diff(x), SymEngineException);
}

TEST_CASE("Piecewise", "[logic]")
{
    auto x = symbol("x");
    auto y = symbol("y");
    auto int1 = interval(integer(1), integer(2), true, false);
    auto int2 = interval(integer(2), integer(5), true, false);
    auto int3 = interval(integer(5), integer(10), true, false);
    auto p = piecewise({{x, contains(x, int1)},
                        {y, contains(x, int2)},
                        {add(x, y), contains(x, int3)}});

    vec_basic v = p->get_args();
    vec_basic u = {x,         contains(x, int1), y, contains(x, int2),
                   add(x, y), contains(x, int3)};
    CHECK(unified_eq(v, u));

    std::string s = "Piecewise((x, Contains(x, (1, 2])), (y, Contains(x, (2, "
                    "5])), (x + y, Contains(x, (5, 10])))";
    CHECK(s == p->__str__());

    auto q = piecewise({{one, contains(x, int1)},
                        {zero, contains(x, int2)},
                        {one, contains(x, int3)}});

    CHECK((p->diff(x))->__hash__() == q->__hash__());
    CHECK(eq(*p->diff(x), *q));
}

TEST_CASE("And, Or : Basic", "[basic]")
{
    set_boolean e;
    CHECK(eq(*logical_and(e), *boolTrue));
    CHECK(eq(*logical_or(e), *boolFalse));

    CHECK(eq(*logical_and({boolTrue}), *boolTrue));
    CHECK(eq(*logical_and({boolFalse}), *boolFalse));
    CHECK(eq(*logical_or({boolTrue}), *boolTrue));
    CHECK(eq(*logical_or({boolFalse}), *boolFalse));

    CHECK(eq(*logical_and({boolTrue, boolFalse}), *boolFalse));
    CHECK(eq(*logical_or({boolTrue, boolFalse}), *boolTrue));

    auto x = symbol("x");
    auto int1 = interval(integer(1), integer(2), false, false);
    auto int2 = interval(integer(1), integer(5), false, false);
    auto c1 = contains(x, int1);
    auto c2 = contains(x, int2);

    auto s1 = logical_and({c1, c2});
    std::string str = s1->__str__();
    CHECK(str.find("And(") == 0);
    CHECK(str.find(c1->__str__()) != std::string::npos);
    CHECK(str.find(c2->__str__()) != std::string::npos);
    auto s2 = logical_and({c2, c1});
    CHECK(s1->__hash__() == s2->__hash__());
    CHECK(eq(*s1, *s2));
    vec_basic v = s2->get_args();
    vec_basic u = {c2, c1};
    CHECK(unified_eq(v, u));

    s1 = logical_or({c1, c2});
    str = s1->__str__();
    CHECK(str.find("Or(") == 0);
    CHECK(str.find(c1->__str__()) != std::string::npos);
    CHECK(str.find(c2->__str__()) != std::string::npos);
    s2 = logical_or({c2, c1});
    CHECK(s1->__hash__() == s2->__hash__());
    CHECK(eq(*s1, *s2));
    v = s2->get_args();
    u = {c2, c1};
    CHECK(unified_eq(v, u));

    CHECK(eq(*logical_and({c1}), *c1));
    CHECK(eq(*logical_or({c1}), *c1));

    CHECK(eq(*logical_and({c1, logical_not(c1)}), *boolFalse));
    CHECK(eq(*logical_or({c1, logical_not(c1)}), *boolTrue));

    CHECK(eq(*logical_and({c1, boolTrue}), *c1));
    CHECK(eq(*logical_and({c1, boolFalse}), *boolFalse));
    CHECK(eq(*logical_or({c1, boolTrue}), *boolTrue));
    CHECK(eq(*logical_or({c1, boolFalse}), *c1));

    CHECK(eq(*logical_and({c1, c1, c2}), *logical_and({c1, c2})));
    CHECK(eq(*logical_or({c1, c1, c2}), *logical_or({c1, c2})));

    auto y = symbol("y");
    auto c3 = contains(y, int1);
    auto c4 = contains(y, int2);
    CHECK(eq(*logical_and({c1, c1, c2}), *logical_and({c1, c2})));
    CHECK(eq(*logical_and({logical_and({c1, c2}), logical_and({c3, c4})}),
             *logical_and({c1, c2, c3, c4})));
    CHECK(eq(
        *logical_and(
            {logical_and({c1, logical_and({c2, logical_and({c3, c4})})}), c2}),
        *logical_and({c1, c2, c3, c4})));
    CHECK(eq(*logical_or({c2, c1, c2}), *logical_or({c1, c2})));
    CHECK(eq(*logical_or({logical_or({c1, c2}), logical_or({c3, c4})}),
             *logical_or({c1, c2, c3, c4})));
    CHECK(eq(
        *logical_or({c1, logical_and({c2, c3, c4}), logical_and({c2, c4}),
                     logical_and({c2, c3, c4}), c1, logical_and({c2, c4})}),
        *logical_or({c1, logical_and({c2, c3, c4}), logical_and({c2, c4})})));
}

TEST_CASE("Nand : Basic", "[basic]")
{
    set_boolean e;
    CHECK(eq(*logical_nand(e), *boolFalse));

    CHECK(eq(*logical_nand({boolTrue}), *boolFalse));
    CHECK(eq(*logical_nand({boolFalse}), *boolTrue));

    auto x = symbol("x");
    auto int1 = interval(integer(1), integer(2), false, false);
    auto int2 = interval(integer(1), integer(5), false, false);
    auto c1 = contains(x, int1);
    auto c2 = contains(x, int2);
    CHECK(eq(*logical_nand({boolTrue, c1}), *logical_not(c1)));
    CHECK(eq(*logical_nand({boolFalse, c2}), *boolTrue));
}

TEST_CASE("Nor : Basic", "[basic]")
{
    CHECK(eq(*logical_nor({boolTrue}), *boolFalse));
    CHECK(eq(*logical_nor({boolFalse}), *boolTrue));

    auto x = symbol("x");
    auto int1 = interval(integer(1), integer(2), false, false);
    auto int2 = interval(integer(1), integer(5), false, false);
    auto c1 = contains(x, int1);
    auto c2 = contains(x, int2);

    CHECK(eq(*logical_nor({boolTrue, c1}), *boolFalse));
    CHECK(eq(*logical_nor({boolFalse, c1}), *logical_not(c1)));
    CHECK(eq(*logical_nor({boolTrue, boolTrue, boolTrue}), *boolFalse));
    CHECK(eq(*logical_nor({boolTrue, boolTrue, c1}), *boolFalse));
    CHECK(eq(*logical_nor({boolTrue, boolFalse, c1}), *boolFalse));
}

TEST_CASE("Not : Basic", "[basic]")
{
    auto x = symbol("x");
    auto int1 = interval(integer(1), integer(2), false, false);
    auto int2 = interval(integer(1), integer(5), false, false);
    auto c1 = contains(x, int1);
    auto c2 = contains(x, int2);

    CHECK(eq(*logical_not(boolTrue), *boolFalse));
    CHECK(eq(*logical_not(boolFalse), *boolTrue));
    CHECK(logical_not(c1)->__str__() == "Not(Contains(x, [1, 2]))");
    CHECK(eq(*logical_not(logical_and({c1, c2})),
             *logical_or({logical_not(c1), logical_not(c2)})));
    CHECK(eq(*logical_not(logical_or({c1, c2})),
             *logical_and({logical_not(c1), logical_not(c2)})));
}

TEST_CASE("Xor : Basic", "[basic]")
{
    vec_boolean e;
    CHECK(eq(*logical_xor(e), *boolFalse));
    CHECK(eq(*logical_xor({boolTrue}), *boolTrue));
    CHECK(eq(*logical_xor({boolFalse}), *boolFalse));
    CHECK(eq(*logical_xor({boolFalse, boolFalse, boolFalse, boolTrue}),
             *boolTrue));
    CHECK(eq(*logical_xor({boolTrue, boolTrue}), *boolFalse));
    CHECK(eq(*logical_xor({boolTrue, boolTrue, boolTrue}), *boolTrue));
    CHECK(eq(*logical_xor({boolFalse, boolFalse}), *boolFalse));

    auto x = symbol("x");
    auto int1 = interval(integer(1), integer(2), false, false);
    auto int2 = interval(integer(1), integer(5), false, false);
    auto c1 = contains(x, int1);
    auto c2 = contains(x, int2);

    auto p = logical_xor({c2, c1});
    vec_basic v = p->get_args();
    vec_basic u = {c2, c1};
    CHECK(unified_eq(v, u));

    auto s1 = logical_xor({c1, c2});
    auto s2 = logical_xor({c2, c1});
    CHECK(s1->__hash__() == s2->__hash__());

    auto y = symbol("y");
    auto c3 = contains(y, int1);
    auto c4 = contains(y, int2);
    CHECK(eq(*logical_xor({c1, c1, c2}), *c2));
    CHECK(eq(*logical_xor({logical_xor({c1, c2}), logical_xor({c3, c4})}),
             *logical_xor({c1, c2, c3, c4})));

    CHECK(eq(*logical_xor({boolTrue, c1, p}), *logical_not(c2)));
    CHECK(eq(*logical_xor({boolTrue, logical_not(c2), p}), *c1));
    CHECK(eq(*logical_xor({boolTrue, c1}), *logical_not(c1)));
    CHECK(eq(*logical_xor({boolTrue, c1, c1, c1}), *logical_not(c1)));
    CHECK(eq(*logical_xor({boolTrue, c1, c2}), *logical_xnor({c1, c2})));
    CHECK(eq(*logical_xor({boolTrue, c1, logical_not(c1), c2, logical_not(c2)}),
             *boolTrue));
    CHECK(eq(*logical_xor({boolTrue, c1, c1}), *boolTrue));
    CHECK(eq(*logical_xor({c1, c1}), *boolFalse));
    CHECK(eq(*logical_xor({boolFalse, c2}), *(c2)));
    CHECK(eq(*logical_xor({logical_not(c2), c2}), *boolTrue));
    CHECK(is_a<Xor>(*logical_xor({c1, c2})));
}

TEST_CASE("Xnor : Basic", "[basic]")
{
    vec_boolean e;
    CHECK(eq(*logical_xnor(e), *boolTrue));
    CHECK(eq(*logical_xnor({boolTrue}), *boolFalse));
    CHECK(eq(*logical_xnor({boolFalse}), *boolTrue));
    CHECK(eq(*logical_xnor({boolFalse, boolFalse, boolFalse, boolTrue}),
             *boolFalse));
    CHECK(eq(*logical_xnor({boolTrue, boolTrue}), *boolTrue));
    CHECK(eq(*logical_xnor({boolTrue, boolTrue, boolTrue}), *boolFalse));

    auto x = symbol("x");
    auto int1 = interval(integer(1), integer(2), false, false);
    auto int2 = interval(integer(1), integer(5), false, false);
    auto c1 = contains(x, int1);
    auto c2 = contains(x, int2);

    CHECK(eq(*logical_xnor({boolFalse, c1}), *logical_not(c1)));
    CHECK(eq(*logical_xnor({c2, c2}), *boolTrue));
    CHECK(eq(*logical_xnor({c2, c2, c1}), *logical_not(c1)));
    CHECK(eq(*logical_xnor({boolTrue, boolFalse, c2}), *(c2)));
    CHECK(is_a<Not>(*logical_xnor({c1, c2})));
}