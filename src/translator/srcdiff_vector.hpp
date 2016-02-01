#ifndef INCLUDED_SRCDIFF_VECTOR
#define INCLUDED_SRCDIFF_VECTOR

#include <vector>

template <typename T>
class srcdiff_vector {

protected:
	std::vector<T> vec;

public:

	typedef typename std::vector<T>::size_type size_type;
	typedef typename std::vector<T>::iterator iterator;
	typedef typename std::vector<T>::const_iterator const_iterator;

	srcdiff_vector() {}
	srcdiff_vector(const srcdiff_vector & srcdiff_vec) : vec(srcdiff_vec.vec) {}

	srcdiff_vector & operator=(srcdiff_vector srcdiff_vec) {

		std::swap(vec, srcdiff_vec.vec);

		return *this;

	}

	T & at(size_type pos) {

		return vec.at(pos);

	}

	const T & at(size_type pos) const {

		return vec.at(pos);

	}

	T & operator[](size_type pos) {

		return vec[pos];

	}

	const T & operator[](size_type pos) const {

		return vec[pos];

	}

	T & front() {

		return vec.front();

	}

	const T & front() const {

		return vec.front();

	}

	T & back() {

		return vec.back();

	}

	const T & back() const {

		return vec.back();

	}

	const T * data() const {

		return vec.data();

	}

	iterator begin() {

		return vec.begin();

	}

	const_iterator begin() const {

		return vec.begin();

	}

	iterator end() {

		return vec.end();

	}

	const_iterator end() const {

		return vec.end();

	}

	bool empty() const {

		return vec.empty();

	}

	size_type size() const {

		return vec.size();

	}

	void push_back(T t) {

		vec.push_back(t);

	}

	template<class... Args>
	void emplace_back(Args &&... args) {

		vec.template emplace_back<Args...>(args...);

	}

};

#endif
