// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#pragma once
#include <string>
namespace neon {
inline const char* JsonSchema(const std::string& name){
  if (name == "ps") return R"JSON({
    "$schema":"https://json-schema.org/draft/2020-12/schema",
    "title":"neon.ps",
    "type":"object",
    "properties":{
      "threads":{"type":"array","items":{"type":"object","properties":{"id":{"type":"integer"},"name":{"type":"string"}},"required":["id","name"]}},
      "uptime_us":{"type":"integer"}
    },
    "required":["threads","uptime_us"]
  })JSON";
  if (name == "meminfo") return R"JSON({
    "$schema":"https://json-schema.org/draft/2020-12/schema",
    "title":"neon.meminfo",
    "type":"object",
    "properties":{"heap_bytes":{"type":"integer"},"objects":{"type":"integer"}},
    "required":["heap_bytes","objects"]
  })JSON";
  return nullptr;
}
} // namespace neon
