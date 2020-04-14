#pragma once

#include <utility>
#include <optional>

namespace core
{

///
/// \name util::Singleton
/// \brief The Singleton class is a CRTP helper for creating Singleton classes.
///
template <typename ManagedType>
class Singleton
{
public:
    Singleton& operator = (const Singleton&) = delete;
    Singleton& operator = (Singleton&&)      = delete;

    template <typename ...Args> static
    auto init(Args&&... args) noexcept(noexcept(ManagedType{std::forward<Args>(args)...})) -> ManagedType&
    {
        static ManagedType instance{std::forward<Args>(args)...};
        instance_ptr = &instance;
        return instance;
    }

    using GetInstanceRetType = std::optional<std::reference_wrapper<ManagedType>>;
    
    static
    auto getInstance() noexcept(true) -> GetInstanceRetType
    {
        return instance_ptr ? GetInstanceRetType{*instance_ptr} : std::nullopt;
    }

protected:
    Singleton() noexcept(true) = default;
    
private:
    inline static ManagedType* instance_ptr = nullptr;

}; // Singleton

} // core
