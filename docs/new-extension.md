# Create your own extension

This page turns a copy of `quantum-script--example` into a new extension,
`quantum-script--hello`, that scripts load with
`Script.requireExtension("Hello")`. Replace `Hello` / `HELLO` / `hello` with
your own name in the three spellings below.

| Spelling | Used for | Example → new |
|----------|----------|---------------|
| PascalCase | script object, C++ namespace, source folder and header names, extension name | `Example` → `Hello` |
| UPPER_CASE | macros and include guards | `EXAMPLE` → `HELLO` |
| lower case | repository, project, DLL file name, `version.json` key | `example` → `hello` |

The DLL name must be `quantum-script--` + the extension name in **lower
case**: that is what `Script.requireExtension` looks for.

## 1. Copy the template

Copy the repository **without** `.git`, `output/`, `temp/`, `release/`,
`docs/` and `.claude/` (write new docs for the new extension), then
`git init` the copy. From the parent folder, with a POSIX shell:

```bash
mkdir quantum-script--hello
cd quantum-script--example
git archive HEAD | tar -x -C ../quantum-script--hello
cd ../quantum-script--hello
rm -rf docs .claude
git init
```

## 2. Rename files and identifiers

```bash
NAME=Hello
UPPER=HELLO
LOWER=hello

cd source/XYO/QuantumScript.Extension
mv Example "$NAME"
mv Example.hpp "$NAME.hpp"
mv Example.Amalgam.cpp "$NAME.Amalgam.cpp"
cd ../../..

grep -rlE "Example|EXAMPLE|example" source fabricare.json version.json test README.md .reuse/dep5 \
	| xargs sed -i -e "s/EXAMPLE/$UPPER/g" -e "s/Example/$NAME/g" -e "s/example/$LOWER/g"
```

Then check what the replacement touched:

| File | Expected after renaming |
|------|-------------------------|
| `fabricare.json` | `"name": "quantum-script--hello"` (solution and project), `"sourcePath": "XYO/QuantumScript.Extension/Hello"` |
| `version.json` | key `"quantum-script--hello"`; set `"version": "1.0.0"`, `"build": "0"` for a new project |
| `Dependency.hpp` | `QUANTUM_SCRIPT__HELLO_INTERNAL`, `XYO_QUANTUMSCRIPT_EXTENSION_HELLO_EXPORT` / `_INTERNAL` / `_LIBRARY` |
| every header | guards `XYO_QUANTUMSCRIPT_EXTENSION_HELLO_*_HPP`, includes `<XYO/QuantumScript.Extension/Hello/...>` |
| `Library.cpp` | `namespace XYO::QuantumScript::Extension::Hello`, `registerInternalExtension("Hello", ...)`, `setExtensionName(..., "Hello")`, `"Hello\r\n"` info |
| `Library.rc` | `"quantum-script--hello.dll"`, `"Quantum Script Extension Hello"` |
| file headers | `// Quantum Script Extension Hello` |
| `.reuse/dep5` | `Upstream-Name: Quantum Script Extension Hello`, `Source: https://github.com/<you>/quantum-script--hello` |
| `test/test.0001.js` | `Script.requireExtension("Hello")` |
| `Copyright.rh`, `LICENSE`, SPDX headers | your copyright holder, if different |

Then regenerate the version header:

```bash
fabricare version     # writes Hello/Version.rh from Version.Template.rh and version.json
```

Build once before changing any code: `fabricare make`. A failure now is a
renaming mistake, not a code one.

## 3. Write the native functions

Replace `print` and `process` in `Library.cpp`. Each function has the same
signature; register it in `initExecutive` **after** creating the object it
belongs to:

```cpp
namespace XYO::QuantumScript::Extension::Hello {

	// Hello.add(a, b) -> number
	static TPointer<Variable> add(VariableFunction *function, Variable *this_, VariableArray *arguments) {
		return VariableNumber::newVariable(arguments->index(0)->toNumber() + arguments->index(1)->toNumber());
	};

	// Hello.greet(name) -> string; name is required
	static TPointer<Variable> greet(VariableFunction *function, Variable *this_, VariableArray *arguments) {
		if (TIsTypeExact<VariableUndefined>(arguments->index(0))) {
			throw Error("greet: name required");     // becomes a script exception
		};
		String retV = "Hello, ";
		retV << arguments->index(0)->toString();
		return VariableString::newVariable(retV);
	};

	// Hello.info() -> {name, version}
	static TPointer<Variable> info(VariableFunction *function, Variable *this_, VariableArray *arguments) {
		TPointer<Variable> retV(VariableObject::newVariable());
		retV->setPropertyBySymbol(Context::getSymbol("name"), VariableString::newVariable("Hello"));
		retV->setPropertyBySymbol(Context::getSymbol("version"), VariableString::newVariable(Version::versionWithBuild()));
		return retV;
	};

	// Hello.forEach(array, fn) -> calls fn(value, index) for each element
	static TPointer<Variable> forEach(VariableFunction *function, Variable *this_, VariableArray *arguments) {
		TPointerX<Variable> &array = arguments->index(0);
		TPointerX<Variable> &fn = arguments->index(1);
		size_t length = (size_t)array->getPropertyBySymbol(Context::getSymbol("length"))->toNumber();
		for (size_t k = 0; k < length; ++k) {
			TPointer<VariableArray> applyArguments(VariableArray::newArray());
			applyArguments->index(0) = array->getPropertyByIndex(k);
			applyArguments->index(1) = VariableNumber::newVariable(k);
			fn->functionApply(Context::getValueUndefined(), applyArguments);   // script exceptions propagate
		};
		return Context::getValueUndefined();
	};

	void registerInternalExtension(Executive *executive) {
		executive->registerInternalExtension("Hello", initExecutive);
	};

	void initExecutive(Executive *executive, void *extensionId) {
		String info_ = "Hello\r\n";
		info_ << License::shortLicense().c_str();

		executive->setExtensionName(extensionId, "Hello");
		executive->setExtensionInfo(extensionId, info_);
		executive->setExtensionVersion(extensionId, Extension::Hello::Version::versionWithBuild());
		executive->setExtensionPublic(extensionId, true);

		executive->compileStringX("var Hello={};");
		executive->setFunction2("Hello.add(a,b)", add);
		executive->setFunction2("Hello.greet(name)", greet);
		executive->setFunction2("Hello.info()", info);
		executive->setFunction2("Hello.forEach(array,fn)", forEach);

		// helpers written in script
		executive->compileStringX(
		    "Hello.sum = function(list) {"
		    "	var s = 0;"
		    "	Hello.forEach(list, function(value) { s += value; });"
		    "	return s;"
		    "};");
	};

};
```

Rules worth keeping in mind:

- Read arguments with `arguments->index(k)`; past the end it is
  `undefined`, so missing arguments never crash. Test with
  `TIsTypeExact<VariableUndefined>(...)` when an argument is required.
- Convert with `toNumber()`, `toString()`, `toBoolean()`; test types with
  `TIsType<VariableString>(...)`, `TIsType<VariableArray>(...)`, ...
- Return `Context::getValueUndefined()` for no value,
  `Context::getValueBoolean(b)` for booleans, `VariableNull::newVariable()`
  for `null`, and `VariableNumber` / `VariableString` / `VariableObject` /
  `VariableArray::newVariable(...)` for new values.
- Report errors with `throw Error("...")`.
- Keep native functions `static`; only `initExecutive` and
  `registerInternalExtension` are exported.
- `initExecutive` runs once **per executive** (per thread): do not keep
  script values in C++ globals; use a per thread context (see the
  `quantum-script` docs, `writing-extensions.md`).
- Need another extension? Load it in `initExecutive`:
  `executive->compileStringX("Script.requireExtension(\"Buffer\");");` and
  add it to the `fabricare.json` dependencies.
- Need another C++ library (`xyo-cryptography`, a `vendor-*` library)? Add
  it to `"dependency"` in `fabricare.json` and include it in
  `Dependency.hpp`.

For **native handles** (`VariableResource`), **objects with methods**
(`new Thing()`, a custom `Variable` subclass with a prototype), and
**native coroutines** (`setFunctionWithYield2`), follow the
`quantum-script` repository `docs/writing-extensions.md`; `quantum-script--file`
and `quantum-script--buffer` are complete examples.

## 4. More source files

To split the code, add `Hello/Thing.hpp` + `Hello/Thing.cpp` inside the
source folder (fabricare compiles everything in `sourcePath`), include
`Dependency.hpp` first in the header, mark exported symbols with
`XYO_QUANTUMSCRIPT_EXTENSION_HELLO_EXPORT`, and add the `.cpp` to
`Hello.Amalgam.cpp`.

## 5. Test

```javascript
// test/test.0001.js
Script.requireExtension("Console");
Script.requireExtension("Hello");

if (Hello.add(2, 3) != 5) {
	throw new Error("add");
};
if (Hello.greet("World") != "Hello, World") {
	throw new Error("greet");
};
if (Hello.sum([1, 2, 3]) != 6) {
	throw new Error("sum");
};

Console.writeLn("-> test 0001 ok");
```

A test fails by throwing or by `Script.exit(1)`. For more tests add
`test/test.0002.js`, ... and raise the loop bound in `fabricare/test.js`
(`for(var k=1;k<=1;++k)`).

```bash
fabricare make
fabricare install     # the test loads the extension from the SDK bin folder
fabricare test
```

## 6. Licensing and release

- Every source file keeps an SPDX header (`SPDX-FileCopyrightText`,
  `SPDX-License-Identifier`); files without one are covered by
  `.reuse/dep5`. Add a `Files:` entry when you add a folder (`docs/*`,
  `.claude/*`, ...). Check with `reuse lint` (VS Code task "Check licenses").
- Bump versions with `fabricare version` (build number) or
  `fabricare version-patch` / `version-minor` / `version-major`.
- `fabricare release` packs `bin`, `dev` and `static.dev` archives into
  `release/` (ignored by git).

## Checklist

- [ ] repository, project, DLL: `quantum-script--hello` (lower case)
- [ ] script object, namespace, folder, extension name: `Hello`
- [ ] macros and guards: `XYO_QUANTUMSCRIPT_EXTENSION_HELLO_*`, `QUANTUM_SCRIPT__HELLO_INTERNAL`
- [ ] `quantumScriptExtension` still present at the end of `Library.cpp`
- [ ] `setExtensionName` equals the name passed to `registerInternalExtension`
- [ ] `compileStringX("var Hello={};")` before the `setFunction2` calls
- [ ] `version.json` reset, `fabricare version` run, `Version.rh` regenerated
- [ ] every extension or library you include is listed in `fabricare.json` `"dependency"`
- [ ] `.reuse/dep5` `Upstream-Name` and `Source` updated, `reuse lint` clean
- [ ] `fabricare make`, `install`, `test` pass
