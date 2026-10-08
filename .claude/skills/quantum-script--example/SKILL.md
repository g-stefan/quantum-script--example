---
name: quantum-script--example
description: >-
  How to use the Quantum Script Extension Example (quantum-script--example),
  the reference template for writing a quantum-script--* native extension:
  the script API loaded with Script.requireExtension("Example") -
  Example.print(str) (C++ called from a script, writes to stdout, no new
  line, returns undefined) and Example.process(fn) (C++ calling back into the
  script, returns fn("Hello"), throws Error: functionApply for a
  non-function); the extension anatomy - initExecutive (setExtensionName /
  Info / Version / Public, compileStringX("var Example={};"), setFunction2),
  registerInternalExtension, the extern "C" quantumScriptExtension DLL entry
  point, the native function signature (VariableFunction *, Variable *this_,
  VariableArray *arguments), functionApply, Dependency.hpp export macro
  (XYO_QUANTUMSCRIPT_EXTENSION_EXAMPLE_EXPORT / _INTERNAL / _LIBRARY,
  QUANTUM_SCRIPT__EXAMPLE_INTERNAL), Copyright / License / Version metadata,
  Version.Template.rh, Library.rc, Example.Amalgam.cpp; building with
  fabricare (dll-or-lib, make / install / test) and creating a new extension
  by copying and renaming Example / EXAMPLE / example. Use when writing,
  reviewing or debugging a new Quantum Script extension, renaming this
  template, linking Example into a C++ host, or when working inside the
  quantum-script--example repository.
---

# quantum-script--example

Template extension of Quantum Script (see the `quantum-script` skill for the
language, embedding, the `Variable` API and `writing-extensions`; see the
`fabricare` skill for the build tool; their rules apply). Purpose: **the
smallest complete `quantum-script--*` extension**, to read and to copy. It
is not meant to be depended on for functionality.

Full documentation: `docs/` in the quantum-script--example repository
(`X:\Storage\XYO\Gitea\CPP\quantum-script--example\docs` on this machine):
README, getting-started, script-api, **how-it-works** (every file explained),
**new-extension** (copy, rename, add functions, test, checklist), reference.
Read the matching page when you need more than this summary. The real code
is `source/XYO/QuantumScript.Extension/Example/Library.cpp` (~80 lines).

## Script API

```javascript
Script.requireExtension("Example");   // quantum-script--example.dll / .so; name case insensitive

Example.print(x);        // printf("%s", x.toString()); no new line; returns undefined; never throws
                         // missing -> "undefined", null -> "null", [1,2] -> "1,2", {a:1} -> "Object"
Example.process(fn);     // returns fn("Hello"); this = undefined; one argument only
                         // exceptions thrown by fn propagate; non-function -> Error: functionApply

Example.print(Example.process(function(x) { return x + " world!\r\n"; }));   // Hello world!
```

`Script.getExtensionList()` lists `{fileName, name: "Example", info,
version: "5.9.0.6"}`.

## Anatomy (what every extension needs)

```cpp
namespace XYO::QuantumScript::Extension::Example {

	// native function: fixed signature, static, not exported
	static TPointer<Variable> print(VariableFunction *function, Variable *this_, VariableArray *arguments) {
		printf("%s", ((arguments->index(0))->toString()).value());   // index past end = undefined
		return Context::getValueUndefined();
	};

	// C++ -> script call
	static TPointer<Variable> process(VariableFunction *function, Variable *this_, VariableArray *arguments) {
		TPointer<VariableArray> applyArguments(VariableArray::newArray());
		(applyArguments->index(0)) = VariableString::newVariable("Hello");
		return (arguments->index(0))->functionApply(Context::getValueUndefined(), applyArguments);
	};

	void registerInternalExtension(Executive *executive) {          // exported; for hosts linking it in
		executive->registerInternalExtension("Example", initExecutive);
	};

	void initExecutive(Executive *executive, void *extensionId) {   // exported; once per executive (thread)
		String info = "Example\r\n";
		info << License::shortLicense().c_str();
		executive->setExtensionName(extensionId, "Example");
		executive->setExtensionInfo(extensionId, info);
		executive->setExtensionVersion(extensionId, Extension::Example::Version::versionWithBuild());
		executive->setExtensionPublic(extensionId, true);           // listed by getExtensionList
		executive->compileStringX("var Example={};");               // object FIRST
		executive->setFunction2("Example.print(str)", print);       // path before "(", params = docs only
		executive->setFunction2("Example.process(fn)", process);
	};
};

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_EXAMPLE_LIBRARY
#	ifdef XYO_PLATFORM_COMPILE_DYNAMIC_LIBRARY
extern "C" XYO_QUANTUMSCRIPT_EXTENSION_EXAMPLE_EXPORT void quantumScriptExtension(XYO::QuantumScript::Executive *executive, void *extensionId) {
	XYO::QuantumScript::Extension::Example::initExecutive(executive, extensionId);
};
#	endif
#endif
```

| File | Role |
|------|------|
| `Example.hpp` | umbrella header users include |
| `Example.Amalgam.cpp` | includes every `.cpp` (single TU); not used by fabricare, keep it in sync |
| `Example/Dependency.hpp` | `<XYO/QuantumScript.hpp>` + export macro (`_INTERNAL` from `QUANTUM_SCRIPT__EXAMPLE_INTERNAL` = building it; `_LIBRARY` = static) |
| `Example/Library.hpp/.cpp` | `initExecutive`, `registerInternalExtension`, native functions, DLL entry |
| `Example/Copyright.*`, `License.*` | metadata; `shortLicense()` feeds the `info` text |
| `Example/Version.Template.rh` → `Version.rh` | **generated** by `fabricare version` from `version.json`; never edit `Version.rh` |
| `Example/Library.rc/.rh` | Windows DLL version resource |
| `fabricare.json` | one project, `"make": "dll-or-lib"`, deps `quantum-script`, `quantum-script--console` |
| `test/test.0001.js`, `fabricare/test.js` | script tests run by the SDK `quantum-script` |

## Hard rules

1. **DLL name = `quantum-script--` + lower case extension name** (`.dll` on
   Windows, `.so` on Linux, no `lib` prefix). The engine searches the path
   as given, then the include path (interpreter folder, script folder,
   `Script.setIncludePath`).
2. **`quantumScriptExtension` is the only entry point the engine looks up**:
   `extern "C"`, exported, exact signature, guarded by
   `XYO_PLATFORM_COMPILE_DYNAMIC_LIBRARY` and not `_LIBRARY`.
3. **Create the object before its functions**: `compileStringX("var X={};")`
   then `setFunction2("X.f(a)", f)`. Methods on a prototype need the
   constructor to exist first.
4. **`setExtensionName` must equal the `registerInternalExtension` name.**
5. **`initExecutive` runs per executive (per thread)**: no script values in
   C++ globals; per thread state goes in a `TSingletonThread` context with
   `setExtensionDeleteContext`.
6. **Errors**: `throw Error("...")` becomes a script exception; let script
   exceptions from `functionApply` propagate.
7. **`fabricare test` uses the installed DLL**, not `output/bin`: run
   `fabricare make` then `fabricare install` then `fabricare test`.
8. **Every included extension / library is a `fabricare.json` dependency**
   (e.g. `"quantum-script--buffer"` before including
   `Buffer/VariableBuffer.hpp`); do not rely on the shared SDK include folder.
   Debug traces use `#ifdef XYO_QUANTUMSCRIPT_DEBUG_RUNTIME` (the engine's
   `Config.hpp` switch).

## New extension from the template (`Hello`)

```bash
git archive HEAD | tar -x -C ../quantum-script--hello      # no .git/output/temp/release
cd ../quantum-script--hello && rm -rf docs .claude && git init
cd source/XYO/QuantumScript.Extension && mv Example Hello && mv Example.hpp Hello.hpp && mv Example.Amalgam.cpp Hello.Amalgam.cpp && cd ../../..
grep -rlE "Example|EXAMPLE|example" source fabricare.json version.json test README.md .reuse/dep5 \
	| xargs sed -i -e "s/EXAMPLE/HELLO/g" -e "s/Example/Hello/g" -e "s/example/hello/g"
# version.json: "1.0.0", build "0"; then:
fabricare version && fabricare make && fabricare install && fabricare test
```

Then: replace `print` / `process`, update `Library.rc` description,
`.reuse/dep5` `Upstream-Name` / `Source`, copyright if different, add
dependencies to `fabricare.json` (`"quantum-script--buffer"`,
`"xyo-cryptography"`, ...), add tests (raise the loop bound in
`fabricare/test.js`). Full checklist: `docs/new-extension.md`.

## Native function idioms

| Need | Code |
|------|------|
| argument k | `arguments->index(k)` (`TPointerX<Variable> &`) |
| required argument | `if (TIsTypeExact<VariableUndefined>(arguments->index(0))) throw Error("...");` |
| type test | `TIsType<VariableString>(v)`, `TIsType<VariableArray>(v)`, `TIsType<VariableObject>(v)` |
| convert | `v->toString()`, `v->toNumber()`, `v->toBoolean()` |
| return nothing | `Context::getValueUndefined()` |
| return value | `VariableNumber::newVariable(n)`, `VariableString::newVariable(s)`, `Context::getValueBoolean(b)`, `VariableNull::newVariable()` |
| object result | `TPointer<Variable> o(VariableObject::newVariable()); o->setPropertyBySymbol(Context::getSymbol("k"), value);` |
| read property / index | `v->getPropertyBySymbol(Context::getSymbol("length"))`, `v->getPropertyByIndex(k)` |
| call script function | `fn->functionApply(thisValue, argsArray)` |
| use another extension | in `initExecutive`: `compileStringX("Script.requireExtension(\"Buffer\");")` + fabricare dependency |
| helpers in script | `compileStringX("Example.helper = function(x) { ... };")` |
| native handle / class / coroutine | see `quantum-script` `docs/writing-extensions.md` (`VariableResource`, custom `Variable`, `setFunctionWithYield2`) |

## Link into a C++ host

```cpp
#include <XYO/QuantumScript.Extension/Example.hpp>
static void initExecutive(Executive *executive) {
	Extension::Example::registerInternalExtension(executive);
};
// ExecutiveX::initExecutive(argc, argv, initExecutive); ExecutiveX::executeFile(...)
```

With the DLL engine, `Script.requireExtension` tries the external DLL
first; use `Script.requireInternalExtension("Example")` to force the linked
copy.
