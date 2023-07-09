#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "Window.h"
#ifdef API_VULKAN
//#define VMA_IMPLEMENTATION
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
#include "Vulkan/Renderer.h"
#elif API_OPENGL
#include "Renderer.h"
#include "OREML/Loader.h"
#include "Objects/Font.h"
#endif