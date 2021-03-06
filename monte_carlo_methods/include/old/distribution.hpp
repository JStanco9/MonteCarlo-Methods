//created by John Stanco on 5/18/18

#include <utility>
#include <vector>

#ifndef distribution_h
#define distribution_h

//needs to be made faster!

template<class T>
class distribution{
protected:
	int len;
	std::vector<std::pair<T*, double> > dist;
public:
	distribution();
	distribution(std::vector<T*>&);
	distribution(std::vector<T>&);

	const std::vector<std::pair<T*, double> >& getData() const;
	const T& data(unsigned) const;
	const double& prob(unsigned) const;
	const unsigned int size() const;
	double expVal(const double (*f)(const T&));
};

#endif /* distribution_h */


template<class T>
inline distribution<T>::distribution() : len(0) {}


template<class T>
bool compare_p(T *a, T *b){
	return *a > *b;
}

template<class T>
bool compare(T a, T b){
	return a > b;
}



template<class T>
inline distribution<T>::distribution(std::vector<T*> &chain) : len(0){
	int chainLen = chain.size();
	std::sort(chain.begin(), chain.end(), compare_p<T>);
	unsigned int i = 0;
	while(i < chainLen){
		double acc = 0;
		T *tmp = chain[i];
		while(i < chainLen && *tmp == *chain[i]){
			acc++;
			i++;
		}
		std::pair<T*, double> x(tmp, acc / chainLen);
		this->dist.push_back(x);
		this->len++;
		//printf("%d", this->len);
	}
}


template<class T>
inline distribution<T>::distribution(std::vector<T> &chain) : len(0){
	int chainLen = chain.size();
	std::sort(chain.begin(), chain.end(), compare<T>);
	unsigned int i = 0;
	while(i < chainLen){
		printf("%d\n\n", i);
		double acc = 0;
		T tmp = chain[i];
		while(i < chainLen && tmp == chain[i]){
			acc++;
			i++;
		}

		std::pair<T*, double> x(new T(tmp), acc / chainLen);
		this->dist.push_back(x);
		this->len++;
	}
}


template<class T>
inline double
distribution<T>::expVal(const double (*f)(const T&)){
	double acc = 0;
	for(unsigned i = 0; i < this->len; i++){
		acc += f(this->data(i)) * this->prob(i);
	}
	return acc;
}


template<class T>
inline const T&
distribution<T>::data(unsigned int i) const {
	return *(dist[i].first);
}


template<class T>
inline const double&
distribution<T>::prob(unsigned int i) const {
	return dist[i].second;
}


template<class T>
inline const std::vector<std::pair<T*, double> >& 
distribution<T>::getData() const {
	return this->dist;
}

template<class T>
inline const unsigned int
distribution<T>::size() const {
	return this->len;
}

//visualization of distribution?
//density of states?