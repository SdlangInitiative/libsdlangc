# Overview

`libsdlang` is a header-only C library used to parse and (todo) emit [Sdlang](https://sdlang.org/) files.

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

# Usage

Please note that the arrays in this library are from the `stb_ds` library, so please refer to that library for
usage details.

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
```

# Limitations

* Base64 encoded values are not supported and won't parse.
* 128-bit precision doubles are not supported and won't parse.
* Timezones on datetimes are currently not supported, but that's just because I'm lazy
* The time component of a datetime must include the `:ss` part as well, contrary to the official language guide.
* Number suffixes must be in upper case (`L`, `D`, `F`), and are for the most part ignored, but are parsed.
* Since the parser is hand written, it'll likely accept a lot of invalid cases, but SDLang is simple enough that it shouldn't matter too much.
