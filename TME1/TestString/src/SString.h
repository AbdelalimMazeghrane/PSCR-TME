#pragma once

#include <iostream>
#include <memory>
#include "strutil.h"  // pr::length, pr::compare, pr::newcat

class TestSString; // Pour accès ami aux tests

namespace pr {

class SString {
private:
    std::shared_ptr<char[]> data; // Partage automatique des données

public:
    // Constructeurs
    explicit SString(const char* s = "");        // Constructeur depuis C-string
    SString(const SString& other);              // Copy ctor (partage la data)
    SString(SString&& other) noexcept;          // Move ctor

    // Opérateurs d'affectation
    SString& operator=(const SString& other);   // Copy assign (partage la data)
    SString& operator=(SString&& other) noexcept; // Move assign

    // Comparaison
    bool operator<(const SString& other) const;

    // Opérateurs friend
    friend std::ostream& operator<<(std::ostream& os, const SString& str);
    friend bool operator==(const SString& a, const SString& b);
    friend SString operator+(const SString& a, const SString& b);

    // Accès pour tests
    friend class ::TestSString;
};

} // namespace pr
