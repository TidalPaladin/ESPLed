#ifndef __ESP_LED_LOOKUP_H__
#define __ESP_LED_LOOKUP_H__

#include <array>

/**
 * @tparam T The type of the calculated value
 * @tparam N The number of elements in the lookup table
 */
template <typename T, size_t N>
class ESPLookupTable {

public:


    constexpr ESPLookupTable(const T &val) : value(val) {  }

    template <typename...Args>
    constexpr ESPLookupTable(Args&&...args) : value(T(std::forward<Args>(args)...)) { 
        static_assert(sizeof...(Args) <= N, "Too many arguments supplied when initializing ESPLookupTable");
    }

    constexpr const T &operator[](size_t i) const {
        return value[i][2];
    }

    // constexpr T &operator[](size_t i){
    //     return value[i][2];
    // }
    
protected:

    typedef std::array<std::array<T, 2>, N> array_t;
    array_t value;

private:

    

};


template <size_t N>
class ESPSineLookupTable : public ESPLookupTable<double, N> {

public:

    typedef typename ESPLookupTable<double, N>::array_t array_t;

    constexpr ESPSineLookupTable() 
    :
    ESPLookupTable<double, N>( assemble() )
    {

    }

    static constexpr double f(size_t i) const {
        const double THETA_RADS = (double)i * TWO_PI / 360; 
        return sin(THETA_RADS);
    }

};





#endif