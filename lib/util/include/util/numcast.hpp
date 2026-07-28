#pragma once

#include <cmath>
#include <concepts>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>

namespace util
{

// Same type: trivially in range.
template <typename to_t, typename from_t>
  requires(std::is_same_v<to_t, from_t>)
[[nodiscard]] constexpr bool in_range(const from_t) noexcept
{
  return true;
}

// float -> integral: in a constant-evaluated context the roundtrip check is
// enough (and avoids std::isfinite, which is not constexpr in C++20). At
// runtime, reject non-finite first, then require an in-bounds value that
// survives a roundtrip (i.e. no fractional loss).
template <typename to_t, typename from_t>
  requires(!std::is_same_v<to_t, from_t>) && std::is_floating_point_v<from_t> && std::is_integral_v<to_t>
[[nodiscard]] constexpr bool in_range(const from_t value) noexcept
{
  if (std::is_constant_evaluated()) { return static_cast<from_t>(static_cast<to_t>(value)) == value; }
  else
  {
    if constexpr (std::numeric_limits<from_t>::is_iec559)
    {
      if (!std::isfinite(value)) return false;
    }
    return value >= static_cast<from_t>(std::numeric_limits<to_t>::lowest())
      && value <= static_cast<from_t>(std::numeric_limits<to_t>::max())
      && static_cast<from_t>(static_cast<to_t>(value)) == value;
  }
}

// integral -> integral, same signedness: direct range comparison in from_t's
// domain.
template <typename to_t, typename from_t>
  requires(!std::is_same_v<to_t, from_t>)
  && std::is_integral_v<to_t> && std::is_integral_v<from_t> && (std::is_signed_v<from_t> == std::is_signed_v<to_t>)
[[nodiscard]] constexpr bool in_range(const from_t value) noexcept
{
  constexpr auto to_min = std::numeric_limits<to_t>::lowest();
  constexpr auto to_max = std::numeric_limits<to_t>::max();
  return value >= static_cast<from_t>(to_min) && value <= static_cast<from_t>(to_max);
}

// integral -> integral, signed -> unsigned: negatives never fit; compare
// magnitudes in the unsigned domain to avoid signed-overflow UB.
template <typename to_t, typename from_t>
  requires(!std::is_same_v<to_t, from_t>)
  && std::is_integral_v<to_t> && std::is_integral_v<from_t> && std::is_signed_v<from_t> && std::is_unsigned_v<to_t>
[[nodiscard]] constexpr bool in_range(const from_t value) noexcept
{
  if (value < 0) return false;
  constexpr auto to_max = std::numeric_limits<to_t>::max();
  return static_cast<std::make_unsigned_t<from_t>>(value) <= static_cast<std::make_unsigned_t<from_t>>(to_max);
}

// integral -> integral, unsigned -> signed: to_max is non-negative here, so a
// plain unsigned comparison against to_max is safe and sufficient.
template <typename to_t, typename from_t>
  requires(!std::is_same_v<to_t, from_t>)
  && std::is_integral_v<to_t> && std::is_integral_v<from_t> && std::is_unsigned_v<from_t> && std::is_signed_v<to_t>
[[nodiscard]] constexpr bool in_range(const from_t value) noexcept
{
  constexpr auto to_max = std::numeric_limits<to_t>::max();
  return value <= static_cast<std::make_unsigned_t<from_t>>(to_max);
}

// integral -> float: always in range by magnitude, but precision loss is
// possible (e.g. long long > 2^53 -> double). A roundtrip check detects it.
template <typename to_t, typename from_t>
  requires(!std::is_same_v<to_t, from_t>) && std::is_integral_v<from_t> && std::is_floating_point_v<to_t>
[[nodiscard]] constexpr bool in_range(const from_t value) noexcept
{
  return static_cast<from_t>(static_cast<to_t>(value)) == value;
}

// float -> float: a value fits iff it round trips through to_t exactly.
// (Loss of precision is treated as out of range.)
template <typename to_t, typename from_t>
  requires(!std::is_same_v<to_t, from_t>) && std::is_floating_point_v<from_t> && std::is_floating_point_v<to_t>
[[nodiscard]] constexpr bool in_range(const from_t value) noexcept
{
  return static_cast<from_t>(static_cast<to_t>(value)) == value;
}

// Casts `value` to `to_t`, returning std::nullopt if it does not fit.
// Use when overflow is recoverable but you have no natural fallback value.
template <typename to_t, typename from_t>
  requires std::is_arithmetic_v<to_t> && std::is_arithmetic_v<from_t>
[[nodiscard]] constexpr std::optional<to_t> narrow_cast(const from_t value) noexcept
{
  if (!in_range<to_t>(value)) return std::nullopt;
  return static_cast<to_t>(value);
}

// Casts `value` to `to_t`, returning `fallback` if it does not fit. No throw.
// Use when overflow is recoverable.
template <typename to_t, typename from_t>
  requires std::is_arithmetic_v<to_t> && std::is_arithmetic_v<from_t>
[[nodiscard]] constexpr to_t narrow_cast(const from_t value, const to_t fallback) noexcept
{
  if (in_range<to_t>(value)) return static_cast<to_t>(value);
  return fallback;
}

// Casts `value` to `to_t`, throwing std::out_of_range if it does not fit.
// Use when overflow would be a bug (e.g. deserialization, untrusted input).
template <typename to_t, typename from_t>
  requires std::is_arithmetic_v<to_t> && std::is_arithmetic_v<from_t>
[[nodiscard]] constexpr to_t narrow(const from_t value)
{
  if (!in_range<to_t>(value)) throw std::out_of_range("narrow: value out of range for target type");
  return static_cast<to_t>(value);
}

// integral -> integral, same signedness: clamp directly in from_t's domain.
template <typename to_t, typename from_t>
  requires(!std::is_same_v<to_t, from_t>)
  && std::is_integral_v<to_t> && std::is_integral_v<from_t> && (std::is_signed_v<from_t> == std::is_signed_v<to_t>)
[[nodiscard]] constexpr to_t clamp_cast(const from_t value) noexcept
{
  constexpr auto to_min = std::numeric_limits<to_t>::lowest();
  constexpr auto to_max = std::numeric_limits<to_t>::max();
  if (value < static_cast<from_t>(to_min)) return to_min;
  if (value > static_cast<from_t>(to_max)) return to_max;
  return static_cast<to_t>(value);
}

// integral -> integral, signed -> unsigned: negatives clamp to 0; compare
// magnitudes unsigned.
template <typename to_t, typename from_t>
  requires(!std::is_same_v<to_t, from_t>)
  && std::is_integral_v<to_t> && std::is_integral_v<from_t> && std::is_signed_v<from_t> && std::is_unsigned_v<to_t>
[[nodiscard]] constexpr to_t clamp_cast(const from_t value) noexcept
{
  constexpr auto to_max = std::numeric_limits<to_t>::max();
  if (value < 0) return static_cast<to_t>(0);
  if (static_cast<std::make_unsigned_t<from_t>>(value) > static_cast<std::make_unsigned_t<to_t>>(to_max)) return to_max;
  return static_cast<to_t>(value);
}

// integral -> integral, unsigned -> signed: to_max is non-negative, so compare
// in unsigned.
template <typename to_t, typename from_t>
  requires(!std::is_same_v<to_t, from_t>)
  && std::is_integral_v<to_t> && std::is_integral_v<from_t> && std::is_unsigned_v<from_t> && std::is_signed_v<to_t>
[[nodiscard]] constexpr to_t clamp_cast(const from_t value) noexcept
{
  constexpr auto to_max = std::numeric_limits<to_t>::max();
  const auto max_in = static_cast<std::make_unsigned_t<from_t>>(to_max);
  if (value > max_in) return static_cast<to_t>(max_in);
  return static_cast<to_t>(value);
}

// float -> integral: non-finite maps to 0, otherwise saturate to range.
template <typename to_t, typename from_t>
  requires(!std::is_same_v<to_t, from_t>) && std::is_floating_point_v<from_t> && std::is_integral_v<to_t>
[[nodiscard]] constexpr to_t clamp_cast(const from_t value) noexcept
{
  if (std::is_constant_evaluated())
  {
    if (value < static_cast<from_t>(std::numeric_limits<to_t>::lowest())) return std::numeric_limits<to_t>::lowest();
    if (value > static_cast<from_t>(std::numeric_limits<to_t>::max())) return std::numeric_limits<to_t>::max();
    return static_cast<to_t>(value);
  }
  else
  {
    if (!std::isfinite(value)) return static_cast<to_t>(0);
    if (value < static_cast<from_t>(std::numeric_limits<to_t>::lowest())) return std::numeric_limits<to_t>::lowest();
    if (value > static_cast<from_t>(std::numeric_limits<to_t>::max())) return std::numeric_limits<to_t>::max();
    return static_cast<to_t>(value);
  }
}

// integral -> float: every integral value is within to_t's exponent range,
// so no bounds check is needed (and the natural bounds check would be UB --
// casting to_t::max() back to from_t overflows). This is just static_cast.
template <typename to_t, typename from_t>
  requires(!std::is_same_v<to_t, from_t>) && std::is_integral_v<from_t> && std::is_floating_point_v<to_t>
[[nodiscard]] constexpr to_t clamp_cast(const from_t value) noexcept
{
  return static_cast<to_t>(value);
}

// float -> float: clamp to to_t's representable range.
template <typename to_t, typename from_t>
  requires(!std::is_same_v<to_t, from_t>) && std::is_floating_point_v<from_t> && std::is_floating_point_v<to_t>
[[nodiscard]] constexpr to_t clamp_cast(const from_t value) noexcept
{
  if (value < static_cast<from_t>(std::numeric_limits<to_t>::lowest()))
    return static_cast<to_t>(std::numeric_limits<to_t>::lowest());
  if (value > static_cast<from_t>(std::numeric_limits<to_t>::max()))
    return static_cast<to_t>(std::numeric_limits<to_t>::max());
  return static_cast<to_t>(value);
}

} // namespace util
