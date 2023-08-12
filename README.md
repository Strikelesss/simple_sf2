# simple_sf2
 
A C++17 header-only simple SoundFont2 reader.

To use it, all you need to do is copy both "sf2_types.hpp" and "simple_sf2.hpp" to your project and then simply include "simple_sf2.hpp".

Code Example:
```cpp
#include "simple_sf2.hpp"

...

simple_sf2::SF2Bank bank;
if(simple_sf2::ReadSF2(SOUNDBANK_PATH, bank) == simple_sf2::ESF2ReadResult::READ_SUCCESS)
{
	//  Use bank result ...
}
```
