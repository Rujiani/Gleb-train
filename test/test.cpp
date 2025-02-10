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

TEST_CASE("Balanced occupancy remains unchanged", "[BalanceOccupancy]") {
    Train train;
    Van van1(100, 50, VanType::Economy);
    Van van2(100, 50, VanType::Economy);
    train += van1;
    train += van2;
    size_t total = train[0].GetOccupiedSeats() + train[1].GetOccupiedSeats();
    train.BalanceOccupancy();
    REQUIRE(train[0].GetOccupiedSeats() == 50);
    REQUIRE(train[1].GetOccupiedSeats() == 50);
    REQUIRE((train[0].GetOccupiedSeats() + train[1].GetOccupiedSeats()) == total);
}

TEST_CASE("Unbalanced occupancy becomes balanced", "[BalanceOccupancy]") {
    Train train;
    Van van1(100, 90, VanType::Economy);
    Van van2(100, 10, VanType::Economy);
    train += van1;
    train += van2;
    train.BalanceOccupancy();
    REQUIRE(train[0].GetOccupiedSeats() == 50);
    REQUIRE(train[1].GetOccupiedSeats() == 50);
}

TEST_CASE("Three vans custom distribution", "[BalanceOccupancy]") {
    Train train;
    Van van1(100, 80, VanType::Economy);
    Van van2(50, 20, VanType::Economy);
    Van van3(150, 90, VanType::Economy);
    train += van1;
    train += van2;
    train += van3;
    train.BalanceOccupancy();
    REQUIRE(train[0].GetOccupiedSeats() == 63);
    REQUIRE(train[1].GetOccupiedSeats() == 32);
    REQUIRE(train[2].GetOccupiedSeats() == 95);
}

TEST_CASE("Restaurant van is ignored", "[BalanceOccupancy]") {
    Train train;
    Van restaurant(0, 0, VanType::Restaurant);
    Van economy(100, 60, VanType::Economy);
    train += restaurant;
    train += economy;
    train.BalanceOccupancy();
    REQUIRE(train[0].GetCapacity() == 0);
    REQUIRE(train[0].GetOccupiedSeats() == 0);
    REQUIRE(train[1].GetOccupiedSeats() == 60);
}

TEST_CASE("Full vans remain full", "[BalanceOccupancy]") {
    Train train;
    Van van1(100, 100, VanType::Economy);
    Van van2(50, 50, VanType::Economy);
    train += van1;
    train += van2;
    train.BalanceOccupancy();
    REQUIRE(train[0].GetOccupiedSeats() == 100);
    REQUIRE(train[1].GetOccupiedSeats() == 50);
}

TEST_CASE("Occupancy percentages are nearly equal", "[BalanceOccupancy]") {
    Train train;
    Van van1(100, 80, VanType::Economy);
    Van van2(150, 60, VanType::Economy);
    Van van3(200, 150, VanType::Economy);
    train += van1;
    train += van2;
    train += van3;
    size_t totalBefore = train[0].GetOccupiedSeats() + train[1].GetOccupiedSeats() + train[2].GetOccupiedSeats();
    train.BalanceOccupancy();
    size_t totalAfter = train[0].GetOccupiedSeats() + train[1].GetOccupiedSeats() + train[2].GetOccupiedSeats();
    REQUIRE(totalBefore == totalAfter);
    double ratio0 = static_cast<double>(train[0].GetOccupiedSeats()) / train[0].GetCapacity();
    double ratio1 = static_cast<double>(train[1].GetOccupiedSeats()) / train[1].GetCapacity();
    double ratio2 = static_cast<double>(train[2].GetOccupiedSeats()) / train[2].GetCapacity();
    double maxRatio = std::max({ratio0, ratio1, ratio2});
    double minRatio = std::min({ratio0, ratio1, ratio2});
    REQUIRE((maxRatio - minRatio) < 0.1);
}

TEST_CASE("MinimizeVans: No merging if all vans are full", "[MinimizeVans]") {
    Train train;
    Van van1(100, 100, VanType::Economy);
    Van van2(100, 100, VanType::Economy);
    train += van1;
    train += van2;
    size_t totalBefore = train[0].GetOccupiedSeats() + train[1].GetOccupiedSeats();
    train.MinimizeVans();
    REQUIRE(train.GetSize() == 2);
    size_t totalAfter = 0;
    for (size_t i = 0; i < train.GetSize(); ++i)
        totalAfter += train[i].GetOccupiedSeats();
    REQUIRE(totalAfter == totalBefore);
}

TEST_CASE("MinimizeVans: Merging economy vans", "[MinimizeVans]") {
    Train train;
    Van van1(100, 60, VanType::Economy);
    Van van2(100, 30, VanType::Economy);
    train += van1;
    train += van2;
    train.MinimizeVans();
    REQUIRE(train.GetSize() == 1);
    REQUIRE(train[0].GetType() == VanType::Economy);
    REQUIRE(train[0].GetCapacity() == 100);
    REQUIRE(train[0].GetOccupiedSeats() == 90);
}

TEST_CASE("MinimizeVans: Merging luxury vans requiring two vans", "[MinimizeVans]") {
    Train train;
    Van van1(14, 5, VanType::Luxury);
    Van van2(14, 10, VanType::Luxury);
    train += van1;
    train += van2;
    train.MinimizeVans();
    REQUIRE(train.GetSize() == 2);
    REQUIRE(train[0].GetType() == VanType::Luxury);
    REQUIRE(train[1].GetType() == VanType::Luxury);
    size_t totalOccupancy = train[0].GetOccupiedSeats() + train[1].GetOccupiedSeats();
    REQUIRE(totalOccupancy == 15);
    if (train[0].GetOccupiedSeats() == 14) {
        REQUIRE(train[1].GetOccupiedSeats() == 1);
    } else {
        REQUIRE(train[0].GetOccupiedSeats() == 1);
        REQUIRE(train[1].GetOccupiedSeats() == 14);
    }
}

TEST_CASE("MinimizeVans: Mixed types", "[MinimizeVans]") {
    Train train;
    Van restaurant(0, 0, VanType::Restaurant);
    Van econ1(100, 60, VanType::Economy);
    Van econ2(100, 30, VanType::Economy);
    train += restaurant;
    train += econ1;
    train += econ2;
    train.MinimizeVans();
    REQUIRE(train.GetSize() == 2);
    REQUIRE(train[0].GetType() == VanType::Restaurant);
    REQUIRE(train[1].GetType() == VanType::Economy);
    REQUIRE(train[1].GetOccupiedSeats() == 90);
}

TEST_CASE("MinimizeVans: Empty train", "[MinimizeVans]") {
    Train train;
    train.MinimizeVans();
    REQUIRE(train.GetSize() == 0);
}

TEST_CASE("Restaurant van repositioned to optimal middle", "[PlaceRestaurantVanOptimally]") {
    Train train;
    Van van1(100, 50, VanType::Economy);
    Van van2(100, 70, VanType::Economy);
    Van van3(100, 30, VanType::Economy);
    train += van1;
    train += van2;
    train += van3;
    Van restaurant(0, 0, VanType::Restaurant);
    train += restaurant;
    // Current order: [economy(50), economy(70), economy(30), restaurant]
    // Counted passengers: 50, 70, 30 => cumulative sums: 0, 50, 120, 150.
    // Insertion indices:
    // 0: diff = |0 - 150| = 150
    // 1: diff = |50 - 100| = 50
    // 2: diff = |120 - 30| = 90
    // 3: diff = |150 - 0| = 150
    // Optimal index is 1.
    train.PlaceRestaurantVanOptimally();
    REQUIRE(train.GetSize() == 4);
    REQUIRE(train[1].GetType() == VanType::Restaurant);
    size_t leftSum = 0;
    for (size_t i = 0; i < 1; ++i)
        leftSum += (train[i].GetType() == VanType::Luxury ? 0 : train[i].GetOccupiedSeats());
    size_t rightSum = 0;
    for (size_t i = 2; i < train.GetSize(); ++i)
        rightSum += (train[i].GetType() == VanType::Luxury ? 0 : train[i].GetOccupiedSeats());
    size_t diff = (leftSum > rightSum) ? leftSum - rightSum : rightSum - leftSum;
    REQUIRE(diff == 50);
}

TEST_CASE("Luxury vans are ignored in optimal placement", "[PlaceRestaurantVanOptimally]") {
    Train train;
    Van van1(100, 60, VanType::Economy);
    Van van2(100, 80, VanType::Luxury);
    Van van3(100, 40, VanType::Economy);
    train += van1;
    train += van2;
    train += van3;
    Van restaurant(0, 0, VanType::Restaurant);
    train += restaurant;
    // Counted: van1=60, van2 ignored, van3=40 => total=100, cumulative: 0,60,60,100.
    // Insertion indices:
    // 0: diff = |0 - 100| = 100
    // 1: diff = |60 - 40| = 20
    // 2: diff = |60 - 40| = 20
    // 3: diff = |100 - 0| = 100
    // Optimal index expected is 1 (first occurrence).
    train.PlaceRestaurantVanOptimally();
    REQUIRE(train.GetSize() == 4);
    REQUIRE(train[1].GetType() == VanType::Restaurant);
}

TEST_CASE("No restaurant van present", "[PlaceRestaurantVanOptimally]") {
    Train train;
    Van van1(100, 50, VanType::Economy);
    Van van2(100, 60, VanType::Economy);
    train += van1;
    train += van2;
    train.PlaceRestaurantVanOptimally();
    REQUIRE(train.GetSize() == 2);
    REQUIRE(train[0].GetType() != VanType::Restaurant);
    REQUIRE(train[1].GetType() != VanType::Restaurant);
}

TEST_CASE("Only restaurant van in train", "[PlaceRestaurantVanOptimally]") {
    Train train;
    Van restaurant(0, 0, VanType::Restaurant);
    train += restaurant;
    train.PlaceRestaurantVanOptimally();
    REQUIRE(train.GetSize() == 1);
    REQUIRE(train[0].GetType() == VanType::Restaurant);
}

TEST_CASE("Optimal placement at beginning when left sum is minimal", "[PlaceRestaurantVanOptimally]") {
    Train train;
    Van van1(100, 10, VanType::Economy);
    Van van2(100, 90, VanType::Economy);
    train += van1;
    train += van2;
    Van restaurant(0, 0, VanType::Restaurant);
    train += restaurant;
    // Counted: 10 and 90 => cumulative: 0,10,100.
    // Indices:
    // 0: diff = |0-100| = 100
    // 1: diff = |10-90| = 80
    // 2: diff = |100-0| = 100
    // Best is index 1.
    train.PlaceRestaurantVanOptimally();
    REQUIRE(train.GetSize() == 3);
    REQUIRE(train[1].GetType() == VanType::Restaurant);
}
