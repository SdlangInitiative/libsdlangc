﻿#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "stb_ds.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef const char* SdlangError;
const SdlangError SDLANG_ERROR_NONE                               = NULL;
const SdlangError SDLANG_ERROR_EXPECTED_EQUALS                    = "Expected an '=' char following attribute name.";
const SdlangError SDLANG_ERROR_EXPECTED_IDENTIFIER_OR_BRACE       = "Expected an identifier, left curly brace, newline, or EOF following tag name.";
const SdlangError SDLANG_ERROR_EXPECTED_SPACE                     = "Expected at least 1 whitespace character.";
const SdlangError SDLANG_ERROR_EXPECTED_NEWLINE                   = "Expected a new line following left curly brace.";
const SdlangError SDLANG_ERROR_EXPECTED_VALUE                     = "Expected a value.";
const SdlangError SDLANG_ERROR_EXPECTED_TWO_DIGITS                = "Expected two digits.";
const SdlangError SDLANG_ERROR_EXPECTED_FOUR_DIGITS               = "Expected four digits when parsing the year of a date.";
const SdlangError SDLANG_ERROR_EXPECTED_COLON                     = "Expected a colon.";
const SdlangError SDLANG_ERROR_EXPECTED_SLASH                     = "Expected a forward slash.";
const SdlangError SDLANG_ERROR_EXPECTED_DAYS                      = "Expected the character 'd'.";
const SdlangError SDLANG_ERROR_EXPECTED_INTEGER                   = "Expected an integer.";
const SdlangError SDLANG_ERROR_EXPECTED_TAG_NAME                  = "Expected the name of a tag.";
const SdlangError SDLANG_ERROR_EXPECTED_END_BRACE                 = "Expected a right curly brace to end the children of a tag.";
const SdlangError SDLANG_ERROR_UNEXPECTED_CHARACTER               = "Unexpected character. Expected either a digit, speech mark, new line, left curly brace, or roman character.";
const SdlangError SDLANG_ERROR_UNEXPECTED_EOF                     = "Unexpected EOF.";
const SdlangError SDLANG_ERROR_UNEXPECTED_DOT                     = "Unexpected decimal place when parsing number. Only one or zero can exist.";
const SdlangError SDLANG_ERROR_UNTERMINATED_STRING                = "Unterminated string.";
const SdlangError SDLANG_ERROR_NUMBER_TOO_LARGE                   = "Number is too large to parse, which likely means the number isn't even valid.";

typedef struct SdlangCharSlice 
{
    const char* ptr;
    size_t length;
} SdlangCharSlice;

typedef struct SdlangCharStream 
{
    const char* text;
    const size_t textLength;
    size_t cursor;
} SdlangCharStream;

char sdlangCharStreamPeek(const SdlangCharStream* stream);
char sdlangCharStreamEat(SdlangCharStream* stream);
bool sdlangCharStreamEof(const SdlangCharStream* stream);
SdlangCharSlice sdlangCharStreamPeekMany(const SdlangCharStream* stream, const size_t amount);
SdlangCharSlice sdlangCharStreamGetLine(const SdlangCharStream* stream, const size_t forCursorAt);

#ifdef SDLANG_IMPLEMENTATION
char sdlangCharStreamPeek(const SdlangCharStream* stream)
{
    assert(stream->cursor < stream->textLength);
    return stream->text[stream->cursor];
}

char sdlangCharStreamEat(SdlangCharStream* stream)
{
    assert(stream->cursor < stream->textLength);
    return stream->text[stream->cursor++];
}

bool sdlangCharStreamEof(const SdlangCharStream* stream)
{
    return stream->cursor >= stream->textLength;
}

SdlangCharSlice sdlangCharStreamPeekMany(const SdlangCharStream* stream, const size_t amount)
{
    const size_t end = stream->cursor + amount;

    SdlangCharSlice slice;
    slice.ptr = stream->text + stream->cursor;
    if(end >= stream->textLength)
        slice.length = stream->textLength - stream->cursor;
    else
        slice.length = amount;

    return slice;
}

SdlangCharSlice sdlangCharStreamGetLine(const SdlangCharStream* stream, const size_t forCursorAt)
{
    size_t start = forCursorAt;
    size_t end   = forCursorAt;
    
    while(start > 0 && stream->text[start] != '\n')
        start--;

    while(end < stream->textLength && stream->text[end] != '\n' && stream->text[end] != '\r')
        end++;

    if(start != 0) // Don't include the new line character.
        start++;

    const size_t len = end - start;
    SdlangCharSlice slice = {
        stream->text + start,
        len
    };
    return slice;
}
#endif

typedef enum SdlangTokenType {
    SDLANG_TOKEN_TYPE_NONE              = 0,
    SDLANG_TOKEN_TYPE_TAG_NAME          = 1,
    SDLANG_TOKEN_TYPE_NEWLINE           = 2,
    SDLANG_TOKEN_TYPE_CHILDREN_START    = 3,
    SDLANG_TOKEN_TYPE_CHILDREN_END      = 4,
    SDLANG_TOKEN_TYPE_VALUE_STRING      = 100,
    SDLANG_TOKEN_TYPE_VALUE_INTEGER     = 101,
    SDLANG_TOKEN_TYPE_VALUE_FLOATING    = 102,
    SDLANG_TOKEN_TYPE_VALUE_BOOLEAN     = 103,
    SDLANG_TOKEN_TYPE_VALUE_DATE        = 104,
    SDLANG_TOKEN_TYPE_VALUE_TIMESPAN    = 105,
    SDLANG_TOKEN_TYPE_VALUE_DATETIME    = 106,
    SDLANG_TOKEN_TYPE_VALUE_NULL        = 107,
    SDLANG_TOKEN_TYPE_EOF               = 200
} SdlangTokenType;

static const int _STATE_LOOKING_FOR_TAG_START = 0;
static const int _STATE_READING_TAG           = 1;

typedef struct SdlangTimeSpan
{
    int64_t days;
    int8_t hours;
    int8_t minutes;
    int8_t seconds;
    int64_t milliseconds;
    bool isNegative;
} SdlangTimeSpan;

typedef struct SdlangDate
{
    int64_t year;
    int8_t month;
    int8_t day;
} SdlangDate;

typedef struct SdlangDateTime
{
    SdlangDate date;
    SdlangTimeSpan time;
} SdlangDateTime;

typedef struct SdlangToken
{
    SdlangTokenType type;
    size_t start;
    size_t end;
    SdlangCharSlice nspace; // Set for TAG_NAME and ATTRIBUTE
    SdlangCharSlice name; // Set for TAG_NAME and ATTRIBUTE
    bool isAttrib;
    
    union
    {
        union
        {
            SdlangCharSlice stringValue;
            bool requiresEscape;
        };

        int64_t intValue;
        long double floatValue;
        bool boolValue;
        SdlangTimeSpan timeSpanValue;
        SdlangDate dateValue;
        SdlangDateTime dateTimeValue;
    };
} SdlangToken;

typedef struct SdlangParser 
{
    SdlangCharStream stream;
    SdlangToken front;
    int _state;
} SdlangParser;

void sdlangParserNext(SdlangParser* parser, SdlangError* error, SdlangCharSlice* errorLine, SdlangCharSlice* errorSlice);

#ifdef SDLANG_IMPLEMENTATION
static bool _spaces(SdlangParser* parser)
{
    const size_t start = parser->stream.cursor;
    while(!sdlangCharStreamEof(&parser->stream) && (
            sdlangCharStreamPeek(&parser->stream) == ' ' 
        ||  sdlangCharStreamPeek(&parser->stream) == '\t' 
    ))
        parser->stream.cursor++;

    return parser->stream.cursor > start;
}

static bool _newline(SdlangParser* parser)
{
    const size_t start = parser->stream.cursor;
    while(!sdlangCharStreamEof(&parser->stream) && (
            sdlangCharStreamPeek(&parser->stream) == '\r' 
        ||  sdlangCharStreamPeek(&parser->stream) == '\n' 
    ))
        parser->stream.cursor++;

    return parser->stream.cursor > start || sdlangCharStreamEof(&parser->stream);
}

static SdlangCharSlice _identifier(SdlangParser* parser)
{
    const size_t start = parser->stream.cursor;
    
    while(!sdlangCharStreamEof(&parser->stream))
    {
        const char ch = sdlangCharStreamPeek(&parser->stream);
        if(!(ch >= 'a' && ch <= 'z') && !(ch >= 'A' && ch <= 'Z') && ch != '_' && !(ch & 0x80)) // poor man's UTF support.
            break;
        parser->stream.cursor++;
    }

    SdlangCharSlice slice = {
        parser->stream.text + start,
        parser->stream.cursor - start
    };

    return slice;
}

static bool _identifierWithNamespace(SdlangParser* parser, SdlangCharSlice* nspace, SdlangCharSlice* name)
{
    *name = _identifier(parser);

    if(!sdlangCharStreamEof(&parser->stream) && sdlangCharStreamPeek(&parser->stream) == ':')
    {
        parser->stream.cursor++;
        *nspace = *name;
        *name = _identifier(parser);
    }
    else
    {
        nspace->ptr = NULL;
        nspace->length = 0;
    }

    return name->length > 0;
}

static bool _string(SdlangParser* parser, SdlangCharSlice* str, bool* wasUnterminated, bool* needsEscape)
{
    const char stringCh = sdlangCharStreamPeek(&parser->stream);

    if(stringCh != '"' && stringCh != '`')
        return false;
    parser->stream.cursor++;
    const size_t start = parser->stream.cursor;

    while(true)
    {
        if(sdlangCharStreamEof(&parser->stream))
        {
            *wasUnterminated = true;
            return false;
        }
        else if(stringCh == '"' && sdlangCharStreamPeek(&parser->stream) == '\n')
        {
            *wasUnterminated = true;
            return false;
        }
        else if(stringCh == '"' && sdlangCharStreamPeek(&parser->stream) == '\\')
        {
            *needsEscape = true;
            parser->stream.cursor += 2;
            continue;
        }
        else if(sdlangCharStreamPeek(&parser->stream) == stringCh)
        {
            str->ptr = parser->stream.text + start;
            str->length = parser->stream.cursor - start;
            parser->stream.cursor++;
            break;
        }
        else
            parser->stream.cursor++;
    }

    return true;
}

static void _number(SdlangParser* parser, int64_t* asInt, long double* asFloat, SdlangTokenType* type, SdlangError* error)
{
    const size_t start = parser->front.start;
    bool foundDot = false;
    char buffer[100];
    int base = 10;

    if(!sdlangCharStreamEof(&parser->stream) && sdlangCharStreamPeek(&parser->stream) == '-')
        parser->stream.cursor++;

    while(true)
    {
        if(sdlangCharStreamEof(&parser->stream))
            break;
        const char ch = sdlangCharStreamPeek(&parser->stream);
        if(ch == '.')
        {
            if(foundDot)
            {
                *error = SDLANG_ERROR_UNEXPECTED_DOT;
                return;
            }
            foundDot = true;
        }
        else if(base == 10 && !(ch >= '0' && ch <= '9'))
            break;
        parser->stream.cursor++;
    }

    const size_t end = parser->stream.cursor;
    const size_t len = end - start;
    if(len >= 100)
    {
        *error = SDLANG_ERROR_NUMBER_TOO_LARGE;
        return;
    }

    memcpy(buffer, parser->stream.text + start, len);
    buffer[len] = '\0';

    if(base == 10)
    {
        if(foundDot)
        {
            *type = SDLANG_TOKEN_TYPE_VALUE_FLOATING;
            *asFloat = atof(buffer);
        }
        else
        {
            *type = SDLANG_TOKEN_TYPE_VALUE_INTEGER;
            *asInt = atoll(buffer);
        }
    }
    else assert(false); // TODO

    // Handle number suffix... eventually, it really isn't that big a deal right now.
    if(!sdlangCharStreamEof(&parser->stream))
    {
        const char ch = sdlangCharStreamPeek(&parser->stream);
        if(ch == 'L')
        {
            parser->stream.cursor++;
        }
        else if(ch == 'F')
        {
            parser->stream.cursor++;
        }
        else if(ch == 'D')
        {
            parser->stream.cursor++;
        }
    }
}

static void _twoDigits(SdlangParser* parser, int8_t* value, SdlangError* error)
{
    const SdlangCharSlice slice = sdlangCharStreamPeekMany(&parser->stream, 2);
    if(slice.length != 2)
    {
        *error = SDLANG_ERROR_EXPECTED_TWO_DIGITS;
        return ;
    }

    char buffer[3] = {
        slice.ptr[0],
        slice.ptr[1],
        '\0'
    };

    *value = atoi(buffer);
    if(*value == 0 && strcmp("00", buffer) != 0) // atoi failed
    {
        *error = SDLANG_ERROR_EXPECTED_TWO_DIGITS;
        return;
    }
    parser->stream.cursor += 2;

    return;
}

static void _timespan(SdlangParser* parser, SdlangTimeSpan* timeSpan, SdlangError* error)
{
    bool isDays;

    if(sdlangCharStreamPeek(&parser->stream) == '-')
    {
        parser->stream.cursor++;
        timeSpan->isNegative = true;
    }
    else
        timeSpan->isNegative = false;

    const size_t start = parser->stream.cursor;
    while(true)
    {
        if(sdlangCharStreamEof(&parser->stream))
        {
            *error = SDLANG_ERROR_UNEXPECTED_EOF;
            return;
        }
        else if(sdlangCharStreamPeek(&parser->stream) == 'd')
        {
            isDays = true;
            break;
        }
        else if(sdlangCharStreamPeek(&parser->stream) == ':')
        {
            isDays = false;
            break;
        }
        else
            parser->stream.cursor++;
    }
    parser->stream.cursor = start;

    if(isDays)
    {
        long double _1;
        SdlangTokenType type;
        _number(parser, &timeSpan->days, &_1, &type, error);
        if(*error)
            return;
        else if(type == SDLANG_TOKEN_TYPE_VALUE_FLOATING)
        {
            *error = SDLANG_ERROR_EXPECTED_INTEGER;
            return ;
        }

        parser->stream.cursor++; // Skip the 'd'
        if(sdlangCharStreamEof(&parser->stream) || sdlangCharStreamEat(&parser->stream) != ':')
        {
            *error = SDLANG_ERROR_EXPECTED_COLON;
            return;
        }
    }

    _twoDigits(parser, &timeSpan->hours, error);
    if(*error)
        return;

    if(sdlangCharStreamEof(&parser->stream) || sdlangCharStreamEat(&parser->stream) != ':')
    {
        *error = SDLANG_ERROR_EXPECTED_COLON;
        return;
    }

    _twoDigits(parser, &timeSpan->minutes, error);
    if(*error)
        return;
    if(sdlangCharStreamEof(&parser->stream) || sdlangCharStreamEat(&parser->stream) != ':')
    {
        *error = SDLANG_ERROR_EXPECTED_COLON;
        return;
    }

    _twoDigits(parser, &timeSpan->seconds, error);
    if(*error)
        return;
    if(sdlangCharStreamEof(&parser->stream) || sdlangCharStreamPeek(&parser->stream) != '.')
        return;
    parser->stream.cursor++;
    
    long double _1;
    SdlangTokenType type;
    _number(parser, &timeSpan->milliseconds, &_1, &type, error);
    if(*error)
        return;
    else if(type == SDLANG_TOKEN_TYPE_VALUE_FLOATING)
    {
        *error = SDLANG_ERROR_EXPECTED_INTEGER;
        return;
    }

    return;
}

static void _dateTime(
    SdlangParser*       parser,
    SdlangDate*         date,
    SdlangDateTime*     dateTime,
    SdlangTokenType*    type,
    SdlangError*        error
)
{
    SdlangDate dateLocal;

    long double _1;
    SdlangTokenType intType;
    _number(parser, &dateLocal.year, &_1, &intType, error);
    if(*error)
        return;
    if(sdlangCharStreamEof(&parser->stream) || sdlangCharStreamEat(&parser->stream) != '/')
    {
        *error = SDLANG_ERROR_EXPECTED_SLASH;
        return;
    }

    _twoDigits(parser, &dateLocal.month, error);
    if(*error)
        return;
    if(sdlangCharStreamEof(&parser->stream) || sdlangCharStreamEat(&parser->stream) != '/')
    {
        *error = SDLANG_ERROR_EXPECTED_SLASH;
        return;
    }

    _twoDigits(parser, &dateLocal.day, error);
    if(*error)
        return;

    _spaces(parser);

    if(sdlangCharStreamEof(&parser->stream))
    {
        *date = dateLocal;
        *type = SDLANG_TOKEN_TYPE_VALUE_DATE;
        return;
    }

    SdlangTimeSpan timeSpan;
    _timespan(parser, &timeSpan, error);

    if(!*error)
    {
        dateTime->date = dateLocal;
        dateTime->time = timeSpan;
        *type = SDLANG_TOKEN_TYPE_VALUE_DATETIME;
    }
    else
    {
        *date = dateLocal;
        *type = SDLANG_TOKEN_TYPE_VALUE_DATE;
    }
    *error = SDLANG_ERROR_NONE;

    return;
}

static void _someNumeric(
    SdlangParser*       parser, 
    int64_t*            asInt, 
    long double*        asFloat,
    SdlangTimeSpan*     asTimespan,
    SdlangDate*         asDate,
    SdlangDateTime*     asDateTime,
    SdlangTokenType*    type, 
    SdlangError*        error
)
{
    const size_t start = parser->stream.cursor;
    while(true)
    {
        if(sdlangCharStreamEof(&parser->stream))
        {
            parser->stream.cursor = start;
            return _number(parser, asInt, asFloat, type, error);
        }
        
        const char ch = sdlangCharStreamPeek(&parser->stream);
        if(ch == ':' || ch == 'd')
        {
            parser->stream.cursor = start;
            *type = SDLANG_TOKEN_TYPE_VALUE_TIMESPAN;
            _timespan(parser, asTimespan, error);
            return;
        }
        else if(ch == '/')
        {
            parser->stream.cursor = start;
            _dateTime(parser, asDate, asDateTime, type, error);
            return;
        }
        else if((ch >= '0' && ch <= '9') || ch == '-')
        {
            parser->stream.cursor++;
            continue;
        }
        else
        {
            parser->stream.cursor = start;
            _number(parser, asInt, asFloat, type, error);
            return;
        }
    }
    assert(false);
}

void sdlangParserNext(SdlangParser* parser, SdlangError* error, SdlangCharSlice* errorLine, SdlangCharSlice* errorSlice)
{
    parser->front.isAttrib = false;
    *error = NULL;
    if(sdlangCharStreamEof(&parser->stream))
    {
        parser->front.type = SDLANG_TOKEN_TYPE_EOF;
        return;
    }

    const char ch = sdlangCharStreamPeek(&parser->stream);
    if(ch == ' ' || ch == '\t')
    {
        _spaces(parser);
        sdlangParserNext(parser, error, errorLine, errorSlice);
        return;
    }
    else if(ch == '\n' || ch == '\r')
    {
        _newline(parser);
        parser->_state = _STATE_LOOKING_FOR_TAG_START;
        parser->front.type = SDLANG_TOKEN_TYPE_NEWLINE;
        return;
    }

    bool foundIdent = false, isString = false, wasUnterminated = false, isIdent = false;

    switch(parser->_state)
    {
        case _STATE_LOOKING_FOR_TAG_START:
            parser->front.start = parser->stream.cursor;

            if(ch == '}')
            {
                parser->front.end = parser->front.start;
                parser->front.type = SDLANG_TOKEN_TYPE_CHILDREN_END;
                parser->stream.cursor++;
                return;
            }

            foundIdent = _identifierWithNamespace(parser, &parser->front.nspace, &parser->front.name);
            if(!foundIdent)
            {
                parser->front.name.ptr = "Content";
                parser->front.name.length = 7;
            }
            parser->front.end = parser->stream.cursor;
            parser->front.type = SDLANG_TOKEN_TYPE_TAG_NAME;
            parser->_state = _STATE_READING_TAG;
            break;

        case _STATE_READING_TAG:
            parser->front.start = parser->stream.cursor;
            if(ch == '{')
            {
                parser->front.end = parser->stream.cursor;
                parser->front.type = SDLANG_TOKEN_TYPE_CHILDREN_START;
                parser->stream.cursor++;
                parser->_state = _STATE_LOOKING_FOR_TAG_START;

                _spaces(parser);
                if(!_newline(parser))
                {
                    *error = SDLANG_ERROR_EXPECTED_NEWLINE;
                    *errorLine = sdlangCharStreamGetLine(&parser->stream, parser->stream.cursor);
                    return;
                }
                return;
            }

            // If it's an identifier, see if it's a boolean value, or otherwise it must be an attribute.
            isIdent = _identifierWithNamespace(parser, &parser->front.nspace, &parser->front.name);
            if(isIdent)
            {
                // check for bools and null
                if(parser->front.name.length && !parser->front.nspace.length)
                {
                    if(strncmp("true", parser->front.name.ptr, 4) == 0 || strncmp("on", parser->front.name.ptr, 2) == 0)
                    {
                        parser->front.end = parser->stream.cursor;
                        parser->front.boolValue = true;
                        parser->front.type = SDLANG_TOKEN_TYPE_VALUE_BOOLEAN;
                        return;
                    }
                    else if(strncmp("false", parser->front.name.ptr, 5) == 0 || strncmp("off", parser->front.name.ptr, 3) == 0)
                    {
                        parser->front.end = parser->stream.cursor;
                        parser->front.boolValue = false;
                        parser->front.type = SDLANG_TOKEN_TYPE_VALUE_BOOLEAN;
                        return;
                    }
                    else if(strncmp("null", parser->front.name.ptr, 4) == 0)
                    {
                        parser->front.end = parser->stream.cursor;
                        parser->front.type = SDLANG_TOKEN_TYPE_VALUE_NULL;
                        return;
                    }
                }
                // otherwise it's an identifier.

                if(sdlangCharStreamPeek(&parser->stream) != '=')
                {
                    *error = SDLANG_ERROR_EXPECTED_EQUALS;
                    *errorLine = sdlangCharStreamGetLine(&parser->stream, parser->stream.cursor);
                    return;
                }
                parser->stream.cursor++;

                // We'll use ParseNext again.
                const size_t start = parser->stream.cursor;
                const SdlangToken tokenCopy = parser->front;
                sdlangParserNext(parser, error, errorLine, errorSlice);
                if(*error)
                    return;
                if(parser->front.start != start)
                {
                    *error = SDLANG_ERROR_EXPECTED_VALUE;
                    *errorLine = sdlangCharStreamGetLine(&parser->stream, parser->stream.cursor);
                    return;
                }

                parser->front.nspace = tokenCopy.nspace;
                parser->front.name = tokenCopy.name;

                // Only certain types can be attribute values.
                switch(parser->front.type)
                {
                    case SDLANG_TOKEN_TYPE_VALUE_BOOLEAN:
                    case SDLANG_TOKEN_TYPE_VALUE_DATE:
                    case SDLANG_TOKEN_TYPE_VALUE_DATETIME:
                    case SDLANG_TOKEN_TYPE_VALUE_TIMESPAN:
                    case SDLANG_TOKEN_TYPE_VALUE_FLOATING:
                    case SDLANG_TOKEN_TYPE_VALUE_INTEGER:
                    case SDLANG_TOKEN_TYPE_VALUE_STRING:
                        break;

                    default:
                        *error = SDLANG_ERROR_EXPECTED_VALUE;
                        *errorLine = sdlangCharStreamGetLine(&parser->stream, parser->stream.cursor);
                        return;
                }

                parser->front.isAttrib = true;
                return;
            }

            // Otherwise, it must be a value.
            wasUnterminated = false;
            isString = _string(parser, &parser->front.stringValue, &wasUnterminated, &parser->front.requiresEscape);
            if(!isString && wasUnterminated)
            {
                *error = SDLANG_ERROR_UNTERMINATED_STRING;
                *errorLine = sdlangCharStreamGetLine(&parser->stream, parser->stream.cursor);
                return;
            }
            else if(isString)
            {
                parser->front.end = parser->stream.cursor;
                parser->front.type = SDLANG_TOKEN_TYPE_VALUE_STRING;
                return;
            }

            _someNumeric(
                parser, 
                &parser->front.intValue, 
                &parser->front.floatValue, 
                &parser->front.timeSpanValue, 
                &parser->front.dateValue, 
                &parser->front.dateTimeValue, 
                &parser->front.type, 
                error
            );
            if(*error)
            {
                *errorLine = sdlangCharStreamGetLine(&parser->stream, parser->stream.cursor);
                return;
            }
            else
            {
                parser->front.end = parser->stream.cursor;
                return;
            }
            break;

        default: assert(false);
    }
}
#endif

typedef enum SdlangValueType
{
    SDLANG_VALUE_TYPE_STRING,
    SDLANG_VALUE_TYPE_INTEGER,
    SDLANG_VALUE_TYPE_FLOATING,
    SDLANG_VALUE_TYPE_BOOLEAN,
    SDLANG_VALUE_TYPE_DATETIME,
    SDLANG_VALUE_TYPE_DATE,
    SDLANG_VALUE_TYPE_TIMESPAN,
    SDLANG_VALUE_TYPE_NULL,
} SdlangValueType;

typedef struct SdlangValue
{
    SdlangValueType type;
    union
    {
        union
        {
            SdlangCharSlice stringValue;
            bool requiresEscape;
        };

        int64_t intValue;
        long double floatValue;
        bool boolValue;
        SdlangTimeSpan timeSpanValue;
        SdlangDate dateValue;
        SdlangDateTime dateTimeValue;
    };
} SdlValue;

typedef struct SdlangAttribute
{
    SdlangCharSlice nspace;
    SdlangCharSlice name;
    SdlangValue     value;
} SdlangAttribute;

typedef struct SdlangTag
{
    SdlangCharSlice nspace;
    SdlangCharSlice name;
    SdlangAttribute* attributes;
    SdlangValue* values;
    SdlangTag* children;
} SdlangTag;

bool sdlangParseCharStream(SdlangCharStream stream, SdlangTag* rootTag, SdlangError* error, SdlangCharSlice* errorLine, SdlangCharSlice* errorSlice);

#ifdef SDLANG_IMPLEMENTATION
static SdlangValue _nextValue(SdlangToken token, SdlangError* error)
{
    SdlangValue v;

    switch(token.type)
    {
    case SDLANG_TOKEN_TYPE_VALUE_BOOLEAN:
        v.type = SDLANG_VALUE_TYPE_BOOLEAN;
        v.boolValue = token.boolValue;
        break;
    case SDLANG_TOKEN_TYPE_VALUE_DATE:
        v.type = SDLANG_VALUE_TYPE_DATE;
        v.dateValue = token.dateValue;
        break;
    case SDLANG_TOKEN_TYPE_VALUE_DATETIME:
        v.type = SDLANG_VALUE_TYPE_DATETIME;
        v.dateTimeValue = token.dateTimeValue;
        break;
    case SDLANG_TOKEN_TYPE_VALUE_FLOATING:
        v.type = SDLANG_VALUE_TYPE_FLOATING;
        v.floatValue = token.floatValue;
        break;
    case SDLANG_TOKEN_TYPE_VALUE_INTEGER:
        v.type = SDLANG_VALUE_TYPE_INTEGER;
        v.intValue = token.intValue;
        break;
    case SDLANG_TOKEN_TYPE_VALUE_NULL:
        v.type = SDLANG_VALUE_TYPE_NULL;
        break;
    case SDLANG_TOKEN_TYPE_VALUE_STRING:
        v.type = SDLANG_VALUE_TYPE_STRING;
        v.stringValue = token.stringValue;
        break;
    case SDLANG_TOKEN_TYPE_VALUE_TIMESPAN:
        v.type = SDLANG_VALUE_TYPE_TIMESPAN;
        v.timeSpanValue = token.timeSpanValue;
        break;

    default: break;
    }

    return v;
}

static SdlangTag _nextTag(SdlangParser* parser, SdlangError* error, SdlangCharSlice* errorLine, SdlangCharSlice* errorSlice)
{
    SdlangTag tag = {};

    if(parser->front.type != SDLANG_TOKEN_TYPE_TAG_NAME)
    {
        *error = SDLANG_ERROR_EXPECTED_TAG_NAME;
        *errorLine = sdlangCharStreamGetLine(&parser->stream, parser->front.start);
        return tag;
    }

    tag.nspace = parser->front.nspace;
    tag.name = parser->front.name;

    SdlangValue value;
    SdlangTag t;

    while(true)
    {
        sdlangParserNext(parser, error, errorLine, errorSlice);
        if(*error)
            return tag;

        switch(parser->front.type)
        {
        case SDLANG_TOKEN_TYPE_NEWLINE:
        case SDLANG_TOKEN_TYPE_EOF:
            return tag;

        case SDLANG_TOKEN_TYPE_VALUE_BOOLEAN:
        case SDLANG_TOKEN_TYPE_VALUE_DATE:
        case SDLANG_TOKEN_TYPE_VALUE_DATETIME:
        case SDLANG_TOKEN_TYPE_VALUE_FLOATING:
        case SDLANG_TOKEN_TYPE_VALUE_INTEGER:
        case SDLANG_TOKEN_TYPE_VALUE_NULL:
        case SDLANG_TOKEN_TYPE_VALUE_STRING:
        case SDLANG_TOKEN_TYPE_VALUE_TIMESPAN:
            value = _nextValue(parser->front, error);
            if(*error)
                return tag;

            if(parser->front.name.ptr 
                && parser->front.name.length 
                && parser->front.type != SDLANG_TOKEN_TYPE_VALUE_BOOLEAN
                && parser->front.type != SDLANG_TOKEN_TYPE_VALUE_NULL)
            {
                SdlangAttribute attrib;
                attrib.nspace = parser->front.nspace;
                attrib.name = parser->front.name;
                attrib.value = value;
                arrput(tag.attributes, attrib);
            }
            else
                arrput(tag.values, value);
            break;

        case SDLANG_TOKEN_TYPE_CHILDREN_START:
            sdlangParserNext(parser, error, errorLine, errorSlice);
            if(*error)
                return tag;

            t = _nextTag(parser, error, errorLine, errorSlice);
            if(*error)
                return tag;

            arrput(tag.children, t);
            sdlangParserNext(parser, error, errorLine, errorSlice);
            if(*error)
                return tag;
            if(parser->front.type != SDLANG_TOKEN_TYPE_CHILDREN_END)
            {
                *error = SDLANG_ERROR_EXPECTED_END_BRACE;
                *errorLine = sdlangCharStreamGetLine(&parser->stream, parser->front.start);
                return tag;
            }
            break;

        default:
            *error = SDLANG_ERROR_UNEXPECTED_CHARACTER;
            *errorLine = sdlangCharStreamGetLine(&parser->stream, parser->front.start);
            return tag;
        }
    }

    return tag;
}

bool sdlangParseCharStream(SdlangCharStream stream, SdlangTag* rootTag, SdlangError* error, SdlangCharSlice* errorLine, SdlangCharSlice* errorSlice)
{
    SdlangParser parser = { stream };

    while(parser.front.type != SDLANG_TOKEN_TYPE_EOF)
    {
        sdlangParserNext(&parser, error, errorLine, errorSlice);
        if(*error)
            return false;
        if(parser.front.type != SDLANG_TOKEN_TYPE_EOF)
        {
            SdlangTag tag = _nextTag(&parser, error, errorLine, errorSlice);
            if(*error)
                return false;
            arrput(rootTag->children, tag);
        }
    }

    return true;
}
#endif

#ifdef __cplusplus
}
#endif