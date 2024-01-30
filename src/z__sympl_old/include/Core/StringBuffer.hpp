#pragma once
#include <__sympl_old/SymplPCH.hpp>
#include <__sympl_old/include/Memory/ManagedObject.hpp>

SymplNamespaceStart

typedef unsigned char uchar;
#define SYMPL_STRING_BUFFER_CAPACITY 256

class SYMPL_API StringBuffer : public ManagedObject
{
	SYMPL_OBJECT(StringBuffer, ManagedObject)

private:
    /// Buffer for holding the string.
    uchar      *_Buffer;

    /// Flag to switch to the dynamic buffer.
    bool        _UseDynamicBuffer = false;

    /// Current length of the string.
    size_t      _Length = 0;
    /// Capacity for the string
    size_t      _Capacity = SYMPL_STRING_BUFFER_CAPACITY;

    //! Initializes the string buffer.
    //! \param str
    //! \param capacity
    void Init(const char *str, size_t capacity);

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
    void __Construct();

    //! Prepends a string to the current buffer.
    //! \param str
    void Prepend(const char *str);

    //! Prepends a string to the current buffer.
    //! \param str
    void PrependByte(const char byte);

    //! Appends a string to the current buffer.
    //! \param str
    void Append(StringBuffer *str);

    //! Appends a string to the current buffer.
    //! \param str
    void Append(const std::string str);

    //! Appends a string to the current buffer.
    //! \param str
    void Append(const char *str);

    //! Appends a string to the current buffer.
    //! \param str
    void AppendByte(const char byte);

    /**
     * Clears and appends the string.
     * @param str
     */
    void Set(const char* str);

    //! Resizes the string buffer.
    //! \param newCapacity
    void Resize(size_t newCapacity);

    //! Copy text from a given point.
    void ReplaceStringAt(const char* str, size_t startIndex, size_t length = 0);

    //! Replaces occurances in the string.
    //! \param search
    //! \param replaceWith
    void Replace(const char *search, const char *replaceWith);

    //! Replaces occurances in the string.
    //! \param search
    //! \param replaceWith
    void Replace2(const char *search, const char *replaceWith);

    //! Attempts to find a single character in the string.
    //! \param search
    //! \return bool
    bool Contains(const char search);

    //! Attempts to find a string in the buffer.
    //! \param search
    //! \return bool
    bool Contains(const char* search);

    //! Checks if this is within a list.
    //! \param searchList
    //! \return
    bool IsWithin(const std::vector<const char*>& searchList);

    //! Returns a substr once it encounters the given string.
    //! \param c
    //! \return
    std::string SubstrFirstOccurrence(const char c);

    //! Returns a substr once it encounters the given character.
    //! \param str
    //! \return
    std::string SubstrFirstOccurrence(const char* str);

    //! Returns the index of the first occurence of a character.
    //! \param c
    //! \param startIndex
    //! \param limit
    //! \return long long
    long long FirstOccurrence(const char c, size_t startIndex = 0, size_t limit  = 0);

    //! Returns the index of the first occurence of a string.
    //! \param str
    //! \param startIndex
    //! \param limit
    //! \return long long
    long long FirstOccurrence(const char* str, size_t startIndex = 0, size_t limit = 0);

    //! Test peek to see if a string is a match.
    //! \param search
    //! \param startIndex
    //! \param endIndex
    //! \return bool
    bool PeekSearch(const char* search, size_t startIndex);

    //! Returns whether or not the buffer begins with a given string.
    //! \param search
    //! \return bool
    bool StartsWith(const char* search);

    //! Returns whether or not the buffer ends with a given string.
    //! \param search
    //! \return bool
    bool EndsWith(const char* search);

    //! Trim left whitespace.
    void LTrim();

    //! Trim right whitespace.
    void RTrim();

    //! Trims the beginning and end of whitespace.
    void Trim();

    //! Clears out the string buffer.
    void Clear();

    //! Clears the buffer from a given point to its length.
    void ClearAt(size_t startIndex = 0);

    //! Releases the object.
    bool Destroy();

    //! Returns the string buffer.
    //! \return uchar*
    inline const uchar* Str() const { return _Buffer; }

    //! Returns the byte value of the buffer.
    //! \return
    inline const char* CStr() const { return reinterpret_cast<char*>(_Buffer); }

    //! Returns the length of the string.
    //! \return size_t
    inline size_t Length() const { return _Length; }

    //! Returns whether or not the string is empty.
    //! \return
    inline bool Empty() const { return _Length == 0; }

    //! Returns the capacity of the string.
    //! \return size_t
    inline size_t Capacity() const { return _Capacity; }

    //! Returns whether or not a string equals the buffer's string.
    //! \return bool
    inline bool Equals(StringBuffer* buffer) { return Equals(buffer->CStr()); }

    //! Returns whether or not a string equals the buffer's string.
    //! \return bool
    inline bool Equals(const char* compare) { return (strcmp(CStr(), compare) == 0); }

    //! Returns the last byte in the buffer.
    //! \return char
    inline char FirstByte() {
        if (_Length == 0) {
            return '\0';
        }
        return reinterpret_cast<char*>(_Buffer)[0];
    }

    //! Returns the last byte in the buffer.
    //! \return char
    inline char LastByte() {
        if (_Length == 0) {
            return '\0';
        }
        return reinterpret_cast<char*>(_Buffer)[_Length - 1];
    }

    //! Sets a byte at a given location.
    //! \param location
    //! \param byte
    inline void SetByte(size_t location, const char byte) {
        if (location >= _Capacity) {
            Resize(_Capacity + 5);
        }
        _Buffer[location] = byte;
    }

    //! Returns the char at a given location.
    //! \param place
    //! \return
    inline const char Get(size_t location) {
        if (location > _Length) {
            return '\0';
        }
        return _Buffer[location];
    }

    // Assignment operators.
    StringBuffer& operator=(const char* Str);
    StringBuffer& operator=(const StringBuffer& Str);
    StringBuffer& operator=(const SharedPtr<StringBuffer>& Str);
    StringBuffer& operator=(const std::string& Str);

    //! Evaluates whether two string buffers are equal.
    //! \return bool
    bool operator == (StringBuffer* rhs) {
        return Equals(rhs->CStr());
    }

    //! Evaluates whether two string buffers are equal.
    //! \return bool
    bool operator == (const std::string& rhs) {
        return Equals(rhs.c_str());
    }

    //! Evaluates whether two string buffers are equal.
    //! \return bool
    bool operator == (const char* rhs) {
        return Equals(rhs);
    }
};

SymplNamespaceEnd
