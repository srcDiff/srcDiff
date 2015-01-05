#ifndef INCLUDED_SRCDIFF_VECTOR
#define INCLUDED_SRCDIFF_VECTOR

#include <vector>

template <typename T>
class srcdiff_vector {

protected:
	std::vector<T> data;

public:

	typedef typename std::vector<T>::size_type size_type;
	typedef typename std::vector<T>::iterator iterator;
	typedef typename std::vector<T>::const_iterator const_iterator;

	srcdiff_vector() {}
	srcdiff_vector(const srcdiff_vector & vec) : data(vec.data) {}

	srcdiff_vector & operator=(srcdiff_vector vec) {

		std::swap(data, vec.data);

		return *this;

	}

	T & at(size_type pos) {

		return data.at(pos);

	}

	const T & at(size_type pos) const {

		return data.at(pos);

	}

	T & operator[](size_type pos) {

		return data[pos];

	}

	const T & operator[](size_type pos) const {

		return data[pos];

	}

	T & front() {

		return data.front();

	}

	const T & front() const {

		return data.front();

	}

	T & back() {

		return data.back();

	}

	const T & back() const {

		return data.back();

	}

	iterator begin() {

		return data.begin();

	}

	const_iterator begin() const {

		return data.begin();

	}

	iterator end() {

		return data.end();

	}



	const_iterator end() const {

		return data.end();

	}


	size_type size() const {

		return data.size();

	}

	void push_back(T t) {

		data.push_back(t);

	}

	template<class... Args>
	void emplace_back(Args &&... args) {

		data.template emplace_back<Args...>(args...);

	}

};

#endif
