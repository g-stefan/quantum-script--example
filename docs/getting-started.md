# Getting started

## 1. Requirements

The extension is built with [fabricare](https://github.com/g-stefan/fabricare),
the build tool of all XYO C++ projects. Before building, these must be
installed to the SDK (`~/.fabricare/<platform>`, e.g.
`~/.fabricare/win64-msvc-2026`):

- `quantum-script` (and everything below it: `xyo-system`, `xyo-encoding`,
  `xyo-multithreading`, `xyo-data-structures`, `xyo-managed-memory`,
  `xyo-platform`);
- `quantum-script--console` (listed as a dependency, used by the test);
- `quantum-script--json` is used by `test/test.0001.js` at run time only.

## 2. Build, install, test

From the repository root:

```bash
fabricare make       # build into output/ (bin, include, lib)
fabricare install    # copy output/ to ~/.fabricare/<platform>
fabricare test       # run test/test.0001.js with quantum-script
fabricare clean      # remove output/ and temp/
fabricare version    # bump the build number in version.json, regenerate Version.rh
```

The same actions are available as VS Code tasks (`.vscode/tasks.json`:
Build, Test, Clean, Install, Version, Check licenses).

The project is `"make": "dll-or-lib"`: the platform decides what is built.

| Platform | Output | Used by |
|----------|--------|---------|
| dynamic (`win64-msvc-2026`, `ubuntu-24.04`, ...) | `bin/quantum-script--example.dll` (Windows) / `quantum-script--example.so` (Linux), plus `lib/quantum-script--example.lib` import library and headers | the `quantum-script` interpreter, any host using the engine DLL |
| static (`win64-msvc-2026.static`) | `lib/quantum-script--example.lib` static library and headers | self-contained hosts that register the extension as internal |

### About `fabricare test`

`fabricare/test.js` runs `quantum-script --execution-time test/test.0001.js`
with the **SDK** interpreter (fabricare puts `~/.fabricare/<platform>/bin`
first in `PATH`). The interpreter looks for `quantum-script--example.dll`
in its own folder and in the script's folder (`test/`), **not** in
`output/bin`. So run `fabricare install` before `fabricare test`, or the
test runs against the previously installed version (or fails with
`Unable to open "Example"`).

Expected output:

```
Hello world!
[
	{ "fileName": ".../quantum-script--json.dll", "name": "JSON", ... },
	{ "fileName": ".../quantum-script--example.dll", "name": "Example", "info": "Example\r\nCopyright ...", "version": "5.9.0.6" },
	{ "fileName": ".../quantum-script--console.dll", "name": "Console", ... }
]
-> test 0001 ok
```

## 3. Use it from a script

```javascript
// hello.js
Script.requireExtension("Example");

Example.print("Hello from C++\n");

var n = Example.process(function(text) {
	return text.length;
});
Example.print(n);          // 5 ("Hello".length)
Example.print("\n");
```

```bash
quantum-script hello.js
```

`Script.requireExtension("Example")` (also `"example"`, the name is
case insensitive) loads `quantum-script--example.dll` / `.so` from the
interpreter's folder, the script's folder or any folder added with
`Script.setIncludePath`. Loading twice does nothing.

## 4. Use it from a fabricare script

fabricare build scripts are Quantum Script programs; an installed
extension is loaded the same way:

```javascript
Script.requireExtension("Example");
Example.print("building...\n");
```

## 5. Link it into a C++ host (internal extension)

A host that runs scripts with `ExecutiveX` can link the extension in and
register it by name, so scripts load it without a DLL on disk.

`fabricare.json` of the host:

```json
"dependency": [
	"quantum-script",
	"quantum-script--example"
]
```

```cpp
#include <XYO/QuantumScript.hpp>
#include <XYO/QuantumScript.Extension/Example.hpp>

using namespace XYO::QuantumScript;

static void initExecutive(Executive *executive) {
	Extension::Example::registerInternalExtension(executive);
};

int main(int argc, char *argv[]) {
	if (ExecutiveX::initExecutive(argc, argv, initExecutive)) {
		if (ExecutiveX::executeString(
		        "Script.requireExtension(\"Example\");"
		        "Example.print(Example.process(function(x){ return x + \" from host\\n\"; }));")) {
			ExecutiveX::endProcessing();
			return 0;
		};
	};
	printf("%s\n", ExecutiveX::getError().value());
	printf("%s", ExecutiveX::getStackTrace().value());
	ExecutiveX::endProcessing();
	return 1;
};
```

Notes:

- `registerInternalExtension` only registers the name; `initExecutive` of
  the extension runs when a script calls `Script.requireExtension("Example")`.
- With the DLL build of the engine, `Script.requireExtension` tries an
  external `quantum-script--example.dll` **first**; use
  `Script.requireInternalExtension("Example")` or
  `Script.requireExtensionInternalOrExternal("Example")` to prefer the
  linked-in copy.
- On a static platform the same code links the static library; the export
  macros are empty and the `quantumScriptExtension` DLL entry point is not
  compiled.

## Next

- [Script API](script-api.md) — exact behavior of `print` and `process`.
- [How it works](how-it-works.md) — what each source file does.
- [Create your own extension](new-extension.md) — turn this into a new
  `quantum-script--name`.
