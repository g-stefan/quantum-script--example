# Script API

```javascript
Script.requireExtension("Example");

Example;               // Object, created by initExecutive with "var Example={};"
Example.print(str);    // write str to standard output, returns undefined
Example.process(fn);   // returns fn("Hello")
```

Behavior below was checked against the 5.9.0 build with the
`quantum-script` interpreter.

## Loading

```javascript
Script.requireExtension("Example");
```

- Loads `quantum-script--example.dll` (Windows) / `quantum-script--example.so`
  (Linux) from the path as given, then from each include path folder (the
  interpreter's folder, the script's folder, folders added with
  `Script.setIncludePath`). If no file is found, an internal extension
  registered by the host under the name `Example` is used.
- The name is case insensitive: `"Example"`, `"example"`, `"EXAMPLE"` load
  the same extension.
- Loading is done once per executive; later calls do nothing.
- Missing everywhere: throws `Error: Unable to open "Example"`.
- After loading, `Script.getExtensionList()` contains:

  ```javascript
  {
  	fileName: ".../quantum-script--example.dll",   // "" when internal
  	name: "Example",
  	info: "Example\r\nCopyright (c) 2016-2026 Grigore Stefan <g_stefan@yahoo.com>\r\nMIT License (MIT) <http://opensource.org/licenses/MIT>\r\n",
  	version: "5.9.0.6"                             // version.build
  }
  ```

## `Example.print(str)`

Writes `str` converted to a string to the process standard output
(C `printf`), **without** a new line. Returns `undefined`.

| Argument | Output |
|----------|--------|
| `"abc\n"` | `abc` and a new line |
| `12.5` | `12.5` |
| missing | `undefined` |
| `null` | `null` |
| `[1, 2]` | `1,2` |
| `{a: 1}` | `Object` |

- The conversion is the value's `toString()`. If the conversion throws a
  native `Error`, the type name of the value (`getVariableType()`) is
  printed instead; `print` itself never throws.
- Only the first argument is used; extra arguments are ignored.
- Output goes through C `stdio` like `Console.write`, so the two can be
  mixed and stay in order.
- Use it to see the minimal shape of a native function; in real scripts use
  `Console.write` / `Console.writeLn`.

## `Example.process(fn)`

Calls `fn` with one argument, the string `"Hello"`, and returns what `fn`
returns.

```javascript
Example.process(function(x) { return x + " world!"; });   // "Hello world!"
Example.process(function(x) { return x.length; });        // 5
Example.process(function(a, b) { return typeof(b); });    // "undefined"
```

- `fn` receives exactly one argument (`arguments.length == 1`); a second
  parameter is `undefined`.
- `this` inside `fn` is `undefined`.
- The return value is passed back unchanged (any type).
- An exception thrown by `fn` propagates out of `Example.process` to the
  caller, unchanged:

  ```javascript
  try {
  	Example.process(function() { throw new Error("inner"); });
  } catch (e) {
  	// e is Error: inner
  };
  ```

- If `fn` is missing or is not a function, throws `Error: functionApply`.

Use it to see how C++ calls back into a script: build an argument array,
call `functionApply`, return the result. That is the pattern for every
native function that takes a callback (iterators, event handlers,
`Job` / `Thread` style extensions).

## Not provided

The template stops there on purpose: no constructors, no prototype
methods, no resources, no properties other than the two functions. See
[Create your own extension](new-extension.md) for how to add them, and the
`quantum-script` repository `docs/writing-extensions.md` for resources and
custom `Variable` types.
