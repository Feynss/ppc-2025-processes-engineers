#pragma once

#include <string>
#include <tuple>

#include "task/include/task.hpp"

namespace vasiliev_m_vec_signs {

using InType = int;
using OutType = int;
using TestType = std::tuple<int, std::string>;
using BaseTask = ppc::task::Task<InType, OutType>;

}  // namespace vasiliev_m_vec_signs
