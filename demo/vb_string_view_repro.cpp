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

constexpr char SIMPLE_ASC_WAT[] = R"((module
  (func $__setArgumentsLength)
  (export "__setArgumentsLength" (func $__setArgumentsLength))
))";

constexpr std::string_view SIMPLE_SOURCE_PATH = "~lib/array.ts";
constexpr std::string_view SIMPLE_SOURCE = R"(// array
export const array_sentinel = 0;
)";

std::string quoteCommandArgument(std::filesystem::path const &path) {
  return '"' + path.string() + '"';
}

std::filesystem::path writeSimpleAscWasm() {
  std::filesystem::path const watPath = std::filesystem::temp_directory_path() / "vb_string_view_repro.wat";
  std::filesystem::path const wasmPath = std::filesystem::temp_directory_path() / "vb_string_view_repro.wasm";
  {
    std::ofstream out{watPath, std::ios::binary | std::ios::trunc};
    if (!out.is_open())
      throw std::runtime_error{"failed to open simple asc wat output"};

    out << SIMPLE_ASC_WAT;
    if (!out)
      throw std::runtime_error{"failed to write simple asc wat output"};
  }

  std::string const command = "wat2wasm " + quoteCommandArgument(watPath) + " -o " + quoteCommandArgument(wasmPath);
  int const exitCode = std::system(command.c_str());

  std::error_code ec;
  std::filesystem::remove(watPath, ec);
  if (exitCode != 0) {
    std::filesystem::remove(wasmPath, ec);
    throw std::runtime_error{"wat2wasm failed"};
  }

  return wasmPath;
}

std::string readBinaryFile(std::filesystem::path const &path) {
  std::ifstream in{path, std::ios::binary};
  if (!in.is_open())
    throw std::runtime_error{"failed to open wasm file"};

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
  std::filesystem::path wasmPath;
  vb::WasmModule::initEnvironment(&malloc, &realloc, &free);

  try {
    wasmPath = writeSimpleAscWasm();
    std::string const wasmBytes = readBinaryFile(wasmPath);

    vb_string_view_repro::initFromBytecode(vb::Span<uint8_t const>{reinterpret_cast<uint8_t const *>(wasmBytes.data()), wasmBytes.size()},
                                           vb::Span<vb::NativeSymbol const>{}, true);
    vb_string_view_repro::start();

    std::optional<std::string_view> const code = SIMPLE_SOURCE;
    parseFile(code, SIMPLE_SOURCE_PATH);

    std::error_code ec;
    std::filesystem::remove(wasmPath, ec);
    vb::WasmModule::destroyEnvironment();
    return 0;
  } catch (std::exception const &e) {
    if (!wasmPath.empty()) {
      std::error_code ec;
      std::filesystem::remove(wasmPath, ec);
    }
    vb::WasmModule::destroyEnvironment();
    std::cout << "ERROR: " << e.what() << '\n';
    return 1;
  }
}