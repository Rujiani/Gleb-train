#include "train.hpp"
#include <algorithm>

namespace mgt {

Train& Train::operator=(const Train& other) {
    if (this != &other) {
        if (other.capacity_ != capacity_) {
            Van* temp = new Van[other.capacity_];
            delete[] vans_;
            vans_ = temp;
            capacity_ = other.capacity_;
        }
        size_ = other.size_;
        std::copy_n(other.vans_, size_, vans_);
    }
    return *this;
}

Train& Train::operator=(Train&& other) noexcept {
    if (this != &other) {
        delete[] vans_;
        vans_ = other.vans_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        other.vans_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }
    return *this;
}

void Train::SitInMin(size_t numOfPassengers) {
    Van* minOccupiedVan = nullptr;
    size_t minOccupiedSeats = 0;
    
    for (size_t i = 0; i < size_; ++i) {
        size_t availableSeats = vans_[i].GetCapacity() - vans_[i].GetOccupiedSeats();
        if (numOfPassengers <= availableSeats && (!minOccupiedVan || vans_[i].GetOccupiedSeats() < minOccupiedSeats)) {
            minOccupiedSeats = vans_[i].GetOccupiedSeats();
            minOccupiedVan = &vans_[i];
        }
    }
    
    if (minOccupiedVan) {
        *minOccupiedVan += numOfPassengers;
    }
}

void Train::StaffingPercentage() noexcept {
    struct VanStats {
        size_t totalCapacity = 0;
        size_t totalOccupied = 0;
    };
    
    std::map<VanType, VanStats> stats = {
        {VanType::Restaurant, {}},
        {VanType::Seated, {}},
        {VanType::Economy, {}},
        {VanType::Luxury, {}}
    };
    
    for (size_t i = 0; i < size_; ++i) {
        VanStats& stat = stats[vans_[i].GetType()];
        stat.totalCapacity += vans_[i].GetCapacity();
        stat.totalOccupied += vans_[i].GetOccupiedSeats();
    }
}

} // namespace mgt
