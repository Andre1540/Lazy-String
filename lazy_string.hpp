//
// Created by andrey on 02.03.17.
//

#ifndef LAZY_STRING_H
#define LAZY_STRING_H

#include <memory>
namespace std_utils {

    struct bad_index : std::exception { };
    template <typename, typename> class lazy_basic_string;

    template <typename T, typename S>
    bool operator==(const T * l, const lazy_basic_string<T, S> & r)
    {
        return (r == l);
    }

    template <typename T, typename S>
    bool operator<(const T * l, const lazy_basic_string<T, S> & r)
    {
        return (!(r < l) && !(r == l));
    }

    template <typename T, typename S>
    bool operator!=(const T * l, const lazy_basic_string<T, S> & r)
    {
        return r != l;
    }

    template <typename T, typename S>
    const lazy_basic_string<T, S> operator+(const T * l, const lazy_basic_string<T, S> & r)
    {
        return r.addFromLeftStar(l);
    }

    template <typename T, typename S>
    const lazy_basic_string<T, S> operator+(const T l, const lazy_basic_string<T, S> & r)
    {
        return r.addFromLeft(l);
    }

    template <typename T, typename S>
    void swap(lazy_basic_string<T, S> & l, lazy_basic_string<T, S> & r)
    {
        l.swap(r);
    }

    template <typename T>
    struct array_deleter {
        void operator() (T const * p)
        {
            delete [] p;
        }
    };

    template <typename CharT, typename Traits = std::char_traits<CharT>>
    class lazy_basic_string {
    public:

        lazy_basic_string (lazy_basic_string const & c)
                : data_(c.data_)
                , size_(c.size_)
        { }

        lazy_basic_string ()
                : data_(std::shared_ptr<CharT> (new CharT()))
                , size_(0)
        { }

        lazy_basic_string (CharT const * c)
        {
            size_ = Traits::length(c);
            data_ = std::shared_ptr<CharT> (new CharT[size_ + 1], array_deleter<CharT>());
            Traits::copy(data_.get(), c, size_ + 1);
        }

        lazy_basic_string (size_t n, const CharT & a)
                : data_(std::shared_ptr<CharT>(new CharT[n + 1], array_deleter<CharT>()))
                , size_(n)
        {
            Traits::assign(data_.get(), n, a);
            Traits::assign(data_.get() + n, 1, CharT());
        }

        ~lazy_basic_string()
        {
            clear();
        }

        friend std::ostream & operator<<(std::ostream & os, lazy_basic_string const & str)
        {
            return os << str.data_.get();
        }

        size_t size() const
        {
            return size_;
        }

        bool operator==(const lazy_basic_string & str) const
        {
            if (str.size() != size())
                return false;
            if (str.data_ == data_)
                return true;
            return (Traits::compare(data_.get(), str.data_.get(), str.size()) == 0);
        }

        bool operator==(const CharT * str) const
        {
            if (Traits::length(str) != size())
                return false;
            else
                return (Traits::compare(data_.get(), str, size()) == 0);
        }

        bool operator<(const lazy_basic_string & str) const
        {
            size_t r = str.size();
            if (r <= size())
                return (Traits::compare(data_.get(), str.data_.get(), r) < 0);
            else
                return (Traits::compare(data_.get(), str.data_.get(), size()) <= 0);
        }

        bool operator<(const CharT * str) const
        {
            size_t r = Traits::length(str);
            if (r <= size())
                return (Traits::compare(data_.get(), str, r) < 0);
            else
                return (Traits::compare(data_.get(), str, size()) <= 0);
        }

        bool operator!=(const lazy_basic_string & str) const
        {
            return !operator==(str);
        }

        bool operator!=(const CharT * str) const
        {
            return !operator==(str);
        }

        void clear()
        {
            data_ = std::shared_ptr<CharT> (new CharT());
            size_ = 0;
        }

        void swap(lazy_basic_string & r)
        {
            std::swap(data_, r.data_);
            std::swap(size_, r.size_);
        }

        CharT const * c_str() const
        {
            return data_.get();
        }

        bool empty() const
        {
            return !size();
        }

        lazy_basic_string& operator=(lazy_basic_string const & c)
        {
            data_ = c.data_;
            size_ = c.size();
            return *this;
        }

        lazy_basic_string& operator=(const CharT * c)
        {
            size_t length = Traits::length(c);
            data_ = std::shared_ptr<CharT> (new CharT[length + 1], array_deleter<CharT>());
            size_ = length;
            Traits::copy(data_.get(), c, size_ + 1);
            return *this;
        }


        lazy_basic_string operator+(const CharT * s2) const
        {
            size_t length1= size();
            size_t length2 = Traits::length(s2);
            size_t newLength = length1 + length2;
            std::shared_ptr<CharT> res (new CharT[newLength + 1], array_deleter<CharT>());
            Traits::copy(res.get(), data_.get(), length1);
            Traits::copy(res.get() + length1, s2, length2 + 1);
            lazy_basic_string result;
            result.data_ = res;
            result.size_ = newLength;
            return result;
        }

        lazy_basic_string operator+(const lazy_basic_string & s2) const
        {
            size_t length1= size();
            size_t length2 = s2.size();
            size_t newLength = length1 + length2;
            std::shared_ptr<CharT> res (new CharT[newLength + 1], array_deleter<CharT>());
            Traits::copy(res.get(), data_.get(), length1);
            Traits::copy(res.get() + length1, s2.data_.get(), length2 + 1);
            lazy_basic_string result;
            result.data_ = res;
            result.size_ = newLength;
            return result;
        }

        lazy_basic_string operator+(const CharT s2) const
        {
            size_t length1= size();
            size_t length2 = 1;
            size_t newLength = length1 + length2;
            std::shared_ptr<CharT> res(new CharT[newLength + 1], array_deleter<CharT>());
            Traits::copy(res.get(), data_.get(), length1);
            Traits::copy(res.get() + length1, &s2, length2);
            Traits::copy(res.get() + newLength, data_.get() + length1, 1);
            lazy_basic_string result;
            result.data_ = res;
            result.size_ = newLength;
            return result;
        }

        lazy_basic_string addFromLeftStar(const CharT * str) const
        {
            size_t currentLength = size();
            size_t lengthToAdd = Traits::length(str);
            size_t newLength = currentLength + lengthToAdd;
            std::shared_ptr<CharT> res (new CharT[newLength + 1], array_deleter<CharT>());
            Traits::copy(res.get(), str, lengthToAdd);
            Traits::copy(res.get() + lengthToAdd, data_.get(), currentLength + 1);
            lazy_basic_string result;
            result.data_ = res;
            result.size_ = newLength;
            return result;
        }

        lazy_basic_string addFromLeft(const CharT str) const
        {
            size_t length1 = size();
            size_t length2 = 1;
            size_t newLength = length1 + length2;
            std::shared_ptr<CharT> res (new CharT[newLength + 1], array_deleter<CharT>());
            Traits::copy(res.get(), &str, length2);
            Traits::copy(res.get() + length2, data_.get(), length1 + 1);
            lazy_basic_string result;
            result.data_ = res;
            result.size_ = newLength;
            return result;
        }

        lazy_basic_string & operator+=(const lazy_basic_string & str)
        {
            size_t currentLength = size();
            size_t lengthToAdd = str.size();
            size_t newLength = currentLength + lengthToAdd;
            std::shared_ptr<CharT> res (new CharT[newLength + 1], array_deleter<CharT>());
            Traits::copy(res.get(), data_.get(), currentLength);
            Traits::copy(res.get() + currentLength, str.data_.get(), lengthToAdd + 1);
            data_ = res;
            size_ = newLength;
            return *this;
        }

        lazy_basic_string & operator+=(const CharT * str)
        {
            size_t currentLength = size();
            size_t lengthToAdd = Traits::length(str);
            size_t newLength = currentLength + lengthToAdd;
            std::shared_ptr<CharT> res (new CharT[newLength + 1], array_deleter<CharT>());
            Traits::copy(res.get(), data_.get(), currentLength);
            Traits::copy(res.get() + currentLength, str, lengthToAdd + 1);
            data_ = res;
            size_ = newLength;
            return *this;
        }

        lazy_basic_string & operator+=(CharT const str)
        {
            size_t currentLength = size();
            size_t newLength = currentLength + 1;
            std::shared_ptr<CharT> res (new CharT[newLength + 1], array_deleter<CharT>());
            Traits::copy(res.get(), data_.get(), currentLength + 1);
            Traits::copy(res.get() + newLength, res.get() + currentLength, 1);
            Traits::copy(res.get() + currentLength, &str, 1);
            data_ = res;
            size_ = newLength;
            return *this;
        }


        class Proxy {
        public:
            Proxy(lazy_basic_string * str_ptr, size_t pos)
                    : str_ptr_(str_ptr)
                    , position_(pos)
            { }

            operator CharT()
            {
                if (position_ > str_ptr_->size())
                {
                    throw bad_index();
                }
                else
                    return *(str_ptr_->data_.get() + position_);
            }

            Proxy operator= (const CharT & value)
            {
                if (position_ > str_ptr_->size())
                {
                    throw bad_index();
                }
                if (str_ptr_->data_.use_count() == 1)
                {
                    *(str_ptr_->data_.get() + position_) = value;
                }
                if (str_ptr_->data_.use_count() != 1)
                {
                    size_t n = str_ptr_->size();
                    std::shared_ptr<CharT> res (new CharT[n + 1], array_deleter<CharT>());
                    Traits::copy(res.get(), str_ptr_->data_.get(), n + 1);
                    *(res.get() + position_) = value;
                    str_ptr_->data_ = res;
                }
                return *this;
            }

        private:
            lazy_basic_string *     str_ptr_;
            size_t                  position_;
        };

        Proxy operator[](size_t i)
        {
            return Proxy(this, i);
        }

        CharT operator[](size_t i) const
        {
            if (i > size())
            {
                throw bad_index();
            }
            return *(data_.get() + i);
        }

    private:
        std::shared_ptr<CharT>  data_;
        size_t                  size_;
    };


    template <typename CharT>
    struct char_itraits : public std::char_traits<CharT>
    {
        static int compare(CharT const * c1, CharT const * c2, size_t n)
        {
            for (size_t i = 0; i != n; ++i)
            {
                if (std::toupper(c1[i]) < std::toupper(c2[i])) return -1;
                if (std::toupper(c1[i]) > std::toupper(c2[i])) return 1;
            }
            return 0;
        }
    };

    typedef lazy_basic_string<char>                     lazy_string;
    typedef lazy_basic_string<wchar_t>                  lazy_wstring;
    typedef lazy_basic_string<char, char_itraits<char>> lazy_istring;
}

#endif // LAZY_STRING_H
