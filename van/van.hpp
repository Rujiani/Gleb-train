#ifndef VAN_HPP_
#define VAN_HPP_

#include <iostream>
#include <stdexcept>
#include <string>
#include <map>
#include <format>

namespace mgt {

enum class VanType {
    Restaurant,
    Seated,
    Economy,
    Luxury
};

const std::map<VanType, size_t> DefaultCapacity{
    {VanType::Restaurant, 0},
    {VanType::Seated, 78},
    {VanType::Economy, 56},
    {VanType::Luxury, 14}
};

const std::map<VanType, std::string> TypeToString{
    {VanType::Restaurant, "restaurant"},
    {VanType::Seated, "seated"},
    {VanType::Economy, "economy"},
    {VanType::Luxury, "luxury"}
};

const std::map<std::string, VanType> StringToType{
    {"restaurant", VanType::Restaurant},
    {"seated", VanType::Seated},
    {"economy", VanType::Economy},
    {"luxury", VanType::Luxury}
};

class Van {
private:
    size_t capacity_;
    size_t occupiedSeats_;
    VanType type_;

    [[nodiscard]] size_t CalculateOccupancyRate(size_t occupiedSeats, size_t capacity) const noexcept {
        return capacity_ ? static_cast<size_t>((static_cast<double>(occupiedSeats) / static_cast<double>(capacity)) * 100) : 0;
    }

public:
    Van() noexcept : capacity_(0), occupiedSeats_(0), type_(VanType::Restaurant) {}

    explicit Van(size_t capacity, size_t occupiedSeats, VanType type);

    explicit Van(VanType type)
        : capacity_(DefaultCapacity.at(type)), occupiedSeats_(0), type_(type) {}

    [[nodiscard]] size_t GetCapacity() const noexcept { return capacity_; }
    [[nodiscard]] size_t GetOccupiedSeats() const noexcept { return occupiedSeats_; }
    [[nodiscard]] VanType GetType() const noexcept { return type_; }
    [[nodiscard]] size_t OccupancyRate() const noexcept { return CalculateOccupancyRate(occupiedSeats_, capacity_); }

    void SetCapacity(size_t capacity) {
        if (type_ == VanType::Restaurant && capacity != 0)
            throw std::invalid_argument("Error: Restaurant van capacity must remain 0.");
        if (occupiedSeats_ > capacity)
            throw std::invalid_argument("Error: Occupied seats exceed new capacity.");
        capacity_ = capacity;
    }

    void SetOccupiedSeats(size_t occupiedSeats) {
        if (occupiedSeats > capacity_)
            throw std::invalid_argument("Error: Occupied seats exceed capacity.");
        occupiedSeats_ = occupiedSeats;
    }

    void SetType(VanType type) {
        if (type == VanType::Restaurant && capacity_ != 0)
            throw std::invalid_argument("Error: Restaurant van capacity must remain 0.");
        type_ = type;
    }

    Van& operator>>(Van& other);

    void AddPassengers(size_t count) { SetOccupiedSeats(occupiedSeats_ + count); }
    void RemovePassengers(size_t count) noexcept { occupiedSeats_ = (occupiedSeats_ < count) ? 0 : occupiedSeats_ - count; }

    void Print(std::ostream& os) const noexcept {
        os << std::format("{}/{} {}", occupiedSeats_, capacity_, TypeToString.at(type_));
    }

    void Read(std::istream& is) noexcept;

    friend std::istream& operator>>(std::istream& is, Van& van) noexcept {
        van.Read(is);
        return is;
    }

    friend std::ostream& operator<<(std::ostream& os, const Van& van) noexcept {
        van.Print(os);
        return os;
    }

    Van& operator+=(size_t count) {
        AddPassengers(count);
        return *this;
    }

    Van& operator-=(size_t count) noexcept {
        RemovePassengers(count);
        return *this;
    }

    bool operator==(const Van& other) const = default;
};

}

#endif
