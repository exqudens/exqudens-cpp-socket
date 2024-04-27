# exqudens-cpp-socket

### How To Build

```
cmake --preset <name>
cmake --build --preset <name>
```

### Easy Logging C++

- GitHub: [easyloggingpp-9.89](https://github.com/abumq/easyloggingpp/tree/9.89)

##### Logging Format Specifiers

You can customize format of logging using following specifiers:

| Specifier   | Replaced By                                                                                                    |
|-------------|----------------------------------------------------------------------------------------------------------------|
| `%logger`   | Logger ID                                                                                                      |
| `%thread`   | Thread ID - Uses std::thread if available, otherwise GetCurrentThreadId() on windows                           |
| `%level`    | Severity level (Info, Debug, Error, Warning, Fatal, Verbose, Trace)                                            |
| `%levshort` | Severity level (Short version i.e, I for Info and respectively D, E, W, F, V, T)                               |
| `%vlevel`   | Verbosity level (Applicable to verbose logging)                                                                |
| `%datetime` | Date and/or time - Pattern is customizable - see Date/Time Format Specifiers below                             |
| `%user`     | User currently running application                                                                             |
| `%host`     | Computer name application is running on                                                                        |
| `%file`*    | File name of source file (Full path) - This feature is subject to availability of `__FILE__` macro of compiler |
| `%fbase`*   | File name of source file (Only base name)                                                                      |
| `%line`*    | Source line number - This feature is subject to availability of `__LINE__` macro of compile                    |
| `%func`*    | Logging function                                                                                               |
| `%loc`*     | Source filename and line number of logging (separated by colon)                                                |
| `%msg`      | Actual log message                                                                                             |
| `%`         | Escape character (e.g, %%level will write %level)                                                              |

##### Date/Time Format Specifiers

You can customize date/time format using following specifiers

| Specifier |                 Replaced By                                                                                     |
|-----------|-----------------------------------------------------------------------------------------------------------------|
| `%d`      | Day of month (zero-padded)                                                                                      |
| `%a`      | Day of the week - short (Mon, Tue, Wed, Thu, Fri, Sat, Sun)                                                     |
| `%A`      | Day of the week - long (Monday, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday)                         |
| `%M`      | Month (zero-padded)                                                                                             |
| `%b`      | Month - short (Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec)                                      |
| `%B`      | Month - Long (January, February, March, April, May, June, July, August, September, October, November, December) |
| `%y`      | Year - Two digit (13, 14 etc)                                                                                   |
| `%Y`      | Year - Four digit (2013, 2014 etc)                                                                              |
| `%h`      | Hour (12-hour format)                                                                                           |
| `%H`      | Hour (24-hour format)                                                                                           |
| `%m`      | Minute (zero-padded)                                                                                            |
| `%s`      | Second (zero-padded)                                                                                            |
| `%g`      | Milliseconds (width is configured by ConfigurationType::MillisecondsWidth)                                      |
| `%F`      | AM/PM designation                                                                                               |
| `%`       | Escape character                                                                                                |
