#include "Char_buffers.hpp"

/**
 Compares two buffers. Returns if they are similar.
 */
bool bufcmp(charBuf_t* tbuf1, charBuf_t* tbuf2)
{
    assert(tbuf1); assert(tbuf2); assert( tbuf1 != tbuf2 );

    if ((tbuf1->Size == tbuf2->Size) || (tbuf1->sizeFilled == tbuf2->sizeFilled))
    {
        for (unsigned int i = 0; i < tbuf1->Size; i++)
        {
            if (tbuf1->buf[i] != tbuf2->buf[i])
                return 0;
        }
    } else
    {
        return 0;
    };
    return 1;
}


/**
 Divides buffer to strings
 Returns array of pointers to the strings in the file
 */
int strDiv(charBuf_t *txt)
{
    //Check input pointer
    assert(txt);

    //Initialize strQuant
    txt->strQuant = 0;

    //Mention that extrabyte from streamToBuf
    txt->Size++; //It was allocated
    txt->buf[txt->Size-1]='\0';

    //Count all the strings
    char *slider = txt->buf - 1;
    while ( (slider = strchr(slider + 1, '\n')) != nullptr )
    {
        *slider = '\0';
        txt->strQuant++;
    };

    //Allocate memory for 'strings' array
    txt->strings = (myString*)calloc( txt->strQuant, sizeof(myString) );
    if (!txt->strings) return ERR_ALLOC;

    //Find strings' pointers
    slider = txt->buf;
    for (unsigned int i = 0; i < txt->strQuant; i++)
    {
        (txt->strings[i]).str = slider;
        slider += ( (txt->strings[i]).len = (unsigned)strlen(slider) ) + 1;
    }


    if (txt->strQuant > 0)
    {
        txt->strValid = true;
        return 0;
    } else return -1;

}

/**
    Doubles memory allocated for buffer
 */
int reallocBuf(charBuf_t *txt)
{
    assert(txt);
    assert(txt->Size);
    assert(txt->buf);

    char *oldbuf;
    if ((oldbuf = (char*)realloc(txt->buf, txt->Size * 2)) == NULL) throw ERR_ALLOC;
    txt->buf = oldbuf;

    //Initialize memory
    for (char* i = txt->buf + txt->Size; i < txt->buf + txt->Size*2; i++)
        *i = '\0';

    txt->Size *= 2;

    //txt->checkSum = _checkSum(txt);
    return 0;
}

/**
    Replaces buffer content with string
*/
void strcpyBuf(charBuf_t *dest, char *src)
{
    assert(dest);
    assert(src);
    assert(dest->Size);
    assert(dest->buf);

    unsigned int src_len = strlen(src);
    if (dest->Size <= (dest->sizeFilled = src_len))
        if (!realloc(dest->buf, dest->Size * 2)) throw ERR_ALLOC;
    strcpy(dest->buf, src);
}

/**
    Puts character in the buffer
 */
int putcBuf(charBuf_t* txt, char c)
{
    assert(txt->sizeFilled); assert(txt->Size); assert(txt->buf);
    if (txt->sizeFilled == txt->Size)
        reallocBuf(txt);
    txt->buf[txt->sizeFilled++] = c;
return 0;
}

/**
    Concatenates string and buffer
*/
int strcatBuf(charBuf_t *dest, const char *src, const unsigned int src_len)
{
    assert(dest);
    assert(src);
    assert(dest->Size);
    assert(dest->buf);

    //Reallocate memory
    while (dest->Size <= (src_len + dest->sizeFilled))
    {
    reallocBuf(dest);
    };

    for (unsigned int i = 0 ; i < src_len; i++)
    {
        *(dest->buf + dest->sizeFilled + i) = src[i];
    }
    dest->sizeFilled += src_len;
    dest->strValid = false;

    return 0;
}

/**
 Constructor for buffers. If no buftxt is passed, allocate 'START_FILE_SIZE' bytes for the string
 and fills it with '\0'.
 */
int bufctor(charBuf_t *bufstruct, const char* buftxt, int len)
{
    //Check input pointers
    assert(bufstruct);
    //Initialize values
    bufstruct->Size = buftxt ?
                      ( (len != -1) ? len : strlen(buftxt) ) :
                      START_FILE_SIZE;
    bufstruct->buf  = (char*)calloc(bufstruct->Size, sizeof(char));

    bufstruct->sizeFilled = 0;
    bufstruct->strings = NULL;
    bufstruct->strQuant = 0;
    bufstruct->strValid = false;
    return 0;
}

/**
 Reads from stream to buffer
*/
int streamToBuf(charBuf_t *txt, FILE *stream)
{
    //Check input pointers
    assert(stream);
    assert(((void*)txt) != ((void*)stream) ); //Who knows how stupid user is?
    /*
    NO ASSERTS THERE. PUSSY WANTS ME TO STROKE HER!

    (\___/)
    (='.'= )
    (")_(")

    */
    txt->Size = ( fseek(stream, 0, SEEK_END), ftell(stream) + 1);
    //Allocate memory for the buffer
    ALLOC(txt->buf = ( (char *)calloc(
                     txt->Size/* = ( fseek(stream, 0, SEEK_END), ftell(stream) + 1)*/,//allocate memory for extrabyte to put '0' there
                     //fseek puts cursor to the end of file, ftell returns position of the cursor == size of file
                     sizeof(char)
                 )
               ));
    txt->Size--; // size doesn't mention that extrabyte, but it remains allocated. see strDiv
    if (txt->buf == nullptr) return ERR_ALLOC;

    assert(stream);
    fseek(stream, 0, SEEK_SET); //return cursor

    assert(txt->buf);
    assert(txt->Size);
    assert(stream);
    if (fread(txt->buf, sizeof(char), txt->Size, stream) != txt->Size) throw ERR_FREAD; //fill the buffer
    txt->strings = NULL;
    txt->strQuant = 0;
    txt->strValid = false;
    txt->sizeFilled = txt->Size;
    return 0;
}

/**
  Free(verb) memory allocated for buffer and add some poison
 */
void bufdtor(charBuf_t *txt)
{
    //Check input pointer
    assert(txt); assert(txt->buf);
    //Some poison to the 'strings'
    char *slider = NULL;
    for (unsigned int i = 0; i < txt->strQuant; i++)
    {
        slider = txt->strings[i].str;
        while (*slider != '\0')
            (*slider++) = POISON_CHAR ;
        txt->strings[i].len = POISON_INT ;
        txt->strings[i].str = nullptr;
    }
    //And for the rest
    if (txt->strings) { free(txt->strings); };
    free(txt->buf);
    txt->strQuant = 0;
    txt->Size = 0;
    txt->strValid = false;
    
    free(txt);
}

/**
  Write array of strings to stream
 */
int bufToStream(charBuf_t *txt, FILE *stream)
{
    //Check input pointers
    assert(txt);
    assert(stream);
    //Write to the stream

        fwrite(txt->buf, sizeof(char), txt->sizeFilled, stream);

    return 0;
}

/**
  Write array of strings to stream
 */
int bufToStream(char **text, unsigned int Size, FILE *stream)
{
    //Check input pointers
    assert(text);
    assert(stream);
    assert((void*)text != (void*)stream); //Who knows how stupid user is?

    //Write to the stream
    for (unsigned int i = 0; i < Size; i++)
    {
        fputs(text[i], stream);
    }
    return 0;
}

/**
  Buffer verificator
 */
bool _bufOk(charBuf_t *txt, bool verbose, const char bufName[MAX_LINE_LEN])
{
    //Check input pointers
    assert(txt);
    assert(txt->buf);
    assert((void*)bufName != (void*)txt);
    assert(strlen(bufName) <= 255);
    //Initializing flags
    bool strTrulyValid = false;
    bool ok;
    {
        //Verificate 'strings'
        if ( txt->strValid )
        {
            strTrulyValid = true;
            assert(txt->strQuant);
            assert(txt->strings);
            for (int i = 0; i < (int)txt->strQuant; i++)
                if ( (txt->strings[i].len <  0) || (!txt->strings[i].str) )
                {
                    strTrulyValid = false;
                    break;
                };
            assert(txt->strQuant);
        }
        //return
        ok = (txt && txt->buf && ((!txt->strValid) || (txt->strings)) && (txt->Size >= 0) && (strTrulyValid == txt->strValid) && (txt->sizeFilled >= txt->strQuant) && (txt->sizeFilled <= txt->Size));
    }
    //Shout if neccesary
    if (verbose)
    {
        printf("\t Buffer '%s' is %s [%p] \n", bufName, (ok ? "okay" : "NOT OKAY"), (txt->buf));
        printf("\t{\n");
        printf("\t buffSize = %lu\n", (int)txt->Size);
        printf("\t filledSize = %lu\n", (int)txt->sizeFilled);
        printf("\t strValid = %lu\n", txt->strValid);
        printf("\t strTrulyValid = %lu\n", strTrulyValid);
        printf("\t strQuant = %lu\n", (int)txt->strQuant);
        printf("\t There is%sstring division(%lu strings detected)[%p]:\n", ((txt->strValid) && (txt->strQuant)) ? " " : " no ", txt->strQuant,
               ((txt->strQuant > 0) ? txt->strings->str : 0));
        // if (txt->strValid)
        {
            /*
            #include <conio.h>
            printf("    Print them? Y/N\n");
            if (_getch() == 'Y')
             */
            for (unsigned int i = 0; i < txt->strQuant; i++)
                printf("\t [%p], len=%4i  |  \"%s\"\n", txt->strings[i].str, (int)txt->strings[i].len, txt->strings[i].str);
        }
        printf("\t Buffer[%p] content:\n\t \"%s\"\n\t ", txt->buf, txt->buf);
        for (unsigned int chr = 0; chr < txt->Size; chr++)
        {
            printf(" '%c'%s", isgraph(txt->buf[chr]) ? txt->buf[chr] : ' ', (chr == txt->sizeFilled-1) ? "|" : " ");
        }
        printf("\nCodes(0d):");
        for (unsigned int chr = 0; chr < txt->Size; chr++)
        {
            printf("%3i %s", (unsigned char)txt->buf[chr], (chr == txt->sizeFilled-1) ? "|" : " ");
        }
        printf("\nCodes(0x):");
        for (unsigned int chr = 0; chr < txt->Size; chr++)
        {
            printf("%3x %s", (unsigned char)txt->buf[chr], (chr == txt->sizeFilled-1) ? "|" : " ");
        }
        printf("\n\t}\n\n");
    };
    return !ok;
}

/**
  prString
 */
void _prString(char *str, unsigned int len, const char strName[MAX_LINE_LEN])
{
    printf("\n     String '%s'[%p] dump:\n", strName, str);
    printf("\t{\n");
    printf("\t String length = %lu\n", len);
    printf("\t Content:\n\t \"%s\"\n\t  ", str);
    for (unsigned int chr = 0; chr < len; chr++)
    {
        printf("'%c' ", isgraph(str[chr]) ? str[chr] : ' ');
    }
    printf("\nCodes(0d):");
    for (unsigned int chr = 0; chr < len; chr++)
    {
        printf("%3i ", (unsigned char)str[chr]);
    }
    printf("\nCodes(0x):");
    for (unsigned int chr = 0; chr < len; chr++)
    {
        printf("%3x ", (unsigned char)str[chr]);
    }

    printf("\n\t}\n\n");
}
