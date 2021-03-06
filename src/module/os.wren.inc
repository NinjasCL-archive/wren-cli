// Please do not edit this file. It has been generated automatically
// from `src/module/os.wren` using `util/wren_to_c_string.py`

static const char* osModuleSource =
"class Platform {\n"
"  foreign static isPosix\n"
"  foreign static name\n"
"\n"
"  static isWindows { name == \"Windows\" }\n"
"}\n"
"\n"
"class Process {\n"
"  // TODO: This will need to be smarter when wren supports CLI options.\n"
"  static arguments { allArguments.count >= 2 ? allArguments[2..-1] : [] }\n"
"\n"
"  foreign static allArguments\n"
"  foreign static cwd\n"
"  foreign static pid\n"
"  foreign static ppid\n"
"  foreign static version\n"
"}\n";
