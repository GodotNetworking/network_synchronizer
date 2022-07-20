#include "networked_properties_list.h"

#include "scene_synchronizer.h"

void NetworkedInputList::_bind_methods() {
}

uint32_t NetworkedInputList::register_input(
		const StringName &p_name,
		const Variant &p_default_value,
		DataBuffer::DataType p_type,
		DataBuffer::CompressionLevel p_compression_level,
		real_t p_comparison_floating_point_precision) {
	switch (p_type) {
		case DataBuffer::DATA_TYPE_BOOL:
			ERR_FAIL_COND_V_MSG(p_default_value.get_type() != Variant::BOOL, UINT32_MAX, "The moveset initialization failed for" + p_name + " the specified data type is `BOOL` but the default parameter is " + itos(p_default_value.get_type()));
			break;
		case DataBuffer::DATA_TYPE_INT:
			ERR_FAIL_COND_V_MSG(p_default_value.get_type() != Variant::INT, UINT32_MAX, "The moveset initialization failed for" + p_name + " the specified data type is `INT` but the default parameter is " + itos(p_default_value.get_type()));
			break;
		case DataBuffer::DATA_TYPE_REAL:
			ERR_FAIL_COND_V_MSG(p_default_value.get_type() != Variant::REAL, UINT32_MAX, "The moveset initialization failed for" + p_name + " the specified data type is `REAL` but the default parameter is " + itos(p_default_value.get_type()));
			break;
		case DataBuffer::DATA_TYPE_POSITIVE_UNIT_REAL:
			ERR_FAIL_COND_V_MSG(p_default_value.get_type() != Variant::REAL, UINT32_MAX, "The moveset initialization failed for" + p_name + " the specified data type is `REAL` but the default parameter is " + itos(p_default_value.get_type()));
			break;
		case DataBuffer::DATA_TYPE_UNIT_REAL:
			ERR_FAIL_COND_V_MSG(p_default_value.get_type() != Variant::REAL, UINT32_MAX, "The moveset initialization failed for" + p_name + " the specified data type is `REAL` but the default parameter is " + itos(p_default_value.get_type()));
			break;
		case DataBuffer::DATA_TYPE_VECTOR2:
			ERR_FAIL_COND_V_MSG(p_default_value.get_type() != Variant::VECTOR2, UINT32_MAX, "The moveset initialization failed for" + p_name + " the specified data type is `Vector2` but the default parameter is " + itos(p_default_value.get_type()));
			break;
		case DataBuffer::DATA_TYPE_NORMALIZED_VECTOR2:
			ERR_FAIL_COND_V_MSG(p_default_value.get_type() != Variant::VECTOR2, UINT32_MAX, "The moveset initialization failed for" + p_name + " the specified data type is `Vector2` but the default parameter is " + itos(p_default_value.get_type()));
			break;
		case DataBuffer::DATA_TYPE_VECTOR3:
			ERR_FAIL_COND_V_MSG(p_default_value.get_type() != Variant::VECTOR3, UINT32_MAX, "The moveset initialization failed for" + p_name + " the specified data type is `Vector3` but the default parameter is " + itos(p_default_value.get_type()));
			break;
		case DataBuffer::DATA_TYPE_NORMALIZED_VECTOR3:
			ERR_FAIL_COND_V_MSG(p_default_value.get_type() != Variant::VECTOR3, UINT32_MAX, "The moveset initialization failed for" + p_name + " the specified data type is `Vector3` but the default parameter is " + itos(p_default_value.get_type()));
			break;
		case DataBuffer::DATA_TYPE_VARIANT:
			/* No need to check variant, anything is accepted at this point.*/
			break;
	};

	const uint32_t index = input_info.size();

	input_info.resize(input_info.size() + 1);
	input_info[index].name = p_name;
	input_info[index].default_value = p_default_value;
	input_info[index].data_type = p_type;
	input_info[index].compression_level = p_compression_level;
	input_info[index].comparison_floating_point_precision = p_comparison_floating_point_precision;

	return index;
}

uint32_t NetworkedInputList::find_input_id(const StringName &p_name) const {
	for (uint32_t i = 0; i < input_info.size(); i += 1) {
		if (input_info[i].name == p_name) {
			return i;
		}
	}
	return INDEX_NONE;
}

void NetworkedInputList::collect_inputs(const LocalVector<Variant> &p_input, DataBuffer &r_buffer) const {
	const uint32_t size = p_input.size() < input_info.size() ? p_input.size() : input_info.size();

	for (uint32_t i = 0; i < size; i += 1) {
		const NetworkedInputInfo &info = input_info[i];
		const Variant &pending_input = p_input[i];

		const bool is_default =
				// Use default if the property is never a boolean.
				info.default_value.get_type() != Variant::BOOL &&
				(
						// Use default if the variable type is different.
						info.default_value.get_type() != pending_input.get_type() ||
						// Use default if the variable value is equal to default.
						info.default_value == pending_input);

		if (info.default_value.get_type() != Variant::BOOL) {
			// No need to specify if this is default for boolean properties, we can
			// right away set the boolean value instead.
			r_buffer.add_bool(is_default);
		}

		if (!is_default) {
			switch (info.data_type) {
				case DataBuffer::DATA_TYPE_BOOL:
					r_buffer.add_bool(pending_input.operator bool());
					break;
				case DataBuffer::DATA_TYPE_INT:
					r_buffer.add_int(pending_input.operator int(), info.compression_level);
					break;
				case DataBuffer::DATA_TYPE_REAL:
					r_buffer.add_real(pending_input.operator real_t(), info.compression_level);
					break;
				case DataBuffer::DATA_TYPE_POSITIVE_UNIT_REAL:
					r_buffer.add_positive_unit_real(pending_input.operator real_t(), info.compression_level);
					break;
				case DataBuffer::DATA_TYPE_UNIT_REAL:
					r_buffer.add_unit_real(pending_input.operator real_t(), info.compression_level);
					break;
				case DataBuffer::DATA_TYPE_VECTOR2:
					r_buffer.add_vector2(pending_input.operator Vector2(), info.compression_level);
					break;
				case DataBuffer::DATA_TYPE_NORMALIZED_VECTOR2:
					r_buffer.add_normalized_vector2(pending_input.operator Vector2(), info.compression_level);
					break;
				case DataBuffer::DATA_TYPE_VECTOR3:
					r_buffer.add_vector3(pending_input.operator Vector3(), info.compression_level);
					break;
				case DataBuffer::DATA_TYPE_NORMALIZED_VECTOR3:
					r_buffer.add_normalized_vector3(pending_input.operator Vector3(), info.compression_level);
					break;
				case DataBuffer::DATA_TYPE_VARIANT:
					r_buffer.add_variant(pending_input);
					break;
			};
		}
	}
}
void NetworkedInputList::set_defaults(LocalVector<Variant> &r_input) const {
	const uint32_t size = r_input.size() < input_info.size() ? r_input.size() : input_info.size();

	for (uint32_t i = 0; i < size; i += 1) {
		r_input[i] = input_info[i].default_value;
	}
}

void NetworkedInputList::fetch_inputs(DataBuffer &p_buffer, LocalVector<Variant> &r_inputs) const {
	if (r_inputs.size() < input_info.size()) {
		r_inputs.resize(input_info.size());
	}

	for (uint32_t i = 0; i < input_info.size(); i += 1) {
		const NetworkedInputInfo &info = input_info[i];

		const bool is_bool = info.default_value.get_type() != Variant::BOOL;

		bool is_default = false;
		if (is_bool == false) {
			is_default = p_buffer.read_bool();
		}

		if (is_default) {
			r_inputs[i] = info.default_value;
		} else {
			switch (info.data_type) {
				case DataBuffer::DATA_TYPE_BOOL:
					r_inputs[i] = p_buffer.read_bool();
					break;
				case DataBuffer::DATA_TYPE_INT:
					r_inputs[i] = p_buffer.read_int(info.compression_level);
					break;
				case DataBuffer::DATA_TYPE_REAL:
					r_inputs[i] = p_buffer.read_real(info.compression_level);
					break;
				case DataBuffer::DATA_TYPE_POSITIVE_UNIT_REAL:
					r_inputs[i] = p_buffer.read_positive_unit_real(info.compression_level);
					break;
				case DataBuffer::DATA_TYPE_UNIT_REAL:
					r_inputs[i] = p_buffer.read_unit_real(info.compression_level);
					break;
				case DataBuffer::DATA_TYPE_VECTOR2:
					r_inputs[i] = p_buffer.read_vector2(info.compression_level);
					break;
				case DataBuffer::DATA_TYPE_NORMALIZED_VECTOR2:
					r_inputs[i] = p_buffer.read_normalized_vector2(info.compression_level);
					break;
				case DataBuffer::DATA_TYPE_VECTOR3:
					r_inputs[i] = p_buffer.read_vector3(info.compression_level);
					break;
				case DataBuffer::DATA_TYPE_NORMALIZED_VECTOR3:
					r_inputs[i] = p_buffer.read_normalized_vector3(info.compression_level);
					break;
				case DataBuffer::DATA_TYPE_VARIANT:
					r_inputs[i] = p_buffer.read_variant();
					break;
			};
		}
	}
}

bool NetworkedInputList::are_different(DataBuffer &p_buffer_A, DataBuffer &p_buffer_B) const {
	for (uint32_t i = 0; i < input_info.size(); i += 1) {
		const NetworkedInputInfo &info = input_info[i];

		const bool is_bool = info.default_value.get_type() != Variant::BOOL;

		bool is_default_A = false;
		bool is_default_B = false;
		if (is_bool == false) {
			is_default_A = p_buffer_A.read_bool();
			is_default_B = p_buffer_B.read_bool();
		}

		bool are_equals = true;
		if (is_default_A && is_default_B) {
			are_equals = true;
		} else {
			switch (info.data_type) {
				case DataBuffer::DATA_TYPE_BOOL:
					are_equals = p_buffer_A.read_bool() == p_buffer_B.read_bool();
					break;
				case DataBuffer::DATA_TYPE_INT:
					are_equals = Math::is_equal_approx(p_buffer_A.read_int(info.compression_level), p_buffer_B.read_int(info.compression_level), info.comparison_floating_point_precision);
					break;
				case DataBuffer::DATA_TYPE_REAL:
					are_equals = Math::is_equal_approx(static_cast<real_t>(p_buffer_A.read_real(info.compression_level)), static_cast<real_t>(p_buffer_B.read_real(info.compression_level)), info.comparison_floating_point_precision);
					break;
				case DataBuffer::DATA_TYPE_POSITIVE_UNIT_REAL:
					are_equals = Math::is_equal_approx(p_buffer_A.read_positive_unit_real(info.compression_level), p_buffer_B.read_positive_unit_real(info.compression_level), info.comparison_floating_point_precision);
					break;
				case DataBuffer::DATA_TYPE_UNIT_REAL:
					are_equals = Math::is_equal_approx(p_buffer_A.read_unit_real(info.compression_level), p_buffer_B.read_unit_real(info.compression_level), info.comparison_floating_point_precision);
					break;
				case DataBuffer::DATA_TYPE_VECTOR2:
					are_equals = SceneSynchronizer::compare(p_buffer_A.read_vector2(info.compression_level), p_buffer_B.read_vector2(info.compression_level), info.comparison_floating_point_precision);
					break;
				case DataBuffer::DATA_TYPE_NORMALIZED_VECTOR2:
					are_equals = SceneSynchronizer::compare(p_buffer_A.read_normalized_vector2(info.compression_level), p_buffer_B.read_normalized_vector2(info.compression_level), info.comparison_floating_point_precision);
					break;
				case DataBuffer::DATA_TYPE_VECTOR3:
					are_equals = SceneSynchronizer::compare(p_buffer_A.read_vector3(info.compression_level), p_buffer_B.read_vector3(info.compression_level), info.comparison_floating_point_precision);
					break;
				case DataBuffer::DATA_TYPE_NORMALIZED_VECTOR3:
					are_equals = SceneSynchronizer::compare(p_buffer_A.read_normalized_vector3(info.compression_level), p_buffer_B.read_normalized_vector3(info.compression_level), info.comparison_floating_point_precision);
					break;
				case DataBuffer::DATA_TYPE_VARIANT:
					are_equals = SceneSynchronizer::compare(p_buffer_A.read_variant(), p_buffer_B.read_variant(), info.comparison_floating_point_precision);
					break;
			};
		}

		if (!are_equals) {
			return true;
		}
	}

	return false;
}

uint32_t NetworkedInputList::count_size(DataBuffer &p_buffer) const {
	int size = 0;
	for (uint32_t i = 0; i < input_info.size(); i += 1) {
		const NetworkedInputInfo &info = input_info[i];

		const bool is_bool = info.default_value.get_type() != Variant::BOOL;

		if (is_bool == false) {
			size += p_buffer.read_bool_size();
		}

		switch (info.data_type) {
			case DataBuffer::DATA_TYPE_BOOL:
				size += p_buffer.read_bool_size();
				break;
			case DataBuffer::DATA_TYPE_INT:
				size += p_buffer.read_int_size(info.compression_level);
				break;
			case DataBuffer::DATA_TYPE_REAL:
				size += p_buffer.read_real_size(info.compression_level);
				break;
			case DataBuffer::DATA_TYPE_POSITIVE_UNIT_REAL:
				size += p_buffer.read_positive_unit_real_size(info.compression_level);
				break;
			case DataBuffer::DATA_TYPE_UNIT_REAL:
				size += p_buffer.read_unit_real_size(info.compression_level);
				break;
			case DataBuffer::DATA_TYPE_VECTOR2:
				size += p_buffer.read_vector2_size(info.compression_level);
				break;
			case DataBuffer::DATA_TYPE_NORMALIZED_VECTOR2:
				size += p_buffer.read_normalized_vector2_size(info.compression_level);
				break;
			case DataBuffer::DATA_TYPE_VECTOR3:
				size += p_buffer.read_vector3_size(info.compression_level);
				break;
			case DataBuffer::DATA_TYPE_NORMALIZED_VECTOR3:
				size += p_buffer.read_normalized_vector3_size(info.compression_level);
				break;
			case DataBuffer::DATA_TYPE_VARIANT:
				size += p_buffer.read_variant_size();
				break;
		};
	}

	return size;
}