<pre>
 _____ _____ _____ _____ _____ _____ 
| __  | __  |  |  |_   _|  |  |   __|
| __ -|    -|  |  | | | |  |  |__   |
|_____|__|__|_____| |_| |_____|_____|
</pre>
locaiton transparent vm for shared-nothing actors.

# Build
## Prequisites
* Python 2.7.3 
* Subversion

## Linux
* `make dependencies`
* `make x64.debug` or `make x64.release`

## Windows
* `svn checkout --force http://gyp.googlecode.com/svn/trunk build/gyp --revision 1451`
* `python build/gyp_brutus`
* Open in Visual Studio