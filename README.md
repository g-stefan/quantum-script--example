# Quantum Script Extension Example

Quantum Script extension template
- The smallest complete `quantum-script--*` extension: fabricare project,
export macro, DLL entry point, internal registration, version / copyright /
license metadata, Windows resources, test, REUSE licensing.
- `Example.print(str)` shows a C++ function called from a script.
- `Example.process(fn)` shows C++ calling back into a script function.
- Copy it and rename `Example` to start a new extension.

```javascript
Script.requireExtension("Example");

Example;
Example.print(str);
Example.process(fn);

Example.print(Example.process(function(x) {
	return x + " world!\r\n";
}));
// Hello world!
```

Built on `quantum-script`, part of the XYO C++ SDK.

## Documentation

- [Overview](docs/README.md) - purpose and design
- [Getting started](docs/getting-started.md) - build, install, test, use from a script, link into a C++ host
- [Script API](docs/script-api.md) - `Example.print`, `Example.process`: exact behavior
- [How it works](docs/how-it-works.md) - every source file explained
- [Create your own extension](docs/new-extension.md) - copy, rename, add functions, test, checklist
- [API reference](docs/reference.md)

A Claude Code skill for this extension is in
[.claude/skills/quantum-script--example](.claude/skills/quantum-script--example/SKILL.md).

## License

Copyright (c) 2016-2026 Grigore Stefan
Licensed under the [MIT](LICENSE) license.
