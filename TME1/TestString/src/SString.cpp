#include "SString.h"

namespace pr {


SString::SString(const char* s) {
    size_t len = length(s);
    char* temp = new char[len + 1];
    for (size_t i = 0; i < len; ++i) temp[i] = s[i];
    temp[len] = '\0';
    data = std::shared_ptr<char[]>(temp);
}

SString::SString(const SString& other) : data(other.data) {
    // Partage la donnée automatiquement via shared_ptr
}

SString::SString(SString&& other) noexcept : data(std::move(other.data)) {
    // Move: data est déplacé, other.data devient nullptr
}


SString& SString::operator=(const SString& other) {
    if (this != &other) {
        data = other.data; // Partage la data
    }
    return *this;
}

SString& SString::operator=(SString&& other) noexcept {
    if (this != &other) {
        data = std::move(other.data);
    }
    return *this;
}


bool SString::operator<(const SString& other) const {
    return compare(data.get(), other.data.get()) < 0;
}


std::ostream& operator<<(std::ostream& os, const SString& str) {
    if (str.data) os << str.data.get();
    else os << "(null)";
    return os;
}

bool operator==(const SString& a, const SString& b) {
    return compare(a.data.get(), b.data.get()) == 0;
}

SString operator+(const SString& a, const SString& b) {
    char* cat = newcat(a.data.get(), b.data.get());
    SString result(cat);
    delete[] cat;
    return result;
}

} // namespace pr
