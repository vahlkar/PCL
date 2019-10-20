#pragma once

#if defined(_MSC_VER)
#  if !defined(COMPILER_MSVC)
#    define COMPILER_MSVC // Set MSVC visibility of exported symbols in the shared library.
#  endif
#  pragma warning(push)
#  pragma warning(disable : 4190)
#endif

#include <tensorflow/c/c_api.h>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace tf_utils {

    TF_Graph* LoadGraphDef(const char* file);

    bool RunSession(TF_Graph* graph,
                    const TF_Output* inputs, TF_Tensor* const* input_tensors, std::size_t ninputs,
                    const TF_Output* outputs, TF_Tensor** output_tensors, std::size_t noutputs);

    bool RunSession(TF_Graph* graph,
                    const std::vector<TF_Output>& inputs, const std::vector<TF_Tensor*>& input_tensors,
                    const std::vector<TF_Output>& outputs, std::vector<TF_Tensor*>& output_tensors);

    TF_Tensor* CreateTensor(TF_DataType data_type,
                            const std::int64_t* dims, std::size_t num_dims,
                            const void* data, std::size_t len);

    template <typename T>
    TF_Tensor* CreateTensor(TF_DataType data_type,
                            const std::vector<std::int64_t>& dims,
                            const std::vector<T>& data) {
      return CreateTensor(data_type,
                          dims.data(), dims.size(),
                          data.data(), data.size() * sizeof(T));
}

void DeleteTensor(TF_Tensor* tensor);

void DeleteTensors(const std::vector<TF_Tensor*>& tensors);

template <typename T>
std::vector<T> TensorData(const TF_Tensor* tensor) {
  const auto data = static_cast<T*>(TF_TensorData(tensor));
  if (data == nullptr) {
    return {};
  }

  return {data, data + (TF_TensorByteSize(tensor) / TF_DataTypeSize(TF_TensorType(tensor)))};
}

template <typename T>
std::vector<std::vector<T>> TensorsData(const std::vector<TF_Tensor*>& tensors) {
  std::vector<std::vector<T>> data;
  data.reserve(tensors.size());
  for (const auto t : tensors) {
    data.push_back(TensorData<T>(t));
  }

  return data;
}

} // namespace tf_utils

#if defined(_MSC_VER)
#  pragma warning(pop)
#endif
