# Overview

`libsdlang` is a header-only C library used to parse and emit [Sdlang](https://sdlang.org/) files.

This library comes bundled with the `ds` (data structure) module from the [stb](https://github.com/nothings/stb) project for convenience.

# Installing

First, grab the `include/libsdlang.h` and `include/stb_ds.h` headers.

If you already use `stb_ds` then you won't need the version bundled in this repo, it's included just to make this library even easier to include.

Then in **one** .C file add:

```c
#define SDLANG_IMPLEMENTATION
#include <libsdlang.h> // or "libsdlang.h"
#define STB_DS_IMPLEMENTATION
#include <stb_ds.h>
```

Then just `#include` those two headers in any file you need to use this library in.

# Parsing & AST usage

Please note that the arrays in this library are from the `stb_ds` library, so please refer to that library for
usage details.

**The lifetime of any textual data is tied to the lifetime of the original string passed into the SdlangCharStream structure.**
This is because this library does not perform copying of textual data by default.

The lifetime of any programmatic additions of textual data to the AST is completely down to the user. In the future
I may replace the root tag with a special type to consolidate this kind of memory into, so it can be freed automatically
by `sdlangTagFree`.

Your best bet at the moment though is to use something like a region allocator, so you can simply "deallocate" all your
custom textual data at the same time as calling `sdlangTagFree`.

In short:

* Create an `SdlangCharStream` containing the SDL to parse.
* Call `sdlangParseCharStream`
* ???
* Yes

In code:

```c
const char* sdl = 
"message \"Hello, world!\" {\n           \
    sent 2021/08/30 18:00:00 by=`brad`\n \
}                                        \
";

SdlangCharStream stream = { sdl, strlen(sdl) };
SdlangError error;                      // Set if an error occurs.
SdlangCharSlice errorLine, errorSlice;  // errorLine = The line an error happened, errorSlice = The text that (probably) caused the error.
SdlangTag tag = {};                     // The root tag.

const bool parsed = sdlangParseCharStream(stream, &tag, &error, &errorLine, &errorSlice);
if(!parsed)
    assert(0);

size_t i;
for(i = 0; i < arrlen(tag.children); i++) // arrlen being from stb_ds
{
    SdlangTag child = tag.children[i];
    if(strncmp("message", child.name.ptr, child.name.length) == 0)
    {
        if(child.values[0].type != SDLANG_VALUE_TYPE_STRING)
            assert(0);

        SdlangCharSlice msg = child.values[0].stringValue;
        SdlangDateTime sent = child.children[0].values[0].dateTimeValue;
        SdlangCharSlice by = child.children[0].attributes[0].value.stringValue;

        printf(
            "Message: '%.*s'\nSent by: %.*s\nOn: %d/%d/%d %d:%d:%d\n",
            (int)msg.length, msg.ptr,
            (int)by.length, by.ptr,
            (int)sent.date.year, sent.date.month, sent.date.day,
            sent.time.hours, sent.time.minutes, sent.time.seconds
        );
    }
}

sdlangTagFree(tag);
```

# Usage for emitting

* Build AST in some way
* Call `sdlangEmitToString`, and don't forget to free the string.
* Or call `sdlangEmit` with a custom emitter function.

# Escaping strings

If an `SdlangValue` is of type `SDLANG_VALUE_TYPE_STRING` and the boolean property `SdlangValue.requiresEscape` is `true`, then
the string contains characters or is formatted in a way that requires the string to be escaped.

To perform string escaping, you must first call `sdlangCharStreamFromValue` to create an `SdlangCharStream` from the `SdlValue`.

Then you must choose between either of these functions:

* sdlangCharStreamEscapeNext - Allocationless iterator over the different "substrings" of the main string.
* sdlangCharStreamEscapeFull - Allocates a new string and fully escapes the main string into it. This memory must be `free`ed.

So in other words - choose between convenience or not having to manage memory.

An example usage of `sdlangCharStreamEscapeNext` would be... well... `sdlangCharStreamEscapeFull`'s source code:

```c
SdlangCharSlice sdlangCharStreamEscapeFull(SdlangCharStream stream)
{
    char* buffer = (char*)calloc(stream.textLength+1, 1);
    size_t written = 0;

    SdlangCharSlice next;
    while(sdlangCharStreamEscapeNext(&stream, &next))
    {
        const size_t end = written + next.length;
        if(end > stream.textLength) // Shouldn't ever really happen, buuuuut better safe than sorry.
            break;
        memcpy(buffer+written, next.ptr, next.length);
        written = end;
    }

    SdlangCharSlice slice = { buffer, written };
    return slice;
}
```

`sdlangCharStreamEscapeFull` is simple enough that it doesn't need an example, but I want to metion again that **the memory must be `free`ed**
since we have to allocate a new string instead of slicing the original.

# Helpers

For quality of life purposes, there are a few helper functions included with the base library.

## SdlangAttribute\* sdlangTagGetAttribute(SdlangTag tag, const char\* name)

This function will return either a pointer to an attribute called `name` in `tag`, or it'll return `NULL` if the attribute doesn't exist.

## SDLANG_CHAR_SLICE(string)

This macro will create an initialiser expression for `SdlangCharSlice`.

# Tests

To run the unittests, run the following commands:

```c
mkdir build
cd build
cmake -G Ninja ..
ninja
./test_runner
```

# Configuration

In the same file where you define `SDLANG_IMPLEMENTATION`, you can also define other values:

## `SDLANG_EMIT_NO_BRANCH` 

By default, `sdlangEmit` constantly checks for error messages so it can abort as soon as possible.
This results in every other line being a branch to perform this check.

Defining `SDLANG_EMIT_NO_BRANCH` means `sdlangEmit` won't check at every possible opportunity, resulting in better performance.

However this does mean the emitter function may be called multiple times after detecting an error before `sdlangEmit` finally aborts
its attempt.

# Limitations

* Base64 encoded values are not supported and won't parse.
* 128-bit precision doubles are not supported and won't parse.
* Timezones on datetimes are currently not supported, but that's just because I'm lazy
* The time component of a datetime must include the `:ss` part as well, contrary to the official language guide.
* Number suffixes must be in upper case (`L`, `D`, `F`), and are for the most part ignored, but are parsed.
* Since the parser is hand written, it'll likely accept a lot of invalid cases, but SDLang is simple enough that it shouldn't matter too much.
