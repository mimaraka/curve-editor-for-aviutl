#pragma once

#include "view_graph.hpp"
#include <mkaul/point.hpp>
#include <cereal/cereal.hpp>



namespace cereal {
	template <class Archive>
	void save(Archive& archive, const mkaul::Point<double>& pt) {
		archive(
			pt.x,
			pt.y
		);
	}

	template <class Archive>
	void load(Archive& archive, mkaul::Point<double>& pt) {
		archive(
			pt.x,
			pt.y
		);
	}
}

namespace cved {
	// 操作可能なポイント
	class ControlPoint {
	private:
		mkaul::Point<double> pt_;
		bool controlled_;
		bool fixed_;

	public:
		constexpr ControlPoint(const mkaul::Point<double>& pt = mkaul::Point<double>{}, bool fixed = false) :
			pt_{pt},
			controlled_{false},
			fixed_{fixed}
		{}

		auto operator = (const mkaul::Point<double>& pt) noexcept {
			this->pt_ = pt;
			return *this;
		}

		// アクセッサ
		const auto& pt() const noexcept { return pt_; }
		auto x() const noexcept { return pt_.x; }
		auto y() const noexcept { return pt_.y; }
		auto is_fixed() const noexcept { return fixed_; }

		// 引数のポイントがコントロールポイント付近にあるかどうか
		bool is_hovered(const mkaul::Point<double>& pt, const GraphView& view) const noexcept;
		bool is_controlled() const noexcept { return controlled_; }
		bool check_hover(const mkaul::Point<double>& pt, const GraphView& view) noexcept;
		
		bool update(const mkaul::Point<double>& pt) noexcept;
		void move(const mkaul::Point<double>& pt) noexcept;

		void end_control() noexcept { controlled_ = false; }

		template <class Archive>
		void save(Archive& archive, const std::uint32_t) const {
			archive(
				pt_,
				fixed_
			);
		}

		template <class Archive>
		void load(Archive& archive, const std::uint32_t) {
			archive(
				pt_,
				fixed_
			);
		}
	};
}

CEREAL_CLASS_VERSION(cved::ControlPoint, 0)