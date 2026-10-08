# How it works

A walk through every file of the extension, in the order the engine uses
them. The C++ rules of the underlying libraries (`TPointer`, `String`,
`Error`, ...) are documented in the `quantum-script`, `xyo-system` and
`xyo-managed-memory` repositories.

## 1. Loading: from `requireExtension` to `initExecutive`

```
script:  Script.requireExtension("Example")
engine:  "quantum-script--" + lowercase(name) + ".dll" / ".so"
         found as given or in an include path folder?
           yes -> load the library, call quantumScriptExtension(executive, extensionId)
           no  -> internal extension registered as "Example"? call its initExecutive
         else throw Error("Unable to open \"Example\"")
both:    Extension::Example::initExecutive(executive, extensionId)
         -> defines the global Example and its functions
```

Two entry points, one implementation:

```cpp
// Library.cpp — DLL / .so build: the engine looks this symbol up by name
#ifndef XYO_QUANTUMSCRIPT_EXTENSION_EXAMPLE_LIBRARY
#	ifdef XYO_PLATFORM_COMPILE_DYNAMIC_LIBRARY
extern "C" XYO_QUANTUMSCRIPT_EXTENSION_EXAMPLE_EXPORT void quantumScriptExtension(XYO::QuantumScript::Executive *executive, void *extensionId) {
	XYO::QuantumScript::Extension::Example::initExecutive(executive, extensionId);
};
#	endif
#endif

// internal build: the host calls this before running scripts
void registerInternalExtension(Executive *executive) {
	executive->registerInternalExtension("Example", initExecutive);
};
```

- `quantumScriptExtension` must be `extern "C"` and exported, with exactly
  this name and signature; it is only compiled when building a shared
  library (`XYO_PLATFORM_COMPILE_DYNAMIC_LIBRARY`) and not when
  `XYO_QUANTUMSCRIPT_EXTENSION_EXAMPLE_LIBRARY` is defined.
- `registerInternalExtension` is always compiled. It does not run any
  script code; it just maps the name to `initExecutive`.

## 2. `initExecutive`: define the script side

```cpp
void initExecutive(Executive *executive, void *extensionId) {
	String info = "Example\r\n";
	info << License::shortLicense().c_str();

	executive->setExtensionName(extensionId, "Example");
	executive->setExtensionInfo(extensionId, info);
	executive->setExtensionVersion(extensionId, Extension::Example::Version::versionWithBuild());
	executive->setExtensionPublic(extensionId, true);

	executive->compileStringX("var Example={};");
	executive->setFunction2("Example.print(str)", print);
	executive->setFunction2("Example.process(fn)", process);
};
```

| Call | Effect |
|------|--------|
| `setExtensionName` | the name in `Script.getExtensionList()` and for "already loaded" checks |
| `setExtensionInfo` | the `info` text: name + copyright + short license |
| `setExtensionVersion` | the `version` text, `"5.9.0.6"` (version.build) |
| `setExtensionPublic(true)` | listed by `Script.getExtensionList()`; `false` hides helper extensions |
| `compileStringX("var Example={};")` | runs script code: creates the global object. **Must come before** registering functions on it |
| `setFunction2("Example.print(str)", print)` | stores a native function calling the C++ `print` at `Example.print`; the text before `(` is the property path, the parameter list is documentation only (the C++ side receives all arguments) |

`initExecutive` runs once per executive (one per thread), the first time a
script loads the extension. Anything a script uses must be defined here.
To depend on another extension, load it from here:
`executive->compileStringX("Script.requireExtension(\"Buffer\");");`.

## 3. Native functions

Every function registered with `setFunction2` has the same signature:

```cpp
static TPointer<Variable> name(VariableFunction *function, Variable *this_, VariableArray *arguments);
```

| Parameter | Meaning |
|-----------|---------|
| `function` | the script function object being called (rarely needed) |
| `this_` | the script `this` (the object before the dot for methods) |
| `arguments` | the script arguments; `arguments->index(k)` returns the k-th, `undefined` past the end |
| return | any `Variable`; `Context::getValueUndefined()` for "no value" |

### `print`: script → C++

```cpp
static TPointer<Variable> print(VariableFunction *function, Variable *this_, VariableArray *arguments) {
	try {
		printf("%s", ((arguments->index(0))->toString()).value());
		return Context::getValueUndefined();
	} catch (const Error &e) {
	};

	printf("%s", ((arguments->index(0))->getVariableType()).value());
	return Context::getValueUndefined();
};
```

- Read an argument: `arguments->index(0)`.
- Convert it: `toString()` (also `toNumber()`, `toBoolean()`, ...).
- Return: `Context::getValueUndefined()`, or a new value
  (`VariableString::newVariable(...)`, `VariableNumber::newVariable(...)`,
  `VariableBoolean::newVariable(...)`, ...).
- Errors: to report a bad argument to the script, `throw Error("...")`; it
  becomes a script exception. `print` instead catches it and falls back to
  the type name, as a demonstration.

### `process`: C++ → script

```cpp
static TPointer<Variable> process(VariableFunction *function, Variable *this_, VariableArray *arguments) {
	TPointer<VariableArray> applyArguments(VariableArray::newArray());
	(applyArguments->index(0)) = VariableString::newVariable("Hello");
	return (arguments->index(0))->functionApply(Context::getValueUndefined(), applyArguments);
};
```

- Build the argument list as a `VariableArray`.
- `functionApply(this_, arguments)` calls a script (or native) function and
  returns its result. The base `Variable::functionApply` throws
  `Error("functionApply")`, which is what happens for a non-function.
- Script exceptions thrown by the callback pass through as C++ exceptions
  and come back out as script exceptions; let them propagate.

Each function starts with an
`#ifdef XYO_QUANTUMSCRIPT_DEBUG_RUNTIME ... printf("- example-print\n")`
trace. `XYO_QUANTUMSCRIPT_DEBUG_RUNTIME` is the debug switch in the engine's
`Config.hpp`; enabling it there makes every native function print its name
when called.

## 4. Export macro: `Dependency.hpp`

```cpp
#include <XYO/QuantumScript.hpp>

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_EXAMPLE_INTERNAL
#	ifdef QUANTUM_SCRIPT__EXAMPLE_INTERNAL          // defined by fabricare when building this project
#		define XYO_QUANTUMSCRIPT_EXTENSION_EXAMPLE_INTERNAL
#	endif
#endif

#ifdef XYO_QUANTUMSCRIPT_EXTENSION_EXAMPLE_INTERNAL
#	define XYO_QUANTUMSCRIPT_EXTENSION_EXAMPLE_EXPORT XYO_PLATFORM_LIBRARY_EXPORT   // building the DLL
#else
#	define XYO_QUANTUMSCRIPT_EXTENSION_EXAMPLE_EXPORT XYO_PLATFORM_LIBRARY_IMPORT   // using the DLL
#endif
#ifdef XYO_QUANTUMSCRIPT_EXTENSION_EXAMPLE_LIBRARY                                 // static library
#	undef XYO_QUANTUMSCRIPT_EXTENSION_EXAMPLE_EXPORT
#	define XYO_QUANTUMSCRIPT_EXTENSION_EXAMPLE_EXPORT
#endif
```

- The fabricare build defines `<PROJECT_NAME>_INTERNAL` (project name upper
  case, `-` → `_`: `quantum-script--example` →
  `QUANTUM_SCRIPT__EXAMPLE_INTERNAL`) while compiling the project itself,
  so the functions are exported from the DLL and imported by its users.
- On static platforms `XYO_PLATFORM_COMPILE_STATIC` makes
  `XYO_PLATFORM_LIBRARY_EXPORT` / `IMPORT` empty anyway.
- Every public function is declared with
  `XYO_QUANTUMSCRIPT_EXTENSION_EXAMPLE_EXPORT`; the native functions
  (`print`, `process`) are `static` and not exported.

## 5. Headers

| File | Role |
|------|------|
| `Example.hpp` | umbrella header; what users include |
| `Example/Library.hpp` | `initExecutive`, `registerInternalExtension` |
| `Example/Dependency.hpp` | engine include and export macro; included first by every header |
| `Example/Copyright.hpp`, `License.hpp`, `Version.hpp` | metadata accessors |

Include guards follow `XYO_QUANTUMSCRIPT_EXTENSION_EXAMPLE_<FILE>_HPP`, and
every include is wrapped in `#ifndef <guard>` like the rest of the XYO code.

## 6. One translation unit: `Example.Amalgam.cpp`

```cpp
#include <XYO/QuantumScript.Extension/Example.hpp>
#include <XYO/QuantumScript.Extension/Example/Copyright.cpp>
#include <XYO/QuantumScript.Extension/Example/License.cpp>
#include <XYO/QuantumScript.Extension/Example/Version.cpp>
#include <XYO/QuantumScript.Extension/Example/Library.cpp>
```

Includes every `.cpp` so the whole extension can be compiled as a single
file (for example in a host that prefers amalgamated sources). fabricare
compiles the files of `sourcePath` individually and does not use it. Add
every new `.cpp` here too.

## 7. Metadata

| File | Content |
|------|---------|
| `Copyright.rh` | `..._COPYRIGHT`, `_PUBLISHER`, `_COMPANY`, `_CONTACT` strings |
| `Copyright.cpp` | `Copyright::copyright()`, `publisher()`, `company()`, `contact()` |
| `License.cpp` | `License::license()` (full MIT text), `shortLicense()` (copyright + one line); used for the extension `info` |
| `Version.Template.rh` | template with `#{VERSION_ABCD}`, `#{VERSION_VERSION}`, `#{VERSION_BUILD}`, `#{VERSION_DATETIME}` |
| `Version.rh` | **generated** by `fabricare version` from the template and `version.json`; do not edit by hand |
| `Version.cpp` | `Version::version()` `"5.9.0"`, `build()` `"6"`, `versionWithBuild()` `"5.9.0.6"`, `datetime()` |
| `Library.rc`, `Library.rh` | Windows version resource of the DLL (`XYO_PLATFORM_VERSION_INFO`) |

Defining `XYO_QUANTUMSCRIPT_EXTENSION_EXAMPLE_NO_VERSION` replaces all
version strings with `0.0.0` / `0.0.0.0`.

## 8. Build and test files

- `fabricare.json` — one project, `"make": "dll-or-lib"`,
  `"sourcePath": "XYO/QuantumScript.Extension/Example"`, dependencies
  `quantum-script` and `quantum-script--console`.
- `version.json` — `version`, `build`, `date`, `time` of the project.
- `fabricare/test.js` — runs `quantum-script --execution-time test/test.000k.js`
  for each test and stops at the first non-zero exit code.
- `test/test.0001.js` — loads `Console`, `Example`, `JSON`, prints
  `Hello world!` and the extension list.

## Using other extensions' C++ types

The template depends only on `quantum-script` (and `quantum-script--console`
for the test). If your extension takes or returns values of another
extension, for example `Buffer`, add that project to the `fabricare.json`
dependencies (`"quantum-script--buffer"`) and include its header
(`<XYO/QuantumScript.Extension/Buffer/VariableBuffer.hpp>`). Do not rely on
a header being found only because the SDK include folder is shared.
