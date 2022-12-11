// Quantum Script Extension Example
// Copyright (c) 2022 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2022 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/QuantumScript.Extension/Example/Library.hpp>
#include <XYO/QuantumScript.Extension/Example/Copyright.hpp>
#include <XYO/QuantumScript.Extension/Example/License.hpp>
#include <XYO/QuantumScript.Extension/Example/Version.hpp>
#include <XYO/QuantumScript.Extension/Buffer/VariableBuffer.hpp>

namespace XYO::QuantumScript::Extension::Example {

	static TPointer<Variable> print(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_VM_DEBUG_RUNTIME
					printf("- example-print\n");
#endif

					try {
						printf("%s", ((arguments->index(0))->toString()).value());
						return Context::getValueUndefined();
					} catch (const Error &e) {
					};

					printf("%s", ((arguments->index(0))->getVariableType()).value());

					return Context::getValueUndefined();
				};

				static TPointer<Variable> process(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_VM_DEBUG_RUNTIME
					printf("- example-process\n");
#endif

					TPointer<VariableArray> applyArguments(VariableArray::newArray());
					(applyArguments->index(0)) = VariableString::newVariable("Hello");
					return (arguments->index(0))->functionApply(Context::getValueUndefined(), applyArguments);
				};

	void registerInternalExtension(Executive *executive) {
		executive->registerInternalExtension("Example", initExecutive);
	};

	void initExecutive(Executive *executive, void *extensionId) {
		String info = "Example\r\n";
		info << License::shortLicense();

		executive->setExtensionName(extensionId, "Example");
		executive->setExtensionInfo(extensionId, info);
		executive->setExtensionVersion(extensionId, Extension::Example::Version::versionWithBuild());
		executive->setExtensionPublic(extensionId, true);

		executive->compileStringX("var Example={};");
					executive->setFunction2("Example.print(str)", print);
					executive->setFunction2("Example.process(fn)", process);
	};

};

#ifdef XYO_COMPILE_DYNAMIC_LIBRARY
extern "C" XYO_QUANTUMSCRIPT_EXTENSION_EXAMPLE_EXPORT void quantumScriptExtension(XYO::QuantumScript::Executive *executive, void *extensionId) {
	XYO::QuantumScript::Extension::Example::initExecutive(executive, extensionId);
};
#endif
