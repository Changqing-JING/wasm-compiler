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

#include <exception>
#include <iostream>

#include "vb_string_view_repro_runner.hpp"

#include "src/utils/STDCompilerLogger.hpp"
#include "src/utils/StackTop.hpp"

namespace {

uint8_t const *stackTop() {
  thread_local static uint8_t const *const cachedStackTop = static_cast<uint8_t const *>(vb::getStackTop());
  return cachedStackTop;
}

struct State final {
  vb::STDCompilerLogger logger_;
  vb::WasmModule instance_;

  State() : logger_{}, instance_{logger_} {
    instance_.setStacktraceRecordCount(32U);
    instance_.setContext(nullptr);
  }
};

State &state() {
  static State s;
  return s;
}

} // namespace

namespace vb_string_view_repro {

void initFromBytecode(vb::Span<uint8_t const> const &bytecode, vb::Span<vb::NativeSymbol const> const &linkedFunctions,
                      bool const allowUnknownImports) {
  state().instance_.initFromBytecode(bytecode, linkedFunctions, allowUnknownImports);
}

void start() {
  state().instance_.start(stackTop());
}

void callSetArgumentsLength() {
  state().instance_.callExportedFunctionWithName<0>(stackTop(), "__setArgumentsLength");
}

uint32_t allocString(std::string_view str) {
  std::cout << str.size() << '\n';
  if (str.size() == 0U)
    std::terminate();

  return 0U;
}

} // namespace vb_string_view_repro