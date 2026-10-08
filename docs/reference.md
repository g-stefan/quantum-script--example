# API reference

## Script

Load with `Script.requireExtension("Example")`
(file `quantum-script--example.dll` / `quantum-script--example.so`).

| Symbol | Returns | Behavior |
|--------|---------|----------|
| `Example` | Object | the extension object, created by `var Example={};` |
| `Example.print(str)` | `undefined` | writes `str.toString()` to stdout, no new line; on conversion error writes the type name; never throws |
| `Example.process(fn)` | what `fn` returns | calls `fn("Hello")` with `this` = `undefined`; exceptions from `fn` propagate; `Error: functionApply` if `fn` is not a function |

`Script.getExtensionList()` entry: `name` `"Example"`, `version`
`"<version>.<build>"`, `info` `"Example\r\n<copyright>\r\n<short MIT license>\r\n"`,
`fileName` the loaded library path (`""` when internal).

Details: [Script API](script-api.md).

## C++

Include `<XYO/QuantumScript.Extension/Example.hpp>`; depend on
`"quantum-script--example"` in `fabricare.json`.

### `XYO::QuantumScript::Extension::Example`

| Declaration | Purpose |
|-------------|---------|
| `void initExecutive(Executive *executive, void *extensionId)` | defines `Example`, `Example.print`, `Example.process`; sets name, info, version, public flag. Called by the engine when a script loads the extension |
| `void registerInternalExtension(Executive *executive)` | registers `initExecutive` under the name `"Example"`, for hosts that link the extension in |

### `XYO::QuantumScript::Extension::Example::Version`

| Declaration | Value (5.9.0 build 6) |
|-------------|-----------------------|
| `const char *version()` | `"5.9.0"` |
| `const char *build()` | `"6"` |
| `const char *versionWithBuild()` | `"5.9.0.6"` |
| `const char *datetime()` | `"2026-09-16 23:03:08"` |

### `XYO::QuantumScript::Extension::Example::Copyright`

| Declaration | Value |
|-------------|-------|
| `const char *copyright()` | `"Copyright (c) 2016-2026 Grigore Stefan <g_stefan@yahoo.com>"` |
| `const char *publisher()` | `"Grigore Stefan"` |
| `const char *company()` | same as `publisher()` |
| `const char *contact()` | `"g_stefan@yahoo.com"` |

### `XYO::QuantumScript::Extension::Example::License`

| Declaration | Value |
|-------------|-------|
| `std::string license()` | MIT header + copyright + full MIT text |
| `std::string shortLicense()` | copyright + one line MIT notice |

### Exported C symbol (shared library builds only)

```cpp
extern "C" void quantumScriptExtension(XYO::QuantumScript::Executive *executive, void *extensionId);
```

Looked up by the engine after loading `quantum-script--example.dll` / `.so`;
forwards to `Example::initExecutive`.

### Macros

| Macro | Meaning |
|-------|---------|
| `XYO_QUANTUMSCRIPT_EXTENSION_EXAMPLE_EXPORT` | `dllexport` while building, `dllimport` when using, empty for static |
| `XYO_QUANTUMSCRIPT_EXTENSION_EXAMPLE_INTERNAL` | building this library (set from `QUANTUM_SCRIPT__EXAMPLE_INTERNAL`) |
| `XYO_QUANTUMSCRIPT_EXTENSION_EXAMPLE_LIBRARY` | static use: empty export macro, no `quantumScriptExtension` |
| `XYO_QUANTUMSCRIPT_EXTENSION_EXAMPLE_NO_VERSION` | version strings become `0.0.0` |
| `XYO_QUANTUMSCRIPT_EXTENSION_EXAMPLE_VERSION_ABCD`, `_VERSION_STR`, `_VERSION_STR_BUILD`, `_VERSION_STR_WITH_BUILD`, `_VERSION_STR_DATETIME` | generated in `Version.rh` |
| `XYO_QUANTUMSCRIPT_EXTENSION_EXAMPLE_COPYRIGHT`, `_PUBLISHER`, `_COMPANY`, `_CONTACT` | `Copyright.rh` |

## Build

| Item | Value |
|------|-------|
| fabricare project | `quantum-script--example`, `"make": "dll-or-lib"` |
| source path | `source/XYO/QuantumScript.Extension/Example` |
| dependencies | `quantum-script`, `quantum-script--console` |
| outputs | `bin/quantum-script--example.dll` + `lib/quantum-script--example.lib` (Windows), `quantum-script--example.so` (Linux), `lib/quantum-script--example.lib` on static platforms |
| commands | `fabricare make`, `install`, `test`, `clean`, `version`, `release` |
| tests | `test/test.0001.js`, run by `fabricare/test.js` with the SDK `quantum-script` |
