#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-DEL");
    BOOST_CHECK(GetBoolArg("-DEL"));
    BOOST_CHECK(GetBoolArg("-DEL", false));
    BOOST_CHECK(GetBoolArg("-DEL", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-DELo"));
    BOOST_CHECK(!GetBoolArg("-DELo", false));
    BOOST_CHECK(GetBoolArg("-DELo", true));

    ResetArgs("-DEL=0");
    BOOST_CHECK(!GetBoolArg("-DEL"));
    BOOST_CHECK(!GetBoolArg("-DEL", false));
    BOOST_CHECK(!GetBoolArg("-DEL", true));

    ResetArgs("-DEL=1");
    BOOST_CHECK(GetBoolArg("-DEL"));
    BOOST_CHECK(GetBoolArg("-DEL", false));
    BOOST_CHECK(GetBoolArg("-DEL", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noDEL");
    BOOST_CHECK(!GetBoolArg("-DEL"));
    BOOST_CHECK(!GetBoolArg("-DEL", false));
    BOOST_CHECK(!GetBoolArg("-DEL", true));

    ResetArgs("-noDEL=1");
    BOOST_CHECK(!GetBoolArg("-DEL"));
    BOOST_CHECK(!GetBoolArg("-DEL", false));
    BOOST_CHECK(!GetBoolArg("-DEL", true));

    ResetArgs("-DEL -noDEL");  // -DEL should win
    BOOST_CHECK(GetBoolArg("-DEL"));
    BOOST_CHECK(GetBoolArg("-DEL", false));
    BOOST_CHECK(GetBoolArg("-DEL", true));

    ResetArgs("-DEL=1 -noDEL=1");  // -DEL should win
    BOOST_CHECK(GetBoolArg("-DEL"));
    BOOST_CHECK(GetBoolArg("-DEL", false));
    BOOST_CHECK(GetBoolArg("-DEL", true));

    ResetArgs("-DEL=0 -noDEL=0");  // -DEL should win
    BOOST_CHECK(!GetBoolArg("-DEL"));
    BOOST_CHECK(!GetBoolArg("-DEL", false));
    BOOST_CHECK(!GetBoolArg("-DEL", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--DEL=1");
    BOOST_CHECK(GetBoolArg("-DEL"));
    BOOST_CHECK(GetBoolArg("-DEL", false));
    BOOST_CHECK(GetBoolArg("-DEL", true));

    ResetArgs("--noDEL=1");
    BOOST_CHECK(!GetBoolArg("-DEL"));
    BOOST_CHECK(!GetBoolArg("-DEL", false));
    BOOST_CHECK(!GetBoolArg("-DEL", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-DEL", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-DEL", "eleven"), "eleven");

    ResetArgs("-DEL -bar");
    BOOST_CHECK_EQUAL(GetArg("-DEL", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-DEL", "eleven"), "");

    ResetArgs("-DEL=");
    BOOST_CHECK_EQUAL(GetArg("-DEL", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-DEL", "eleven"), "");

    ResetArgs("-DEL=11");
    BOOST_CHECK_EQUAL(GetArg("-DEL", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-DEL", "eleven"), "11");

    ResetArgs("-DEL=eleven");
    BOOST_CHECK_EQUAL(GetArg("-DEL", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-DEL", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-DEL", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-DEL", 0), 0);

    ResetArgs("-DEL -bar");
    BOOST_CHECK_EQUAL(GetArg("-DEL", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-DEL=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-DEL", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-DEL=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-DEL", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--DEL");
    BOOST_CHECK_EQUAL(GetBoolArg("-DEL"), true);

    ResetArgs("--DEL=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-DEL", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noDEL");
    BOOST_CHECK(!GetBoolArg("-DEL"));
    BOOST_CHECK(!GetBoolArg("-DEL", true));
    BOOST_CHECK(!GetBoolArg("-DEL", false));

    ResetArgs("-noDEL=1");
    BOOST_CHECK(!GetBoolArg("-DEL"));
    BOOST_CHECK(!GetBoolArg("-DEL", true));
    BOOST_CHECK(!GetBoolArg("-DEL", false));

    ResetArgs("-noDEL=0");
    BOOST_CHECK(GetBoolArg("-DEL"));
    BOOST_CHECK(GetBoolArg("-DEL", true));
    BOOST_CHECK(GetBoolArg("-DEL", false));

    ResetArgs("-DEL --noDEL");
    BOOST_CHECK(GetBoolArg("-DEL"));

    ResetArgs("-noDEL -DEL"); // DEL always wins:
    BOOST_CHECK(GetBoolArg("-DEL"));
}

BOOST_AUTO_TEST_SUITE_END()
