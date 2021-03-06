// Copyright (c) 2019 PaddlePaddle Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "lite/kernels/npu/bridges/graph.h"
#include "lite/kernels/npu/bridges/registry.h"
#include "lite/kernels/npu/bridges/utility.h"

namespace paddle {
namespace lite {
namespace subgraph {
namespace npu {

int TransposeConverter(void* ctx, OpLite* op) {
  CHECK(ctx != nullptr);
  CHECK(op != nullptr);
  auto graph = static_cast<Graph*>(ctx);
  auto op_info = op->op_info();
  auto op_type = op_info->Type();
  auto scope = op->scope();
  VLOG(3) << "[NPU] Converting " + op_type + "...";

  auto x_var_name = op_info->Input("X").front();
  auto out_var_name = op_info->Input("Out").front();
  auto axis = op_info->GetAttr<std::vector<int>>("axis");

  auto transpose_node = graph->AddNode<ge::op::Permute>(out_var_name);
  transpose_node->set_input_x(*graph->GetNode(x_var_name));
  auto w_const_node = graph->AddNode(out_var_name + "/w", 1.0f);
  transpose_node->set_input_w(*w_const_node);
  transpose_node->set_attr_order(
      ge::AttrValue::LIST_INT(axis.begin(), axis.end()));
  return SUCCESS;
}

}  // namespace npu
}  // namespace subgraph
}  // namespace lite
}  // namespace paddle

REGISTER_SUBGRAPH_BRIDGE(NPU,
                         transpose,
                         paddle::lite::subgraph::npu::TransposeConverter);
REGISTER_SUBGRAPH_BRIDGE(NPU,
                         transpose2,
                         paddle::lite::subgraph::npu::TransposeConverter);
