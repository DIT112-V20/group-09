#ifndef SMARTCAR_EMUL_TEST_UTILITY_HXX
#define SMARTCAR_EMUL_TEST_UTILITY_HXX
#include <filesystem>
#include <string_view>

bool test_compile(std::string_view src_name, std::string_view src);
bool write_file(const std::filesystem::path& source_path, std::string_view src);

#endif // SMARTCAR_EMUL_TEST_UTILITY_HXX
