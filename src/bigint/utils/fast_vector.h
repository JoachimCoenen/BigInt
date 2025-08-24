#pragma once

#define BIGINT_ENABLE_FAST_VECTOR_STATS 0

#include "utils.h"

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <type_traits>

#if BIGINT_ENABLE_FAST_VECTOR_STATS
namespace bigint::fast {
/// statistics for profiling
struct fast_vector_statistics {
	uint64_t created_cnt = 0;
	uint64_t copied_cnt = 0;
	uint64_t copied_digits_cnt = 0;
	uint64_t moved_cnt = 0;
	uint64_t swapped_cnt = 0;
	uint64_t realloc_cnt = 0;
	uint64_t destroyed_cnt = 0;
	uint64_t alive_cnt = 0;

	CONSTEXPR_VOID reset() { *this = fast_vector_statistics(); }
};

CONSTEXPR_AUTO_DISCARD
operator<<(std::ostream& o, const fast_vector_statistics& p) -> std::ostream& {
	const auto to_str = [](uint64_t v) { return  std::to_string(v); };

	o << utils::_private::build_table({
		{ "",     "created",          "copied",          "copied digits",          "moved",          "swapped",          "realloc",          "destroyed",          "alive", },
		{ },
		{ "vectors", to_str(p.created_cnt), to_str(p.copied_cnt), to_str(p.copied_digits_cnt), to_str(p.moved_cnt), to_str(p.swapped_cnt), to_str(p.realloc_cnt), to_str(p.destroyed_cnt), to_str(p.alive_cnt), },
		//{ "", , , , },
	}) << std::endl;
	return o;
}
}
#endif



#define FAST_VEC_DEFINE_FWD_ITERATORS(NAME, RET_TYPE, BODY)\
	CONSTEXPR_AUTO NAME() noexcept -> RET_TYPE { return BODY; } \
	CONSTEXPR_AUTO NAME() const noexcept -> const_##RET_TYPE { return BODY; } \
	CONSTEXPR_AUTO c##NAME() const noexcept -> const_##RET_TYPE { return BODY; }

#define FAST_VEC_DEFINE_ITERATORS(NAME, BODY)\
	FAST_VEC_DEFINE_FWD_ITERATORS(NAME, iterator, BODY) \
	FAST_VEC_DEFINE_FWD_ITERATORS(r##NAME, reverse_iterator, std::make_reverse_iterator(BODY))

namespace bigint::fast {
namespace _private {
	template<typename T>
	concept is_trivial = std::is_trivial_v<T>;
}


/**
 * A vector implementation that does not initialize its elements..
 */
template<_private::is_trivial T>
class fast_vector {
public:
	using value_type = T;
	using pointer = T* ;
	using const_pointer = const T* ;
	using reference = T& ;
	using const_reference = const T& ;
	using iterator = pointer;
	using const_iterator = const_pointer;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;

public:
	constexpr fast_vector() noexcept {
		#if BIGINT_ENABLE_FAST_VECTOR_STATS
			++get_stats().created_cnt;
			++get_stats().alive_cnt;
		#endif
	}


	explicit constexpr fast_vector(size_type size)
	: fast_vector() {
		resize(size);
	}

	constexpr fast_vector(std::initializer_list<T> init_list)
	: fast_vector() {
		reserve(init_list.size());
		for (auto& val: init_list) {
			push_back(val);
		}
	}

	constexpr fast_vector(const fast_vector& other)
	: fast_vector() {
		#if BIGINT_ENABLE_FAST_VECTOR_STATS
			++get_stats().copied_cnt;
			get_stats().copied_digits_cnt += other.size();
		#endif
		if (other._data != nullptr) {
			resize(other.size());
			std::copy(other._data, other._data_end, _data);
		}
	}

	constexpr fast_vector(fast_vector&& other) noexcept
	: fast_vector() {
		#if BIGINT_ENABLE_FAST_VECTOR_STATS
			++get_stats().moved_cnt;
		#endif
		std::swap(*this, other);
	}

	CONSTEXPR_AUTO_DISCARD
	operator=(const fast_vector& other) -> fast_vector& {
		if (this != &other) {
			#if BIGINT_ENABLE_FAST_VECTOR_STATS
				++get_stats().copied_cnt;
				get_stats().copied_digits_cnt += other.size();
			#endif
			if (other._data == nullptr) {
				resize(0);
			} else {
				resize(other.size());
				std::copy(other._data, other._data_end, _data);
			}
		}
		return* this;
	}

	CONSTEXPR_AUTO_DISCARD
	operator=(fast_vector&& other) noexcept -> fast_vector& {
		#if BIGINT_ENABLE_FAST_VECTOR_STATS
			++get_stats().moved_cnt;
		#endif
		swap(*this, other);
		return* this;
	}

	constexpr ~fast_vector() {
		#if BIGINT_ENABLE_FAST_VECTOR_STATS
			++get_stats().destroyed_cnt;
			--get_stats().alive_cnt;
		#endif
		std::free(_data);
	}

	// Element access

	CONSTEXPR_AUTO
	operator[](size_type index) noexcept -> T& {
		assert(index < size() && "Index is out of range");
		return _data[index];
	}

	CONSTEXPR_AUTO
	operator[](size_type index) const noexcept -> const T& {
		assert(index < size() && "Index is out of range");
		return _data[index];
	}

	CONSTEXPR_AUTO
	at(size_type index) -> T& {
		utils::check_bounds(index, size());
		return _data[index];
	}

	CONSTEXPR_AUTO
	at(size_type index) const -> const T& {
		utils::check_bounds(index, size());
		return _data[index];
	}

	CONSTEXPR_AUTO
	front() noexcept -> T& {
		assert(!empty() && "Container is empty");
		return *_data;
	}

	CONSTEXPR_AUTO
	front() const noexcept -> const T& {
		assert(!empty() && "Container is empty");
		return *_data;
	}

	CONSTEXPR_AUTO
	back() noexcept -> T& {
		assert(!empty() && "Container is empty");
		return *(_data_end - 1);
	}

	CONSTEXPR_AUTO
	back() const noexcept -> const T& {
		assert(!empty() && "Container is empty");
		return *(_data_end - 1);
	}

	CONSTEXPR_AUTO
	data() noexcept -> T* {
		return _data;
	}

	CONSTEXPR_AUTO
	data() const noexcept -> const T* {
		return _data;
	}

	// Iterators
	FAST_VEC_DEFINE_ITERATORS(begin, _data)
	FAST_VEC_DEFINE_ITERATORS(end, _data_end)

	// Capacity

	CONSTEXPR_AUTO
	empty() const noexcept -> bool {
		return _data == _data_end;
	}

	CONSTEXPR_AUTO
	size() const noexcept -> size_type {
		return _data_end - _data;
	}

	CONSTEXPR_AUTO
	size_bytes() const noexcept -> size_type {
		return size() * sizeof(T);
	}

	CONSTEXPR_AUTO
	capacity() const noexcept -> size_type {
		return _storage_end - _data;
	}

	CONSTEXPR_AUTO
	capacity_bytes() const noexcept -> size_type {
		return capacity() * sizeof(T);
	}

	CONSTEXPR_VOID
	reserve(size_type new_cap) {
		if (new_cap > capacity()) {
			reallocate(new_cap);
		}
	}

	CONSTEXPR_VOID
	shrink_to_fit() {
		if (_data_end < _storage_end) {
			reallocate(size());
		}
	}

	// Modifiers

	CONSTEXPR_VOID
	clear() noexcept {
		_data_end = _data;
	}

	CONSTEXPR_VOID
	push_back(const T& value) { // r-value reference push_back(T&&) not needed for trivial types
		if (_data_end == _storage_end) {
			reserve(std::max<size_type>(capacity() * grow_factor, min_alloc_size));
		}

		*_data_end = value;
		++_data_end;
	}

	template<class... Args>
	CONSTEXPR_VOID
	emplace_back(Args&& ... args) {
		push_back(T{args...});
	}

	CONSTEXPR_VOID
	pop_back() noexcept {
		assert(!empty() && "Container is empty");
		--_data_end;
	}

	CONSTEXPR_VOID
	resize(size_type count) {
		reserve(count);
		_data_end = _data + count;
	}

	CONSTEXPR_VOID
	resize(size_type count, const value_type& default_value) {
		reserve(count);
		auto* new_data_end = _data + count;
		if (new_data_end > _data_end) {
			std::fill(_data_end, new_data_end, default_value);
		}
		_data_end = new_data_end;
	}

	static constexpr size_type grow_factor = 2;
	static constexpr size_type min_alloc_size = 4;

private:

	CONSTEXPR_VOID
	reallocate(const size_type new_capacity) {
		#if BIGINT_ENABLE_FAST_VECTOR_STATS
			++get_stats().realloc_cnt;
		#endif
		if(new_capacity > PTRDIFF_MAX) {
			throw std::length_error("fast_vector::reallocate");
		}
		const auto count = size();
		_data = static_cast<T* >(std::realloc(_data, sizeof(T) * new_capacity));
		assert(_data != nullptr && "Reallocation failed");
		_data_end = _data + count;
		_storage_end = _data + new_capacity;
	}


#if BIGINT_ENABLE_FAST_VECTOR_STATS
	struct stats_holder {
		fast_vector_statistics _stats;

		~stats_holder() {
			print_statistics();
		}

		CONSTEXPR_VOID
		print_statistics() const {
			std::cout << "========================================" << std::endl;
			std::cout << _stats;
			std::cout << "========================================" << std::endl;
		}
	};

	[[nodiscard]]
	static constexpr auto get_stats() -> fast_vector_statistics& {
		static thread_local stats_holder stats;
		return stats._stats;
	}
#endif

private:
	T* _data = nullptr;
	T* _data_end = nullptr;
	T* _storage_end = nullptr;

	template<_private::is_trivial T2>
	friend CONSTEXPR_VOID
	swap(fast_vector<T2>& a, fast_vector<T2>& b) noexcept;
};


template<_private::is_trivial T>
CONSTEXPR_VOID
swap(fast_vector<T>& a, fast_vector<T>& b) noexcept {
	#if BIGINT_ENABLE_FAST_VECTOR_STATS
		++fast_vector<T>::get_stats().swapped_cnt;
	#endif
	std::swap(a._data, b._data);
	std::swap(a._data_end, b._data_end);
	std::swap(a._storage_end, b._storage_end);
}

template<_private::is_trivial T>
CONSTEXPR_AUTO
operator==(const fast_vector<T>& a, const fast_vector<T>& b) noexcept -> bool {
	if (a.size() != b.size()) {
		return false;
	}
	const T* bi = b.begin();
	for (const auto ai: a) {
		if (ai != *bi) {
			return false;
		}
		++bi;
	}
	return true;
}

}

#undef FAST_VEC_DEFINE_ITERATORS
#undef FAST_VEC_DEFINE_FWD_ITERATORS
