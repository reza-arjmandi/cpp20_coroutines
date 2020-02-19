#pragma once

#include "generator.hpp"

#include <experimental/coroutine>
#include <type_traits>
#include <utility>
#include <cassert>
#include <functional>

template<typename T>
class recursive_generator;

template<typename T>
class recursive_generator_promise final
{
public:

	recursive_generator_promise() noexcept
		: m_value(nullptr)
		, m_exception(nullptr)
		, m_root(this)
		, m_parentOrLeaf(this)
	{}

	recursive_generator_promise(const recursive_generator_promise&) = delete;
	recursive_generator_promise(recursive_generator_promise&&) = delete;

	auto get_return_object() noexcept
	{
		return recursive_generator<T>{ *this };
	}

	std::experimental::suspend_always initial_suspend() noexcept
	{
		return {};
	}

	std::experimental::suspend_always final_suspend() noexcept
	{
		return {};
	}

	void unhandled_exception() noexcept
	{
		m_exception = std::current_exception();
	}

	void return_void() noexcept {}

	std::experimental::suspend_always yield_value(T& value) noexcept
	{
		m_value = std::addressof(value);
		return {};
	}

	std::experimental::suspend_always yield_value(T&& value) noexcept
	{
		m_value = std::addressof(value);
		return {};
	}

	auto yield_value(recursive_generator<T>&& generator) noexcept
	{
		return yield_value(generator);
	}

	auto yield_value(recursive_generator<T>& generator) noexcept
	{
		struct awaitable
		{

			awaitable(recursive_generator_promise<T>* childPromise)
				: m_childPromise(childPromise)
			{}

			bool await_ready() noexcept
			{
				return this->m_childPromise == nullptr;
			}

			void await_suspend(std::experimental::coroutine_handle<recursive_generator_promise<T>>) noexcept
			{}

			void await_resume()
			{
				if (this->m_childPromise != nullptr)
				{
					this->m_childPromise->throw_if_exception();
				}
			}

		private:
			recursive_generator_promise<T>* m_childPromise;
		};

		if (generator.m_promise != nullptr)
		{
			m_root->m_parentOrLeaf = generator.m_promise;
			generator.m_promise->m_root = m_root;
			generator.m_promise->m_parentOrLeaf = this;
			generator.m_promise->resume();

			if (!generator.m_promise->is_complete())
			{
				return awaitable{ generator.m_promise };
			}

			m_root->m_parentOrLeaf = this;
		}

		return awaitable{ nullptr };
	}

	// Don't allow any use of 'co_await' inside the recursive_generator coroutine.
	template<typename U>
	std::experimental::suspend_never await_transform(U&& value) = delete;

	void destroy() noexcept
	{
		std::experimental::coroutine_handle<recursive_generator_promise<T>>::from_promise(*this).destroy();
	}

	void throw_if_exception()
	{
		if (m_exception != nullptr)
		{
			std::rethrow_exception(std::move(m_exception));
		}
	}

	bool is_complete() noexcept
	{
		return std::experimental::coroutine_handle<recursive_generator_promise<T>>::from_promise(*this).done();
	}

	T& value() noexcept
	{
		assert(this == m_root);
		assert(!is_complete());
		return *(m_parentOrLeaf->m_value);
	}

	void pull() noexcept
	{
		assert(this == m_root);
		assert(!m_parentOrLeaf->is_complete());

		m_parentOrLeaf->resume();

		while (m_parentOrLeaf != this && m_parentOrLeaf->is_complete())
		{
			m_parentOrLeaf = m_parentOrLeaf->m_parentOrLeaf;
			m_parentOrLeaf->resume();
		}
	}

private:

	void resume() noexcept
	{
		std::experimental::coroutine_handle<recursive_generator_promise<T>>::from_promise(*this).resume();
	}

	std::add_pointer_t<T> m_value;
	std::exception_ptr m_exception;

	recursive_generator_promise<T>* m_root;

	// If this is the promise of the root generator then this field
	// is a pointer to the leaf promise.
	// For non-root generators this is a pointer to the parent promise.
	recursive_generator_promise<T>* m_parentOrLeaf;

};

template<typename T>
class recursive_generator_iterator
{
public:

	using iterator_category = std::input_iterator_tag;
	// What type should we use for counting elements of a potentially infinite sequence?
	using difference_type = std::ptrdiff_t;
	using value_type = std::remove_reference_t<T>;
	using reference = std::conditional_t<std::is_reference_v<T>, T, T&>;
	using pointer = std::add_pointer_t<T>;

	recursive_generator_iterator() noexcept
		: m_promise(nullptr)
	{}

	explicit recursive_generator_iterator(recursive_generator_promise<T>* promise) noexcept
		: m_promise(promise)
	{}

	bool operator==(const recursive_generator_iterator& other) const noexcept
	{
		return m_promise == other.m_promise;
	}

	bool operator!=(const recursive_generator_iterator& other) const noexcept
	{
		return m_promise != other.m_promise;
	}

	recursive_generator_iterator& operator++()
	{
		assert(m_promise != nullptr);
		assert(!m_promise->is_complete());
		m_promise->pull();
		if (m_promise->is_complete())
		{
			auto* temp = m_promise;
			m_promise = nullptr;
			temp->throw_if_exception();
		}
		return *this;
	}

	void operator++(int)
	{
		(void)operator++();
	}

	reference operator*() const noexcept
	{
		assert(m_promise != nullptr);
		return static_cast<reference>(m_promise->value());
	}

	pointer operator->() const noexcept
	{
		return std::addressof(operator*());
	}

private:

	recursive_generator_promise<T>* m_promise;

};

template<typename T>
class recursive_generator
{
public:

	using promise_type = recursive_generator_promise<T>;

	recursive_generator() noexcept
		: m_promise(nullptr)
	{}

	recursive_generator(recursive_generator_promise<T>& promise) noexcept
		: m_promise(&promise)
	{}

	recursive_generator(recursive_generator&& other) noexcept
		: m_promise(other.m_promise)
	{
		other.m_promise = nullptr;
	}

	recursive_generator(const recursive_generator& other) = delete;
	recursive_generator& operator=(const recursive_generator& other) = delete;

	~recursive_generator()
	{
		if (m_promise != nullptr)
		{
			m_promise->destroy();
		}
	}

	recursive_generator& operator=(recursive_generator&& other) noexcept
	{
		if (this != &other)
		{
			if (m_promise != nullptr)
			{
				m_promise->destroy();
			}

			m_promise = other.m_promise;
			other.m_promise = nullptr;
		}
		return *this;
	}

	recursive_generator_iterator<T> begin()
	{
		if (m_promise != nullptr)
		{
			m_promise->pull();
			if (!m_promise->is_complete())
			{
				return recursive_generator_iterator<T>(m_promise);
			}
			m_promise->throw_if_exception();
		}
		return recursive_generator_iterator<T>(nullptr);
	}

	recursive_generator_iterator<T> end() noexcept
	{
		return recursive_generator_iterator<T>(nullptr);
	}

	void swap(recursive_generator& other) noexcept
	{
		std::swap(m_promise, other.m_promise);
	}

private:

	friend class recursive_generator_promise<T>;

	recursive_generator_promise<T>* m_promise;

};

template<typename T>
void swap(recursive_generator<T>& a, recursive_generator<T>& b) noexcept
{
	a.swap(b);
}