//
// GameSencha, LLC 2/2/24.
//
#pragma once
#include <sympl/memory/managed_object.hpp>

SymplNamespaceStart

typedef unsigned char uchar;
#define SYMPL_STRING_BUFFER_CAPACITY 256

class SYMPL_API StringBuffer : public ManagedObject
{
	SYMPL_OBJECT(StringBuffer, ManagedObject)

private:
    /// Buffer for holding the string.
    uchar      *_buffer;

    /// Flag to switch to the dynamic buffer.
    bool        _use_dynamic_buffer = false;

    /// Current length of the string.
    size_t      _length = 0;
    /// Capacity for the string
    size_t      _capacity = SYMPL_STRING_BUFFER_CAPACITY;

    //! Initializes the string buffer.
    //! \param str
    //! \param capacity
    void init(const char *str, size_t capacity);

public:
    //! Constructor.
    //! \param str
    //! \param capacity
    StringBuffer(const char *str, size_t capacity);

    //! Constructor.
    //! \param str
    StringBuffer(const char *str);

    //! Constructor.
    StringBuffer();

    //! Called in place of the constructor.
    void __construct();

    //! Prepends a string to the current buffer.
    //! \param str
    void prepend(const char *str);

    //! Prepends a string to the current buffer.
    //! \param str
    void prepend_byte(const char byte);

    //! Appends a string to the current buffer.
    //! \param str
    void append(StringBuffer *str);

    //! Appends a string to the current buffer.
    //! \param str
    void append(const std::string str);

    //! Appends a string to the current buffer.
    //! \param str
    void append(const char *str);

    //! Appends a string to the current buffer.
    //! \param str
    void append_byte(const char byte);

    /**
     * Clears and appends the string.
     * @param str
     */
    void set_string(const char* str);

    //! Resizes the string buffer.
    //! \param newCapacity
    void resize_string(size_t newCapacity);

    //! Copy text from a given point.
    void replace_string_at(const char* str, size_t startIndex, size_t length = 0);

    //! Replaces occurances in the string.
    //! \param search
    //! \param replaceWith
    void replace_string(const char *search, const char *replaceWith);

    //! Replaces occurances in the string.
    //! \param search
    //! \param replaceWith
    void safe_replace_string(const char *search, const char *replaceWith);

    //! Attempts to find a single character in the string.
    //! \param search
    //! \return bool
    bool contains(const char search);

    //! Attempts to find a string in the buffer.
    //! \param search
    //! \return bool
    bool contains(const char* search);

    //! Checks if this is within a list.
    //! \param searchList
    //! \return
    bool is_within(const std::vector<const char*>& searchList);

    //! Returns a substr once it encounters the given string.
    //! \param c
    //! \return
    std::string substr_first_occurrence(const char c);

    //! Returns a substr once it encounters the given character.
    //! \param str
    //! \return
    std::string substr_first_occurrence(const char* str);

    //! Returns the index of the first occurence of a character.
    //! \param c
    //! \param startIndex
    //! \param limit
    //! \return long long
    long long first_occurrence(const char c, size_t startIndex = 0, size_t limit  = 0);

    //! Returns the index of the first occurence of a string.
    //! \param str
    //! \param startIndex
    //! \param limit
    //! \return long long
    long long first_occurrence(const char* str, size_t startIndex = 0, size_t limit = 0);

    //! Test peek to see if a string is a match.
    //! \param search
    //! \param startIndex
    //! \param endIndex
    //! \return bool
    bool peek_search(const char* search, size_t startIndex);

    //! Returns whether or not the buffer begins with a given string.
    //! \param search
    //! \return bool
    bool starts_with(const char* search);

    //! Returns whether or not the buffer ends with a given string.
    //! \param search
    //! \return bool
    bool ends_with(const char* search);

    //! Trim left whitespace.
    void ltrim();

    //! Trim right whitespace.
    void rtrim();

    //! Trims the beginning and end of whitespace.
    void trim();

    //! Clears out the string buffer.
    void clear();

    //! Clears the buffer from a given point to its length.
    void clear_at(size_t startIndex = 0);

    //! Releases the object.
    bool destroy();

    //! Returns the string buffer.
    //! \return uchar*
    inline const uchar* str() const { return _buffer; }

    //! Returns the byte value of the buffer.
    //! \return
    inline const char* cstr() const { return reinterpret_cast<char*>(_buffer); }

    //! Returns the length of the string.
    //! \return size_t
    inline size_t length() const { return _length; }

    //! Returns whether or not the string is empty.
    //! \return
    inline bool is_empty() const { return _length == 0; }

    //! Returns the capacity of the string.
    //! \return size_t
    inline size_t capacity() const { return _capacity; }

    //! Returns whether or not a string equals the buffer's string.
    //! \return bool
    inline bool equals(StringBuffer* buffer) { return equals(buffer->cstr()); }

    //! Returns whether or not a string equals the buffer's string.
    //! \return bool
    inline bool equals(const char* compare) { return (strcmp(cstr(), compare) == 0); }

    //! Returns the last byte in the buffer.
    //! \return char
    inline char first_byte() {
        if (_length == 0) {
            return '\0';
        }
        return reinterpret_cast<char*>(_buffer)[0];
    }

    //! Returns the last byte in the buffer.
    //! \return char
    inline char last_byte() {
        if (_length == 0) {
            return '\0';
        }
        return reinterpret_cast<char*>(_buffer)[_length - 1];
    }

    //! Sets a byte at a given location.
    //! \param location
    //! \param byte
    inline void set_byte(size_t location, const char byte) {
        if (location >= _capacity) {
            resize_string(_capacity + 5);
        }
        _buffer[location] = byte;
    }

    //! Returns the char at a given location.
    //! \param place
    //! \return
    inline const char get_char_at(size_t location) {
        if (location > _length) {
            return '\0';
        }
        return _buffer[location];
    }

    // Assignment operators.
    StringBuffer& operator=(const char* Str);
    StringBuffer& operator=(const StringBuffer& Str);
    StringBuffer& operator=(const SharedPtr<StringBuffer>& Str);
    StringBuffer& operator=(const std::string& Str);

    //! Evaluates whether two string buffers are equal.
    //! \return bool
    bool operator == (StringBuffer* rhs) {
        return equals(rhs->cstr());
    }

    //! Evaluates whether two string buffers are equal.
    //! \return bool
    bool operator == (const std::string& rhs) {
        return equals(rhs.c_str());
    }

    //! Evaluates whether two string buffers are equal.
    //! \return bool
    bool operator == (const char* rhs) {
        return equals(rhs);
    }
};

SymplNamespaceEnd