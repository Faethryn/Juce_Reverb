/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   Cheese_knob_png;
    const int            Cheese_knob_pngSize = 104024;

    extern const char*   Cheese_knobV2_png;
    const int            Cheese_knobV2_pngSize = 131953;

    extern const char*   Cheese_OFF_png;
    const int            Cheese_OFF_pngSize = 113418;

    extern const char*   Cheese_ON_png;
    const int            Cheese_ON_pngSize = 95113;

    extern const char*   Kraft_single_png;
    const int            Kraft_single_pngSize = 287469;

    extern const char*   Wood_Board_png;
    const int            Wood_Board_pngSize = 93561;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 6;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
