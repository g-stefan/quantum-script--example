//
// Quantum Script Extension Example
//
// Copyright (c) 2020-2021 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "quantum-script-extension-example-license.hpp"
#include "quantum-script-extension-example.hpp"
#ifndef QUANTUM_SCRIPT_EXTENSION_EXAMPLE_NO_VERSION
#include "quantum-script-extension-example-version.hpp"
#endif

#include "quantum-script-variablestring.hpp"

//#define QUANTUM_SCRIPT_VM_DEBUG_RUNTIME

namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace Example {

				using namespace XYO;
				using namespace Quantum::Script;

				static TPointer<Variable> print(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_VM_DEBUG_RUNTIME
					printf("- example-print\n");
#endif

					try {
						printf("%s", ((arguments->index(0))->toString()).value());
						return Context::getValueUndefined();
					} catch(const Error &e) {};


					printf("%s", ((arguments->index(0))->getType()).value());

					return Context::getValueUndefined();
				};

				static TPointer<Variable> process(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_VM_DEBUG_RUNTIME
					printf("- example-process\n");
#endif

					TPointer<VariableArray> applyArguments(VariableArray::newArray());
					(applyArguments->index(0))=VariableString::newVariable("Hello");
					return (arguments->index(0))->functionApply(
							Context::getValueUndefined(),
							applyArguments
						);
				};

				void registerInternalExtension(Executive *executive) {
					executive->registerInternalExtension("Example", initExecutive);
				};

				void initExecutive(Executive *executive, void *extensionId) {

					String info = "Example\r\n";
					info << License::shortContent();

					executive->setExtensionName(extensionId, "Example");
					executive->setExtensionInfo(extensionId, info);
#ifndef QUANTUM_SCRIPT_EXTENSION_EXAMPLE_NO_VERSION
					executive->setExtensionVersion(extensionId, Extension::Example::Version::versionWithBuild());
#endif
					executive->setExtensionPublic(extensionId, true);

					executive->compileStringX("var Example={};");
					executive->setFunction2("Example.print(str)", print);
					executive->setFunction2("Example.process(fn)", process);
				};

			};
		};
	};
};

#ifdef XYO_COMPILE_DYNAMIC_LIBRARY
extern "C" QUANTUM_SCRIPT_EXTENSION_EXAMPLE_EXPORT void quantumScriptExtension(Quantum::Script::Executive *executive, void *extensionId) {
	Quantum::Script::Extension::Example::initExecutive(executive, extensionId);
};
#endif

