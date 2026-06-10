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

#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>

#include "vb_string_view_repro_runner.hpp"

namespace {

constexpr std::string_view SIMPLE_SOURCE_PATH = "~lib/array.ts";
constexpr std::string_view SIMPLE_SOURCE = R"(// array
export const array_sentinel = 0;
)";
constexpr char COMPILED_BINARY_PATH[] = "D:/code/workspace/wasm-compiler/vb_string_view_repro.compiled.bin";

std::filesystem::path compiledBinaryPath() {
  return std::filesystem::path{COMPILED_BINARY_PATH};
}

std::string readBinaryFile(std::filesystem::path const &path) {
  std::ifstream in{path, std::ios::binary};
  if (!in.is_open())
    throw std::runtime_error{"failed to open binary file"};

  return std::string{std::istreambuf_iterator<char>{in}, std::istreambuf_iterator<char>{}};
}

void parseFile(std::optional<std::string_view> const &code, std::string_view path) {
  vb_string_view_repro::callSetArgumentsLength();

  if (code.has_value()) {
    vb_string_view_repro::allocString(code.value());
    vb_string_view_repro::allocString(path);
  }
}

} // namespace

int main() {
  vb::WasmModule::initEnvironment(&malloc, &realloc, &free);

  try {
    std::string const compiledBinary = readBinaryFile(compiledBinaryPath());

    vb_string_view_repro::initFromCompiledBinary(
        vb::Span<uint8_t const>{reinterpret_cast<uint8_t const *>(compiledBinary.data()), compiledBinary.size()}, vb::Span<vb::NativeSymbol const>{});
    vb_string_view_repro::start();

    std::optional<std::string_view> const code = SIMPLE_SOURCE;
    parseFile(code, SIMPLE_SOURCE_PATH);

    vb::WasmModule::destroyEnvironment();
    return 0;
  } catch (std::exception const &e) {
    vb::WasmModule::destroyEnvironment();
    std::cout << "ERROR: " << e.what() << '\n';
    return 1;
  }
}