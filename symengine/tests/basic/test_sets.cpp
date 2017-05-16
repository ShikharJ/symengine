#include "catch.hpp"

#include <symengine/sets.h>
#include <symengine/logic.h>
#include <symengine/infinity.h>
#include <symengine/symengine_exception.h>

using SymEngine::Basic;
using SymEngine::Integer;
using SymEngine::integer;
using SymEngine::Rational;
using SymEngine::one;
using SymEngine::zero;
using SymEngine::Number;
using SymEngine::RCP;
using SymEngine::Interval;
using SymEngine::interval;
using SymEngine::FiniteSet;
using SymEngine::finiteset;
using SymEngine::Set;
using SymEngine::EmptySet;
using SymEngine::emptyset;
using SymEngine::UniversalSet;
using SymEngine::universalset;
using SymEngine::Union;
using SymEngine::set_union;
using SymEngine::rcp_dynamic_cast;
using SymEngine::Complex;
using SymEngine::symbol;
using SymEngine::is_a;
using SymEngine::symbol;
using SymEngine::boolean;
using SymEngine::Inf;
using SymEngine::NegInf;
using SymEngine::NotImplementedError;
using SymEngine::SymEngineException;
using SymEngine::boolTrue;
using SymEngine::boolFalse;
using SymEngine::Contains;
using SymEngine::make_rcp;
using SymEngine::set_set;

TEST_CASE("Interval : Basic", "[basic]")
{
    RCP<const Set> r1, r2, r3, r4;
    RCP<const Number> i2 = integer(2);
    RCP<const Number> i20 = integer(20);
    RCP<const Number> im5 = integer(-5);
    RCP<const Number> rat1 = Rational::from_two_ints(*integer(5), *integer(6));
    RCP<const Number> rat2
        = Rational::from_two_ints(*integer(500), *integer(6));

    r1 = interval(zero, i20); // [0, 20]
    r2 = interval(im5, i2);   // [-5, 2]

    REQUIRE(is_a<Interval>(*r1));
    REQUIRE(not is_a<EmptySet>(*r1));
    REQUIRE(not is_a<UniversalSet>(*r1));

    r3 = r1->set_intersection(r2); // [0, 2]
    CHECK(eq(*r1->contains(one), *boolTrue));
    r4 = interval(zero, i2); // [0, 2]
    CHECK(eq(*r3, *r4));
    r3 = interval(im5, i2, true, true); // (-5, 2)
    CHECK(eq(*r3->contains(i2), *boolFalse));
    CHECK(eq(*r3->contains(im5), *boolFalse));
    CHECK(eq(*r3->contains(rat2), *boolFalse));
    CHECK(eq(*r3->contains(integer(-7)), *boolFalse));
    r4 = r3->set_intersection(r2);
    CHECK(eq(*r3, *r4));
    r3 = r1->set_union(r2); // [-5, 20]
    CHECK(eq(*r3, *set_union({r1, r2})));
    r4 = interval(im5, i20);
    CHECK(eq(*r3, *r4));
    r3 = r2->set_union(r1); // [-5, 20]
    CHECK(eq(*r3, *set_union({r1, r2})));
    CHECK(eq(*r3, *r4));
    r3 = interval(integer(21), integer(22));
    r4 = r1->set_intersection(r3);
    CHECK(eq(*r4, *emptyset()));
    r3 = interval(im5, i2, false, false); // [-5, 2]
    r4 = interval(integer(3), i20, false, false);
    CHECK(r3->compare(*r4) == -1);

    CHECK(eq(*r3->set_union(r4), *set_union({r3, r4})));
    CHECK(eq(*r4->set_union(r3), *set_union({r3, r4})));

    r3 = interval(zero, i2, true, true); // (0, 2)
    CHECK(eq(*r3->contains(sqrt(i2)), *make_rcp<Contains>(sqrt(i2), r3)));

    r3 = interval(im5, i2, false, false); // [-5, 2]
    CHECK(r3->is_subset(r2));
    CHECK(not r3->is_subset(emptyset()));
    CHECK(not r3->is_proper_subset(emptyset()));
    CHECK(not r3->is_proper_subset(r2));
    CHECK(not r3->is_proper_superset(r2));
    r3 = interval(im5, i20);
    r4 = interval(zero, i2);
    CHECK(r3->is_superset(r4));
    CHECK(r3->is_proper_superset(r4));

    r1 = interval(rat1, rat2); // [5/6, 500/6]
    r2 = interval(im5, i2);    // [-5, 2]
    r3 = r1->set_intersection(r2);
    r4 = interval(rat1, i2);
    CHECK(eq(*r3, *r4));
    r3 = r2->set_intersection(r1);
    CHECK(eq(*r3, *r4));
    CHECK(eq(*emptyset(), *r1->set_intersection(emptyset())));
    CHECK(eq(*r1, *r1->set_union(emptyset())));
    CHECK(eq(*r1, *set_union({r1, emptyset()})));

    CHECK(r4->__str__() == "[5/6, 2]");
    CHECK(r4->compare(*r3) == 0);
    r4 = interval(rat1, i2, true, true);
    CHECK(r4->__str__() == "(5/6, 2)");

    r1 = interval(one, zero);
    CHECK(eq(*r1, *emptyset()));
    r1 = interval(one, one, true, true);
    CHECK(eq(*r1, *emptyset()));

    r1 = interval(zero, one);
    RCP<const Interval> r5 = rcp_dynamic_cast<const Interval>(r1);

    r2 = interval(zero, one, false, false);
    CHECK(eq(*r5->close(), *r1));
    r2 = interval(zero, one, true, false);
    CHECK(eq(*r5->Lopen(), *r2));
    r2 = interval(zero, one, false, true);
    CHECK(eq(*r5->Ropen(), *r2));
    r2 = interval(zero, one, true, true);
    CHECK(eq(*r5->open(), *r2));

    r1 = interval(zero, Inf, false, true);
    r2 = interval(NegInf, one, true, true);
    r3 = interval(zero, one, false, true);
    CHECK(eq(*r3, *r1->set_intersection(r2)));

    CHECK(not r5->__eq__(*r2));
    CHECK(r5->__hash__() != emptyset()->__hash__());
    CHECK(not r5->__eq__(*emptyset()));

    CHECK(r5->Lopen()->compare(*r5) == -1);
    CHECK(r5->compare(*r5->Lopen()) == 1);
    CHECK(r5->Ropen()->compare(*r5) == 1);
    CHECK(r5->compare(*r5->Ropen()) == -1);

    CHECK(eq(*r5->get_args()[0], *r5->get_start()));
    CHECK(eq(*r5->get_args()[1], *r5->get_end()));
    CHECK(eq(*r5->get_args()[2], *boolean(r5->get_left_open())));
    CHECK(eq(*r5->get_args()[3], *boolean(r5->get_right_open())));
    RCP<const Number> c1 = Complex::from_two_nums(*i2, *i20);
    CHECK_THROWS_AS(interval(c1, one), NotImplementedError);
    CHECK_THROWS_AS(r5->diff(symbol("x")), SymEngineException);
}

TEST_CASE("EmptySet : Basic", "[basic]")
{
    RCP<const Set> r1 = emptyset();
    RCP<const Set> r2 = interval(zero, one);

    REQUIRE(not is_a<Interval>(*r1));
    REQUIRE(is_a<EmptySet>(*r1));
    REQUIRE(not is_a<UniversalSet>(*r1));
    CHECK(r1->is_subset(r2));
    CHECK(r1->is_proper_subset(r2));
    CHECK(not r1->is_proper_superset(r2));
    CHECK(r1->is_superset(r1));
    CHECK(not r1->is_superset(r2));
    CHECK(eq(*r1, *r1->set_intersection(r2)));
    CHECK(eq(*r2, *r1->set_union(r2)));
    CHECK(eq(*r2, *set_union({r1, r2})));
    CHECK(r1->__str__() == "EmptySet");
    CHECK(r1->__hash__() == emptyset()->__hash__());
    CHECK(not r1->is_proper_subset(r1));
    CHECK(not r1->__eq__(*r2));
    CHECK(r1->compare(*emptyset()) == 0);
    CHECK(eq(*r1->contains(zero), *boolFalse));
    CHECK(r1->get_args().empty());
    CHECK_THROWS_AS(r1->diff(symbol("x")), SymEngineException);
}

TEST_CASE("UniversalSet : Basic", "[basic]")
{
    RCP<const Set> r1 = universalset();
    RCP<const Set> r2 = interval(zero, one);
    RCP<const Set> e = emptyset();

    REQUIRE(not is_a<Interval>(*r1));
    REQUIRE(not is_a<EmptySet>(*r1));
    REQUIRE(is_a<UniversalSet>(*r1));
    CHECK(not r1->is_subset(r2));
    CHECK(not r1->is_subset(e));
    CHECK(not r1->is_proper_subset(r2));
    CHECK(not r1->is_proper_subset(e));
    CHECK(r1->is_proper_superset(r2));
    CHECK(r1->is_proper_superset(e));
    CHECK(r1->is_superset(r2));
    CHECK(r1->is_superset(e));
    CHECK(r1->is_subset(r1));
    CHECK(not r1->is_proper_subset(r1));
    CHECK(r1->is_superset(r1));
    CHECK(not r1->is_proper_superset(r1));
    CHECK(eq(*r1, *r1->set_union(r2)));
    CHECK(eq(*r1, *r1->set_union(e)));
    CHECK(eq(*r1, *set_union({r1, r2})));
    CHECK(eq(*r1, *set_union({r1, e})));
    CHECK(eq(*r2, *r1->set_intersection(r2)));
    CHECK(eq(*e, *r1->set_intersection(e)));
    CHECK(eq(*r1->contains(zero), *boolTrue));
    CHECK(r1->__str__() == "UniversalSet");
    CHECK(r1->__hash__() == universalset()->__hash__());
    CHECK(not r1->__eq__(*r2));
    CHECK(r1->compare(*universalset()) == 0);
    CHECK(r1->get_args().empty());
    CHECK_THROWS_AS(r1->diff(symbol("x")), SymEngineException);
}

TEST_CASE("FiniteSet : Basic", "[basic]")
{
    RCP<const Set> r1 = finiteset({zero, one, symbol("x")});
    RCP<const Set> r2 = finiteset({zero, one, integer(2)});
    RCP<const Set> r3 = r1->set_union(r2); // {0, 1, 2, x}
    CHECK(eq(*r3, *set_union({r1, r2})));
    r3 = r1->set_intersection(r2); // {0, 1}
    CHECK(eq(*r3, *finiteset({zero, one})));
    CHECK(r3->__hash__() == finiteset({zero, one})->__hash__());
    CHECK(r3->compare(*r2) == -1);
    CHECK(eq(*r3->contains(one), *boolTrue));
    CHECK(eq(*r3->contains(zero), *boolTrue));
    CHECK(eq(*r3->contains(integer(3)), *boolFalse));
    CHECK(r3->is_subset(r2));
    CHECK(r3->is_proper_subset(r2));
    CHECK(r1->get_args().empty());

    r1 = finiteset({zero, one});
    CHECK(r1->__str__() == "{0, 1}");
    RCP<const Set> r4 = interval(zero, one);
    r3 = r2->set_intersection(r4);
    CHECK(eq(*r3->contains(one), *boolTrue));
    CHECK(eq(*r3->contains(zero), *boolTrue));
    r2 = finiteset({zero, one});
    r3 = r2->set_union(r4);
    CHECK(eq(*r3, *set_union({r2, r4})));
    CHECK(r3->__str__() == "[0, 1]");
    CHECK(r1->is_subset(r4));
    CHECK(r1->is_proper_subset(r4));
    r4 = interval(zero, zero);
    r1 = finiteset({zero});
    CHECK(r1->is_subset(r4));
    CHECK(not r1->is_proper_subset(r4));
    CHECK(r1->__eq__(*r4));
    CHECK(r4->__eq__(*r1));
    r1 = finiteset({zero, one});
    r4 = interval(zero, one, true, true); // (0, 1)
    r3 = r1->set_union(r4);
    r2 = interval(zero, one); // [0, 1]
    CHECK(eq(*r2, *r3));
    CHECK(eq(*r2, *set_union({r1, r4})));
    r1 = finiteset({zero, one, integer(2)});
    r3 = r1->set_union(r4);
    CHECK(eq(*r3, *set_union({r1, r4})));
    r4 = interval(zero, one, false, true); // [0, 1)
    r3 = r1->set_union(r4);
    CHECK(eq(*r3, *set_union({r1, r4})));

    r4 = emptyset();
    r3 = r2->set_intersection(r4);
    CHECK(eq(*r3, *emptyset()));
    r3 = r2->set_union(r4);
    CHECK(eq(*r3, *r2));
    CHECK(eq(*r3, *set_union({r2, r4})));
    CHECK(r1->is_superset(r4));
    CHECK(not r1->is_proper_subset(r4));

    r4 = universalset();
    r3 = r2->set_intersection(r4);
    CHECK(eq(*r3, *r2));
    r3 = r2->set_union(r4);
    CHECK(eq(*r3, *set_union({r2, r4})));
    CHECK(eq(*r3, *universalset()));
    CHECK(not r1->is_superset(r4));
    CHECK(r1->is_proper_subset(r4));
}

TEST_CASE("Union : Basic", "[basic]")
{
    auto check_union_str = [](std::string to_chk, set_set sets) {
        if ((size_t)std::count(to_chk.begin(), to_chk.end(), 'U')
            != sets.size() - 1)
            return false;
        for (auto &a : sets) {
            if (to_chk.find(a->__str__()) == std::string::npos)
                return false;
        }
        return true;
    };
    RCP<const Set> f1 = finiteset({zero, one, symbol("x")});
    RCP<const Set> r1 = set_union({f1, emptyset()});
    CHECK(r1->get_args().empty());
    CHECK(eq(*r1, *f1));
    r1 = set_union({emptyset()});
    CHECK(eq(*r1, *emptyset()));
    r1 = set_union({universalset()});
    CHECK(eq(*r1, *universalset()));
    r1 = set_union({f1});
    CHECK(eq(*r1, *f1));
    r1 = set_union({f1, emptyset(), universalset()});
    CHECK(eq(*r1, *universalset()));
    RCP<const Set> i1 = interval(zero, integer(3));
    RCP<const Set> i2 = interval(integer(4), integer(5));
    RCP<const Set> i3 = interval(integer(3), integer(4));
    r1 = set_union({i1, i2, i3});
    CHECK(eq(*r1, *interval(integer(0), integer(5))));

    i1 = interval(zero, one);
    i2 = interval(integer(3), integer(4));
    i3 = interval(integer(2), integer(3));
    RCP<const Set> r2 = set_union({i1, i2, i3});
    RCP<const Union> u = rcp_dynamic_cast<const Union>(r2);
    CHECK(u->get_container().size() == 2);
    CHECK(u->get_container().find(interval(zero, one))
          != u->get_container().end());
    CHECK(u->get_container().find(interval(integer(2), integer(4)))
          != u->get_container().end());
    CHECK(
        check_union_str(u->__str__(), {i1, interval(integer(2), integer(4))}));

    r2 = set_union({r1, r2});
    CHECK(eq(*r1, *r2));

    r2 = set_union({i1, i2, i3});
    r1 = set_union({finiteset({zero}), i2});
    u = rcp_dynamic_cast<const Union>(set_union({r1, r2}));
    CHECK(u->get_container().find(interval(integer(2), integer(4)))
          != u->get_container().end());
    CHECK(u->get_container().find(interval(zero, one))
          != u->get_container().end());
    CHECK(
        check_union_str(u->__str__(), {i1, interval(integer(2), integer(4))}));
    CHECK(eq(*u->contains(one), *boolTrue));
    CHECK(eq(*u->contains(integer(2)), *boolTrue));
    CHECK(eq(*u->contains(integer(7)), *boolFalse));
    CHECK(u->is_superset(r1));
    CHECK(u->is_superset(r2));
    CHECK(u->is_superset(u));
    CHECK(r2->is_subset(u));
    CHECK(r1->is_subset(u));
    CHECK(u->is_subset(u));
    CHECK(u->is_proper_superset(r1));
    CHECK(not u->is_proper_superset(r2));
    CHECK(not u->is_proper_superset(u));
    CHECK(not r2->is_proper_subset(u));
    CHECK(r1->is_proper_subset(u));
    CHECK(not u->is_proper_subset(u));
}
