#pragma once

#include "../texture/texture.hpp"

namespace Onion::Rendering {

	class Material {

	public:

		Material() {
		}

		Texture* Albedo = nullptr;
		Texture* Normal = nullptr;
		Texture* Roughness = nullptr;

	};

}