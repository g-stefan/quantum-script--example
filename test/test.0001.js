// Public domain
// http://unlicense.org/

Script.requireExtension("Console");
Script.requireExtension("Example");
Script.requireExtension("JSON");

Example.print(Example.process(function(x) {
	return x + " world!\r\n";
}));

Console.writeLn(JSON.encodeWithIndentation(Script.getExtensionList()));
Console.writeLn("-> test 0001 ok");

