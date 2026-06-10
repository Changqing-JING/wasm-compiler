/*
 * Copyright (C) 2025 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 * SPDX-License-Identifier: Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef VB_STRING_VIEW_REPRO_RUNNER_HPP
#define VB_STRING_VIEW_REPRO_RUNNER_HPP

#include <cstdint>
#include <string_view>

#include "src/WasmModule/WasmModule.hpp"

namespace vb_string_view_repro {

void initFromCompiledBinary(vb::Span<uint8_t const> const &compiledBinary, vb::Span<vb::NativeSymbol const> const &linkedFunctions);

void start();

void callSetArgumentsLength();

uint32_t allocString(std::string_view str);

} // namespace vb_string_view_repro

#endif