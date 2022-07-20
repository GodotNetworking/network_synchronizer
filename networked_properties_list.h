#pragma once

#include "core/local_vector.h"
#include "core/resource.h"
#include "network_synchronizer/data_buffer.h"

struct NetworkedInputInfo {
	StringName name;
	Variant default_value;
	uint32_t index;
	DataBuffer::DataType data_type;
	DataBuffer::CompressionLevel compression_level;
	real_t comparison_floating_point_precision;
};

class NetworkedInputList : public Resource {
	GDCLASS(NetworkedInputList, Resource)

public:
	constexpr static uint32_t INDEX_NONE = UINT32_MAX;

private:
	LocalVector<NetworkedInputInfo> input_info;

protected:
	static void _bind_methods();

public:
	uint32_t register_input(
			const StringName &p_name,
			const Variant &p_default_value,
			DataBuffer::DataType p_type,
			DataBuffer::CompressionLevel p_compression_level,
			real_t p_comparison_floating_point_precision = CMP_EPSILON);

	uint32_t find_input_id(const StringName &p_name) const;

	void collect_inputs(const LocalVector<Variant> &p_inputs, DataBuffer &r_buffer) const;
	void set_defaults(LocalVector<Variant> &r_inputs) const;
	void fetch_inputs(DataBuffer &p_buffer, LocalVector<Variant> &r_inputs) const;
	bool are_different(DataBuffer &p_buffer_A, DataBuffer &p_buffer_B) const;
	uint32_t count_size(DataBuffer &p_buffer) const;
};
