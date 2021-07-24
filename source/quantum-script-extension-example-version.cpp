//
// Quantum Script Extension Example
//
// Copyright (c) 2020-2021 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#include "quantum-script-extension-example-version.hpp"

namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace Example {
				namespace Version {

					static const char *version_ = "2.0.0";
					static const char *build_ = "9";
					static const char *versionWithBuild_ = "2.0.0.9";
					static const char *datetime_ = "2021-07-23 16:34:03";

					const char *version() {
						return version_;
					};
					const char *build() {
						return build_;
					};
					const char *versionWithBuild() {
						return versionWithBuild_;
					};
					const char *datetime() {
						return datetime_;
					};

				};
			};
		};
	};
};



