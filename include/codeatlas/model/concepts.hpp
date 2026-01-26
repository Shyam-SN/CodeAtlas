#pragma once

#include <concepts>
#include <string>

namespace codeatlas {

template <typename T>
concept Identifiable = requires(T t) {
    { t.id() } -> std::same_as<std::string>;
};

} // namespace codeatlas
