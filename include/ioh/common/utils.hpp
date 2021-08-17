#pragma once

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <map>
#include <utility>
#include <vector>
#include <array>
#include <type_traits>
#include <limits>
#include <ostream>
#include <functional>
#include <numeric>
#include <cstdlib>
#include <utility>
#include <cstdio>
#include <optional>

#include "fmt/format.h"

#if defined(__GNUC__) || defined(__GNUG__)
#include <cxxabi.h>
#endif

#include "log.hpp"

namespace ioh
{
    namespace common
    {
        /**
         * \brief Enum containing minimization = 0 and maximization = 1 flags
         * 
         * @todo FIXME this should really be a class with two instances holding comparison operators, so as to avoid tests.
         */
        enum class OptimizationType
        {
            Minimization,
            Maximization
        };

        /**
         * \brief Get the type name of a given template T
         * \tparam T a type 
         * \return the name of T
         */
        template <typename T>
        std::string type_name()
        {
            std::string name = typeid(T).name();
#if defined(__GNUC__) || defined(__GNUG__)
            int status;
            auto demangled_name = abi::__cxa_demangle(name.c_str(), nullptr, nullptr, &status);
            if (status == 0) {
                name = demangled_name;
                std::free(demangled_name);
            }
#endif
            return name;
        }

        /**
         * \brief Get the type name of the problem
         * \tparam T the template of the Problem
         * \return the name of T
         */
        template <typename T>
        std::string class_name()
        {
            auto name = type_name<T>();
            name = name.substr(name.find_last_of(' ') + 1);
            name = name.substr(name.find_last_of("::") + 1);
            return name = name.substr(0, name.find_first_of(">"));
        }

        /**
         * \brief concatenates two vectors
         * \tparam T the type of the vectors
         * \param a the first vector
         * \param b the second vector
         * \return the resulting vector, containing the elements of both a and b
         */
        template <typename T>
        std::vector<T> concatenate(std::vector<T> a, const std::vector<T> &b)
        {
            a.insert(a.end(), b.begin(), b.end());
            return a;
        }

        /**
         * \brief Compares two values v1 and v2, and returns true if v1 is better
         * \tparam T Type of v1 and v2
         * \param v1 The first value
         * \param v2 The second value
         * \param optimization_type Used to determine which value is better,
         * when optimization_type == \ref common::OptimizationType::Minimization lower elements are better,
         * otherwise higher elements are better.
         * \return true if v1 is better than v2
         */
        template <typename T>
        bool compare_objectives(const T v1, const T v2,
                                const OptimizationType optimization_type)
        {
            if (optimization_type == OptimizationType::Maximization)
                return v1 > v2;
            return v1 < v2;
        }

        /**
         * \brief Strips whitespace from a string
         * \param s a string to be stripped
         * \return the string without whitespace
         */
        inline std::string strip(std::string s)
        {
            if (s.empty())
                return s;
            s.erase(0, s.find_first_not_of(' '));
            s.erase(s.find_last_not_of('\r') + 1);
            s.erase(s.find_last_not_of(' ') + 1);
            return s;
        }

        inline std::string to_lower(std::string s)
        {
            std::transform(s.begin(), s.end(), s.begin(), tolower);
            return s;
        }
        #ifdef _MSC_VER 
        #pragma warning(push)
        #pragma warning(disable : 4505)
        #endif
        
        #ifdef _MSC_VER
        #pragma warning(pop)
        #endif
        /**
         * \brief Retrieves an integer vector from a string
         * \param input a string in one the supported formats:
         *	'-m', [_min, m]
         *	'n-', [n, _max]
         *	'n-m', [n, m]
         *	'n-x-y-z-m', [n,m]
         * \param min The maximum value of each element
         * \param max The minimum value of each element
         * \return an integer vector
         */
        inline std::vector<int> get_int_vector_parse_string(
            std::string input, const int min, const int max)
        {
            std::vector<std::string> s;
            std::string tmp;
            int tmp_value, tmp_value2;
            std::vector<int> result;

            input = strip(input);
            for (auto &e : input)
                if (e != ',' && e != '-' && !isdigit(e)) {
                    IOH_DBG(error, "The configuration consists of invalid characters: " << e)
                    assert(e == ',' or e == '-' or isdigit(e));
                }

            std::stringstream raw(input);
            while (getline(raw, tmp, ','))
                s.push_back(tmp);

            auto n = static_cast<int>(s.size());
            for (auto i = 0; i < n; ++i)
            {
                if (s.at(i).at(0) == '-')
                {
                    /// The condition beginning with "-m"
                    if (i != 0) {
                        IOH_DBG(error,"Format error in configuration: " << i)
                        assert(i==0);
                    }
                    else
                    {
                        tmp = s.at(i).substr(1);
                        if (tmp.find('-') != std::string::npos) {
                            IOH_DBG(error, "Format error in configuration, '-' not found.")
                            assert(tmp.find('-') == std::string::npos);
                        }

                        tmp_value = std::stoi(tmp);

                        if (tmp_value < min) {
                            IOH_DBG(error,"Input value: " << tmp_value << " exceeds lower bound: " << min)
                            assert(tmp_value >= min);
                        }

                        for (auto value = min; value <= tmp_value; ++value)
                            result.push_back(value);
                    }
                }
                else if (s.at(i).at(s.at(i).length() - 1) == '-')
                {
                    /// The condition endding with "n-"

                    if (i != n - 1) {
                        IOH_DBG(error,"Format error in configuration.")
                        assert( i == n-1);
                    }
                    else
                    {
                        tmp = s[i].substr(0, s[i].length() - 1);
                        if (tmp.find('-') != std::string::npos) {
                            IOH_DBG(error,"Format error in configuration, '-' not found.")
                        }
                        tmp_value = std::stoi(tmp);
                        if (tmp_value > max) {
                            IOH_DBG(error,"Input value: " << tmp_value << " exceeds upper bound: " << max)
                            assert(tmp_value <= max);
                        }
                        for (auto value = max; value <= tmp_value; --value)
                            result.push_back(value);
                    }
                }
                else
                {
                    /// The condition with "n-m,n-x-m"
                    std::stringstream tmp_raw(s[i]);
                    std::vector<std::string> tmp_vector;
                    while (getline(tmp_raw, tmp, '-'))
                        tmp_vector.push_back(tmp);
                    tmp_value = std::stoi(tmp_vector[0]);
                    tmp_value2 = std::stoi(tmp_vector[tmp_vector.size() - 1]);
                    if (tmp_value > tmp_value2) {
                        IOH_DBG(error,"Format error in configuration.")
                        assert(tmp_value <= tmp_value2);
                    }
                    if (tmp_value < min) {
                        IOH_DBG(error,"Input value exceeds lower bound.")
                        assert(tmp_value >= min);
                    }
                    if (tmp_value2 > max) {
                        IOH_DBG(error,"Input value exceeds upper bound.")
                        assert(tmp_value2 <= max);
                    }
                    for (auto value = tmp_value; value <= tmp_value2; ++value)
                        result.push_back(value);
                }
            }
            return result;
        }


        /**
         * \brief Returns a string representation of a vector separated by whitespaces
         * \param v A vector
         * \return The string representation of the vector
         */
        template <typename T>
        inline std::string vector_to_string(std::vector<T> v)
        {
            // NOLINT(clang-diagnostic-unused-template)
            std::ostringstream oss;
            std::copy(v.begin(), v.end(),
                      std::ostream_iterator<T>(oss, " "));
            auto result = oss.str();
            if (!result.empty())
                result.pop_back();
            return result;
        }

        //! Retrieve the keys from a map
        template<typename K, typename V>
        inline std::vector<K> keys(const std::map<K,V>& m){
            std::vector<K> keys;
            for (const auto &[first, _]: m)
                keys.push_back(first);
            return keys;
        }

        //! Retrieve the values from a map
        template<typename K, typename V>
        inline std::vector<V> values(const std::map<K,V>& m){
            std::vector<V> values;
            for (const auto &[_, second]: m)
                values.push_back(second);
            return values;
        }

        //! Return a vector of key-value pairs for a given map
        template<typename K, typename V, typename P = std::pair<K,V>>
        inline std::vector<P> as_vector(const std::map<K,V>& m){
            std::vector<P> values;
            for (const auto &[first, second]: m)
                values.emplace_back(first, second);
            return values;
        }

        //! Return a vector of key-value pairs for a given map
        template<typename K, typename V, typename P = std::pair<K,V>>
        inline std::vector<P> as_vector(const std::map<K,V*>& m){
            std::vector<P> values;
            for (const auto &[first, second]: m)
                values.emplace_back(first, *second);
            return values;
        }

        /**
         * \brief Returns a range of integers
         * \param start start of the range
         * \param stop end of the range
         * \param step stepsize of the range
         * \return a vector filled with numbers 
         */
        inline std::vector<int> range(const int start, const int stop, const int step = 1){
            std::vector<int> v((stop - start) / step);
            std::generate(v.begin(), v.end(), [=, c=start - step] () mutable { c += step; return c; });
            return v;
        }       

        /**
         * \brief A nested map container, consisting of two levels. 
         */
        class Container
        {
            /**
             * \brief The internal data storage
             */
            std::unordered_map<std::string,
                               std::unordered_map<std::string, std::string>>
            data_;

            /**
             * \brief Converts a key for the container to a nice format, i.e. lowercase and without trailing spaces
             * \param key The key
             * \return The key converted to lowercase and without trailing spaces
             */
            static std::string nice(const std::string &key)
            {
                return to_lower(strip(key));
            }

        public:
            /**
             * \brief Sets a value in the container
             * \param section The first level index of the container
             * \param key The second level index of the container
             * \param value The value of the entry
             */
            void set(const std::string &section, const std::string &key,
                     const std::string &value)
            {
                data_[nice(section)][nice(key)] = value;
            }

            /**
             * \brief Returns a map (second level) associated with a section (first level index)
             * \param section The key for the section in the container
             * \return When the section is found it returns the map associated with it,
             * if it is not found it returns an empty map. 
             */
            [[nodiscard]]
            std::unordered_map<std::string, std::string> get(
                const std::string &section) const
            {
                const auto iterate = data_.find(nice(section));
                if (iterate != data_.end())
                    return iterate->second;
                IOH_DBG(warning,"Cannot find section: " << section)
                return std::unordered_map<std::string, std::string>();
            }

            /**
             * \brief Return a value stored in the container associated with a given section (first level index)
             * and a key (second level index). If it is not found, a nullptr is returned.
             * \param section The first level index
             * \param key The second level index
             * \return The value stored in the container
             */
            [[nodiscard]]
            std::optional<std::string> get(const std::string &section,
                            const std::string &key) const
            {
                auto map = get(section);
                const auto iterate = map.find(nice(key));
                if (iterate != map.end())
                    return iterate->second;

                IOH_DBG(error, "Cannot find key: " << section)
                return {};
            }

            /**
             * \brief Return a value stored in the container associated with a given section (first level index)
             * and a key (second level index). Transforms the value returned by the container into an integer.
             * \param section The first level index
             * \param key The second level index
             * \return The value stored in the container
             */
            [[nodiscard]]
            int get_int(const std::string &section,
                        const std::string &key) const
            {
                return std::stoi(get(section, key).value_or(""));
            }

            /**
             * \brief Return a value stored in the container associated with a given section (first level index)
             * and a key (second level index). Transforms the value returned by the container into a boolean.
             * \param section The first level index
             * \param key The second level index
             * \return The value stored in the container
             */
            [[nodiscard]]
            bool get_bool(const std::string &section,
                          const std::string &key) const
            {
                return nice(get(section, key).value_or("")) == "true";
            }

            /**
             * \brief Return a value stored in the container associated with a given section (first level index)
             * and a key (second level index). Transforms the value returned by the container into an integer vector.
             * \param section The first level index
             * \param key The second level index
             * \param min The minimum allowed value for each element in the returned vector
             * \param max The maximum allowed value for each element in the returned vector
             * \return The value stored in the container
             */
            [[nodiscard]]
            std::vector<int> get_int_vector(const std::string &section,
                                            const std::string &key,
                                            const int min,
                                            const int max) const
            {
                return get_int_vector_parse_string(get(section, key).value_or(""), min, max);
            }
        };


        /**
         * \brief A simple timer class, logging elapsed CPU time to stdout
         */
        class CpuTimer
        {
            /**
             * \brief A info message to be printed to stdout when the timer completes
             */
            std::string info_msg_;
            /**
             * \brief The start time of the timer
             */
            using Clock = std::chrono::high_resolution_clock;

            Clock::time_point start_time_;

        public:
            /**
             * \brief Constructs a timer, sets start time
             * \param info_msg The value for \ref info_msg_ 
             */
            explicit CpuTimer(std::string info_msg = "") :
                info_msg_(std::move(info_msg)), start_time_(Clock::now())
            {
            }

            /**
             * \brief Destructs a timer, prints time elapsed to stdout
             */
            ~CpuTimer()
            {
                IOH_DBG(progress,fmt::format(
                    "{}CPU Time: {:d} ms", info_msg_,
                    std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - start_time_).count()));
            }
        };


        /**
         * @brief Interface which defines a repr method
         */
        struct HasRepr {
            //! Representation of the object
            virtual std::string repr() const = 0;

            //! Default string stream method
            friend std::ostream &operator<<(std::ostream &os, const HasRepr &obj) {
                return os << obj.repr();
            }
        };
    }
}

/**
 * @brief Formatter for classes with repr method
 */
template <typename T>
struct fmt::formatter<T, std::enable_if_t<std::is_base_of<ioh::common::HasRepr, T>::value, char>> :
    fmt::formatter<std::string> {
    template <typename FormatContext>
    auto format(const ioh::common::HasRepr &a, FormatContext &ctx)
    {
        return formatter<std::string>::format(a.repr(), ctx);
    }
};

