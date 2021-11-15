//
// Quantum Script Extension Example
//
// Copyright (c) 2020-2021 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#ifndef QUANTUM_SCRIPT_EXTENSION_EXAMPLE_VERSION_HPP
#define QUANTUM_SCRIPT_EXTENSION_EXAMPLE_VERSION_HPP

#define QUANTUM_SCRIPT_EXTENSION_EXAMPLE_VERSION_ABCD                 2,2,0,20
#define QUANTUM_SCRIPT_EXTENSION_EXAMPLE_VERSION_STR                 "2.2.0"
#define QUANTUM_SCRIPT_EXTENSION_EXAMPLE_VERSION_STR_BUILD           "20"
#define QUANTUM_SCRIPT_EXTENSION_EXAMPLE_VERSION_STR_DATETIME        "2021-11-15 16:51:51"

#ifndef XYO_RC

#ifndef QUANTUM_SCRIPT_EXTENSION_EXAMPLE__EXPORT_HPP
#include "quantum-script-extension-example--export.hpp"
#endif

namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace Example {
				namespace Version {
					QUANTUM_SCRIPT_EXTENSION_EXAMPLE_EXPORT const char *version();
					QUANTUM_SCRIPT_EXTENSION_EXAMPLE_EXPORT const char *build();
					QUANTUM_SCRIPT_EXTENSION_EXAMPLE_EXPORT const char *versionWithBuild();
					QUANTUM_SCRIPT_EXTENSION_EXAMPLE_EXPORT const char *datetime();
				};
			};
		};
	};
};

#endif
#endif

