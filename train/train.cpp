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

void Train::BalanceOccupancy() {
    size_t count = 0, totalOccupancy = 0, totalCapacity = 0;
    for (size_t i = 0; i < size_; ++i) {
        if (vans_[i].GetCapacity() > 0) {
            ++count;
            totalOccupancy += vans_[i].GetOccupiedSeats();
            totalCapacity += vans_[i].GetCapacity();
        }
    }
    if (totalCapacity == 0 || count == 0)
        return;
    double targetRatio = static_cast<double>(totalOccupancy) / totalCapacity;
    Assignment* assignments = new Assignment[count];
    size_t j = 0, sumBase = 0;
    for (size_t i = 0; i < size_; ++i) {
        if (vans_[i].GetCapacity() > 0) {
            size_t cap = vans_[i].GetCapacity();
            double ideal = targetRatio * cap;
            size_t baseOcc = static_cast<size_t>(ideal);
            double frac = ideal - baseOcc;
            sumBase += baseOcc;
            assignments[j].index = i;
            assignments[j].baseOccupancy = baseOcc;
            assignments[j].fraction = frac;
            assignments[j].capacity = cap;
            ++j;
        }
    }
    size_t remainder = totalOccupancy - sumBase;
    quickSortAssignments(assignments, 0, count - 1);
    for (size_t i = 0; i < count && remainder > 0; ++i) {
        if (assignments[i].baseOccupancy < assignments[i].capacity) {
            assignments[i].baseOccupancy++;
            remainder--;
        }
    }
    while (remainder > 0) {
        bool assignedAny = false;
        for (size_t i = 0; i < count && remainder > 0; ++i) {
            if (assignments[i].baseOccupancy < assignments[i].capacity) {
                assignments[i].baseOccupancy++;
                remainder--;
                assignedAny = true;
            }
        }
        if (!assignedAny)
            break;
    }
    for (size_t i = 0; i < count; ++i)
        vans_[assignments[i].index].SetOccupiedSeats(assignments[i].baseOccupancy);
    delete[] assignments;
}

void Train::MinimizeVans() {
    struct VanInfo { size_t capacity; size_t occupied; };
    const size_t NUM_TYPES = 4;
    VanType types[NUM_TYPES] = {VanType::Restaurant, VanType::Seated, VanType::Economy, VanType::Luxury};
    struct Group { size_t count; size_t totalOccupancy; VanInfo* infos; } groups[NUM_TYPES];
    for (size_t i = 0; i < NUM_TYPES; ++i) {
        groups[i].count = 0;
        groups[i].totalOccupancy = 0;
        groups[i].infos = nullptr;
    }
    for (size_t i = 0; i < size_; ++i) {
        VanType t = vans_[i].GetType();
        for (size_t j = 0; j < NUM_TYPES; ++j) {
            if (types[j] == t) {
                groups[j].count++;
                groups[j].totalOccupancy += vans_[i].GetOccupiedSeats();
                break;
            }
        }
    }
    for (size_t i = 0; i < NUM_TYPES; ++i) {
        if (groups[i].count > 0) {
            groups[i].infos = new VanInfo[groups[i].count];
            groups[i].count = 0;
        }
    }
    for (size_t i = 0; i < size_; ++i) {
        VanType t = vans_[i].GetType();
        for (size_t j = 0; j < NUM_TYPES; ++j) {
            if (types[j] == t && groups[j].infos) {
                groups[j].infos[groups[j].count].capacity = vans_[i].GetCapacity();
                groups[j].infos[groups[j].count].occupied = vans_[i].GetOccupiedSeats();
                groups[j].count++;
                break;
            }
        }
    }
    size_t groupNewCount[NUM_TYPES];
    for (size_t i = 0; i < NUM_TYPES; ++i) {
        if (!groups[i].infos) {
            groupNewCount[i] = 0;
            continue;
        }
        if (types[i] == VanType::Restaurant) {
            groupNewCount[i] = groups[i].count;
        } else {
            for (size_t a = 0; a < groups[i].count; ++a) {
                size_t maxIdx = a;
                for (size_t b = a + 1; b < groups[i].count; ++b) {
                    if (groups[i].infos[b].capacity > groups[i].infos[maxIdx].capacity)
                        maxIdx = b;
                }
                if (maxIdx != a) {
                    VanInfo tmp = groups[i].infos[a];
                    groups[i].infos[a] = groups[i].infos[maxIdx];
                    groups[i].infos[maxIdx] = tmp;
                }
            }
            size_t required = 0, capSum = 0;
            while (required < groups[i].count && capSum < groups[i].totalOccupancy) {
                capSum += groups[i].infos[required].capacity;
                required++;
            }
            groupNewCount[i] = required;
            size_t remaining = groups[i].totalOccupancy;
            for (size_t a = 0; a < required; ++a) {
                size_t assign = (remaining > groups[i].infos[a].capacity) ? groups[i].infos[a].capacity : remaining;
                groups[i].infos[a].occupied = assign;
                remaining -= assign;
            }
        }
    }
    size_t newTotal = 0;
    for (size_t i = 0; i < NUM_TYPES; ++i)
        newTotal += groupNewCount[i];
    Van* newVans = new Van[newTotal];
    size_t pos = 0;
    for (size_t i = 0; i < NUM_TYPES; ++i) {
        if (!groups[i].infos)
            continue;
        if (types[i] == VanType::Restaurant) {
            for (size_t a = 0; a < groups[i].count; ++a)
                newVans[pos++] = Van(groups[i].infos[a].capacity, groups[i].infos[a].occupied, types[i]);
        } else {
            for (size_t a = 0; a < groupNewCount[i]; ++a)
                newVans[pos++] = Van(groups[i].infos[a].capacity, groups[i].infos[a].occupied, types[i]);
        }
        delete[] groups[i].infos;
    }
    delete[] vans_;
    vans_ = newVans;
    size_ = newTotal;
    capacity_ = newTotal;
}


void Train::PlaceRestaurantVanOptimally() {
    int restIndex = -1;
    for (size_t i = 0; i < size_; ++i) {
        if (vans_[i].GetType() == VanType::Restaurant) {
            restIndex = static_cast<int>(i);
            break;
        }
    }
    if (restIndex == -1)
        return;
    Van restaurantVan = vans_[restIndex];
    for (size_t i = restIndex; i < size_ - 1; ++i)
        vans_[i] = vans_[i + 1];
    --size_;
    size_t* cumSums = new size_t[size_ + 1];
    cumSums[0] = 0;
    for (size_t i = 0; i < size_; ++i) {
        size_t cnt = (vans_[i].GetType() == VanType::Luxury) ? 0 : vans_[i].GetOccupiedSeats();
        cumSums[i + 1] = cumSums[i] + cnt;
    }
    size_t total = cumSums[size_];
    size_t bestIndex = 0, bestDiff = total;
    for (size_t k = 0; k <= size_; ++k) {
        size_t left = cumSums[k];
        size_t right = total - left;
        size_t diff = (left > right) ? left - right : right - left;
        if (diff < bestDiff) {
            bestDiff = diff;
            bestIndex = k;
        }
    }
    delete[] cumSums;
    for (size_t i = size_; i > bestIndex; --i)
        vans_[i] = vans_[i - 1];
    vans_[bestIndex] = restaurantVan;
    ++size_;
}

} // namespace mgt
