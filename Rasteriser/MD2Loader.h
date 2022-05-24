#pragma once

#include "Model.h"

class MD2Loader
{
	public:
		static bool LoadModel(const char* md2Filename, Model& model);
};
