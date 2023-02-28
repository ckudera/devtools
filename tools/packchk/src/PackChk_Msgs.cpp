/*
 * Copyright (c) 2020-2021 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ErrLog.h"
#include "PackChk.h"

const MsgTable PackChk::msgTable = {
// 020 Constant Text (help, ...)
  { "M020", { MsgLevel::LEVEL_TEXT,     CRLF_B, "" } },
  { "M021", { MsgLevel::LEVEL_TEXT,     CRLF_NO,"" } },
  { "M022", { MsgLevel::LEVEL_TEXT,     CRLF_B, "Found %ERR% Error(s) and %WARN% Warning(s)." } },
  { "M023", { MsgLevel::LEVEL_TEXT,     CRLF_B, "\nPhase%CHECK%" } },

// 50... Info Messages (INFO = verbose)
  { "M050", { MsgLevel::LEVEL_INFO,     CRLF_B, "" } },
  { "M051", { MsgLevel::LEVEL_INFO,     CRLF_B, "Adding PDSC File: '%PATH%'" } },
  { "M052", { MsgLevel::LEVEL_PROGRESS, CRLF_B, "Checking Pack information for '%PATH%'" } },
  { "M053", { MsgLevel::LEVEL_INFO,     CRLF_B, "" } },
  { "M054", { MsgLevel::LEVEL_PROGRESS, CRLF_B, "Checking if URL exists: '%URL%'" } },
  { "M055", { MsgLevel::LEVEL_INFO,     CRLF_B, "" } },
  { "M056", { MsgLevel::LEVEL_INFO,     CRLF_B, "Checking if File category '%CAT%' has correct extension: '%PATH%'" } },
  { "M057", { MsgLevel::LEVEL_INFO,     CRLF_B, "Checking File with attribute '%ATTR%': '%PATH%'" } },
  { "M058", { MsgLevel::LEVEL_INFO,     CRLF_B, "Checking (case sensitive) if PDSC name matches File Name: '%PATH%'" } },
  { "M059", { MsgLevel::LEVEL_INFO,     CRLF_B, "Checking if File category '%CAT%' has '%COND%' condition: '%PATH%'" } },
  { "M060", { MsgLevel::LEVEL_INFO,     CRLF_B, "Checking Board '%BOARD%' for referenced device(s): '%DEVICE%'" } },
  { "M061", { MsgLevel::LEVEL_INFO,     CRLF_B, "Checking Package Description" } },
  { "M062", { MsgLevel::LEVEL_INFO,     CRLF_B, "Checking Example '%EXAMPLE%' if referenced Board is defined: '[%VENDOR%] %BOARD%'" } },
  { "M063", { MsgLevel::LEVEL_INFO,     CRLF_B, "Search: '[%VENDOR%] %MCU%', found '[%VENDOR2%] %MCU2%'" } },
  { "M064", { MsgLevel::LEVEL_INFO,     CRLF_B, "Checking for other PDSC files: '%PATH%'" } },
  { "M065", { MsgLevel::LEVEL_INFO,     CRLF_B, "Checking %TAG% '%NAME%' against pattern \"%CHAR%\"" } },
  { "M066", { MsgLevel::LEVEL_INFO,     CRLF_B, "Checking Semantic Version: '%RELEASEVER%'" } },
  { "M067", { MsgLevel::LEVEL_INFO,     CRLF_B, "Checking releases for consistency: '%RELEASEVER%', %RELEASEDATE%" } },
  { "M068", { MsgLevel::LEVEL_INFO,     CRLF_B, "Checking if file extension '.pdsc' is lowercase" } },
  { "M069", { MsgLevel::LEVEL_INFO,     CRLF_B, "Checking Component 'Cclass=%CCLASS%, Cgroup=%CGROUP%, Csub=%CSUB%, Cversion=%CVER%' Dependencies." } },
  { "M070", { MsgLevel::LEVEL_INFO,     CRLF_B, "Checking Memory '%NAME%' for device '%NAME2%'" } },
  { "M071", { MsgLevel::LEVEL_INFO,     CRLF_B, "Searching for memory for device '%NAME%'" } },
  { "M072", { MsgLevel::LEVEL_INFO,     CRLF_B, "" } },
  { "M073", { MsgLevel::LEVEL_INFO,     CRLF_B, "File open failed: '%PATH%'" } },
  { "M074", { MsgLevel::LEVEL_PROGRESS, CRLF_B, "Checking if File is in Pack: '%PATH%'" } },
  { "M075", { MsgLevel::LEVEL_PROGRESS, CRLF_B, "Reading PDSC File: %TIME%ms. Passed" } },
  { "M076", { MsgLevel::LEVEL_PROGRESS, CRLF_B, "Constructing Model: %TIME%ms. Passed" } },
  { "M077", { MsgLevel::LEVEL_PROGRESS, CRLF_B, "Validating Model: %TIME%ms. Passed" } },
  { "M078", { MsgLevel::LEVEL_TEXT,     CRLF_BE, "Pack #%NUM%: '%PATH%'" } },
  { "M079", { MsgLevel::LEVEL_INFO,     CRLF_B, "Found Compiler Dependencies (Conditions: ACCEPT or REQUIRE): '%COMPILER%'" } },
  { "M080", { MsgLevel::LEVEL_INFO,     CRLF_NO, "" } },
  { "M081", { MsgLevel::LEVEL_PROGRESS, CRLF_B, "Checking File category '%CAT%' for '%COMP%' dependency: '%PATH%'" } },
  { "M082", { MsgLevel::LEVEL_PROGRESS, CRLF_B, "Checking defined Condition: '%COND%'" } },
  { "M083", { MsgLevel::LEVEL_PROGRESS, CRLF_B, "Checking used Condition: '%COND%'" } },
  { "M084", { MsgLevel::LEVEL_INFO,     CRLF_B, "" } },
  { "M085", { MsgLevel::LEVEL_PROGRESS, CRLF_B, "Checking if Component has Condition: 'Cclass=%CCLASS%, Cgroup=%CGROUP%, Csub=%CSUB%, Cversion=%CVER%'" } },
  { "M086", { MsgLevel::LEVEL_PROGRESS, CRLF_B, "Checking if File has Version: '%PATH%'" } },
  { "M087", { MsgLevel::LEVEL_PROGRESS, CRLF_B, "Checking if Component declared as '%TYPE%' has reference to a device: 'Cclass=%CCLASS%, Cgroup=%CGROUP%, Cversion=%CVER%'" } },
  { "M088", { MsgLevel::LEVEL_PROGRESS, CRLF_B, "Checking if generator ID in Component 'Cclass=%CCLASS%, Cgroup=%CGROUP%, Cversion=%CVER%' is defined: '%GENID%'" } },
  { "M089", { MsgLevel::LEVEL_INFO,     CRLF_B, "Checking Feature for '%MCU%': '%ID%'" } },
  { "M090", { MsgLevel::LEVEL_INFO,     CRLF_B, "Cannot read Attributes (Windows Error #%NUM%): '%PATH%'" } },
  { "M091", { MsgLevel::LEVEL_INFO,     CRLF_B, "Checking '%COMP%' (%COMPID%) dependencies for '[%VENDOR%] %MCU%' (Compiler: %COMPILER% [%OPTION%])" } },
  { "M092", { MsgLevel::LEVEL_INFO,     CRLF_B, "Checking if Path in Pack '%PACK%' is conformant: '%PATH%'" } },
  { "M093", { MsgLevel::LEVEL_PROGRESS, CRLF_B, "Checking if generator ID in Taxonomy 'Cclass=%CCLASS%, Cgroup=%CGROUP%' is defined: '%GENID%'" } },
  { "M094", { MsgLevel::LEVEL_PROGRESS, CRLF_B, "Checking Condition '%COND%' for available devices." } },
  { "M095", { MsgLevel::LEVEL_INFO,     CRLF_B, "Found Feature for '%MCU%': '%TAG%'" } },
  { "M096", { MsgLevel::LEVEL_INFO,     CRLF_B, "Found Schema Version: '%VER%'" } },
  { "M097", { MsgLevel::LEVEL_INFO,     CRLF_B, "Checking %SECTION% Feature against Feature Set for '%MCU%': '%TYPE%', detected '%DESCR%'" } },
  { "M098", { MsgLevel::LEVEL_INFO,     CRLF_B, "Checking Release date '%RELEASEDATE%'" } },
  { "M099", { MsgLevel::LEVEL_INFO,     CRLF_B, "" } },

// 100... Internal Errors
  { "M100", { MsgLevel::LEVEL_ERROR,    CRLF_B, "" } },
  { "M101", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Unknown error!" } },
  { "M102", { MsgLevel::LEVEL_ERROR,    CRLF_B, "" } },
  { "M103", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Internal Error: %REF%" } },
  { "M104", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Path empty when searching for other PDSC files" } },
  { "M105", { MsgLevel::LEVEL_ERROR,    CRLF_B, "" } },
  { "M106", { MsgLevel::LEVEL_ERROR,    CRLF_B, "" } },
  { "M107", { MsgLevel::LEVEL_WARNING,  CRLF_B, "%FEATURE% is not supported by current version of %OS% packchk." } },
  { "M108", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Reading PDSC File failed!" } },
  { "M109", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Constructing RTE Model failed!" } },
  { "M110", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Verifying RTE Model failed" } },

// 200... Invocation Errors
  { "M200", { MsgLevel::LEVEL_ERROR,    CRLF_BE, "" } },
  { "M201", { MsgLevel::LEVEL_ERROR,    CRLF_BE, "Error adding PDSC input file '%PATH%'!" } },
  { "M202", { MsgLevel::LEVEL_ERROR,    CRLF_BE, "No PDSC input file specified" } },
  { "M203", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Error reading PDSC file(s) '%PATH%'!" } },
  { "M204", { MsgLevel::LEVEL_ERROR,    CRLF_B, "File not found: '%PATH%'!" } },
  { "M205", { MsgLevel::LEVEL_ERROR,    CRLF_BE, "Cannot create Pack Name file '%PATH%'" } },
  { "M206", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Multiple PDSC file found in package: %FILES%" } },
  { "M207", { MsgLevel::LEVEL_ERROR,    CRLF_B, "PDSC file name mismatch!\n"\
                                                "  Expected: '%PDSC1%.pdsc'\n"\
                                                "  Actual  : '%PDSC2%.pdsc'" } },
  { "M208", { MsgLevel::LEVEL_ERROR,    CRLF_BE, "" } },
  { "M209", { MsgLevel::LEVEL_TEXT,     CRLF_B, "" } },
  { "M210", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Only one input file to be checked is allowed." } },
  { "M211", { MsgLevel::LEVEL_ERROR,    CRLF_BE, "" } },
  { "M212", { MsgLevel::LEVEL_ERROR,    CRLF_BE, "" } },
  { "M213", { MsgLevel::LEVEL_WARNING,  CRLF_BE, "Found blank char '%NUM%' in Packname output filename, deleted" } },
  { "M214", { MsgLevel::LEVEL_ERROR,    CRLF_BE, "Invalid argument: %OPT%" } },
  { "M215", { MsgLevel::LEVEL_ERROR,    CRLF_BE, "" } },
  { "M216", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Unable to get executable path %MSG%!"} },
  { "M217", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Unable to get executable path %MSG%!"} },
  { "M218", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Unable to find schema file '%PATH%' for validation"} },

// 300... Validation Errors
  { "M300", { MsgLevel::LEVEL_ERROR,    CRLF_B, "" } },
  { "M301", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Checking Pack URL of PDSC file failed:\n"\
                                                "  Expected URL : '%URL1%'\n"\
                                                "  Package  URL : '%URL2%'" } },
  { "M302", { MsgLevel::LEVEL_ERROR,    CRLF_B, "No vendor tag found in the PDSC file. Add the <vendor> tag and provide the\n"\
                                                "vendor name. For example: <vendor>Keil</vendor>. " } },
  { "M303", { MsgLevel::LEVEL_ERROR,    CRLF_B, "No package name found in the PDSC file. Add the <name> tag and provide the\n"\
                                                "package name. For example: <name>MCU-Name_DFP</name>." } },
  { "M304", { MsgLevel::LEVEL_WARNING,  CRLF_B, "No package URL (<url>-tag and/or value) found in PDSC file!" } },
  { "M305", { MsgLevel::LEVEL_ERROR,    CRLF_B, "No releases (\"<release>\" elements in a <releases>-tag) found in PDSC file!" } },
  { "M306", { MsgLevel::LEVEL_ERROR,    CRLF_B, "No package description found in the PDSC file. Add the <description> tag\n"\
                                                "and provide a descriptive text.\n"\
                                                "For example: <description>Device Family Pack for  ...</description>." } },
  { "M307", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Either attribute '%TAG%' or '%TAG2%' + '%TAG3%' must be specified for 'memory'" } },
  { "M308", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Attribute '%TAG%' missing on '%TAG2%'" } },
  { "M309", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Attribute '%TAG%' missing on '%TAG2%' (when '%TAG3%' is specified)" } },
  { "M310", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Filename mismatch (case sensitive):\n"\
                                                "  PDSC name : '%PDSC%'\n"\
                                                "  Filename  : '%SYSTEM%'" } },
  { "M311", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Redefinition of '%TAG%' : '%NAME%', see Line %LINE%" } },
  { "M312", { MsgLevel::LEVEL_WARNING3, CRLF_B, "No '%TAG%' found for device '%NAME%'" } },
  { "M313", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Associated File/Path is outside PACK root: '%PATH%'" } },
  { "M314", { MsgLevel::LEVEL_WARNING,  CRLF_B, "Filename '%NAME%' contains whitespaces, this is not recommended" } },
  { "M315", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Invalid URL / Paths to Drives are not allowed in Package URL: '%URL%'" } },
  { "M316", { MsgLevel::LEVEL_WARNING,  CRLF_B, "URL must end with slash '/': '%URL%'" } },
  { "M317", { MsgLevel::LEVEL_ERROR,    CRLF_B, "" } },
  { "M318", { MsgLevel::LEVEL_ERROR,    CRLF_B, "" } },
  { "M319", { MsgLevel::LEVEL_ERROR,    CRLF_B, "" } },
  { "M320", { MsgLevel::LEVEL_ERROR,    CRLF_B, "" } },
  { "M321", { MsgLevel::LEVEL_ERROR,    CRLF_B, "DS5 project requires '.project' file." } },
  { "M322", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Associated File/Path not found: '%PATH%'" } },
  { "M323", { MsgLevel::LEVEL_ERROR,    CRLF_B, "File/Path not found: '%PATH%'. Check correct spelling." } },
  { "M324", { MsgLevel::LEVEL_WARNING,  CRLF_B, "Board referenced in Example '%EXAMPLE%' not found: '[%VENDOR%] %BOARD%'" } },
  { "M325", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Board '%NAME%' redefined, already defined in Line %LINE%: '%PATH%'" } },
  { "M326", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Path is not conformant: '%PATH%':\n  Absolute Paths or Drive references are not allowed, use Paths relative to PDSC file." } },
  { "M327", { MsgLevel::LEVEL_WARNING,  CRLF_B, "Path is not conformant: '%PATH%':\n  Backslashes are not recommended, use forward slashes." } },
  { "M328", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Version not set for Release Information '%DESCR%'" } },
  { "M329", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Description not set for Release Information '%VER%'" } },
  { "M330", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Condition redefined: '%COND%', already defined in Line %LINE%" } },
  { "M331", { MsgLevel::LEVEL_WARNING,  CRLF_B, "Condition unused: '%COND%'" } },
  { "M332", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Condition undefined: '%COND%'" } },
  { "M333", { MsgLevel::LEVEL_WARNING,  CRLF_B, "Component has no condition: 'Cclass=%CCLASS%, Cgroup=%CGROUP%, Csub=%CSUB%, Cversion=%CVER%'" } },
  { "M334", { MsgLevel::LEVEL_WARNING,  CRLF_B, "Config File has no version: '%PATH%'" } },
  { "M335", { MsgLevel::LEVEL_WARNING,  CRLF_B, "Component 'Cclass=%CCLASS%, Cgroup=%CGROUP%, Cversion=%CVER%' is member of the Cclass='%CCLASS%' which shall contain device specific components.\n"\
                                                "  No condition id set to specify a rule filtering for a device or set of devices." } },
  { "M336", { MsgLevel::LEVEL_WARNING,  CRLF_B, "Component 'Cclass=%CCLASS%, Cgroup=%CGROUP%, Cversion=%CVER%, condition=%COND%' is member of the Cclass='%CCLASS%' which shall contain device specific components.\n"\
                                                "  The referenced condition id does not contain a rule filtering for a device or set of devices." } },  // No reference to a device or device not found
  { "M337", { MsgLevel::LEVEL_WARNING,  CRLF_B, "File with category '%CAT%' has wrong extension '%EXT%': '%PATH%'" } },
  { "M338", { MsgLevel::LEVEL_WARNING,  CRLF_B, "No releases found!" } },
  { "M339", { MsgLevel::LEVEL_WARNING,  CRLF_B, "Include Path '%PATH%' must not be a file!" } },
  { "M340", { MsgLevel::LEVEL_WARNING,  CRLF_B, "Include Path '%PATH%' must end with '/'" } },
  { "M341", { MsgLevel::LEVEL_WARNING,  CRLF_B, "File with '%COMP%' dependency must have extension '%EXT%': '%PATH%'" } },
  { "M342", { MsgLevel::LEVEL_WARNING,  CRLF_B, "File with attribute '%ATTR%' must not have category '%CAT%': '%PATH%'" } },
  { "M343", { MsgLevel::LEVEL_WARNING,  CRLF_B, "File with attribute '%ATTR%' requires '%ATTR2%' attribute: '%PATH%'" } },
  { "M344", { MsgLevel::LEVEL_WARNING,  CRLF_B, "File shall have condition containing '%COND%': '%PATH%'" } },
  { "M345", { MsgLevel::LEVEL_WARNING,  CRLF_B, "URL not found: '%URL%'" } },
  { "M346", { MsgLevel::LEVEL_WARNING,  CRLF_B, "Referenced device(s) in '%BOARD%' not found: '%DEVICE%'" } },
  { "M347", { MsgLevel::LEVEL_WARNING,  CRLF_B, "Generator ID in Component 'Cclass=%CCLASS%, Cgroup=%CGROUP%, Cversion=%CVER%' is undefined: '%GENID%'" } },
  { "M348", { MsgLevel::LEVEL_WARNING,  CRLF_B, "Feature redefined for '%MCU%', see Line %LINE%: '%PROP%'" } },
  { "M349", { MsgLevel::LEVEL_WARNING,  CRLF_B, "Examples found, but no board description(s) found" } },
  { "M350", { MsgLevel::LEVEL_WARNING,  CRLF_B, "No '%COMP%' component found for [%VENDOR%] '%MCU%' (Compiler: %COMPILER% [%OPTION%])" } },
  { "M351", { MsgLevel::LEVEL_WARNING3, CRLF_B, "Component '%COMP%' (%COMPID%) error for [%VENDOR%] '%MCU%' (Compiler: %COMPILER% [%OPTION%]).%MSG%" } },
  { "M352", { MsgLevel::LEVEL_WARNING3, CRLF_B, "No Directories/Files found for '%COMP%' (%COMPID%) for [%VENDOR%] %MCU% (Compiler: %COMPILER% [%OPTION%])" } },
  { "M353", { MsgLevel::LEVEL_WARNING3, CRLF_B, "No '%FILECAT%' File found for Component '%COMP%' for '[%VENDOR%] %MCU%' (Compiler: %COMPILER% [%OPTION%])" } },
  { "M354", { MsgLevel::LEVEL_WARNING3, CRLF_B, "Multiple '%FILECAT%' Files found for Component '%COMP%' for '[%VENDOR%] %MCU%' (Compiler: %COMPILER% [%OPTION%])" } },
  { "M355", { MsgLevel::LEVEL_WARNING3, CRLF_B, "No '%FILECAT%' Directory found for Component '%COMP%' (%COMPID%) for '[%VENDOR%] %MCU%' (Compiler: %COMPILER% [%OPTION%])" } },
  { "M356", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Path '%PATH%' does not specify a file!" } },
  { "M357", { MsgLevel::LEVEL_WARNING,  CRLF_B, "" } },
  { "M358", { MsgLevel::LEVEL_WARNING3, CRLF_B, "Header File '%HFILE%' for '%CFILE%', used by Component '%COMP%' (%COMPID%)\n"\
                                                "  for Device '[%VENDOR%] %MCU%' (Compiler: %COMPILER% [%OPTION%]) not found on file system in any include paths: %PATH%" } },
  { "M359", { MsgLevel::LEVEL_WARNING,  CRLF_B, "Family has no Device(s) or Subfamilies: '%FAMILY%" } },
  { "M360", { MsgLevel::LEVEL_WARNING,  CRLF_B, "Subfamily has no Device(s): '%SUBFAMILY%" } },
  { "M361", { MsgLevel::LEVEL_WARNING,  CRLF_B, "Generator ID in Taxonomy 'Cclass=%CCLASS%, Cgroup=%CGROUP%' is undefined: '%GENID%'" } },
  { "M362", { MsgLevel::LEVEL_WARNING,  CRLF_B, "Not all Component Dependencies for 'Cclass=%CCLASS%, Cgroup=%CGROUP%, Csub=%CSUB%, Cversion=%CVER%, Capiversion=%APIVER%' can be resolved.%MSG%" } },
  { "M363", { MsgLevel::LEVEL_WARNING,  CRLF_B, "No API defined for Component 'Cclass=%CCLASS%, Cgroup=%CGROUP%, Csub=%CSUB%, Cversion=%CVER%, Capiversion=%APIVER%'" } },
  { "M364", { MsgLevel::LEVEL_WARNING,  CRLF_B, "No Devices for Condition '%COND% [%VENDOR%] %MCU%' available." } },
  { "M365", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Redefined %DEVTYPE% '%MCU%' found, see Line %LINE%" } },
  { "M366", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Redefined %DEVTYPEEXIST% as %DEVTYPE% '%MCU%' found, see Line %LINE%" } },
  { "M367", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Redefined %TYPE% '%NAME%' found, see Line %LINE%" } },
  { "M368", { MsgLevel::LEVEL_ERROR,    CRLF_B, "" } },
  { "M369", { MsgLevel::LEVEL_WARNING3, CRLF_B, "Feature is already defined for '%MCU%' and will be added, see Line %LINE%: '%PROP%'" } },
  { "M370", { MsgLevel::LEVEL_WARNING,  CRLF_B, "URL is not conformant: '%URL%':\n  Backslashes are not allowed in URL, use forward slashes." } },
  { "M371", { MsgLevel::LEVEL_ERROR,    CRLF_B, "%SECTION% Feature for '%MCU%': '%FEATURE%' unknown." } },
  { "M372", { MsgLevel::LEVEL_ERROR,    CRLF_B, "%SECTION% Feature for '%MCU%': '%FEATURE%' misspelled, did you mean '%KNOWNFEATURE%' (%DESCR%)." } },
  { "M373", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Unsupported Schema Version: '%VER%'" } },
  { "M374", { MsgLevel::LEVEL_ERROR,    CRLF_B, "While checking Feature for '%MCU%': Pname '%CPU%' not found." } },
  { "M375", { MsgLevel::LEVEL_ERROR,    CRLF_B, "No <mountedDevices> for Board '%BOARD%' found." } },
  { "M376", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Schema Version not set!" } },
  { "M377", { MsgLevel::LEVEL_WARNING,  CRLF_B, "File '%NAME%' %TYPE% must have 'attr=\"config\"'" } },
  { "M378", { MsgLevel::LEVEL_WARNING,  CRLF_B, "Component 'Cclass=%CCLASS%, Cgroup=%CGROUP%, Csub=%CSUB%, Cversion=%CVER%, implements the API defined in '%NAME%' but does not attribute 'Capiversion' specifying the version it implements." } },
  { "M379", { MsgLevel::LEVEL_WARNING,  CRLF_B, "No example(s) found for Board '[%VENDOR%] %BOARD%'." } },
  { "M380", { MsgLevel::LEVEL_WARNING,  CRLF_B, "No description found for [%VENDOR%] '%MCU%'" } },
  { "M381", { MsgLevel::LEVEL_WARNING,  CRLF_B, "Vendor names are not equal: '[%VENDOR%] %MCU%', MCU '[%VENDOR2%] %MCU2%', see Line %LINE%" } },
  { "M382", { MsgLevel::LEVEL_WARNING,  CRLF_B, "Requirement <%TAG%> '[%VENDOR%] %NAME% %VER%' could not be resolved.%MSG%. Use option -i PackName.pdsc to resolve external dependencies." } },
  { "M383", { MsgLevel::LEVEL_ERROR,    CRLF_B, "%TAG% '%NAME%' is not conformant to the pattern \"%CHAR%\"" } },
  { "M384", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Condition '%NAME%': Direct or indirect recursion detected. Skipping condition while searching for '%NAME2%'." } },
  { "M385", { MsgLevel::LEVEL_INFO,     CRLF_B, "Release date is empty." } },
  { "M386", { MsgLevel::LEVEL_WARNING,  CRLF_B, "Release date is in future: '%RELEASEDATE%' (today: %TODAYDATE%)" } },
  { "M389", { MsgLevel::LEVEL_WARNING,  CRLF_B, "The component '%NAME%' (Line %LINE%) has dependency '%EXPR%' : '%NAME2%' that is resolved by the component itself." } },
  { "M390", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Condition '%NAME%': Direct or indirect recursion detected. %MSG%" } },
  { "M391", { MsgLevel::LEVEL_WARNING3, CRLF_B, "Redefined Item '%NAME%': %MSG%" } },
  { "M392", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Redefined Device or Variant '%NAME%': %MSG%" } },
  { "M393", { MsgLevel::LEVEL_WARNING3, CRLF_B, "Package has development version '%DEVVERSION%': '%RELEASEVER%'" } },
  { "M394", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Not Semantic Versioning, ignoring version: '%RELEASEVER%'" } },
  { "M395", { MsgLevel::LEVEL_WARNING3, CRLF_B, "Release date attribute is not set for release version: '%RELEASEVER%'" } },
  { "M396", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Release %TAG% not consecutive (newest first): '%RELEASEVER%', '%RELEASEDATE%' (prev.: '%LATESTVER%', '%LATESTDATE%', see Line %LINE%)" } },
  { "M397", { MsgLevel::LEVEL_WARNING,  CRLF_B, "File extension '.pdsc' must be lowercase: '%PATH%'" } },
  { "M398", { MsgLevel::LEVEL_ERROR,    CRLF_B, "Attribute 'Dname' missing in expression using 'Pname=\"%NAME%\"" } },
  { "M399", { MsgLevel::LEVEL_WARNING3, CRLF_B, "Attribute '%TAG%' is ignored, because '%TAG2%' + '%TAG3%' is specified" } },

  // 400... Xml Reader Errors

  // 500... Model Errors
  { "M500", { MsgLevel::LEVEL_TEXT,     CRLF_B, "RTE Model reports: %MSG%" } },
  { "M501", { MsgLevel::LEVEL_TEXT ,    CRLF_B, "" } },
  { "M502", { MsgLevel::LEVEL_TEXT,     CRLF_NO,"RTE Model reports: #error %NUM%:  %NAME% : %MSG%" } },
  { "M503", { MsgLevel::LEVEL_TEXT ,    CRLF_B, "" } },
  { "M504", { MsgLevel::LEVEL_TEXT,     CRLF_B, "RTE Model reports: MISSING:     -- %SPACE%%NAME%" } },
  { "M505", { MsgLevel::LEVEL_ERROR,    CRLF_B, "RTE Model reports: %MSG%" } },
  { "M506", { MsgLevel::LEVEL_WARNING,  CRLF_B, "RTE Model reports: %MSG%" } },
};

const MsgTableStrict PackChk::msgStrictTable = {
  //{ "M200",   MsgLevel::LEVEL_ERROR },
};

