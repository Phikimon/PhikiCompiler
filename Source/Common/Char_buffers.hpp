#ifndef CHARBUFS_H_INCLUDED
#define CHARBUFS_H_INCLUDED

#include <cstdio>
#include "Common/Common.hpp"

enum { START_FILE_SIZE = 16 };

struct myString
{
    char *str;

    size_t len;
};

struct charBuf_t
{
    size_t checkSum;
    size_t leftCanary;
    size_t seed[2];

    char* buf;
    myString *strings;

    size_t sizeFilled;
    size_t Size;
    size_t strQuant;
    bool strValid;

    size_t rightCanary;
};

/*int strDiv(textBuffer *txt);
int streamToBuf(textBuffer *txt, FILE *stream);
void bufdtor(textBuffer *txt);
int bufToStream(textBuffer *txt, FILE *stream);
int bufToStream(char **text, size_t Size, FILE *stream);
bool bufcmp(textBuffer* tbuf1, textBuffer* tbuf2);
bool _bufOk(textBuffer *txt, bool verbose = false, const char bufName[MAX_LINE_LEN] = "name");
void _prString(char *str, size_t len, const char strName[MAX_LINE_LEN]);
int reallocBuf(textBuffer *txt);*/


/**
 Compares two buffers. Returns if they are similar.
 */
bool bufcmp(charBuf_t* tbuf1, charBuf_t* tbuf2);

/**
 Calculates the checkSum
 */
size_t _checkSum(charBuf_t* stack, bool verbose = false, const char bufName[MAX_LINE_LEN] = "name");

/**
 Divides buffer to strings
 Returns array of pointers to the strings in the file
 */
int strDiv(charBuf_t *txt);

/**
    Doubles memory allocated for buffer
 */
int reallocBuf(charBuf_t *txt);

/**
    Replaces buffer content with string
*/
void strcpyBuf(charBuf_t *dest, char *src);


/**
    Puts character in the buffer
 */
int putcBuf(charBuf_t* txt, char c);

/**
    Puts double in the buffer
 */
int putdBuf(charBuf_t* txt, double d);

/**
    Concatenates string and buffer
*/
int strcatBuf(charBuf_t *dest, const char *src, const size_t src_len);

/**
    Concatenates string and buffer
*/
int strcatBuf(charBuf_t *dest, const char *src);

/**
 Constructor for buffers. If no buftxt is passed, allocate 'START_FILE_SIZE' bytes for the string
 and fills it with '\0'.
 */
int bufctor(charBuf_t *bufstruct, const char* buftxt = NULL, int len = -1);


/**
  Reads from stream to buffer
*/
int streamToBuf(charBuf_t *txt, FILE *stream);


/**
  Free(verb) memory allocated for buffer and add some poison
 */
void bufdtor(charBuf_t *txt);

/**
  Write array of strings to stream
 */
int bufToStream(charBuf_t *txt, FILE *stream);

/**
  Write array of strings to stream
 */
int bufToStream(char **text, size_t Size, FILE *stream);


/**
  Buffer verificator and dump
 */
bool _bufOk(charBuf_t *txt, bool verbose = false, const char bufName[MAX_LINE_LEN] = "name");
#define bufOk(buf, verb) { assert(strlen(#buf) <= MAX_LINE_LEN); _bufOk(buf, verb, #buf); }
#define lnbufOk(buf, verb) bufOk(buf, true)

/**
  Print string
 */
void _prString(const char *str, size_t len, const char strName[MAX_LINE_LEN]);
#define prString(str, len)     \
{                              \
    prln();                    \
    _prString(str, len, #str); \
}

#define prStringLen(str)               \
{                                      \
    prln();                            \
    _prString(str, strlen(str), #str); \
}

#endif
