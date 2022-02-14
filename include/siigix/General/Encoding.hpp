#ifndef ENCODING_HPP_CKKYVXTU
#define ENCODING_HPP_CKKYVXTU

#include <string>

namespace sgx {

    class Encoding {
        public:
            typedef int CharacterMap[256];

            enum ByteOrderType {
                BIG_ENDIAN_BYTE_ORDER,
                LITTLE_ENDIAN_BYTE_ORDER,
                NATIVE_BYTE_ORDER
            };

            enum CharacterType {
                UCP_CONTROL,
                UCP_FORMAT,
                UCP_UNASSIGNED,
                UCP_PRIVATE_USE,
                UCP_SURROGATE,
                UCP_LOWER_CASE_LETTER,
                UCP_MODIFIER_LETTER,
                UCP_OTHER_LETTER,
                UCP_TITLE_CASE_LETTER,
                UCP_UPPER_CASE_LETTER,
                UCP_SPACING_MARK,
                UCP_ENCLOSING_MARK,
                UCP_NON_SPACING_MARK,
                UCP_DECIMAL_NUMBER,
                UCP_LETTER_NUMBER,
                UCP_OTHER_NUMBER,
                UCP_CONNECTOR_PUNCTUATION,
                UCP_DASH_PUNCTUATION,
                UCP_CLOSE_PUNCTUATION,
                UCP_FINAL_PUNCTUATION,
                UCP_INITIAL_PUNCTUATION,
                UCP_OTHER_PUNCTUATION,
                UCP_OPEN_PUNCTUATION,
                UCP_CURRENCY_SYMBOL,
                UCP_MODIFIER_SYMBOL,
                UCP_MATHEMATICAL_SYMBOL,
                UCP_OTHER_SYMBOL,
                UCP_LINE_SEPARATOR,
                UCP_PARAGRAPH_SEPARATOR,
                UCP_SPACE_SEPARATOR
            };
            Encoding();
            virtual ~Encoding ();
        private:
    };

    using CharacterMap = Encoding::CharacterMap;

    class UTF8Encoding {
        public:
            UTF8Encoding();
            ~UTF8Encoding();
            const char* canonicalName() const;
            bool isA(const std::string& encodingName) const;
            const CharacterMap& characterMap() const;
            int convert(const unsigned char* bytes) const;
            int convert(int ch, unsigned char* bytes, int length) const;
            int queryConvert(const unsigned char* bytes, int length) const;
            int sequenceLength(const unsigned char* bytes, int length) const;

            static bool isLegal(const unsigned char *bytes, int length);
        private:
            static const char* _names[];
            static const CharacterMap _charMap;
    };

    class UTF32Encoding {
        public:

            UTF32Encoding(int ByteOrderMark);
            UTF32Encoding(Encoding::ByteOrderType = Encoding::NATIVE_BYTE_ORDER);

            void setByteOrder(int);
            void setByteOrder(Encoding::ByteOrderType);
            Encoding::ByteOrderType getByteOrder() const;

            const char*         canonicalName() const;
            bool                isA(const std::string& encodingName) const;
            const CharacterMap& characterMap() const;
            int                 convert(const unsigned char* bytes) const;
            int                 convert(int ch, unsigned char* bytes, int length) const;
            int                 queryConvert(const unsigned char* bytes, int length) const;
            int                 sequenceLength(const unsigned char* bytes, int length) const;

            virtual ~UTF32Encoding();
        private:
            bool _flipBytes;
            static const char* _names[];
            static const CharacterMap _charMap;
    };
} /* sgx  */ 

#endif /* end of include guard: ENCODING_HPP_CKKYVXTU */
