#pragma once

namespace hcube
{

	template <class T>
	class link
	{
	public:

		using link_t = link < T >;

		link()
		{

		}

		link(T&& obj)
		: m_object(std::move(obj))
		{
		}

		link(const T& obj)
		: m_object(obj)
		{
		}

		link(link_t&& ln)
		: m_object(ln.m_object)
		, m_next(ln.m_next)
		{
		}

		link(const link_t& ln)
		: m_object(std::move(ln.m_object))
		, m_next(std::move(ln.m_next))
		{
		}

		T& operator *()
		{
			return m_object;
		}
		
		T* operator ->()
		{
			return &m_object;
		}

		const T& get() const
		{
			return m_object;
		}

		link<T>* ref_to_link() 
		{
			return this;
		}

		const link<T>* ref_to_link() const
		{
			return this;
		}

		void set_next(link_t* next)
		{
			m_next = next;
		}

		link_t* get_next() const
		{
			return m_next;
		}

		void next_to_null()
		{
			m_next = nullptr;
		}

	protected:

		T       m_object;
		link_t* m_next{ nullptr };

	};



	template <class T>
	class linked_queue_lifo
	{
	public:

		class const_iterator
		{
		public:
			
			const_iterator& operator++() 
			{
				m_node = m_node->get_next();
				return *this; 
			}

			bool operator==(const const_iterator& rhs)
			{ 
				return m_node == rhs.m_node;
			}

			bool operator!=(const const_iterator& rhs)
			{
				return m_node != rhs.m_node;
			}

			const T& operator*() const
			{ 
				return m_node->get();
			}
			
			const_iterator(link< T >* in_node)
			{
				m_node = in_node;
			}

		private:

			link< T >* m_node;
		};
	
		link< T >* top() const
		{
			return m_queue_top;
		}

		void push(link< T >& ln)
		{
			push(ln.ref_to_link());
		}

		void push(link< T >* ln)
		{
			ln->set_next(m_queue_top);
			m_queue_top = ln;
			++m_size;
		}

		void clear()
		{
			m_queue_top = nullptr;
			m_size = 0;
		}

		const_iterator begin() const
		{
			return const_iterator(m_queue_top);
		}

		const_iterator end() const
		{
			return const_iterator(nullptr);
		}

		size_t size() const
		{
			return m_size;
		}

	protected:

		size_t		m_size{ 0 };
		link < T >* m_queue_top{ nullptr };
	};

	template <class T>
	class linked_queue_fifo
	{
	public:

		class const_iterator
		{
		public:

			const_iterator& operator++()
			{
				m_node = m_node->get_next();
				return *this;
			}

			bool operator==(const const_iterator& rhs)
			{
				return m_node == rhs.m_node;
			}

			bool operator!=(const const_iterator& rhs)
			{
				return m_node != rhs.m_node;
			}

			const T& operator*() const
			{
				return m_node->get();
			}
			
			const_iterator(link< T >* in_node)
			{
				m_node = in_node;
			}

		private:

			link< T >* m_node;
		};

		link< T >* top() const
		{
			return m_queue_top;
		}

		void push(link< T >& ln)
		{
			push(ln.ref_to_link());
		}

		void push(link< T >* ln)
		{
			if (m_queue_last)
			{
				m_queue_last->set_next(ln);
				m_queue_last = ln;
				m_queue_last->set_next(nullptr);
			}
			else
			{
				m_queue_top  = ln;
				m_queue_last = ln;
				m_queue_last->set_next(nullptr);
			}
			++m_size;
		}

		void clear()
		{
			m_queue_top  = nullptr;
			m_queue_last = nullptr;
			m_size = 0;
		}

		const_iterator begin() const
		{
			return const_iterator(m_queue_top);
		}

		const_iterator end() const
		{
			return const_iterator(nullptr);
		}
		
		size_t size() const
		{
			return m_size;
		}

	protected:

		size_t		m_size{ 0 };
		link < T >* m_queue_top{ nullptr };
		link < T >* m_queue_last{ nullptr };
	};
}