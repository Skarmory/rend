#ifndef REND_API_VULKAN_EXTENSIONS_H
#define REND_API_VULKAN_EXTENSIONS_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

#include <string>

namespace rend::vk::instance_ext
{
    static const char* acquire_drm_display{ "VK_EXT_acquire_drm_display" };
    static const char* acquire_xlib_display{ "VK_EXT_acquire_xlib_display" };
    static const char* debug_report{ "VK_EXT_debug_report" };
    static const char* debug_utils{ "VK_EXT_debug_utils" };
    static const char* direct_mode_display{ "VK_EXT_direct_mode_display" };
    static const char* display_surface_counter{ "VK_EXT_display_surface_counter" };
    static const char* swapchain_colorspace{ "VK_EXT_swapchain_colorspace" };

    namespace khr
    {
        static const char* device_group_creation{ "VK_KHR_device_group_creation" };
        static const char* display{ "VK_KHR_display" };
        static const char* external_fence_capabilities{ "VK_KHR_external_fence_capabilities" };
        static const char* external_memory_capabilities{ "VK_KHR_external_memory_capabilities" };
        static const char* external_semaphore_capabilities{ "VK_KHR_external_semaphore_capabilities" };
        static const char* get_display_properties2{ "VK_KHR_get_display_properties2" };
        static const char* get_physical_device_properties2{ "VK_KHR_get_physical_device_properties2" };
        static const char* get_surface_capabilities2{ "VK_KHR_get_surface_capabilities2" };
        static const char* portability_enumeration{ "VK_KHR_portability_enumeration" };
        static const char* surface{ "VK_KHR_surface" };
        static const char* surface_protected_capabilities{ "VK_KHR_surface_protected_capabilities" };
        static const char* wayland_surface{ "VK_KHR_wayland_surface" };
        static const char* xcb_surface{ "VK_KHR_xcb_surface" };
        static const char* xlib_surface{ "VK_KHR_xlib_surface" };
    }
    
    namespace lunarg
    {
        static const char* direct_driver_loading{ "VK_LUNARG_direct_driver_loading" };
    }
}

namespace rend::vk::device_ext
{    
    static const char* _4444_formats{ "VK_EXT_4444_formats" };
    static const char* border_color_swizzle{ "VK_EXT_border_color_swizzle" };
    static const char* buffer_device_address{ "VK_EXT_buffer_device_address" };
    static const char* calibrated_timestamps{ "VK_EXT_calibrated_timestamps" };
    static const char* color_write_enable{ "VK_EXT_color_write_enable" };
    static const char* conditional_rendering{ "VK_EXT_conditional_rendering" };
    static const char* conservative_rasterization{ "VK_EXT_conservative_rasterization" };
    static const char* custom_border_color{ "VK_EXT_custom_border_color" };
    static const char* depth_bias_control{ "VK_EXT_depth_bias_control" };
    static const char* depth_clamp_zero_one{ "VK_EXT_depth_clamp_zero_one" };
    static const char* depth_clip_control{ "VK_EXT_depth_clip_control" };
    static const char* depth_clip_enable{ "VK_EXT_depth_clip_enable" };
    static const char* descriptor_indexing{ "VK_EXT_descriptor_indexing" };
    static const char* display_control{ "VK_EXT_display_control" };
    static const char* dynamic_rendering_unused_attachments{ "VK_EXT_dynamic_rendering_unused_attachments" };
    static const char* extended_dynamic_state{ "VK_EXT_extended_dynamic_state" };
    static const char* extended_dynamic_state2{ "VK_EXT_extended_dynamic_state2" };
    static const char* extended_dynamic_state3{ "VK_EXT_extended_dynamic_state3" };
    static const char* external_memory_dma_buf{ "VK_EXT_external_memory_dma_buf" };
    static const char* external_memory_host{ "VK_EXT_external_memory_host" };
    static const char* fragment_shader_interlock{ "VK_EXT_fragment_shader_interlock" };
    static const char* global_priority{ "VK_EXT_global_priority" };
    static const char* global_priority_query{ "VK_EXT_global_priority_query" };
    static const char* graphics_pipeline_library{ "VK_EXT_graphics_pipeline_library" };
    static const char* host_query_reset{ "VK_EXT_host_query_reset" };
    static const char* image_2d_view_of_3d{ "VK_EXT_image_2d_view_of_3d" };
    static const char* image_drm_format_modifier{ "VK_EXT_image_drm_format_modifier" };
    static const char* image_robustness{ "VK_EXT_image_robustness" };
    static const char* image_sliced_view_of_3d{ "VK_EXT_image_sliced_view_of_3d" };
    static const char* image_view_min_lod{ "VK_EXT_image_view_min_lod" };
    static const char* index_type_uint8{ "VK_EXT_index_type_uint8" };
    static const char* inline_uniform_block{ "VK_EXT_inline_uniform_block" };
    static const char* line_rasterization{ "VK_EXT_line_rasterization" };
    static const char* load_store_op_none{ "VK_EXT_load_store_op_none" };
    static const char* memory_budget{ "VK_EXT_memory_budget" };
    static const char* multi_draw{ "VK_EXT_multi_draw" };
    static const char* mutable_descriptor_type{ "VK_EXT_mutable_descriptor_type" };
    static const char* nested_command_buffer{ "VK_EXT_nested_command_buffer" };
    static const char* non_seamless_cube_map{ "VK_EXT_non_seamless_cube_map" };
    static const char* pci_bus_info{ "VK_EXT_pci_bus_info" };
    static const char* physical_device_drm{ "VK_EXT_physical_device_drm" };
    static const char* pipeline_creation_cache_control{ "VK_EXT_pipeline_creation_cache_control" };
    static const char* pipeline_creation_feedback{ "VK_EXT_pipeline_creation_feedback" };
    static const char* pipeline_robustness{ "VK_EXT_pipeline_robustness" };
    static const char* post_depth_coverage{ "VK_EXT_post_depth_coverage" };
    static const char* primitive_topology_list_restart{ "VK_EXT_primitive_topology_list_restart" };
    static const char* primitives_generated_query{ "VK_EXT_primitives_generated_query" };
    static const char* private_data{ "VK_EXT_private_data" };
    static const char* provoking_vertex{ "VK_EXT_provoking_vertex" };
    static const char* queue_family_foreign{ "VK_EXT_queue_family_foreign" };
    static const char* robustness2{ "VK_EXT_robustness2" };
    static const char* sample_locations{ "VK_EXT_sample_locations" };
    static const char* sampler_filter_minmax{ "VK_EXT_sampler_filter_minmax" };
    static const char* scalar_block_layout{ "VK_EXT_scalar_block_layout" };
    static const char* separate_stencil_usage{ "VK_EXT_separate_stencil_usage" };
    static const char* shader_atomic_float{ "VK_EXT_shader_atomic_float" };
    static const char* shader_atomic_float2{ "VK_EXT_shader_atomic_float2" };
    static const char* shader_demote_to_helper_invocation{ "VK_EXT_shader_demote_to_helper_invocation" };
    static const char* shader_module_identifier{ "VK_EXT_shader_module_identifier" };
    static const char* shader_stencil_export{ "VK_EXT_shader_stencil_export" };
    static const char* shader_subgroup_ballot{ "VK_EXT_shader_subgroup_ballot" };
    static const char* shader_subgroup_vote{ "VK_EXT_shader_subgroup_vote" };
    static const char* shader_viewport_index_layer{ "VK_EXT_shader_viewport_index_layer" };
    static const char* subgroup_size_control{ "VK_EXT_subgroup_size_control" };
    static const char* texel_buffer_alignment{ "VK_EXT_texel_buffer_alignment" };
    static const char* tooling_info{ "VK_EXT_tooling_info" };
    static const char* transform_feedback{ "VK_EXT_transform_feedback" };
    static const char* vertex_attribute_divisor{ "VK_EXT_vertex_attribute_divisor" };
    static const char* vertex_input_dynamic_state{ "VK_EXT_vertex_input_dynamic_state" };
    static const char* ycbcr_image_arrays{ "VK_EXT_ycbcr_image_arrays" };

    namespace amd
    {
        static const char* buffer_marker{ "VK_AMD_buffer_marker" };
    }

    namespace google
    {
        static const char* decorate_string{ "VK_GOOGLE_decorate_string" };
        static const char* hlsl_functionality1{ "VK_GOOGLE_hlsl_functionality1" };
        static const char* user_type{ "VK_GOOGLE_user_type" };
    }

    namespace intel
    {
        static const char* shader_integer_functions2{ "VK_INTEL_shader_integer_functions2" };
    }

    namespace khr
    {
        static const char* _16bit_storage{ "VK_KHR_16bit_storage" };
        static const char* _8bit_storage{ "VK_KHR_8bit_storage" };
        static const char* bind_memory2{ "VK_KHR_bind_memory2" };
        static const char* buffer_device_address{ "VK_KHR_buffer_device_address" };
        static const char* calibrated_timestamps{ "VK_KHR_calibrated_timestamps" };
        static const char* cooperative_matrix{ "VK_KHR_cooperative_matrix" };
        static const char* copy_commands2{ "VK_KHR_copy_commands2" };
        static const char* create_renderpass2{ "VK_KHR_create_renderpass2" };
        static const char* dedicated_allocation{ "VK_KHR_dedicated_allocation" };
        static const char* deferred_host_operations{ "VK_KHR_deferred_host_operations" };
        static const char* depth_stencil_resolve{ "VK_KHR_depth_stencil_resolve" };
        static const char* descriptor_update_template{ "VK_KHR_descriptor_update_template" };
        static const char* device_group{ "VK_KHR_device_group" };
        static const char* draw_indirect_count{ "VK_KHR_draw_indirect_count" };
        static const char* driver_properties{ "VK_KHR_driver_properties" };
        static const char* dynamic_rendering{ "VK_KHR_dynamic_rendering" };
        static const char* external_fence{ "VK_KHR_external_fence" };
        static const char* external_fence_fd{ "VK_KHR_external_fence_fd" };
        static const char* external_memory{ "VK_KHR_external_memory" };
        static const char* external_memory_fd{ "VK_KHR_external_memory_fd" };
        static const char* external_semaphore{ "VK_KHR_external_semaphore" };
        static const char* external_semaphore_fd{ "VK_KHR_external_semaphore_fd" };
        static const char* format_feature_flags2{ "VK_KHR_format_feature_flags2" };
        static const char* get_memory_requirements2{ "VK_KHR_get_memory_requirements2" };
        static const char* global_priority{ "VK_KHR_global_priority" };
        static const char* image_format_list{ "VK_KHR_image_format_list" };
        static const char* imageless_framebuffer{ "VK_KHR_imageless_framebuffer" };
        static const char* incremental_present{ "VK_KHR_incremental_present" };
        static const char* maintenance1{ "VK_KHR_maintenance1" };
        static const char* maintenance2{ "VK_KHR_maintenance2" };
        static const char* maintenance3{ "VK_KHR_maintenance3" };
        static const char* maintenance4{ "VK_KHR_maintenance4" };
        static const char* maintenance5{ "VK_KHR_maintenance5" };
        static const char* maintenance6{ "VK_KHR_maintenance6" };
        static const char* map_memory2{ "VK_KHR_map_memory2" };
        static const char* multiview{ "VK_KHR_multiview" };
        static const char* pipeline_executable_properties{ "VK_KHR_pipeline_executable_properties" };
        static const char* pipeline_library{ "VK_KHR_pipeline_library" };
        static const char* push_descriptor{ "VK_KHR_push_descriptor" };
        static const char* relaxed_block_layout{ "VK_KHR_relaxed_block_layout" };
        static const char* sampler_mirror_clamp_to_edge{ "VK_KHR_sampler_mirror_clamp_to_edge" };
        static const char* sampler_ycbcr_conversion{ "VK_KHR_sampler_ycbcr_conversion" };
        static const char* separate_depth_stencil_layouts{ "VK_KHR_separate_depth_stencil_layouts" };
        static const char* shader_atomic_int64{ "VK_KHR_shader_atomic_int64" };
        static const char* shader_clock{ "VK_KHR_shader_clock" };
        static const char* shader_draw_parameters{ "VK_KHR_shader_draw_parameters" };
        static const char* shader_float16_int8{ "VK_KHR_shader_float16_int8" };
        static const char* shader_float_controls{ "VK_KHR_shader_float_controls" };
        static const char* shader_integer_dot_product{ "VK_KHR_shader_integer_dot_product" };
        static const char* shader_non_semantic_info{ "VK_KHR_shader_non_semantic_info" };
        static const char* shader_subgroup_extended_types{ "VK_KHR_shader_subgroup_extended_types" };
        static const char* shader_subgroup_uniform_control_flow{ "VK_KHR_shader_subgroup_uniform_control_flow" };
        static const char* shader_terminate_invocation{ "VK_KHR_shader_terminate_invocation" };
        static const char* spirv_1_4{ "VK_KHR_spirv_1_4" };
        static const char* storage_buffer_storage_class{ "VK_KHR_storage_buffer_storage_class" };
        static const char* swapchain{ "VK_KHR_swapchain" };
        static const char* swapchain_mutable_format{ "VK_KHR_swapchain_mutable_format" };
        static const char* synchronization2{ "VK_KHR_synchronization2" };
        static const char* timeline_semaphore{ "VK_KHR_timeline_semaphore" };
        static const char* uniform_buffer_standard_layout{ "VK_KHR_uniform_buffer_standard_layout" };
        static const char* variable_pointers{ "VK_KHR_variable_pointers" };
        static const char* vertex_attribute_divisor{ "VK_KHR_vertex_attribute_divisor" };
        static const char* vulkan_memory_model{ "VK_KHR_vulkan_memory_model" };
        static const char* workgroup_memory_explicit_layout{ "VK_KHR_workgroup_memory_explicit_layout" };
        static const char* zero_initialize_workgroup_memory{ "VK_KHR_zero_initialize_workgroup_memory" };
    }

    namespace nv
    {
        static const char* compute_shader_derivatives{ "VK_NV_compute_shader_derivatives" };
    }

    namespace valve
    {
        static const char* mutable_descriptor_type{ "VK_VALVE_mutable_descriptor_type" };
    }
}

#pragma GCC diagnostic pop

#endif
