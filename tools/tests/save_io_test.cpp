#include "SaveIO.h"
#include "json/document.h"
#include <cassert>
#include <fstream>
#include <iterator>

static std::string read(const std::string& path) {
    std::ifstream input(path);
    return std::string(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
}
int main(int argc, char** argv) {
    assert(argc == 2);
    const std::string folder = argv[1];
    const auto path = folder + "/checkpoint.json";
    const std::string first = "{\"money\":1000,\"carrotHealth\":8,\"waveIndex\":0}";
    assert(SaveIO::writeJSON(path, first));
    assert(read(path) == first);
    // A partially written temporary file must not damage the published save.
    { std::ofstream interrupted(path + ".tmp"); interrupted << "{\"money\":"; }
    assert(read(path) == first);
    const std::string next = "{\"money\":540,\"carrotHealth\":5,\"waveIndex\":2,\"pendingCounts\":[0,1,4]}";
    assert(SaveIO::writeJSON(path, next));
    assert(read(path) == next);
    assert(!std::ifstream(path + ".tmp").good());
    rapidjson::Document state;
    state.Parse(read(path).c_str());
    assert(!state.HasParseError());
    assert(state["money"].GetInt() == 540);
    assert(state["carrotHealth"].GetInt() == 5);
    assert(state["waveIndex"].GetInt() == 2);
    assert(state["pendingCounts"][2].GetInt() == 4);
    // A failed publication is reported and its temporary file is cleaned up.
    assert(!SaveIO::writeJSON(folder + "/missing/checkpoint.json", next));
    assert(read(path) == next);
    assert(!std::ifstream(folder + "/missing/checkpoint.json.tmp").good());
}
