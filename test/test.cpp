#include "../van/van.hpp"
#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include <sstream>
#include <stdexcept>

using namespace mgt;
using std::invalid_argument;

TEST_CASE("Van"){
    SECTION("Default constructor"){
        Van temp;
        REQUIRE(!temp.GetCapacity());
        REQUIRE(!temp.GetOccupiedSeats());
        REQUIRE(temp.GetType() == mgt::VanType::Restaurant);
    }

    SECTION("Constructor"){
        VanType a = mgt::VanType::Seated;
        size_t cp = 12, os2 = 5;
        Van Van1(cp, os2, a);
    }

    SECTION("Setters"){
        Van Van;
        REQUIRE_THROWS_AS(Van.SetOccupiedSeats(12), invalid_argument);
        REQUIRE_THROWS_AS(Van.SetCapacity(12), invalid_argument);
        REQUIRE_NOTHROW(Van.SetType(VanType::Seated));
        REQUIRE_NOTHROW(Van.SetCapacity(50));
        REQUIRE_NOTHROW(Van.SetOccupiedSeats(45));
        REQUIRE_THROWS_AS(Van.SetType(VanType::Restaurant), invalid_argument);
        REQUIRE(Van.OccupancyRate() == 90);
    }

    SECTION("move operator >>"){
        Van a(40, 25, mgt::VanType::Seated), b(10, 0, mgt::VanType::Luxury);
        REQUIRE_THROWS_AS(a >> b, invalid_argument);
        b.SetType(VanType::Seated);
        REQUIRE_NOTHROW(a >> b);
        REQUIRE(a.OccupancyRate() == b.OccupancyRate());
        REQUIRE(a.OccupancyRate() == 50);
        REQUIRE(a.GetOccupiedSeats() == 20);
        REQUIRE(b.GetOccupiedSeats() == 5);
    }

    SECTION("Output"){
        std::ostringstream ostr;
        Van a(12, 2, mgt::VanType::Seated);
        ostr << a;
        REQUIRE(ostr.str() == "2/12 seated");
    }

    SECTION("Input"){
        Van a;
        std::istringstream istr("13/14 seated");
        istr >> a;
        REQUIRE(a.GetCapacity() == 14);
        REQUIRE(a.GetOccupiedSeats() == 13);
        REQUIRE(a.GetType() == mgt::VanType::Seated);
        istr.str("15/1 restaurant");
        istr >> a;
        REQUIRE(!istr); 
    }

    SECTION("Add and remove pass"){
        Van a(15, 2, mgt::VanType::Seated);
        REQUIRE_NOTHROW(a.AddPassengers(12));
        REQUIRE(a.GetOccupiedSeats() == 14);
        REQUIRE_NOTHROW(a.RemovePassengers(2));
        REQUIRE(a.GetOccupiedSeats() == 12);
        REQUIRE_NOTHROW(a.RemovePassengers(40));
        REQUIRE(!a.GetOccupiedSeats());
    }
}

#include "../train/train.hpp"

TEST_CASE("Train"){
    SECTION("operator == "){
        Van *a = new Van[10], *b = new Van[2];
        Train tr1(a, 10), tr2(a, 10), tr3(b, 2);
        REQUIRE(tr1 == tr2);
        REQUIRE(tr1 != tr3);
        delete [] a;
        delete [] b;
    }

    SECTION("Move and copy construct"){
        Van *a = new Van[10];
        Train tr1(a, 10), tr2(tr1);
        REQUIRE(tr1 == tr2);
        Train tr3(std::move(tr1));
        REQUIRE(tr1 != tr3);
        delete[]a;
    }

    SECTION("Move and copy ="){
        Van *a = new Van[10];
        Train tr1(a, 10), tr2;
        tr2 = tr1;
        REQUIRE(tr1 == tr2);
        Train tr3;
        tr3 = (std::move(tr1));
        REQUIRE(tr1 != tr3);
        delete[]a;
    }

    SECTION("input output"){
        std::istringstream istr("12/13, seated");
        Train a;
        istr >> a;
        REQUIRE(istr);
        std::ostringstream ostr;
        ostr << a;
        REQUIRE(ostr.str() == "{12/13 seated}");
    }

    SECTION("Operator []"){
        Van *a = new Van[12];
        Train tr(a, 12);
        REQUIRE(tr[0] == tr[1]);
        REQUIRE_THROWS(tr[43]);
        delete[] a;
    }

    SECTION("+="){
        Van temp = Van{23, 12, mgt::VanType::Seated};
        Train a(Van{23, 12, mgt::VanType::Seated});
        REQUIRE_NOTHROW(a += temp);
        REQUIRE(a[0] == temp);
        REQUIRE(a[1] == temp);
        REQUIRE(a.GetSize() == 2);
    }

    SECTION("Deleting Van"){
        Van temp = Van{23, 12, mgt::VanType::Seated};
        Train a(Van{23, 12, mgt::VanType::Seated});
        a += temp;
        a.RemoveVan(1);
        REQUIRE(a.GetSize() == 1);
        REQUIRE_THROWS(a[1]);        
    }

    SECTION("Add to min"){
        Van *a = new Van[3];
        a[0] = Van(12, 5, mgt::VanType::Seated);
        a[1] = Van(12, 5, mgt::VanType::Seated);
        a[2] = Van(12, 3, mgt::VanType::Seated);

        Train tr(a, 3);
        delete []a;

        tr.SitInMin(4);
        REQUIRE(tr[2].GetOccupiedSeats() == 7);
    }
}