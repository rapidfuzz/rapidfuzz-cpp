#include "foo_lib.hpp"

double fooFunc() {
    std::string_view a("aaaa"), b("abaa");
    FooType cache(a.begin(), a.end());
    return cache.similarity(b);
}
