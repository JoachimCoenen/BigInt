#pragma once

#define BIGINT_ENABLE_POOL_STATS 0

#include "utils.h"
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <type_traits>
#include <algorithm>
#include <memory>

#if BIGINT_ENABLE_POOL_STATS
namespace bigint::pool {
/// statistics for profiling
struct pool_statistics {
	uint64_t ptr_request_cnt = 0;
	uint64_t ptr_created_cnt = 0;
	uint64_t ptr_reuse_cnt = 0;
	uint64_t ptr_returned_cnt = 0;
	uint64_t ptrs_in_circulation = 0;

	CONSTEXPR_VOID reset() { *this = pool_statistics(); }
};

CONSTEXPR_AUTO_DISCARD
operator<<(std::ostream& o, const pool_statistics& p) -> std::ostream& {
	const auto to_str = [](uint64_t v) { return  std::to_string(v); };

	o << utils::_private::build_table({
		{ "",      "requested",            "created",              "reused",             "returned",              "in circulation", },
		{ },
		{ "pointers", to_str(p.ptr_request_cnt), to_str(p.ptr_created_cnt), to_str(p.ptr_reuse_cnt), to_str(p.ptr_returned_cnt), to_str(p.ptrs_in_circulation), },
		//{ "", , , , },
	}) << std::endl;
	return o;
}
}
#endif

namespace bigint::pool {

template <class T>
struct pooled_reset {
	/// (re-)initializes an already allocated value of type T (e.g. sets it to 0).
	/// customized versions of this function can have arbitrary additional parameters.
	/// It might choose to do nothing.
	[[nodiscard]] static constexpr auto
	reset(T&) {}
};

template <class T>
struct pooled_reset<std::vector<T>> {
	static CONSTEXPR_VOID
	reset(std::vector<T>& v) { v.clear(); }
};

template<>
struct pooled_reset<std::string> {
	static CONSTEXPR_VOID
	reset(std::string& v) { v.clear(); }
};


// forward declaration
template<typename T>
struct pooled_ptr;

template<typename T>
class object_pool {
private:
#if BIGINT_ENABLE_POOL_STATS
	pool_statistics _stats;
#endif
	std::vector<std::unique_ptr<T>> _resources;
public:
	object_pool() = default;
	object_pool(const object_pool&) = delete;
	object_pool& operator=(const object_pool&) = delete;
	object_pool(object_pool&&) = delete;
	object_pool& operator=(object_pool&&) = delete;

#if BIGINT_ENABLE_POOL_STATS
	~object_pool() {
		print_statistics();
	}

	CONSTEXPR_VOID
	print_statistics() const {
		std::cout << "========================================" << std::endl;
		std::cout << _stats;
		std::cout << "========================================" << std::endl;
	}
#endif

	[[nodiscard]]
	static constexpr auto get() -> object_pool& {
		static thread_local std::unique_ptr _pool = std::make_unique<object_pool>();
		return *_pool;
	}

private:
	/**
	 * Returns a pointer to an instance of T.
	 *
	 * A new instance of T will be created if the pool is empty.
	 * @return a pointer to an instance of T.
	 */
	CONSTEXPR_AUTO
	get_ptr() -> std::unique_ptr<T> {
		#if BIGINT_ENABLE_POOL_STATS
			++_stats.ptr_request_cnt;
			++_stats.ptrs_in_circulation;
		#endif

		std::unique_ptr<T> tmp;
		if (_resources.empty()) {
			#if BIGINT_ENABLE_POOL_STATS
				++_stats.ptr_created_cnt;
			#endif
			tmp = std::make_unique<T>();
		} else {
			#if BIGINT_ENABLE_POOL_STATS
				++_stats.ptr_reuse_cnt;
			#endif
			tmp = std::move(_resources.back());
			_resources.pop_back();
			pooled_reset<T>::reset(*tmp);
		}
		return tmp;
	}

	/**
	 * puts the ptr back into the pool.
	 *
	 * @param ptr the pointer, that is returned.
	 * @return void
	 */
	CONSTEXPR_VOID
	return_ptr(std::unique_ptr<T>&& ptr) {
		_resources.push_back(std::move(ptr));
		#if BIGINT_ENABLE_POOL_STATS
			++_stats.ptr_returned_cnt;
			--_stats.ptrs_in_circulation;
		#endif
	}

	// friend class pooled_delete<T>;
	friend class pooled_ptr<T>;

public:
#if BIGINT_ENABLE_POOL_STATS
	CONSTEXPR_AUTO
	stats() -> pool_statistics& {
		return _stats;
	}

	CONSTEXPR_AUTO
	stats() const -> const pool_statistics& {
		return _stats;
	}

	CONSTEXPR_VOID
	reset_stats() {
		_stats.reset();
	}
#endif
};

/**
 * very simple unique_ptr that supports constexpr.
 */
template<typename T>
struct pooled_ptr {
public:
	constexpr pooled_ptr()
	: _ptr(object_pool<T>::get().get_ptr())
	{ }

	constexpr pooled_ptr(std::nullptr_t) noexcept : _ptr(nullptr) { }

	constexpr ~pooled_ptr() {
		if (_ptr) {
			object_pool<T>::get().return_ptr(std::move(_ptr));
		}
	}

	constexpr pooled_ptr(const pooled_ptr& other)
	    : pooled_ptr() {
		*_ptr = *other._ptr;
	}

	pooled_ptr(pooled_ptr&& other) noexcept : _ptr(std::move(other._ptr)) { }

	CONSTEXPR_AUTO_DISCARD
	operator=(pooled_ptr&& other) noexcept -> pooled_ptr& {
		std::swap(_ptr, other._ptr);
		return *this;
	}

	CONSTEXPR_AUTO_DISCARD
	operator=(const pooled_ptr& other) -> pooled_ptr& {
		*_ptr = *other._ptr;
		return *this;
	}

	CONSTEXPR_AUTO_DISCARD
	operator=(std::nullptr_t) -> pooled_ptr& {
		pooled_ptr tmp{nullptr};
		std::swap(tmp, *this);
		return *this;
	}

	CONSTEXPR_AUTO
	operator *() const noexcept -> const T& { return *_ptr; }

	CONSTEXPR_AUTO
	operator *() noexcept -> T& { return *_ptr; }

	CONSTEXPR_AUTO
	operator ->() const noexcept -> const T* { return &*_ptr; }

	CONSTEXPR_AUTO
	operator ->() noexcept -> T* { return &*_ptr; }

	[[nodiscard]] constexpr
	explicit operator bool() const noexcept { return _ptr != nullptr; }

private:
	std::unique_ptr<T> _ptr = nullptr;
};

}
