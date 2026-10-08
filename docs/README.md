# Quantum Script Extension Example — Documentation

`quantum-script--example` is the **reference template for writing a Quantum
Script extension**. It is a complete, buildable, installable extension that
does as little as possible: loaded with `Script.requireExtension("Example")`,
it adds one global object, `Example`, with two native functions:

- `Example.print(str)` — a C++ function **called from a script**: writes its
  argument to standard output.
- `Example.process(fn)` — a C++ function that **calls back into the script**:
  calls `fn("Hello")` and returns what `fn` returns.

Those two directions (script → C++, C++ → script) are the core of every
extension. Everything else in the repository is the scaffolding that every
`quantum-script--*` extension shares: the fabricare project, the export
macro, the DLL entry point, the internal (static) registration, version,
copyright and license metadata, Windows resources, a test, REUSE licensing.

```javascript
Script.requireExtension("Example");

Example.print(Example.process(function(x) {
	return x + " world!\r\n";
}));
// Hello world!
```

```
scripts: quantum-script .js, fabricare build scripts, ...
quantum-script--*         (Console, File, JSON, Buffer, ... built the same way)
quantum-script--example   <-- this repository: the template
quantum-script            (Executive, Variable, Context, the interpreter)
xyo-system, xyo-encoding, xyo-multithreading, xyo-data-structures, xyo-managed-memory, xyo-platform
```

## Why it exists

| Need | What this repository gives |
|------|----------------------------|
| Start a new extension without guessing the boilerplate | copy it, rename `Example` / `EXAMPLE` / `example`, replace `Library.cpp` — see [Create your own extension](new-extension.md) |
| See the minimum an extension must contain | `Library.cpp` is about 80 lines; everything else is metadata |
| A working example of a native function | `Example.print` — read an argument, convert it, return `undefined` |
| A working example of calling a script function from C++ | `Example.process` — build an argument array, `functionApply` |
| One source for DLL and static builds | `quantumScriptExtension` for `quantum-script--example.dll` / `.so`, `registerInternalExtension` for hosts that link it in |
| Check the toolchain end to end | `fabricare make`, `fabricare install`, `fabricare test` |

It is **not** a library to depend on for functionality: `Example.print`
and `Example.process` exist to be read and replaced.

## Concepts at a glance

| Need | Use | Notes |
|------|-----|-------|
| Load the extension | `Script.requireExtension("Example");` | name is case insensitive; file is `quantum-script--example.dll` / `.so` |
| Print without a new line | `Example.print(x)` | writes `x` converted to string to stdout; returns `undefined` |
| Call a function with `"Hello"` | `Example.process(fn)` | returns `fn("Hello")`; `this` is `undefined`; throws `Error: functionApply` if `fn` is not a function |
| See it is loaded | `Script.getExtensionList()` | entry with `name: "Example"`, `version`, `info`, `fileName` |
| Build / install / test | `fabricare make` / `install` / `test` | test needs the installed DLL, see [Getting started](getting-started.md) |
| Link it into a C++ host | `Extension::Example::registerInternalExtension(executive)` | include `<XYO/QuantumScript.Extension/Example.hpp>` |
| Start a new extension | copy and rename | [Create your own extension](new-extension.md) |

## Contents

| Document | What it covers |
|----------|----------------|
| [Getting started](getting-started.md) | Build, install, test, use from a script, link into a C++ host, static builds |
| [Script API](script-api.md) | `Example.print` and `Example.process`: arguments, exact behavior, errors |
| [How it works](how-it-works.md) | Walk through every source file: entry point, `initExecutive`, native function signature, export macro, metadata |
| [Create your own extension](new-extension.md) | Step by step: copy, rename, add functions, objects and resources, test, release |
| [API reference](reference.md) | Every script and C++ symbol on one page |

The language, `Script.requireExtension`, embedding and the `Variable` API
are documented in the `quantum-script` repository, `docs/` (in particular
`writing-extensions.md`, `embedding.md` and `modules.md`).

## Source map

```
fabricare.json                                           project: dll-or-lib, depends on quantum-script, quantum-script--console
version.json                                             version, build number, date (updated by fabricare version)
source/XYO/QuantumScript.Extension/Example.hpp           umbrella header, include this from C++
source/XYO/QuantumScript.Extension/Example.Amalgam.cpp   the whole extension in one translation unit
source/XYO/QuantumScript.Extension/Example/
    Dependency.hpp                                       <XYO/QuantumScript.hpp>, the export macro
    Library.hpp / Library.cpp                            initExecutive, registerInternalExtension,
                                                         print, process, quantumScriptExtension (DLL entry)
    Copyright.hpp / .cpp / .rh                           copyright, publisher, company, contact
    License.hpp / .cpp                                   MIT license text (full and short)
    Version.hpp / .cpp / .rh, Version.Template.rh        version strings (Version.rh is generated)
    Library.rc / Library.rh                              Windows version resource of the DLL
test/test.0001.js                                        loads Console, Example, JSON; prints "Hello world!"
fabricare/test.js                                        runs test/test.000N.js with quantum-script
.reuse/dep5, LICENSES/                                   REUSE licensing of files without headers
```

## AI assistant skill

A Claude Code skill describing this extension and how to build a new one
from it lives in
[`.claude/skills/quantum-script--example/`](../.claude/skills/quantum-script--example/SKILL.md).
It is picked up automatically inside this repository; copy the folder to
`~/.claude/skills/` to have it available when you write a new
`quantum-script--*` extension in another repository.
