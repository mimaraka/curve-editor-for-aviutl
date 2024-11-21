#pragma once

#include "constants.hpp"
#include "curve_graph.hpp"
#include "string_table.hpp"



namespace curve_editor {
	// カーブ(直線)
	class LinearCurve : public GraphCurve {
	public:
		LinearCurve(
			const mkaul::Point<double>& anchor_start = mkaul::Point{ 0., 0. },
			const mkaul::Point<double>& anchor_end = mkaul::Point{ 1., 1. },
			bool pt_fixed = false,
			GraphCurve* prev = nullptr,
			GraphCurve* next = nullptr
		) noexcept :
			GraphCurve{ anchor_start, anchor_end, pt_fixed, prev, next }
		{}

		[[nodiscard]] constexpr std::string get_name() const noexcept override { return global::CURVE_NAME_LINEAR; }
		[[nodiscard]] std::string get_disp_name() const noexcept override { return global::string_table[global::StringTable::StringId::LabelCurveSegmentTypeLinear]; }

		// カーブの値を取得
		[[nodiscard]] double curve_function(double progress, double start, double end) const noexcept override;
		// カーブを初期化
		void clear() noexcept override {}
		// カーブがデフォルトかどうか
		[[nodiscard]] bool is_default() const noexcept override { return true; }

		template <class Archive>
		void save(Archive& archive, const std::uint32_t) const {
			archive(
				cereal::base_class<GraphCurve>(this)
			);
		}

		template <class Archive>
		void load(Archive& archive, const std::uint32_t) {
			archive(
				cereal::base_class<GraphCurve>(this)
			);
		}
	};
} // namespace curve_editor

CEREAL_CLASS_VERSION(curve_editor::LinearCurve, 0)
CEREAL_REGISTER_TYPE(curve_editor::LinearCurve)
CEREAL_REGISTER_POLYMORPHIC_RELATION(curve_editor::GraphCurve, curve_editor::LinearCurve)