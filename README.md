# Limitations

* Base64 encoded values are not supported and won't parse.
* 128-bit precision doubles are not supported and won't parse.
* Timezones on datetimes are currently not supported, but that's just because I'm lazy
* The time component of a datetime must include the `:ss` part as well, contrary to the official language guide.
* Number suffixes must be in upper case (`L`, `D`, `F`), and are for the most part ignored, but are parsed.
* Since the parser is hand written, it'll likely accept a lot of invalid cases, but SDLang is simple enough that it shouldn't matter too much.
