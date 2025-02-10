#include "../van/van.hpp"
#include <stdexcept>
#include <algorithm>

namespace mgt {

class Train {
private:
    Van* vans_;
    size_t size_;
    size_t capacity_;

    void Resize(size_t newSize) {
        Van* temp = new Van[newSize];
        std::copy_n(vans_, size_, temp);
        delete[] vans_;
        vans_ = temp;
        capacity_ = newSize;
    }

    void Expand() {
        Resize(capacity_ * 2);
    }

    void Shrink() {
        Resize(capacity_ / 2);
    }

    void CheckResize() {
        if (capacity_ / 2 == size_)
            Shrink();
    }

public:
    Train() noexcept : vans_(new Van[1]), size_(0), capacity_(1) {}

    Train(const Van* ptr, size_t size) : vans_(new Van[size]), size_(size), capacity_(size) {
        std::copy_n(ptr, size, vans_);
    }

    Train(const Van& van) : vans_(new Van[1]), size_(1), capacity_(1) {
        vans_[0] = van;
    }

    Train(const Train& other) : vans_(new Van[other.capacity_]), size_(other.size_), capacity_(other.capacity_) {
        std::copy_n(other.vans_, size_, vans_);
    }

    Train(Train&& other) noexcept : vans_(other.vans_), size_(other.size_), capacity_(other.capacity_) {
        other.vans_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    ~Train() {
        delete[] vans_;
    }

    Train& operator=(const Train& other);
    Train& operator=(Train&& other) noexcept;

    bool operator==(const Train& other) const {
        if (!vans_ || !other.vans_)
            return vans_ == other.vans_;
        return std::equal(vans_, vans_ + size_, other.vans_);
    }

    bool operator!=(const Train& other) const {
        return !(*this == other);
    }

    Van& operator[](size_t index) {
        if (index >= size_)
            throw std::out_of_range("Index out of train range");
        return vans_[index];
    }

    const Van& operator[](size_t index) const {
        if (index >= size_)
            throw std::out_of_range("Index out of train range");
        return vans_[index];
    }

    Train& operator+=(const Van& van) {
        if (size_ == capacity_)
            Expand();
        vans_[size_++] = van;
        return *this;
    }

    void RemoveVan(size_t index) {
        if (index >= size_)
            throw std::out_of_range("Index out of train range");
        if (index != --size_)
            vans_[index] = vans_[size_];
        CheckResize();
    }

    void SitInMin(size_t numOfPassengers);
    void StaffingPercentage() noexcept;

    size_t GetSize() const noexcept { return size_; }

    void Write(std::ostream& os) const noexcept {
        os << "{";
        for (size_t i = 0; i < size_ - 1; ++i) {
            os << vans_[i] << ", ";
        }
        os << vans_[size_ - 1] << "}";
    }

    void Read(std::istream& is) noexcept {
        Van temp;
        is >> temp;
        if (is)
            *this = Train(temp);
    }

    friend std::ostream& operator<<(std::ostream& os, const Train& train) noexcept {
        train.Write(os);
        return os;
    }

    friend std::istream& operator>>(std::istream& is, Train& train) noexcept {
        train.Read(is);
        return is;
    }
};

} // namespace mgt
