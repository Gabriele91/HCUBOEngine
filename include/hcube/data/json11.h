/* Copyright (c) 2013 Dropbox, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <initializer_list>
#include <hcube/config.h>

#ifdef _MSC_VER
    #if _MSC_VER <= 1800 // VS 2013
        #ifndef noexcept
            #define noexcept throw()
        #endif

        #ifndef snprintf
            #define snprintf _snprintf_s
        #endif
    #endif
#endif

namespace json_parse11
{
    
    enum json_parse
    {
        STANDARD, COMMENTS
    };
    
    class json_value;
    
    class HCUBE_API json final
    {
    public:
        // Types
        enum Type
        {
            NUL, NUMBER, BOOL, STRING, ARRAY, OBJECT
        };
        
        // Array and object typedefs
        typedef std::vector<json> array;
        typedef std::map<std::string, json> object;
        
        // Constructors for the various types of JSON value.
        json() noexcept;                // NUL
        json(std::nullptr_t) noexcept;  // NUL
        json(double value);             // NUMBER
        json(int value);                // NUMBER
        json(bool value);               // BOOL
        json(const std::string &value); // STRING
        json(std::string &&value);      // STRING
        json(const char * value);       // STRING
        json(const array &values);      // ARRAY
        json(array &&values);           // ARRAY
        json(const object &values);     // OBJECT
        json(object &&values);          // OBJECT
        
        // Implicit constructor: anything with a to_json() function.
        template <class T, class = decltype(&T::to_json)>
        json(const T & t) : json(t.to_json()) {}
        
        // Implicit constructor: map-like objects (std::map, std::unordered_map, etc)
        template <class M, typename std::enable_if<
        std::is_constructible<std::string, typename M::key_type>::value
        && std::is_constructible<json, typename M::mapped_type>::value,
        int>::type = 0>
        json(const M & m) : json(object(m.begin(), m.end())) {}
        
        // Implicit constructor: vector-like objects (std::list, std::vector, std::set, etc)
        template <class V, typename std::enable_if<
        std::is_constructible<json, typename V::value_type>::value,
        int>::type = 0>
        json(const V & v) : json(array(v.begin(), v.end())) {}
        
        // This prevents json(some_pointer) from accidentally producing a bool. Use
        // json(bool(some_pointer)) if that behavior is desired.
        json(void *) = delete;
        
        // Accessors
        Type type() const;
        
        bool is_null()   const { return type() == NUL; }
        bool is_number() const { return type() == NUMBER; }
        bool is_bool()   const { return type() == BOOL; }
        bool is_string() const { return type() == STRING; }
        bool is_array()  const { return type() == ARRAY; }
        bool is_object() const { return type() == OBJECT; }
        
        // Return the enclosed value if this is a number, 0 otherwise. Note that json11 does not
        // distinguish between integer and non-integer numbers - number_value() and int_value()
        // can both be applied to a NUMBER-typed object.
        double number_value() const;
        int int_value() const;
        
        // Return the enclosed value if this is a boolean, false otherwise.
        bool bool_value() const;
        // Return the enclosed string if this is a string, "" otherwise.
        const std::string &string_value() const;
        // Return the enclosed std::vector if this is an array, or an empty vector otherwise.
        const array &array_items() const;
        // Return the enclosed std::map if this is an object, or an empty map otherwise.
        const object &object_items() const;
        
        // Return a reference to arr[i] if this is an array, json() otherwise.
        const json & operator[](size_t i) const;
        // Return a reference to obj[key] if this is an object, json() otherwise.
        const json & operator[](const std::string &key) const;
        
        // Serialize.
        void dump(std::string &out) const;
        std::string dump() const
        {
            std::string out;
            dump(out);
            return out;
        }
        
        // Parse. If parse fails, return json() and assign an error message to err.
        static json parse(const std::string & in,
                          std::string & err,
                          json_parse strategy = json_parse::STANDARD);
        
        static json parse(const char * in,
                          std::string & err,
                          json_parse strategy = json_parse::STANDARD)
        {
            if (in)
            {
                return parse(std::string(in), err, strategy);
            }
            else
            {
                err = "null input";
                return nullptr;
            }
        }
        // Parse multiple objects, concatenated or separated by whitespace
        static std::vector<json> parse_multi(const std::string & in,
                                             std::string::size_type & parser_stop_pos,
                                             std::string & err,
                                             json_parse strategy = json_parse::STANDARD);
        
        static inline std::vector<json> parse_multi(const std::string & in,
                                                    std::string & err,
                                                    json_parse strategy = json_parse::STANDARD)
        {
            std::string::size_type parser_stop_pos;
            return parse_multi(in, parser_stop_pos, err, strategy);
        }
        
        bool operator== (const json &rhs) const;
        bool operator<  (const json &rhs) const;
        bool operator!= (const json &rhs) const { return !(*this == rhs); }
        bool operator<= (const json &rhs) const { return !(rhs < *this); }
        bool operator>  (const json &rhs) const { return  (rhs < *this); }
        bool operator>= (const json &rhs) const { return !(*this < rhs); }
        
        /* has_shape(types, err)
         *
         * Return true if this is a JSON object and, for each item in types, has a field of
         * the given type. If not, return false and set err to a descriptive message.
         */
        typedef std::initializer_list<std::pair<std::string, Type>> shape;
        bool has_shape(const shape & types, std::string & err) const;
        
    private:
        std::shared_ptr<json_value> m_ptr;
    };
    
    // Internal class hierarchy - json_value objects are not exposed to users of this API.
    class HCUBE_API json_value
    {
    protected:
        friend class json;
        friend class jsonInt;
        friend class jsonDouble;
        virtual json::Type type() const = 0;
        virtual bool equals(const json_value * other) const = 0;
        virtual bool less(const json_value * other) const = 0;
        virtual void dump(std::string &out) const = 0;
        virtual double number_value() const;
        virtual int int_value() const;
        virtual bool bool_value() const;
        virtual const std::string &string_value() const;
        virtual const json::array& array_items() const;
        virtual const json& operator[](size_t i) const;
        virtual const json::object& object_items() const;
        virtual const json& operator[](const std::string &key) const;
        virtual ~json_value() {}
    };
    
} // namespace json11