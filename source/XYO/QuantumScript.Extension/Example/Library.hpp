// Quantum Script Extension Example
// Copyright (c) 2016-2023 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2023 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_EXAMPLE_LIBRARY_HPP
#define XYO_QUANTUMSCRIPT_EXTENSION_EXAMPLE_LIBRARY_HPP

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_EXAMPLE_DEPENDENCY_HPP
#	include <XYO/QuantumScript.Extension/Example/Dependency.hpp>
#endif

namespace XYO::QuantumScript::Extension::Example {

	XYO_QUANTUMSCRIPT_EXTENSION_EXAMPLE_EXPORT void initExecutive(Executive *executive, void *extensionId);
	XYO_QUANTUMSCRIPT_EXTENSION_EXAMPLE_EXPORT void registerInternalExtension(Executive *executive);

};

#endif
