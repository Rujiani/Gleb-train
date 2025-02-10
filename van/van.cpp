#include "van.hpp"
#include <stdexcept>
#include <exception>

namespace mgt {

Van::Van(size_t capacity, size_t occupiedSeats, VanType type)
    : capacity_(capacity), occupiedSeats_(occupiedSeats), type_(type) {
    if (type_ == VanType::Restaurant && capacity_ != 0)
        throw std::invalid_argument("Error: Restaurant van capacity must remain 0.");
    if (occupiedSeats_ > capacity_)
        throw std::invalid_argument("Error: Occupied seats exceed capacity.");
}

Van& Van::operator>>(Van& other) {
    if (type_ != other.type_)
        throw std::invalid_argument("Cannot transfer passengers between different van types.");
    
    size_t totalCapacity = capacity_ + other.capacity_;
    size_t totalOccupied = occupiedSeats_ + other.occupiedSeats_;
    size_t occupancyRate = CalculateOccupancyRate(totalOccupied, totalCapacity);

    size_t adjustedSeats = static_cast<size_t>(static_cast<double>(capacity_) * (static_cast<double>(occupancyRate) * 0.01));
    occupiedSeats_ = adjustedSeats;
    other.occupiedSeats_ = totalOccupied - adjustedSeats;
    
    return *this;
}

void Van::Read(std::istream& is) noexcept {
    size_t capacity, occupiedSeats;
    std::string input, typeStr;
    VanType tempType;
    
    is >> input >> typeStr;
    if (is) {
        try {
            size_t delimiter = input.find('/');
            occupiedSeats = std::stoul(input.substr(0, delimiter));
            capacity = std::stoul(input.substr(delimiter + 1));
            tempType = StringToType.at(typeStr);
            *this = Van(capacity, occupiedSeats, tempType);
        } catch (const std::exception&) {
            is.setstate(std::istream::failbit);
        }
    }
}

} // namespace mgt
