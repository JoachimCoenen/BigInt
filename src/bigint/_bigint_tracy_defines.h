#ifndef BIGINT_TRACY_DEFINES_H
#define BIGINT_TRACY_DEFINES_H

#ifdef BIGINT_TRACY_ENABLE
//      [[nodiscard]] constexpr auto
#	define BIGINT_TRACY_CONSTEXPR_AUTO [[nodiscard]] inline auto
//      constexpr auto
#	define BIGINT_TRACY_CONSTEXPR_AUTO_DISCARD inline auto
//      constexpr void
#	define BIGINT_TRACY_CONSTEXPR_VOID inline void
//      constexpr
#	define BIGINT_TRACY_CONSTEXPR inline
#else
//      [[nodiscard]] constexpr auto
#	define BIGINT_TRACY_CONSTEXPR_AUTO CONSTEXPR_AUTO
//      constexpr auto
#	define BIGINT_TRACY_CONSTEXPR_AUTO_DISCARD CONSTEXPR_AUTO_DISCARD
//      constexpr void
#	define BIGINT_TRACY_CONSTEXPR_VOID CONSTEXPR_VOID
//      constexpr
#	define BIGINT_TRACY_CONSTEXPR constexpr

#endif

#ifdef ZoneScoped
#	define BIGINT_TRACY_ZONE_SCOPED ZoneScoped
#else
#	define BIGINT_TRACY_ZONE_SCOPED
#endif

#endif // BIGINT_TRACY_DEFINES_H
