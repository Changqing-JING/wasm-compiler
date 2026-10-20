#include <array>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <vector>

#include "src/WasmModule/WasmModule.hpp"
#include "src/config.hpp"
#include "src/core/common/NativeSymbol.hpp"
#include "src/core/common/function_traits.hpp"
#include "src/core/common/util.hpp"
#include "src/core/runtime/TrapException.hpp"
#include "src/utils/MemUtils.hpp"
#include "src/utils/STDCompilerLogger.hpp"
#include "src/utils/StackTop.hpp"

using namespace vb;

std::vector<uint8_t> loadFile(std::string const &filePath) {
  std::ifstream file(filePath, std::ios::binary);
  if (!file) {
    throw std::runtime_error("Failed to open file: " + filePath);
  }
  return std::vector<uint8_t>((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

void saveFile(std::string const &filePath, uint8_t const *data, size_t size) {
  std::ofstream file(filePath, std::ios::binary);
  if (!file) {
    throw std::runtime_error("Failed to open file for writing: " + filePath);
  }
  file.write(reinterpret_cast<char const *>(data), static_cast<std::streamsize>(size));
}

int32_t logInt(uint32_t const data, void *const ctx) {
  static_cast<void>(ctx);
  std::cout << "wasm log " << data << std::endl;
  return 0;
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    std::cout << "Usage:" << std::endl;
    std::cout << "  " << argv[0] << " compile <input.wasm> <output.bin>" << std::endl;
    std::cout << "  " << argv[0] << " run <input.bin>" << std::endl;
    return 1;
  }

  std::string mode = argv[1];

  try {
    vb::WasmModule::initEnvironment(&malloc, &realloc, &free);

    if (mode == "compile") {
      std::string wasmPath = argv[2];
      std::string outBinPath = argv[3];

      std::vector<uint8_t> const bytecode = loadFile(wasmPath);
      vb::STDCompilerLogger stdCompilerLogger{};
      vb::WasmModule module(10000000U, stdCompilerLogger, false, nullptr, 0U);

      std::cout << "Compiling " << wasmPath << " ..." << std::endl;
      WasmModule::CompileResult const compileResult =
          module.compile(vb::Span<const uint8_t>(bytecode.data(), bytecode.size()), vb::Span<vb::NativeSymbol const>());

      saveFile(outBinPath, compileResult.getModule().data(), compileResult.getModule().size());
      std::cout << "Saved compiled binary to " << outBinPath << " (" << compileResult.getModule().size() << " bytes)" << std::endl;
    } else if (mode == "run") {
      std::string binPath = argv[2];
      std::vector<uint8_t> const machineCodeBytes = loadFile(binPath);

      vb::STDCompilerLogger stdCompilerLogger{};
      vb::WasmModule module(10000000U, stdCompilerLogger, false, nullptr, 0U);

      module.initFromCompiledBinary(vb::Span<const uint8_t>(machineCodeBytes.data(), machineCodeBytes.size()), vb::Span<vb::NativeSymbol const>(),
                                    vb::Span<const uint8_t>());

      uint8_t const *stackTop = vb::pCast<uint8_t const *>(vb::getStackTop());

      module.start(stackTop);

      std::array<vb::WasmValue, 1U> res{};
      try {
        res = module.callExportedFunctionWithName<1U>(stackTop, "_start");
      } catch (TrapException &e) {
        std::cout << e.what() << ": " << static_cast<uint32_t>(e.getTrapCode()) << std::endl;
        vb::WasmModule::destroyEnvironment();
        return 1;
      }
      std::cout << "Execution finished successfully! Result: " << res[0].i32 << std::endl;
    } else {
      std::cerr << "Unknown mode: " << mode << std::endl;
      vb::WasmModule::destroyEnvironment();
      return 1;
    }

    vb::WasmModule::destroyEnvironment();
    return 0;
  } catch (const std::exception &e) {
    std::cout << "Exception: " << e.what() << std::endl;
    return 1;
  }
}
