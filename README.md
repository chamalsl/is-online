# is-online
Simple application to check whether web sites are online.
This application will connect to a list of web-sites and display HTTP status codes.

## Build instructions
1. Install build dependancies.   
   Ubuntu:
   ```   
   apt update   
   apt install libgtkmm-3.0-dev libcurl4-openssl-dev libsqlite3-dev
   ```   
3. `git clone https://github.com/chamalsl/is-online.git src`
4. `cd src`
5. `make BUILD=release`

These build types are also supported.   
1. make BUILD=debug - Build with debug sysmbols
2. make BUILD=sanitize_address
3. make BUILD=sanitize_thread

Build output is saved in build/{build_type} directory.   
For example `make BUILD=debug` will save out output in build/debug directory.

## Modify Images and compile Resources

Follow these instructions, if you want to change icons such as floppy.svg or delete.svg.   
1. Modify svg files. Do not change file names.
2. Compile them to resources.cc   
   `glib-compile-resources --target=resources.cc --generate-source resources.xml`   




