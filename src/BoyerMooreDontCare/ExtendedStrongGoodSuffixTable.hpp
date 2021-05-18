#include <xString>

class ExtendedStrongGoodSuffixTable {
private:
    using SymbolMap = std::map<uint8_t, int>;
    std::basic_string_view<SymbolMap> _table;
};